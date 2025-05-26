#pragma once
#include "Logger.h"
#include <chrono>
#include <ctime>
#include <format>
#include <iostream>
#include <Windows.h>
#include <sstream>

constexpr WORD  GREEN = 2;
constexpr WORD  RED = 4;
constexpr WORD  YELLOW = 6;
constexpr WORD  WHITE = 7;

namespace VORTEK_LOGGER {
    template <typename... Args>
    void Logger::Log(const std::string_view& message, Args&&... args)
    {
        assert(m_bInitialized && "The logger must be initialized before it is used!");

        if (!m_bInitialized)
        {
            std::cout << "The logger must be initialized before it is used!" << std::endl;
            return;
        }

        std::stringstream ss;
        ss << "VORTEK [INFO]: " << CurrentDateTime() << " - " << std::vformat(message, std::make_format_args(args...)) << "\n";

        if (m_bConsoleLog)
        {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, GREEN);
            std::cout << ss.str();
            SetConsoleTextAttribute(hConsole, WHITE);
        }

        if (m_bRetainLogs)
        {
            m_LogEntries.emplace_back(LogEntry::LogType::INFO, ss.str());
            m_bLogAdded = true;
        }
    }

    template <typename... Args>
    void Logger::Warn(const std::string_view& message, Args&&... args)
    {

        assert(m_bInitialized && "The logger must be initialized before it is used!");

        if (!m_bInitialized)
        {
            std::cout << "The logger must be initialized before it is used!" << std::endl;
            return;
        }

        std::stringstream ss;
        ss << "VORTEK [WARN]: " << CurrentDateTime() << " - " << std::vformat(message, std::make_format_args(args...)) << "\n";

        if (m_bConsoleLog)
        {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, YELLOW);
            std::cout << ss.str();
            SetConsoleTextAttribute(hConsole, WHITE);
        }

        if (m_bRetainLogs)
        {
            m_LogEntries.emplace_back(LogEntry::LogType::WARN, ss.str());
            m_bLogAdded = true;
        }
    }

    template <typename... Args>
    void Logger::Error(std::source_location location, const std::string_view& message, Args&&... args)
    {
        assert(m_bInitialized && "The logger must be initialized before it is used!");

        if (!m_bInitialized)
        {
            std::cout << "The logger must be initialized before it is used!" << std::endl;
            return;
        }

        std::stringstream ss;
        ss << "VORTEK [ERROR]: " << CurrentDateTime() << " - " << std::vformat(message, std::make_format_args(args...)) <<
            "\nFUNC: " << location.function_name() <<
            "\nLINE: " << location.line() << "\n\n";

        if (m_bConsoleLog)
        {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, RED);
            std::cout << ss.str();
            SetConsoleTextAttribute(hConsole, WHITE);
        }

        if (m_bRetainLogs)
        {
            m_LogEntries.emplace_back(LogEntry::LogType::ERR, ss.str());
            m_bLogAdded = true;
        }
    }
}