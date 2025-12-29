#include "vk_engine.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

//#include <vk_initializers.h>
#include <vk_types.h>

#include <chrono>
#include <thread>

constexpr bool bUseValidationLayers = false;

VulkanEngine* loadedEngine = nullptr;

VulkanEngine&
VulkanEngine::Get()
{ return *loadedEngine; }

void 
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

    // everything went fine
    _isInitialized = true;

    m_logger->info("Vulkan Engine initialization completed");
}

void
VulkanEngine::cleanup()
{
    m_logger->info("Vulkan Engine cleanup started");

    if (_isInitialized)
    {
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
                m_logger->info("Vulkan Engine run got SDL_EVENT_QUIT event");
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
