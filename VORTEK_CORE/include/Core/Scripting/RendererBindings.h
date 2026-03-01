#pragma once
#include <sol/sol.hpp>

namespace Vortek::Core
{
namespace ECS
{
class Registry;
}
} // namespace Vortek::Core

namespace Vortek::Core::Scripting
{
struct RendererBinder
{
	static void CreateRenderingBind( sol::state& lua, Vortek::Core::ECS::Registry& registry );
};
} // namespace Vortek::Core::Scripting
