#pragma once
#include "Core/ECS/Components/AllComponents.h"

namespace VORTEK_CORE::ECS
{
	class Entity;
}

namespace VORTEK_EDITOR
{
	class DrawComponentsUtil
	{
	public:
		DrawComponentsUtil() = delete;

		template <typename TComponent>
		static void DrawEntityComponentInfo(VORTEK_CORE::ECS::Entity& entity);

		template <typename TComponent>
		static void DrawComponentInfo(TComponent& component);

		template <typename TComponent>
		static void DrawComponentInfo(VORTEK_CORE::ECS::Entity& entity, TComponent& component);

		template <typename TComponent>
		static void RegisterUIComponent();

	private:
		static void DrawImGuiComponent(VORTEK_CORE::ECS::TransformComponent& transform);
		static void DrawImGuiComponent(VORTEK_CORE::ECS::SpriteComponent& sprite);
		static void DrawImGuiComponent(VORTEK_CORE::ECS::AnimationComponent& animation);
		static void DrawImGuiComponent(VORTEK_CORE::ECS::BoxColliderComponent& boxCollider);
		static void DrawImGuiComponent(VORTEK_CORE::ECS::CircleColliderComponent& circleCollider);
		static void DrawImGuiComponent(VORTEK_CORE::ECS::PhysicsComponent& physics);
		static void DrawImGuiComponent(VORTEK_CORE::ECS::RigidBodyComponent& rigidbody);
		static void DrawImGuiComponent(VORTEK_CORE::ECS::TextComponent& textComponent);
		static void DrawImGuiComponent(VORTEK_CORE::ECS::Identification& identification);

		// Test to deal with Relationships.
		static void DrawImGuiComponent(VORTEK_CORE::ECS::Entity& entity, VORTEK_CORE::ECS::TransformComponent& transform);
		static void DrawImGuiComponent(VORTEK_CORE::ECS::Entity& entity, VORTEK_CORE::ECS::SpriteComponent& sprite);
		static void DrawImGuiComponent(VORTEK_CORE::ECS::Entity& entity, VORTEK_CORE::ECS::AnimationComponent& animation);
		static void DrawImGuiComponent(VORTEK_CORE::ECS::Entity& entity,
			VORTEK_CORE::ECS::BoxColliderComponent& boxCollider);
		static void DrawImGuiComponent(VORTEK_CORE::ECS::Entity& entity,
			VORTEK_CORE::ECS::CircleColliderComponent& circleCollider);
		static void DrawImGuiComponent(VORTEK_CORE::ECS::Entity& entity, VORTEK_CORE::ECS::PhysicsComponent& physics);
		static void DrawImGuiComponent(VORTEK_CORE::ECS::Entity& entity, VORTEK_CORE::ECS::RigidBodyComponent& rigidbody);
		static void DrawImGuiComponent(VORTEK_CORE::ECS::Entity& entity, VORTEK_CORE::ECS::TextComponent& textComponent);
		static void DrawImGuiComponent(VORTEK_CORE::ECS::Entity& entity, VORTEK_CORE::ECS::Identification& identification);
	};

} // namespace VORTEK_EDITOR

#include "DrawComponentUtils.inl"