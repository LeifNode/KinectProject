#pragma once

#include <vector>
#include <string>

enum LOG_LEVEL
{
	LOG_MESSAGE = 0,
	LOG_WARNING,
	LOG_ERROR,
	LOG_CRITICAL //Will also cause application to exit
};

class Logger
{
public:
	struct LogEntry
	{
		std::string message;
		std::string tag;
		LOG_LEVEL errorLevel;
		std::string fileName;
		int lineNumber;
	};

	void Log(const char* message, const char* tag, LOG_LEVEL errorLevel, const char* fileName, int lineNumber);

	static Logger& getInstance()
    {
        static Logger instance;

        return instance;
    }

private:
	Logger() {}
	~Logger() {}

	void dispatchLogEvent(const LogEntry& entry);

	std::vector<LogEntry> mMessages;
};

#ifndef LE_LOG
#define LE_LOG(message, tag, errorLevel) \
	(Logger::getInstance().Log(message, tag, errorLevel, __FILE__, __LINE__))
#endif