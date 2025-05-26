#pragma once
#include <sol/sol.hpp>

namespace VORTEK_CORE { namespace ECS { class Registry; } }

namespace VORTEK_CORE::Scripting {
	struct RendererBinder
	{
		static void CreateRenderingBind(sol::state& lua, VORTEK_CORE::ECS::Registry& registry);
	};
}