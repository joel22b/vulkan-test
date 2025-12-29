#include "vk_engine.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <vk_initializers.h>
#include <vk_types.h>

#include <VkBootstrap.h>

#include <chrono>
#include <thread>

// Enabling this added a validation layer in Vulkan's API to catch some errors and
// nicely report them.
// This does come with a performance cost.
// TODO: Make this toggle with whether cmake is built in debug or release mode.
constexpr bool bUseValidationLayers = true;

VulkanEngine* loadedEngine = nullptr;

VulkanEngine&
VulkanEngine::Get()
{ return *loadedEngine; }

bool 
VulkanEngine::init(std::shared_ptr<spdlog::logger> logger)
{
    m_logger = logger;

    // only one engine initialization is allowed with the application.
    assert(loadedEngine == nullptr);
    loadedEngine = this;

    // We initialize SDL and create a window with it.
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

    _window = SDL_CreateWindow(
        "Vulkan Engine",
        //SDL_WINDOWPOS_UNDEFINED,
        //SDL_WINDOWPOS_UNDEFINED,
        _windowExtent.width,
        _windowExtent.height,
        window_flags);
    if (_window == NULL)
    {
        // Window creation failed
        m_logger->error("SDL failed to create window with error: [{}]", std::string(SDL_GetError()));
        return false;
    }

    if (!init_vulkan())
    {
        m_logger->error("Failed to initialize Vulkan");
        return false;
    }

	if(!init_swapchain())
    {
        m_logger->error("Failed to initialize Swapchain");
        return false;
    }

	init_commands();

	init_sync_structures();

    // everything went fine
    _isInitialized = true;
    m_logger->info("Vulkan Engine initialization completed");
    return _isInitialized;
}

void
VulkanEngine::cleanup()
{
    m_logger->info("Vulkan Engine cleanup started");

    if (_isInitialized)
    {
        //make sure the gpu has stopped doing its things
		vkDeviceWaitIdle(_device);

		for (int i = 0; i < FRAME_OVERLAP; i++) {
			vkDestroyCommandPool(_device, _frames[i]._commandPool, nullptr);
		}
        
        destroy_swapchain();

		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkDestroyDevice(_device, nullptr);
		
		vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
		vkDestroyInstance(_instance, nullptr);

        SDL_DestroyWindow(_window);
    }

    // clear engine pointer
    loadedEngine = nullptr;
}

void
VulkanEngine::draw()
{
    // nothing yet
}

void
VulkanEngine::run()
{
    SDL_Event e;
    bool bQuit = false;

    // main loop
    while (!bQuit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            // close the window when user alt-f4s or clicks the X button
            if (e.type == SDL_EVENT_QUIT)
            {
                bQuit = true;
                m_logger->info("Window was closed");
            }

            if (e.type == SDL_EVENT_WINDOW_MINIMIZED)
            {
                stop_rendering = true;
                m_logger->debug("Window was minimized");
            }
            if (e.type == SDL_EVENT_WINDOW_RESTORED)
            {
                stop_rendering = false;
                m_logger->debug("Window was restored");
            }
        }

        // do not draw if we are minimized
        if (stop_rendering) {
            // throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        draw();
    }
}

