#pragma once
#include <sol/sol.hpp>
#include <Rendering/Essentials/Vertex.h>

namespace VORTEK_CORE::ECS
{
	struct TextComponent
	{
		std::string sFontName{ "SourceSans" }, sTextStr{ "" };
		int padding{ 0 };
		float wrap{ -1.f };
		VORTEK_RENDERING::Color color{ 255, 255, 255, 255 };
		bool bHidden{ false };

		[[nodiscard]] std::string to_string();

		static void CreateLuaTextBindings(sol::state& lua);
	};
} // namespace VORTEK_CORE::ECS