#pragma once

namespace VORTEK_CORE::ECS
{
class Registry;
}

namespace VORTEK_CORE::Systems
{
class PhysicsSystem
{
  public:
	PhysicsSystem();
	~PhysicsSystem() = default;

	void Update( VORTEK_CORE::ECS::Registry& registry );
};
} // namespace VORTEK_CORE::Systems
