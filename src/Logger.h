#pragma once

#include "d3dStd.h"

enum LOG_LEVEL
{
	LOG_MESSAGE = 0,
	LOG_WARNING,
	LOG_ERROR,
	LOG_CRITICAL
};

class Logger
{

public:
	void Log(const std::string& message, const std::string& tag, LOG_LEVEL errorLevel, )

private:
	Logger();
	~Logger();

	static Logger* spLogger;
};