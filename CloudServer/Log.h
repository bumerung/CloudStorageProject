#pragma once
#include "spdlog/spdlog.h"
#include <string>
#include <memory>

class Logger
{
public:
	static Logger& GetServerLogger();
	static Logger& GetClientLogger();
public:;
	static bool SetPattern(const char* pattern);
public:
	Logger(const char* name);
	Logger(const std::string& name);
	~Logger();
public:
	template<typename tystring, typename ...args>
	bool Trace(const tystring& log, args&&...);
	bool Trace(const std::string& log);

	template<typename tystring, typename ...args>
	bool Debug(const tystring& log, args&&...);
	bool Debug(const std::string& log);

	template<typename tystring, typename ...args>
	bool Info(const tystring& log, args&&...);
	bool Info(const std::string& log);

	template<typename tystring, typename ...args>
	bool Warn(const tystring& log, args&&...);
	bool Warn(const std::string& log);

	template<typename tystring, typename ...args>
	bool Error(const tystring& log, args&&...);
	bool Error(const std::string& log);

	template<typename tystring, typename ...args>
	bool Critical(const tystring& log, args&&...);
	bool Critical(const std::string& log);
public:
	bool SetLevel(spdlog::level::level_enum level);
private:
	std::shared_ptr<spdlog::logger> m_Logger;
};
template<typename tystring, typename ...args>
inline bool Logger::Trace(const tystring& log, args && ...vargs)
{
	m_Logger->trace(log, std::forward<args>(vargs)...);
	return true;
}

template<typename tystring, typename ...args>
inline bool Logger::Debug(const tystring& log, args && ...vargs)
{
	m_Logger->debug(log, std::forward<args>(vargs)...);
	return true;
}

template<typename tystring, typename ...args>
inline bool Logger::Info(const tystring& log, args && ... vargs)
{
	m_Logger->info(log, std::forward<args>(vargs)...);
	return true;
}

template<typename tystring, typename ...args>
inline bool Logger::Warn(const tystring& log, args && ...vargs)
{
	m_Logger->warn(log, std::forward<args>(vargs)...);
	return true;
}

template<typename tystring, typename ...args>
inline bool Logger::Error(const tystring& log, args && ...vargs)
{
	m_Logger->error(log, std::forward<args>(vargs)...);
	return true;
}

template<typename tystring, typename ...args>
inline bool Logger::Critical(const tystring& log, args && ...vargs)
{
	m_Logger->critical(log, std::forward<args>(vargs)...);
	return true;
}


#define SERVER_TRACE(...)		::Logger::GetServerLogger().Trace(__VA_ARGS__)
#define SERVER_DEBUG(...)		::Logger::GetServerLogger().Debug(__VA_ARGS__)
#define SERVER_INFO(...)		::Logger::GetServerLogger().Info(__VA_ARGS__)
#define SERVER_WARN(...)		::Logger::GetServerLogger().Warn(__VA_ARGS__)
#define SERVER_ERROR(...)		::Logger::GetServerLogger().Error(__VA_ARGS__)
#define SERVER_CRITICAL(...)	::Logger::GetServerLogger().Critical(__VA_ARGS__)

//App related logs
#define CLIENT_TRACE(...)		::Logger::GetClientLogger().Trace(__VA_ARGS__)
#define CLIENT_DEBUG(...)		::Logger::GetClientLogger().Debug(__VA_ARGS__)
#define CLIENT_INFO(...)		::Logger::GetClientLogger().Info(__VA_ARGS__)
#define CLIENT_WARN(...)		::Logger::GetClientLogger().Warn(__VA_ARGS__)
#define CLIENT_ERROR(...)		::Logger::GetClientLogger().Error(__VA_ARGS__)
#define CLIENT_CRITICAL(...)	::Logger::GetClientLogger().Critical(__VA_ARGS__)