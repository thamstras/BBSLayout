#pragma once
#include <string>

enum LogChannel
{

};

enum LogLevel
{

};

class LoggerSettings
{
public:
	LoggerSettings& RegisterChannel(LogChannel channel, std::string name, LogLevel initialLevel);
};

class Logger
{
public:
	static void InitLogger(LoggerSettings settings);
	static void Log(LogChannel channel, LogLevel level, std::string log);
};