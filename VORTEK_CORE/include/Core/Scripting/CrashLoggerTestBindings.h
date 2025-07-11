#pragma once
#include <sol/sol.hpp>

namespace VORTEK_CORE::Scripting
{
/*
 * Simple test to verify that the crash logger is working.
 * Allows the user to crash the program from a lua function.
 * It should log the crash position and the lua stack.
 */
struct CrashLoggerTests
{
	static void CreateLuaBind( sol::state& lua );
};
} // namespace VORTEK_CORE::Scripting
