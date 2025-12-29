#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

//#include <vk_engine.h>

int main(int argc, char* argv[])
{
    // Create logger first
    std::shared_ptr<spdlog::logger> logger;
    {
        std::vector<spdlog::sink_ptr> sinks;
        spdlog::sink_ptr sinkConsole = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        spdlog::sink_ptr sinkFile = std::make_shared<spdlog::sinks::basic_file_sink_mt>("vulkan-test.log", true);

        sinkConsole->set_level(spdlog::level::debug);
        sinkFile->set_level(spdlog::level::trace);

        sinkConsole->set_pattern("[%T.%f] [%^%L%$] %n: %v");
        sinkFile->set_pattern("[%T.%f] [%^%L%$] %n: %v");

        sinks.push_back(sinkConsole);
        sinks.push_back(sinkFile);

        logger = std::make_shared<spdlog::logger>("vulkan-test", std::begin(sinks), std::end(sinks));
        spdlog::register_logger(logger);
        spdlog::set_level(spdlog::level::trace);
        spdlog::flush_every(std::chrono::seconds(1));

        logger->debug("Logger created successfully");
    }

	/*VulkanEngine engine;

	engine.init();	
	
	engine.run();	

	engine.cleanup();*/

	return 0;
}
