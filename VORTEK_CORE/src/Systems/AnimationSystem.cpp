#include "Core/Systems/AnimationSystem.h"
#include "Core/ECS/Components/AnimationComponent.h"
#include "Core/ECS/Components/SpriteComponent.h"
#include "Core/ECS/Components/TransformComponent.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/ECS/Registry.h"

#include "Logger/Logger.h"

#include <SDL.h>

using namespace VORTEK_CORE::ECS;
using namespace VORTEK_RENDERING;

namespace VORTEK_CORE::Systems
{

void AnimationSystem::Update( VORTEK_CORE::ECS::Registry& registry, VORTEK_RENDERING::Camera2D& camera )
{

	if ( !CORE_GLOBALS().AnimationRenderEnabled() )
		return;

	auto view = registry.GetRegistry().view<AnimationComponent, SpriteComponent, TransformComponent>();
	if ( view.size_hint() < 1 )
		return;

	for ( auto entity : view )
	{
		const auto& transform = view.get<TransformComponent>( entity );
		auto& sprite = view.get<SpriteComponent>( entity );
		auto& animation = view.get<AnimationComponent>( entity );

		// We don't want to check if entities with UIComponents are out of the camera.
		// Since they use a different camera.
		if ( !registry.GetRegistry().all_of<UIComponent>( entity ) &&
			 !VORTEK_CORE::EntityInView( transform, sprite.width, sprite.height, camera ) )
			continue;

		if ( animation.numFrames <= 0 )
			continue;

		// if we are not looped and the current from == num frames, skip
		if ( !animation.bLooped && animation.currentFrame >= animation.numFrames - 1 )
			continue;

		// Get the current frame
		animation.currentFrame =
			( ( SDL_GetTicks() - animation.startTime ) * animation.frameRate / 1000 ) % animation.numFrames;

		if ( animation.bVertical )
		{
			sprite.uvs.v = animation.currentFrame * sprite.uvs.uv_height;
			sprite.uvs.u = animation.frameOffset * sprite.uvs.uv_width;
		}
		else
		{
			sprite.uvs.u =
				( animation.currentFrame * sprite.uvs.uv_width ) + ( animation.frameOffset * sprite.uvs.uv_width );
		}
	}
}

void AnimationSystem::CreateAnimationSystemLuaBind( sol::state& lua, VORTEK_CORE::ECS::Registry& registry )
{
	auto& pCamera = registry.GetContext<std::shared_ptr<Camera2D>>();

	VORTEK_ASSERT( pCamera && "A camera must exist in the current scene!" );

	lua.new_usertype<AnimationSystem>(
		"AnimationSystem",
		sol::call_constructor,
		sol::constructors<AnimationSystem()>(),
		"update",
		[ & ]( AnimationSystem& system, Registry& reg ) { system.Update( reg, *pCamera ); } );
}
} // namespace VORTEK_CORE::Systems
