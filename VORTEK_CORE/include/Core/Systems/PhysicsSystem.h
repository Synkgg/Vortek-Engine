#pragma once

namespace Vortek::Core::ECS
{
class Registry;
}

namespace Vortek::Core::Systems
{
class PhysicsSystem
{
  public:
	PhysicsSystem();
	~PhysicsSystem() = default;

	void Update( Vortek::Core::ECS::Registry& registry );
};
} // namespace Vortek::Core::Systems
