#pragma once

#include <fstream>
#include <mutex>
#include <string>

#include "EngineCommon/Debug/LogCategory.h"
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
	void writeError(LogCategory category, const std::string& text);
	void writeWarning(LogCategory category, const std::string& text);
	void writeInfo(LogCategory category, const std::string& text);
	void writeInit(LogCategory category, const std::string& text);

private:
	/** Write line with timestamp */
	void writeLine(const char* logPrefix, LogCategory category, const std::string& text);

	/** Filestream that holds the logfile handler */
	std::ofstream mLogFileStream;
	std::mutex mLogWriteMutex;

	/* Turn off unusable operations */
	Log();
	~Log();
};

template<typename... Args>
void LogInit(const LogCategory category, const std::string& message, Args&&... args) noexcept
{
	Log::Instance().writeInit(category, FormatString(message, std::forward<Args>(args)...));
}

template<typename... Args>
void LogInfo(const LogCategory category, const std::string& message, Args&&... args) noexcept
{
	Log::Instance().writeInfo(category, FormatString(message, std::forward<Args>(args)...));
}

template<typename... Args>
void LogWarning(const LogCategory category, const std::string& message, Args&&... args) noexcept
{
	Log::Instance().writeWarning(category, FormatString(message, std::forward<Args>(args)...));
}

template<typename... Args>
void LogError(const LogCategory category, const std::string& message, Args&&... args) noexcept
{
	Log::Instance().writeError(category, FormatString(message, std::forward<Args>(args)...));
}
