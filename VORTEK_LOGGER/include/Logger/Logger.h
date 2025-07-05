#pragma once
#include <string>
#include <string_view>
#include <source_location>
#include <vector>
#include <cassert>
#include <mutex>

/*
 * @brief Variadic Macro for logging Information. This macro takes in a string message, followed by the
 * necessary arguments.
 * @param Takes an std::string_view or string in the form of "This is a log value: {0}, and {1}", followed by
 * the arguments
 */
#define VORTEK_LOG( x, ... ) VORTEK_LOGGER::Logger::GetInstance().Log( x __VA_OPT__(, ) __VA_ARGS__ )

/*
 * @brief Variadic Macro for logging warnings. This macro takes in a string message, followed by the
 * necessary arguments.
 * @param Takes an std::string_view or string in the form of "This is a log value: {0}, and {1}", followed by
 * the arguments
 */
#define VORTEK_WARN( x, ... ) VORTEK_LOGGER::Logger::GetInstance().Warn( x __VA_OPT__(, ) __VA_ARGS__ )

/*
 * @brief Variadic Macro for logging Errors. This macro takes in a string message, followed by the
 * necessary arguments.
 * @param Takes an std::string_view or string in the form of "This is a log value: {0}, and {1}", followed by
 * the arguments
 */
#define VORTEK_ERROR( x, ... )                                                                                          \
	VORTEK_LOGGER::Logger::GetInstance().Error( std::source_location::current(), x __VA_OPT__(, ) __VA_ARGS__ )

#define VORTEK_ASSERT( x ) assert( x )
#define VORTEK_INIT_LOGS( console, retain ) VORTEK_LOGGER::Logger::GetInstance().Init( console, retain )

#define VORTEK_GET_LOGS() VORTEK_LOGGER::Logger::GetInstance().GetLogs()
#define VORTEK_CLEAR_LOGS() VORTEK_LOGGER::Logger::GetInstance().ClearLogs()

namespace VORTEK_LOGGER
{
struct LogEntry
{
	enum class LogType
	{
		INFO,
		WARN,
		ERR,
		NONE
	};
	LogType type{ LogType::INFO };
	std::string log{};
};

class Logger
{
  public:
	static Logger& GetInstance();

	~Logger() = default;
	// Make the logger non-copyable
	Logger( const Logger& ) = delete;
	Logger& operator=( const Logger& ) = delete;

	void Init( bool consoleLog = true, bool retainLogs = true );

	template <typename... Args>
	void Log( const std::string_view message, Args&&... args );

	template <typename... Args>
	void Warn( const std::string_view message, Args&&... args );

	template <typename... Args>
	void Error( std::source_location location, const std::string_view message, Args&&... args );

	template <typename... Args>
	void Error( const std::string_view message, Args&&... args );

	void LuaLog( const std::string_view message );
	void LuaWarn( const std::string_view message );
	void LuaError( const std::string_view message );

	inline void ClearLogs() { m_LogEntries.clear(); }
	inline const std::vector<LogEntry>& GetLogs() { return m_LogEntries; }
	
  private:
	std::mutex m_Mutex;
	std::vector<LogEntry> m_LogEntries;
	bool m_bInitialized{ false };
	bool m_bConsoleLog{ true };
	bool m_bRetainLogs{ true };

	Logger() = default;

	struct LogTime
	{
		std::string day, dayNumber, month, year, time;
		LogTime( const std::string& date );
	};

	std::string CurrentDateTime();
};
} // namespace VORTEK_LOGGER

#include "Logger.inl"
