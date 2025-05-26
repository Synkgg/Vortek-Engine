#pragma once
#include "../ECS/Components/AllComponents.h"
#include <Rendering/Core/Camera2D.h>

namespace VORTEK_CORE
{
	bool EntityInView(const VORTEK_CORE::ECS::TransformComponent& transform, float width, float height,
		const VORTEK_RENDERING::Camera2D& camera);
	glm::mat4 RSTModel(const VORTEK_CORE::ECS::TransformComponent& transform, float width, float height);

	void GenerateUVs(VORTEK_CORE::ECS::SpriteComponent& sprite, int textureWidth, int textureHeight);

} // namespace VORTEK_CORE