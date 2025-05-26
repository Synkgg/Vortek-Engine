#pragma once
#include <Physics/Box2DWrappers.h>
#include <Physics/UserData.h>
#include <Physics/PhysicsUtilities.h>
#include <sol/sol.hpp>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace VORTEK_CORE::ECS
{

	struct PhysicsAttributes
	{
		VORTEK_PHYSICS::RigidBodyType eType{ VORTEK_PHYSICS::RigidBodyType::STATIC };
		float density{ 1.f }, friction{ 0.2f }, restitution{ 0.2f };
		float restitutionThreshold{ 1.f }, radius{ 0.f }, gravityScale{ 1.f };

		glm::vec2 position{ 0.f }, scale{ 1.f }, boxSize{ 0.f }, offset{ 0.f };
		bool bCircle{ false }, bBoxShape{ true }, bFixedRotation{ true }, bIsSensor{ false };

		bool bIsBullet{ false };

		uint16_t filterCategory{ 0 };
		uint16_t filterMask{ 0 };
		int16_t groupIndex{ 0 };
		VORTEK_PHYSICS::ObjectData objectData{};
	};

	class PhysicsComponent
	{
		std::shared_ptr<b2Body> m_pRigidBody;
		std::shared_ptr<VORTEK_PHYSICS::UserData> m_pUserData;
		PhysicsAttributes m_InitialAttribs;

	public:
		PhysicsComponent();
		PhysicsComponent(const PhysicsAttributes& physicsAttr);
		~PhysicsComponent() = default;

		void Init(VORTEK_PHYSICS::PhysicsWorld pPhysicsWorld, int windowWidth, int windowHeight);
		b2Body* GetBody() { return m_pRigidBody.get(); }
		VORTEK_PHYSICS::UserData* GetUserData() { return m_pUserData.get(); }
		const bool IsSensor() const;

		/* The attributes may have changed. we need to make a function that will refill the attributes */
		inline const PhysicsAttributes& GetAttributes() const { return m_InitialAttribs; }
		PhysicsAttributes& GetChangableAttributes() { return m_InitialAttribs; }

		static void CreatePhysicsLuaBind(sol::state& lua, entt::registry& registry);
	};
} // namespace VORTEK_CORE::ECS