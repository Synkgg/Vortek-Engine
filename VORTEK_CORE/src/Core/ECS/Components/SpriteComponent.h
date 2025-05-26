#pragma once
#include <Rendering/Essentials/Vertex.h>
#include <sol/sol.hpp>
#include "../../ECS/Registry.h"

namespace VORTEK_CORE::ECS
{
	struct UVs
	{
		float u{ 0.f }, v{ 0.f }, uv_width{ 0.f }, uv_height{ 0.f };
	};

	struct SpriteComponent
	{
		float width{ 16.f }, height{ 16.f };
		UVs uvs{ .u = 0.f, .v = 0.f, .uv_width = 0.f, .uv_height = 0.f };

		VORTEK_RENDERING::Color color{ .r = 255, .g = 255, .b = 255, .a = 255 };
		int start_x{ 0 }, start_y{ 0 }, layer{ 0 };

		bool bHidden{ false };
		std::string texture_name{ "" };

		//void generate_uvs( int textureWidth, int textureHeight );
		[[nodiscard]] std::string to_string() const;

		static void CreateSpriteLuaBind(sol::state& lua);
	};
} // namespace VORTEK_CORE::ECS