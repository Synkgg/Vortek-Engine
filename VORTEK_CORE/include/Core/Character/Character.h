#pragma once
#include "Core/ECS/Entity.h"

namespace Vortek::Core::ECS
{
struct PhysicsAttributes;
struct TransformComponent;
struct SpriteComponent;
struct AnimationComponent;
struct BoxColliderComponent;
struct CircleColliderComponent;
} // namespace Vortek::Core::ECS

namespace Vortek::Core
{
class StateMachine;

struct CharacterParams
{
	std::string sName{};
	std::string sGroup{};

	std::optional<std::unique_ptr<Vortek::Core::ECS::AnimationComponent>> animation{ std::nullopt };
	std::optional<std::unique_ptr<Vortek::Core::ECS::SpriteComponent>> sprite{ std::nullopt };
	std::optional<std::unique_ptr<Vortek::Core::ECS::BoxColliderComponent>> boxCollider{ std::nullopt };
	std::optional<std::unique_ptr<Vortek::Core::ECS::CircleColliderComponent>> circleCollider{ std::nullopt };
	std::optional<std::unique_ptr<Vortek::Core::ECS::PhysicsAttributes>> physicsParams{ std::nullopt };
};

class Character : public Vortek::Core::ECS::Entity
{
  public:
	Character( Vortek::Core::ECS::Registry& registry, const CharacterParams& params );
	Character( Vortek::Core::ECS::Registry& registry, entt::entity entity );
	Character( const Vortek::Core::ECS::Entity& entity );
	~Character();

	StateMachine& GetStateMachine();

	Vortek::Core::ECS::TransformComponent& GetTransformComponent();
	Vortek::Core::ECS::SpriteComponent& GetSpriteComponent();
	Vortek::Core::ECS::AnimationComponent& GetAnimationComponent();

	static void CreateCharacterLuaBind( sol::state& lua, Vortek::Core::ECS::Registry& registry );

  protected:
	std::shared_ptr<StateMachine> m_pStateMachine;

	// TODO: Add whatever else might be needed for characters!
};
} // namespace Vortek::Core
