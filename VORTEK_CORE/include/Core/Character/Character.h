#pragma once
#include "Core/ECS/Entity.h"
#include <optional>

namespace VORTEK_CORE::ECS
{
struct PhysicsAttributes;
struct TransformComponent;
struct SpriteComponent;
struct AnimationComponent;
struct BoxColliderComponent;
struct CircleColliderComponent;
} // namespace VORTEK_CORE::ECS

namespace VORTEK_CORE
{
class StateMachine;

struct CharacterParams
{
	std::string sName{};
	std::string sGroup{};

	std::optional<std::unique_ptr<VORTEK_CORE::ECS::AnimationComponent>> animation{ std::nullopt };
	std::optional<std::unique_ptr<VORTEK_CORE::ECS::SpriteComponent>> sprite{ std::nullopt };
	std::optional<std::unique_ptr<VORTEK_CORE::ECS::BoxColliderComponent>> boxCollider{ std::nullopt };
	std::optional<std::unique_ptr<VORTEK_CORE::ECS::CircleColliderComponent>> circleCollider{ std::nullopt };
	std::optional<std::unique_ptr<VORTEK_CORE::ECS::PhysicsAttributes>> physicsParams{ std::nullopt };
};

class Character : public VORTEK_CORE::ECS::Entity
{
  public:
	Character( VORTEK_CORE::ECS::Registry& registry, const CharacterParams& params );
	Character( VORTEK_CORE::ECS::Registry& registry, entt::entity entity );
	Character( const VORTEK_CORE::ECS::Entity& entity );
	~Character();

	StateMachine& GetStateMachine();

	VORTEK_CORE::ECS::TransformComponent& GetTransformComponent();
	VORTEK_CORE::ECS::SpriteComponent& GetSpriteComponent();
	VORTEK_CORE::ECS::AnimationComponent& GetAnimationComponent();

	static void CreateCharacterLuaBind( sol::state& lua, VORTEK_CORE::ECS::Registry& registry );

  protected:
	std::shared_ptr<StateMachine> m_pStateMachine;

	// TODO: Add whatever else might be needed for characters!
};
} // namespace VORTEK_CORE
