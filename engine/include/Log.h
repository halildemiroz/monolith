#pragma once
#include <string>
#include <sstream>
#include <cstdlib>

namespace Monolith {

	enum class LogLevel{ Trace = 0, Info, Warn, Error, Off };

	class Log{
		public:
			static void Init(const std::string& filePath = "");
			static void Shutdown();

			static void SetLevel(LogLevel level);
			static LogLevel GetLevel();

			template<typename... Args>
				static void Write(LogLevel level, const char* category, Args&&... args){
					if(level < GetLevel())
						return;
					
					std::ostringstream stream;
					(stream << ... << args);
					Emit(level, category, stream.str());
				}
		
		private:
			static void Emit(LogLevel level, const char* category, const std::string& message);
	};

}

#define MONO_TRACE(...) ::Monolith::Log::Write(::Monolith::LogLevel::Trace, "ENGINE", __VA_ARGS__)
#define MONO_INFO(...) ::Monolith::Log::Write(::Monolith::LogLevel::Info, "ENGINE", __VA_ARGS__)
#define MONO_WARN(...) ::Monolith::Log::Write(::Monolith::LogLevel::Warn, "ENGINE", __VA_ARGS__)
#define MONO_ERROR(...) ::Monolith::Log::Write(::Monolith::LogLevel::Error, "ENGINE", __VA_ARGS__)

#define APP_TRACE(...) ::Monolith::Log::Write(::Monolith::LogLevel::Trace, "APP", __VA_ARGS__)
#define APP_INFO(...) ::Monolith::Log::Write(::Monolith::LogLevel::Info, "APP", __VA_ARGS__)
#define APP_WARN(...) ::Monolith::Log::Write(::Monolith::LogLevel::Warn, "APP", __VA_ARGS__)
#define APP_ERROR(...) ::Monolith::Log::Write(::Monolith::LogLevel::Error, "APP", __VA_ARGS__)

#define MONO_ASSERT(condition, ...)                                      \
	do{                                                                  \
		if(!(condition)){                                                \
			MONO_ERROR("Assertion failed: " #condition " | ", __VA_ARGS__, \
				" (", __FILE__, ":", __LINE__, ")");                     \
			std::abort();                                                \
		}                                                                \
	}while(0)

