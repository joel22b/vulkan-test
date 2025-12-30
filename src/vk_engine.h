#pragma once

#include <vk_types.h>
#include <deletion_queue.h>

struct FrameData {

	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;
    VkSemaphore _swapchainSemaphore;
	VkFence _renderFence;
	DeletionQueue _deletionQueue;
};

// Double buffering so we can prepare the next frame
// while the GPU is rendering the current frame
constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanEngine
{
public:
    // Generic objects for class management
	bool _isInitialized{ false };
	int _frameNumber {0};
	bool stop_rendering{ false };
	VkExtent2D _windowExtent{ 1700 , 900 };
    std::shared_ptr<spdlog::logger> m_logger;
	DeletionQueue _mainDeletionQueue;

    // SDL objects for window creation
	struct SDL_Window* _window{ nullptr };

    // Vulkan objects for GPU rendering
    VkInstance _instance;// Vulkan library handle
	VkDebugUtilsMessengerEXT _debug_messenger;// Vulkan debug output handle
	VkPhysicalDevice _chosenGPU;// GPU chosen as the default device
	VkDevice _device; // Vulkan device for commands
	VkSurfaceKHR _surface;// Vulkan window surface

    // Swapchain objects for displaying the final image in the window
    // NOTE: Swapchain needs to be recreated if window size changes
    VkSwapchainKHR _swapchain;
	VkFormat _swapchainImageFormat;

	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;
    std::vector<VkSemaphore> _renderSemaphores;
	VkExtent2D _swapchainExtent;

    // Frame and Vulkan Command objects
    FrameData _frames[FRAME_OVERLAP]; // Should not be accessed directly outside init logic, use get_current_frame()
	FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; };

	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;

	// Vulkan Memory Allocator objects
	VmaAllocator _allocator;

	// Vulkan image objects
	AllocatedImage _drawImage;
	VkExtent2D _drawExtent;

    // Acts like singleton but allows up to control creation and deletion
	static VulkanEngine& Get();

	//initializes everything in the engine
	bool init(std::shared_ptr<spdlog::logger> logger);

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();
	void draw_background(VkCommandBuffer cmd);

	//run main loop
	void run();

private:
    bool init_vulkan();
	bool init_swapchain();
	void init_commands();
	void init_sync_structures();

    bool create_swapchain(uint32_t width, uint32_t height);
	void destroy_swapchain();
};
