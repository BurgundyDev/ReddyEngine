#pragma once

#include "Engine/Platform.h"
#include "Engine/Utils.h"

#if !defined(DEBUG)
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#endif

#include <spdlog/spdlog.h>

namespace Engine
{
	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return pCoreLogger; };
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return pClientLogger; };

	private:
		static std::shared_ptr<spdlog::logger> pCoreLogger;
		static std::shared_ptr<spdlog::logger> pClientLogger;
	};
}

#define CORE_TRACE(...)					SPDLOG_LOGGER_TRACE(Engine::Log::GetCoreLogger(), __VA_ARGS__)
#define CORE_INFO(...)					SPDLOG_LOGGER_INFO(Engine::Log::GetCoreLogger(), __VA_ARGS__)
#define CORE_WARN(...)					SPDLOG_LOGGER_WARN(Engine::Log::GetCoreLogger(), __VA_ARGS__)
#define CORE_ERROR(...)					SPDLOG_LOGGER_ERROR(Engine::Log::GetCoreLogger(), __VA_ARGS__)
#define CORE_FATAL(expression, ...)		if(!expression) { SPDLOG_LOGGER_CRITICAL(Engine::Log::GetCoreLogger(), __VA_ARGS__);								\
											Engine::Platform::ShowFatalErrorMessageBox(Engine::Utils::concatenateChars(__VA_ARGS__));						\
											__debugbreak();  }


#define CLIENT_TRACE(...)				(Engine::Log::GetClientLogger(), __VA_ARGS__)
#define CLIENT_INFO(...)				SPDLOG_LOGGER_INFO(Engine::Log::GetClientLogger(), __VA_ARGS__)
#define CLIENT_WARN(...)				SPDLOG_LOGGER_WARN(Engine::Log::GetClientLogger(), __VA_ARGS__)
#define CLIENT_ERROR(...)				SPDLOG_LOGGER_ERROR(Engine::Log::GetClientLogger(), __VA_ARGS__)
#define CLIENT_FATAL(expression, ...)	if(!expression) { SPDLOG_LOGGER_CRITICAL(Engine::Log::GetClientLogger(), __VA_ARGS__);								\
											Engine::Platform::ShowFatalErrorMessageBox(Engine::Utils::concatenateChars(__VA_ARGS__));						\
											__debugbreak();  }

//_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)
