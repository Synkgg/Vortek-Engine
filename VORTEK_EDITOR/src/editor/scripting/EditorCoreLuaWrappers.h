#pragma once
#include <sol/sol.hpp>

namespace VORTEK_CORE
{
namespace ECS
{
class Registry;
}
} // namespace VORTEK_CORE

namespace VORTEK_EDITOR
{
struct LuaCoreBinder
{
	static void CreateLuaBind( sol::state& lua, VORTEK_CORE::ECS::Registry& registry );
};
} // namespace VORTEK_EDITOR
