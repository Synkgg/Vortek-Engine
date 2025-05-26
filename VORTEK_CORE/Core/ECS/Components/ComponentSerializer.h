#pragma once
#include "AllComponents.h"
#include <rapidjson/document.h>

namespace VORTEK_FILESYSTEM
{
	class JSONSerializer;
}

#define SERIALIZE_COMPONENT( serializer, component )                                                                   \
	VORTEK_CORE::ECS::ComponentSerializer::Serialize( serializer, component )

#define DESERIALIZE_COMPONENT( table, compref )                                                                   \
	VORTEK_CORE::ECS::ComponentSerializer::Deserialize( table, compref )


namespace VORTEK_CORE::ECS
{
	class ComponentSerializer
	{
	public:
		ComponentSerializer() = delete;

		template <typename TComponent, typename TSerializer>
		static void Serialize(TSerializer& serializer, const TComponent& component);

		template <typename TComponent, typename TTable>
		static void Deserialize(const TTable& table, TComponent& component);

	private:
		// JSON serializer
		static void SerializeComponent(VORTEK_FILESYSTEM::JSONSerializer& serializer, const TransformComponent& transform);
		static void SerializeComponent(VORTEK_FILESYSTEM::JSONSerializer& serializer, const SpriteComponent& sprite);
		static void SerializeComponent(VORTEK_FILESYSTEM::JSONSerializer& serializer, const AnimationComponent& animation);
		static void SerializeComponent(VORTEK_FILESYSTEM::JSONSerializer& serializer,
			const BoxColliderComponent& boxCollider);
		static void SerializeComponent(VORTEK_FILESYSTEM::JSONSerializer& serializer,
			const CircleColliderComponent& circleCollider);
		static void SerializeComponent(VORTEK_FILESYSTEM::JSONSerializer& serializer, const TextComponent& text);
		static void SerializeComponent(VORTEK_FILESYSTEM::JSONSerializer& serializer, const PhysicsComponent& physics);
		static void SerializeComponent(VORTEK_FILESYSTEM::JSONSerializer& serializer, const RigidBodyComponent& rigidBody);

		static void DeserializeComponent(const rapidjson::Value& jsonValue, TransformComponent& transform);
		static void DeserializeComponent(const rapidjson::Value& jsonValue, SpriteComponent& sprite);
		static void DeserializeComponent(const rapidjson::Value& jsonValue, AnimationComponent& animation);
		static void DeserializeComponent(const rapidjson::Value& jsonValue, BoxColliderComponent& boxCollider);
		static void DeserializeComponent(const rapidjson::Value& jsonValue, CircleColliderComponent& circleCollider);
		static void DeserializeComponent(const rapidjson::Value& jsonValue, TextComponent& text);
		static void DeserializeComponent(const rapidjson::Value& jsonValue, PhysicsComponent& physics);
		static void DeserializeComponent(const rapidjson::Value& jsonValue, RigidBodyComponent& rigidBody);
	};

} // namespace VORTEK_CORE::ECS

#include "ComponentSerializer.inl"