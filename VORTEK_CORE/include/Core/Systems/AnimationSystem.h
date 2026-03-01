#pragma once
#include <sol/sol.hpp>

namespace Vortek::Core::ECS
{
class Registry;
}

namespace Vortek::Rendering
{
class Camera2D;
}

namespace Vortek::Core::Systems
{
class AnimationSystem
{
  public:
	AnimationSystem() = default;
	~AnimationSystem() = default;

	void Update( Vortek::Core::ECS::Registry& registry, Vortek::Rendering::Camera2D& camera );

	static void CreateAnimationSystemLuaBind( sol::state& lua, Vortek::Core::ECS::Registry& registry );
};
} // namespace Vortek::Core::Systems
