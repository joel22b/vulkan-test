#pragma once

#include <vk_types.h>

class VulkanEngine
{
public:
    // Generic objects for class management
	bool _isInitialized{ false };
	int _frameNumber {0};
	bool stop_rendering{ false };
	VkExtent2D _windowExtent{ 1700 , 900 };
    std::shared_ptr<spdlog::logger> m_logger;

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
	VkExtent2D _swapchainExtent;

    // Acts like singleton but allows up to control creation and deletion
	static VulkanEngine& Get();

	//initializes everything in the engine
	bool init(std::shared_ptr<spdlog::logger> logger);

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();

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
