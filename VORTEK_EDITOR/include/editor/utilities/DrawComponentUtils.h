#pragma once
#include "Core/ECS/Components/AllComponents.h"

namespace Vortek::Core::ECS
{
class Entity;
}

namespace Vortek::Editor
{
class DrawComponentsUtil
{
  public:
	DrawComponentsUtil() = delete;

	template <typename TComponent>
	static void DrawEntityComponentInfo( Vortek::Core::ECS::Entity& entity );

	template <typename TComponent>
	static void DrawComponentInfo( TComponent& component );

	template <typename TComponent>
	static void DrawComponentInfo( Vortek::Core::ECS::Entity& entity, TComponent& component );

	template <typename TComponent>
	static void RegisterUIComponent();

  private:
	static void DrawImGuiComponent( Vortek::Core::ECS::TransformComponent& transform );
	static void DrawImGuiComponent( Vortek::Core::ECS::SpriteComponent& sprite );
	static void DrawImGuiComponent( Vortek::Core::ECS::AnimationComponent& animation );
	static void DrawImGuiComponent( Vortek::Core::ECS::BoxColliderComponent& boxCollider );
	static void DrawImGuiComponent( Vortek::Core::ECS::CircleColliderComponent& circleCollider );
	static void DrawImGuiComponent( Vortek::Core::ECS::PhysicsComponent& physics );
	static void DrawImGuiComponent( Vortek::Core::ECS::RigidBodyComponent& rigidbody );
	static void DrawImGuiComponent( Vortek::Core::ECS::TextComponent& textComponent );
	static void DrawImGuiComponent( Vortek::Core::ECS::Identification& identification );

	// Test to deal with Relationships.
	static void DrawImGuiComponent( Vortek::Core::ECS::Entity& entity, Vortek::Core::ECS::TransformComponent& transform );
	static void DrawImGuiComponent( Vortek::Core::ECS::Entity& entity, Vortek::Core::ECS::SpriteComponent& sprite );
	static void DrawImGuiComponent( Vortek::Core::ECS::Entity& entity, Vortek::Core::ECS::AnimationComponent& animation );
	static void DrawImGuiComponent( Vortek::Core::ECS::Entity& entity,
									Vortek::Core::ECS::BoxColliderComponent& boxCollider );
	static void DrawImGuiComponent( Vortek::Core::ECS::Entity& entity,
									Vortek::Core::ECS::CircleColliderComponent& circleCollider );
	static void DrawImGuiComponent( Vortek::Core::ECS::Entity& entity, Vortek::Core::ECS::PhysicsComponent& physics );
	static void DrawImGuiComponent( Vortek::Core::ECS::Entity& entity, Vortek::Core::ECS::RigidBodyComponent& rigidbody );
	static void DrawImGuiComponent( Vortek::Core::ECS::Entity& entity, Vortek::Core::ECS::TextComponent& textComponent );
	static void DrawImGuiComponent( Vortek::Core::ECS::Entity& entity, Vortek::Core::ECS::Identification& identification );
};

} // namespace Vortek::Editor

#include "DrawComponentUtils.inl"
