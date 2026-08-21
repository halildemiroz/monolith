#include "Input.h"
#include <Log.h>

#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <unistd.h>

namespace Monolith {
	
	namespace {
		std::mutex s_mutex;
		LogLevel s_level = LogLevel::Trace;
		std::ofstream s_file;
		bool s_useColor = false;

		const char* LevelName(LogLevel level){
			switch(level){
				case LogLevel::Trace: return "TRACE";
				case LogLevel::Info: return "INFO";
				case LogLevel::Warn: return "WARN";
				case LogLevel::Error: return "ERROR";
				default: return "?????";
			}
		}

		const char* LevelColor(LogLevel level){
			switch(level){
				case LogLevel::Trace: return "\033[90m";
				case LogLevel::Info: return "\033[32m";  
				case LogLevel::Warn: return "\033[33m";  
				case LogLevel::Error: return "\033[31m";  
				default: return "";
			}
		}

		std::string Timestamp(){
			using namespace std::chrono;

			auto now = system_clock::now();
			auto time = system_clock::to_time_t(now);
			auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

			std::tm tm{};
			localtime_r(&time, &tm);

			std::ostringstream out;
			out << std::put_time(&tm, "%H:%M:%S") << '.' << std::setfill('0') <<std::setw(3) << ms.count();
			return out.str();
		}

	}

	void Log::Init(const std::string& filePath){
		std::lock_guard<std::mutex> lock(s_mutex);

		s_useColor = isatty(fileno(stdout)) != 0;

		if(!filePath.empty()){
			s_file.open(filePath, std::ios::out | std::ios::trunc);
			if(!s_file.is_open())
				std::cerr << "Log: failed to open " << filePath << std::endl;
		}
	}

	void Log::Shutdown(){
		std::lock_guard<std::mutex> lock(s_mutex);
		if(s_file.is_open())
			s_file.close();
	}

	void Log::SetLevel(LogLevel level){
		std::lock_guard<std::mutex> lock(s_mutex);
		s_level = level;
	}

	LogLevel Log::GetLevel(){
		std::lock_guard<std::mutex> lock(s_mutex);
		return s_level;
	}

	void Log::Emit(LogLevel level, const char* category, const std::string& message){
		std::lock_guard<std::mutex> lock(s_mutex);

		std::ostream& out = (level >= LogLevel::Warn) ? std::cerr : std::cout;

		if(s_useColor)
			out << LevelColor(level);

		out << '['  << Timestamp() << "]" << LevelName(level) << "] [" << category << "] " << message;
		
		if(s_useColor)
			out << "\033[0m";
		out << std::endl;

		if(s_file.is_open())
			s_file << '[' << Timestamp() << "] [" << LevelName(level) << "] [" << category << "] " << message << std::endl;
	}
	
}

