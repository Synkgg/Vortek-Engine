#include "AnimationSystem.h"
#include "../ECS/Components/AnimationComponent.h"
#include "../ECS/Components/SpriteComponent.h"
#include "../ECS/Components/TransformComponent.h"
#include "../CoreUtilities/CoreUtilities.h"
#include "../ECS/Registry.h"

#include <SDL.h>

using namespace VORTEK_CORE::ECS;

namespace VORTEK_CORE::Systems
{

	void AnimationSystem::Update(VORTEK_CORE::ECS::Registry& registry, VORTEK_RENDERING::Camera2D& camera)
	{
		auto view = registry.GetRegistry().view<AnimationComponent, SpriteComponent, TransformComponent>();
		if (view.size_hint() < 1)
			return;

		for (auto entity : view)
		{
			const auto& transform = view.get<TransformComponent>(entity);
			auto& sprite = view.get<SpriteComponent>(entity);
			auto& animation = view.get<AnimationComponent>(entity);

			if (!VORTEK_CORE::EntityInView(transform, sprite.width, sprite.height, camera))
				continue;

			if (animation.numFrames <= 0)
				continue;

			// if we are not looped and the current from == num frames, skip
			if (!animation.bLooped && animation.currentFrame >= animation.numFrames - 1)
				continue;

			// Get the current frame
			animation.currentFrame =
				((SDL_GetTicks() - animation.startTime) * animation.frameRate / 1000) % animation.numFrames;

			if (animation.bVertical)
			{
				sprite.uvs.v = animation.currentFrame * sprite.uvs.uv_height;
				sprite.uvs.u = animation.frameOffset * sprite.uvs.uv_width;
			}
			else
			{
				sprite.uvs.u =
					(animation.currentFrame * sprite.uvs.uv_width) + (animation.frameOffset * sprite.uvs.uv_width);
			}
		}
	}
} // namespace VORTEK_CORE::Systems