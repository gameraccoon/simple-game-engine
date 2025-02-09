#include "EngineCommon/precomp.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <thread>

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
	writeInfo("Log file created");
}

Log::~Log()
{
	writeInfo("End of log");
	mLogFileStream.close();
}

Log& Log::Instance()
{
	static Log log;
	return log;
}

void Log::writeError(const std::string& text)
{
	writeLine("[ERR]: ", text);
}

void Log::writeWarning(const std::string& text)
{
	writeLine("[WARN]: ", text);
}

void Log::writeInfo(const std::string& text)
{
	writeLine("[INFO]: ", text);
}

void Log::writeInit(const std::string& text)
{
	writeLine("[INIT]: ", text);
}

void Log::writeLine(const char* logPrefix, const std::string& text)
{
	if (mLogFileStream.is_open())
	{
		const auto now = std::chrono::system_clock::now();
		const auto in_time_t = std::chrono::system_clock::to_time_t(now);

		mLogWriteMutex.lock();
		mLogFileStream << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X [") << std::this_thread::get_id() << "] ";
		mLogFileStream << logPrefix << text << "\n"
					   << std::flush;
		mLogWriteMutex.unlock();
	}

#ifndef WEB_BUILD
	std::clog << logPrefix << text << "\n";
#else
	std::cout << logPrefix << text << "\n";
#endif
}
