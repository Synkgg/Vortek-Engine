#include "Logger/Logger.h"

#include <chrono>

namespace VORTEK_LOGGER
{

Logger::LogTime::LogTime( const std::string& date )
	: day{ date.substr( 0, 3 ) }
	, dayNumber{ date.substr( 8, 2 ) }
	, month{ date.substr( 4, 3 ) }
	, year{ date.substr( 20, 4 ) }
	, time{ date.substr( 11, 8 ) }
{
	// Ensure that the day number does not contain a space
	if ( !dayNumber.empty() && std::isspace( dayNumber[ 0 ] ) )
	{
		dayNumber[ 0 ] = '0';
	}
}

std::string Logger::CurrentDateTime()
{
	auto time = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );

	char buf[ 30 ];
#ifdef _WIN32
	ctime_s( buf, sizeof( buf ), &time );
#else
	ctime_r( &time, buf );
#endif
	LogTime logTime{ std::string{ buf } };
	return std::format( "{0}-{1}-{2} {3}", logTime.year, logTime.month, logTime.dayNumber, logTime.time );
}

Logger& Logger::GetInstance()
{
	static Logger instance{};
	return instance;
}

void Logger::Init( bool consoleLog, bool retainLogs )
{
	assert( !m_bInitialized && "Don not call Initialize more than once!" );

	if ( m_bInitialized )
	{
		std::cout << "Logger has already been initialized!" << std::endl;
		return;
	}

	m_bConsoleLog = consoleLog;
	m_bRetainLogs = retainLogs;
	m_bInitialized = true;
}

void Logger::LuaLog( const std::string_view message )
{
	std::scoped_lock lock{ m_Mutex };
	assert( m_bInitialized && "The logger must be initialized before it is used!" );

	if ( !m_bInitialized )
	{
		std::cout << "The logger must be initialized before it is used!" << std::endl;
		return;
	}

	std::stringstream ss;
	ss << "LUA [INFO]: " << CurrentDateTime() << " - " << message << "\n";

	if ( m_bConsoleLog )
	{
#ifdef _WIN32
		HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
		SetConsoleTextAttribute( hConsole, WHITE );
		std::cout << ss.str();
		SetConsoleTextAttribute( hConsole, WHITE );
#else
		std::cout << WHITE << ss.str() << CLOSE << "\n\n";
#endif
	}

	if ( m_bRetainLogs )
	{
		m_LogEntries.emplace_back( LogEntry::LogType::INFO, ss.str() );
	}
}

void Logger::LuaWarn( const std::string_view message )
{
	std::scoped_lock lock{ m_Mutex };
	assert( m_bInitialized && "The logger must be initialized before it is used!" );

	if ( !m_bInitialized )
	{
		std::cout << "The logger must be initialized before it is used!" << std::endl;
		return;
	}

	std::stringstream ss;
	ss << "LUA [WARN]: " << CurrentDateTime() << " - " << message << "\n";

	if ( m_bConsoleLog )
	{
#ifdef _WIN32
		HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
		SetConsoleTextAttribute( hConsole, YELLOW );
		std::cout << ss.str();
		SetConsoleTextAttribute( hConsole, WHITE );
#else
		std::cout << YELLOW << ss.str() << CLOSE << "\n\n";
#endif
	}

	if ( m_bRetainLogs )
	{
		m_LogEntries.emplace_back( LogEntry::LogType::WARN, ss.str() );
	}
}

void Logger::LuaError( const std::string_view message )
{
	std::scoped_lock lock{ m_Mutex };
	assert( m_bInitialized && "The logger must be initialized before it is used!" );

	if ( !m_bInitialized )
	{
		std::cout << "The logger must be initialized before it is used!" << std::endl;
		return;
	}

	std::stringstream ss;
	ss << "LUA [ERROR]: " << CurrentDateTime() << " - " << message << "\n";

	if ( m_bConsoleLog )
	{
#ifdef _WIN32
		HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
		SetConsoleTextAttribute( hConsole, RED );
		std::cout << ss.str();
		SetConsoleTextAttribute( hConsole, WHITE );
#else
		std::cout << RED << ss.str() << CLOSE << "\n\n";
#endif
	}

	if ( m_bRetainLogs )
	{
		m_LogEntries.emplace_back( LogEntry::LogType::ERR, ss.str() );
	}
}
} // namespace VORTEK_LOGGER
