#pragma once
#include <glm/glm.hpp>
#include <sol/sol.hpp>

namespace VORTEK_CORE::ECS
{
	struct TransformComponent
	{
		glm::vec2 position{ glm::vec2{ 0.f } };

		glm::vec2 localPosition{ 0.f };

		glm::vec2 scale{ 1.f };

		float rotation{ 0.f };

		[[nodiscard]] std::string to_string();

		static void CreateLuaTransformBind(sol::state& lua);
	};
} // namespace VORTEK_CORE::ECS