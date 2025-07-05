#pragma once
#include <sol/sol.hpp>

namespace VORTEK_CORE::ECS
{
class Registry;
}

namespace VORTEK_RENDERING
{
class Camera2D;
}

namespace VORTEK_CORE::Systems
{
class AnimationSystem
{
  public:
	AnimationSystem() = default;
	~AnimationSystem() = default;

	void Update( VORTEK_CORE::ECS::Registry& registry, VORTEK_RENDERING::Camera2D& camera );

	static void CreateAnimationSystemLuaBind( sol::state& lua, VORTEK_CORE::ECS::Registry& registry );
};
} // namespace VORTEK_CORE::Systems
