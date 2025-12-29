#pragma once

#include <vk_types.h>

class VulkanEngine {
public:

	bool _isInitialized{ false };
	int _frameNumber {0};
	bool stop_rendering{ false };
	VkExtent2D _windowExtent{ 1700 , 900 };

	struct SDL_Window* _window{ nullptr };

    std::shared_ptr<spdlog::logger> m_logger;

	static VulkanEngine& Get();

	//initializes everything in the engine
	void init(std::shared_ptr<spdlog::logger> logger);

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();

	//run main loop
	void run();
};
