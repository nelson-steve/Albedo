#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Albedo {

	std::shared_ptr<spdlog::logger> Log::m_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::m_AppLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		m_CoreLogger = spdlog::stdout_color_mt("ALBEDO");
		m_CoreLogger->set_level(spdlog::level::trace);

		m_AppLogger = spdlog::stdout_color_mt("APP");
		m_AppLogger->set_level(spdlog::level::trace);
	}
}
