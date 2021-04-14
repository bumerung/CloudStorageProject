#include "Log.h"
#include "spdlog//sinks/stdout_color_sinks.h"

Logger& Logger::GetServerLogger()
{
    static Logger logger("SERVER");
    return logger;
}

Logger& Logger::GetClientLogger()
{
    static Logger logger("CLIENT");
    return logger;
}

bool Logger::SetPattern(const char* pattern)
{
    spdlog::set_pattern(pattern);
    return true;
}

Logger::Logger(const char* name)
{
    m_Logger = spdlog::stdout_color_mt(name);
    SetLevel(spdlog::level::trace);
}
Logger::Logger(const std::string& name)
{
    m_Logger = spdlog::stdout_color_mt(name);
}
Logger::~Logger()
{
}

bool Logger::Trace(const std::string& log)
{
    m_Logger->trace(log.c_str());
    return true;
}

bool Logger::Debug(const std::string& log)
{
    m_Logger->debug(log.c_str());
    return true;
}

bool Logger::Info(const std::string& log)
{
    m_Logger->info(log.c_str());
    return true;
}

bool Logger::Warn(const std::string& log)
{
    m_Logger->warn(log.c_str());
    return true;
}

bool Logger::Error(const std::string& log)
{
    m_Logger->error(log.c_str());
    return true;
}

bool Logger::Critical(const std::string& log)
{
    m_Logger->critical(log.c_str());
    return true;
}

bool Logger::SetLevel(spdlog::level::level_enum level)
{
    m_Logger->set_level(level);
    return true;
}