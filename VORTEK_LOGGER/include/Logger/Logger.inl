#pragma once
#include "Logger.h"
#include <fmt/format.h>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <Windows.h>
constexpr WORD GREEN = 2;
constexpr WORD RED = 4;
constexpr WORD YELLOW = 6;
constexpr WORD WHITE = 7;
#else
static const char* GREEN = "\033[0;32m";
static const char* YELLOW = "\033[0;33m";
static const char* RED = "\033[0;31m";
static const char* WHITE = "\033[0;30m";
static const char* CLOSE = "\022[0m";
#endif

namespace VORTEK_LOGGER
{
template <typename... Args>
void Logger::Log( const std::string_view message, Args&&... args )
{
	std::scoped_lock lock{ m_Mutex };
	assert( m_bInitialized && "The logger must be initialized before it is used!" );

	if ( !m_bInitialized )
	{
		std::cout << "The logger must be initialized before it is used!" << std::endl;
		return;
	}

	std::stringstream ss;
	ss << "VORTEK [INFO]: " << CurrentDateTime() << " - " << fmt::vformat( message, fmt::make_format_args( args... ) );

	if ( m_bConsoleLog )
	{
#ifdef _WIN32
		HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
		SetConsoleTextAttribute( hConsole, WHITE );
		std::cout << ss.str() << "\n";
		SetConsoleTextAttribute( hConsole, WHITE );
#else
		std::cout << WHITE << ss.str() << CLOSE << "\n";
#endif
	}

	if ( m_bRetainLogs )
	{
		m_LogEntries.emplace_back( LogEntry::LogType::INFO, ss.str() );
	}
}

template <typename... Args>
void Logger::Warn( const std::string_view message, Args&&... args )
{
	std::scoped_lock lock{ m_Mutex };
	assert( m_bInitialized && "The logger must be initialized before it is used!" );

	if ( !m_bInitialized )
	{
		std::cout << "The logger must be initialized before it is used!" << std::endl;
		return;
	}

	std::stringstream ss;
	ss << "VORTEK [WARN]: " << CurrentDateTime() << " - " << fmt::vformat( message, fmt::make_format_args( args... ) );

	if ( m_bConsoleLog )
	{
#ifdef _WIN32
		HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
		SetConsoleTextAttribute( hConsole, YELLOW );
		std::cout << ss.str() << "\n";
		SetConsoleTextAttribute( hConsole, WHITE );
#else
		std::cout << YELLOW << ss.str() << CLOSE << "\n";
#endif
	}

	if ( m_bRetainLogs )
	{
		m_LogEntries.emplace_back( LogEntry::LogType::WARN, ss.str() );
	}
}

template <typename... Args>
void Logger::Error( std::source_location location, const std::string_view message, Args&&... args )
{
	std::scoped_lock lock{ m_Mutex };
	assert( m_bInitialized && "The logger must be initialized before it is used!" );

	if ( !m_bInitialized )
	{
		std::cout << "The logger must be initialized before it is used!" << std::endl;
		return;
	}

	std::stringstream ss;
	ss << "VORTEK [ERROR]: " << CurrentDateTime() << " - " << fmt::vformat( message, fmt::make_format_args( args... ) )
	   << "\nFUNC: " << location.function_name() << "\nLINE: " << location.line();

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

template <typename... Args>
void Logger::Error( const std::string_view message, Args&&... args )
{
	std::scoped_lock lock{ m_Mutex };
	assert( m_bInitialized && "The logger must be initialized before it is used!" );

	if ( !m_bInitialized )
	{
		std::cout << "The logger must be initialized before it is used!" << std::endl;
		return;
	}

	std::stringstream ss;
	ss << "VORTEK [ERROR]: " << CurrentDateTime() << " - " << fmt::vformat( message, fmt::make_format_args( args... ) );

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
