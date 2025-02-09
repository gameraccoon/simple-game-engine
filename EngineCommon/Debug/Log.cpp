#include "EngineCommon/precomp.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <thread>

#include "EngineCommon/EngineLogCategories.h"

Log::Log()
{
	const std::filesystem::path LOGS_DIR{ "./logs" };
	const char* LOG_EXTENSION = ".log.txt";

	namespace fs = std::filesystem;
	if (!fs::is_directory(LOGS_DIR) || !fs::exists(LOGS_DIR))
	{
		fs::create_directory(LOGS_DIR);
	}

	const time_t in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%y%m%d-%H%M%S");

	mLogFileStream = std::ofstream(LOGS_DIR / (ss.str() + LOG_EXTENSION));
	writeInfo(LOG_LOGGING, "Log file created");
}

Log::~Log()
{
	writeInfo(LOG_LOGGING, "End of log");
	mLogFileStream.close();
}

Log& Log::Instance()
{
	static Log log;
	return log;
}

void Log::writeError(const LogCategory category, const std::string& text)
{
	writeLine("[ERRR] ", category, text);
}

void Log::writeWarning(const LogCategory category, const std::string& text)
{
	writeLine("[WARN] ", category, text);
}

void Log::writeInfo(const LogCategory category, const std::string& text)
{
	writeLine("[INFO] ", category, text);
}

void Log::writeInit(const LogCategory category, const std::string& text)
{
	writeLine("[INIT] ", category, text);
}

void Log::writeLine(const char* logPrefix, const LogCategory category, const std::string& text)
{
	if (mLogFileStream.is_open())
	{
		const auto now = std::chrono::system_clock::now();
		const auto in_time_t = std::chrono::system_clock::to_time_t(now);

		mLogWriteMutex.lock();
		mLogFileStream << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X [") << std::this_thread::get_id() << "] ";
		mLogFileStream << logPrefix << "[" << category.name << "]: " << text << "\n"
					   << std::flush;
		mLogWriteMutex.unlock();
	}

#ifndef WEB_BUILD
	std::clog << logPrefix << text << "\n";
#else
	std::cout << logPrefix << text << "\n";
#endif
}