bool
VulkanEngine::init_vulkan()
{
    vkb::InstanceBuilder builder;

	//make the vulkan instance, with basic debug features
	vkb::Result<vkb::Instance> inst_ret = builder.set_app_name("Example Vulkan Application")
		.request_validation_layers(bUseValidationLayers)
        .set_debug_callback([](VkDebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
                               VkDebugUtilsMessageTypeFlagsEXT              messageType,
                               const VkDebugUtilsMessengerCallbackDataEXT*  pCallbackData,
                               void*                                        userData) -> VkBool32
        {
            spdlog::level::level_enum logLevel;
            switch (messageSeverity)
            {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                    logLevel = spdlog::level::level_enum::err;
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                    logLevel = spdlog::level::level_enum::warn;
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                    logLevel = spdlog::level::level_enum::info;
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                    logLevel = spdlog::level::level_enum::debug;
                    break;
                default:
                    logLevel = spdlog::level::level_enum::critical;
                    break;
            }
            const char* type = vkb::to_string_message_type(messageType);

            spdlog::logger* logger = reinterpret_cast<spdlog::logger*>(userData);
            logger->log(logLevel, fmt::format("VVL: Type={} [{}]", *type, std::string(pCallbackData->pMessage)));

            // Return false to move on, but return true for validation to skip passing down the call to the driver
            return VK_TRUE;
        })
        .set_debug_callback_user_data_pointer(m_logger.get())
		.require_api_version(1, 3, 0)
		.build();

    if (!inst_ret.has_value())
    {
        m_logger->error("VkInstance builder failed with error: 0x{:x} [{}]",
            inst_ret.error().value(), inst_ret.error().message());
        return false;
    }
	vkb::Instance vkb_inst = inst_ret.value();

	//grab the instance 
	_instance = vkb_inst.instance;
	_debug_messenger = vkb_inst.debug_messenger;

    if (!SDL_Vulkan_CreateSurface(_window, _instance, NULL, &_surface))
    {
        // Wasn't successful
        m_logger->error("Failed to create surface: [{}]", std::string(SDL_GetError()));
        return false;
    }

	//vulkan 1.3 features
	VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
	features.dynamicRendering = true;
	features.synchronization2 = true;

	//vulkan 1.2 features
	VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	features12.bufferDeviceAddress = true;
	features12.descriptorIndexing = true;

	//use vkbootstrap to select a gpu. 
	//We want a gpu that can write to the SDL surface and supports vulkan 1.3 with the correct features
	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	vkb::Result<vkb::PhysicalDevice> physicalDevice_ret = selector
		.set_minimum_version(1, 3)
		.set_required_features_13(features)
		.set_required_features_12(features12)
		.set_surface(_surface)
		.select();

    if (!physicalDevice_ret.has_value())
    {
        m_logger->error("VkPhysicalDevice selector failed with error: 0x{:x} [{}]",
            physicalDevice_ret.error().value(), physicalDevice_ret.error().message());
        return false;
    }

	//create the final vulkan device
	vkb::DeviceBuilder deviceBuilder{ physicalDevice_ret.value() };

	vkb::Result<vkb::Device> vkbDevice_ret = deviceBuilder.build();
    if (!vkbDevice_ret.has_value())
    {
        m_logger->error("VkDevice builder failed with error: 0x{:x} [{}]",
            vkbDevice_ret.error().value(), vkbDevice_ret.error().message());
        return false;
    }

	// Get the VkDevice handle used in the rest of a vulkan application
	_device = vkbDevice_ret.value().device;
	_chosenGPU = physicalDevice_ret.value().physical_device;

    {
        // Output some info on what GPU was chosen
        VkPhysicalDeviceProperties gpuProperties;
        vkGetPhysicalDeviceProperties(_chosenGPU, &gpuProperties);
        m_logger->debug("Chosen GPU: ID={} Type=[{}] Version=[API={} Driver={}] Name=[{}]", gpuProperties.deviceID,
            gpuProperties.deviceType, gpuProperties.apiVersion, gpuProperties.driverVersion,
            std::string(gpuProperties.deviceName));
    }

    // use vkbootstrap to get a Graphics queue
	_graphicsQueue = vkbDevice_ret.value().get_queue(vkb::QueueType::graphics).value();
	_graphicsQueueFamily = vkbDevice_ret.value().get_queue_index(vkb::QueueType::graphics).value();
    m_logger->debug("Using GPU queue family: {}", _graphicsQueueFamily);

    // Everything was successful
    return true;
}

bool
VulkanEngine::init_swapchain()
{
    return create_swapchain(_windowExtent.width, _windowExtent.height);
}

void
VulkanEngine::init_commands()
{
    // Create a command pool for commands submitted to the graphics queue.
	// We also want the pool to allow for resetting of individual command buffers
	VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	for (int i = 0; i < FRAME_OVERLAP; i++) {

		VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_frames[i]._commandPool));

		// allocate the default command buffer that we will use for rendering
		VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_frames[i]._commandPool, 1);

		VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_frames[i]._mainCommandBuffer));
	}
}

void
VulkanEngine::init_sync_structures()
{
    //nothing yet
}

bool
VulkanEngine::create_swapchain(uint32_t width, uint32_t height)
{
	vkb::SwapchainBuilder swapchainBuilder{ _chosenGPU,_device,_surface };

	_swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

	vkb::Result<vkb::Swapchain> vkbSwapchain_ret = swapchainBuilder
		//.use_default_format_selection()
		.set_desired_format(VkSurfaceFormatKHR{ .format = _swapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		//use vsync present mode
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) // VSync setting: https://vkguide.dev/docs/new_chapter_1/vulkan_init_flow/
		.set_desired_extent(width, height)
		.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.build();
    if (!vkbSwapchain_ret.has_value())
    {
        m_logger->error("VkSwapchainKHR builder failed with error: 0x{:x} [{}]",
            vkbSwapchain_ret.error().value(), vkbSwapchain_ret.error().message());
        return false;
    }

	_swapchainExtent = vkbSwapchain_ret.value().extent;
	//store swapchain and its related images
	_swapchain = vkbSwapchain_ret.value().swapchain;
	_swapchainImages = vkbSwapchain_ret.value().get_images().value();
	_swapchainImageViews = vkbSwapchain_ret.value().get_image_views().value();

    return true;
}

void
VulkanEngine::destroy_swapchain()
{
	vkDestroySwapchainKHR(_device, _swapchain, nullptr);

	// destroy swapchain resources
	for (int i = 0; i < _swapchainImageViews.size(); i++)
    {
		vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
	}
}
