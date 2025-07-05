#pragma once
#include <sol/sol.hpp>

namespace VORTEK_CORE::Scripting
{
struct LuaFilesystem
{
	static void CreateLuaFileSystemBind( sol::state& lua );
};
} // namespace VORTEK_CORE::Scripting
