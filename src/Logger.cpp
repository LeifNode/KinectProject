#include "Logger.h"

void Logger::Log(const char* message, const char* tag, LOG_LEVEL errorLevel, const char* fileName, int lineNumber)
{
	LogEntry entry;
	entry.message = message;
	entry.tag = tag;
	entry.errorLevel = errorLevel;
	entry.fileName = fileName;
	entry.lineNumber = lineNumber;

	mMessages.push_back(entry);
}