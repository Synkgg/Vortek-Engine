#pragma once
#include <sol/sol.hpp>

namespace Vortek::Core
{
namespace ECS
{
class Registry;
}
} // namespace Vortek::Core

namespace Vortek::Editor
{
struct LuaCoreBinder
{
	static void CreateLuaBind( sol::state& lua, Vortek::Core::ECS::Registry& registry );
};
} // namespace Vortek::Editor
