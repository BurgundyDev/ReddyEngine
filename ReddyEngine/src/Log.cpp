#include "Engine/Log.h"

#include <iostream>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Engine
{
	std::shared_ptr<spdlog::logger> Log::pCoreLogger;
	std::shared_ptr<spdlog::logger> Log::pClientLogger;

	void Log::Init()
	{
		try
		{
			spdlog::set_pattern("%^ [%l] %D %T > %v");

			pCoreLogger = spdlog::stdout_color_mt("ReddyEngine");
			pClientLogger = spdlog::stdout_color_mt("ReddyGame");
		}
		catch (spdlog::spdlog_ex& ex)
		{
#if defined(DEBUG)
			std::cout << "Log initialization failed: " << ex.what() << std::endl;
			assert(false);
			return;
#endif
		}
	}
}