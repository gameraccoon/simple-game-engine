#pragma once

#include <fstream>
#include <mutex>
#include <string>

#include "EngineCommon/Types/String/StringHelpers.h"

/**
 * Class that helps to write log messages
 */
class Log
{
public:
	Log(const Log&) = delete;
	void operator=(const Log&) = delete;

	/**
	 * Return single instance of class Log
	 * Usage: Log::Instance().function(...);
	 */
	static Log& Instance();

	/* Logging functions */
	void writeError(const std::string& text);
	void writeWarning(const std::string& text);
	void writeInfo(const std::string& text);
	void writeInit(const std::string& text);

private:
	/** Write line with timestamp */
	void writeLine(const char* logPrefix, const std::string& text);

	/** Filestream that holds the logfile handler */
	std::ofstream mLogFileStream;
	std::mutex mLogWriteMutex;

	/* Turn off unusable operations */
	Log();
	~Log();
};

template<typename... Args>
void LogInit(const std::string& message, Args&&... args) noexcept
{
	Log::Instance().writeInit(FormatString(message, std::forward<Args>(args)...));
}

template<typename... Args>
void LogInfo(const std::string& message, Args&&... args) noexcept
{
	Log::Instance().writeInfo(FormatString(message, std::forward<Args>(args)...));
}

template<typename... Args>
void LogWarning(const std::string& message, Args&&... args) noexcept
{
	Log::Instance().writeWarning(FormatString(message, std::forward<Args>(args)...));
}

template<typename... Args>
void LogError(const std::string& message, Args&&... args) noexcept
{
	Log::Instance().writeError(FormatString(message, std::forward<Args>(args)...));
}
