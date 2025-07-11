#pragma once
#include <sol/sol.hpp>

namespace VORTEK_CORE::ECS
{
struct ScriptComponent
{
	sol::protected_function update{ sol::lua_nil };
	sol::protected_function render{ sol::lua_nil };
};
} // namespace VORTEK_CORE::ECS
