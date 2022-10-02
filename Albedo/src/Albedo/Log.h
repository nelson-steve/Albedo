#pragma once

#include <memory>

#include "Core.h"
#include "spdlog/spdlog.h"

namespace Albedo {

	class Albedo_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetAppLogger() { return m_AppLogger; }
		
	private:
		static std::shared_ptr<spdlog::logger> m_CoreLogger;
		static std::shared_ptr<spdlog::logger> m_AppLogger;
	};
}

//Core Log Macors
#define Albedo_Core_TRACE(...) ::Albedo::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define Albedo_Core_INFO(...)  ::Albedo::Log::GetCoreLogger()->info(__VA_ARGS__)
#define Albedo_Core_WARN(...)  ::Albedo::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define Albedo_Core_ERROR(...) ::Albedo::Log::GetCoreLogger()->error(__VA_ARGS__)

//Application Log Macors
#define Albedo_TRACE(...) ::Albedo::Log::GetAppLogger()->trace(__VA_ARGS__)
#define Albedo_INFO(...)  ::Albedo::Log::GetAppLogger()->info(__VA_ARGS__)
#define Albedo_WARN(...)  ::Albedo::Log::GetAppLogger()->warn(__VA_ARGS__)
#define Albedo_ERROR(...) ::Albedo::Log::GetAppLogger()->error(__VA_ARGS__)
