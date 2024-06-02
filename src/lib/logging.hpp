#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>

// Standard LOG(msg) is INFO
#define LOG(msg) Logger::log(__FILE__, __LINE__, msg)
#define LOG_INFO(msg) Logger::log(__FILE__, __LINE__, msg, LogLevel::INFO)
#define LOG_WARNING(msg) Logger::log(__FILE__, __LINE__, msg, LogLevel::WARNING)
#define LOG_ERROR(msg) Logger::log(__FILE__, __LINE__, msg, LogLevel::ERROR)

enum class LogLevel
{
    INFO,
    WARNING,
    ERROR
};

class Logger
{
public:
    static void log(const char *file, int line, const std::string &message, LogLevel level = LogLevel::INFO)
    {
        switch (level)
        {
        case LogLevel::INFO:
            std::cout << "[INFO] [" << file << "] in line [" << line << "]: " << message << std::endl;
            break;
        case LogLevel::WARNING:
            std::cout << "[WARNING] [" << file << "] in line [" << line << "]: " << message << std::endl;
            break;
        case LogLevel::ERROR:
            std::cout << "[ERROR] [" << file << "] in line [" << line << "]: " << message << std::endl;
            break;
        }
    }
};

#endif // LOGGER_H