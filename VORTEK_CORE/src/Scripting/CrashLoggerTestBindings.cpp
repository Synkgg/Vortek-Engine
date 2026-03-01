#include "Core/Scripting/CrashLoggerTestBindings.h"
#include "Core/ECS/MainRegistry.h"
#include "Logger/CrashLogger.h"

namespace Vortek::Core::Scripting
{
static void FunctionC()
{
	// Crash happens here
	int* ptr{ nullptr };
	*ptr = 194;
}

static void FunctionB()
{
	FunctionC();
}

static void FunctionA()
{
	FunctionB();
}

// Bind this function to Lua
static void CrashFromLua()
{
	FunctionA();
}

void CrashLoggerTests::CreateLuaBind( sol::state& lua )
{
	lua_State* L = lua.lua_state();
	VORTEK_CRASH_LOGGER().SetLuaState( L );

	lua.set_function( "CrashLoggerTest", &CrashFromLua );

	lua.script( R"(
        function CrashTest_level5()
            print("Entering Level 5...")
			-- Calls the C++ function that crashes
            CrashLoggerTest()  
        end

        function CrashTest_level4()
            print("Entering Level 4...")
            CrashTest_level5()
        end

        function CrashTest_level3()
            print("Entering Level 3...")
            CrashTest_level4()
        end

        function CrashTest_level2()
            print("Entering Level 2...")
            CrashTest_level3()
        end

        function CrashTest_level1()
            print("Entering Level 1...")
            CrashTest_level2()
        end

        function CrashTest_Start()
            print("Starting Lua Crash Test Execution...")
            CrashTest_level1()
        end
    )" );
}
} // namespace Vortek::Core::Scripting
