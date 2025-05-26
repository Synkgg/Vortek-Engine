#pragma once
#include <sol/sol.hpp>
namespace VORTEK_CORE { namespace ECS { class Registry; } }

namespace VORTEK_CORE::Scripting {
	struct SoundBinder
	{
		static void CreateSoundBind(sol::state& lua);
	};
}