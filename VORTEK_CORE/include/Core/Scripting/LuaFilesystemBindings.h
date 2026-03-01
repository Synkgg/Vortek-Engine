#pragma once
#include <sol/sol.hpp>

namespace Vortek::Core::Scripting
{
struct LuaFilesystem
{
	static void CreateLuaFileSystemBind( sol::state& lua );
};
} // namespace Vortek::Core::Scripting
