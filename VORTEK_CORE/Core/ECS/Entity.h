#pragma once
#include "Registry.h"
#include <sol/sol.hpp>

namespace VORTEK_CORE::ECS
{
	class Entity
	{
	private:
		Registry& m_Registry;
		entt::entity m_Entity;
		std::string m_sName, m_sGroup;

	public:
		Entity(Registry& registry);
		Entity(Registry& registry, const std::string& name = "", const std::string& group = "");

		Entity(Registry& registry, const entt::entity& entity);
		Entity& operator=(const Entity& other)
		{
			this->m_Entity = other.m_Entity;
			this->m_sName = other.m_sName;
			this->m_sGroup = other.m_sGroup;

			return *this;
		}

		~Entity() = default;

		bool AddChild(entt::entity child);
		void UpdateTransform();

		inline const std::string& GetName() const { return m_sName; }
		inline const std::string& GetGroup() const { return m_sGroup; }

		inline std::uint32_t Kill() { return m_Registry.GetRegistry().destroy(m_Entity); }

		inline entt::entity& GetEntity() { return m_Entity; }

		inline entt::registry& GetEnttRegistry() { return m_Registry.GetRegistry(); }

		inline Registry& GetRegistry() { return m_Registry; }

		static void CreateLuaEntityBind(sol::state& lua, Registry& registry);

		template <typename TComponent>
		static void RegisterMetaComponent();

		template <typename TComponent, typename... Args>
		TComponent& AddComponent(Args&&... args);

		template <typename TComponent, typename... Args>
		TComponent& ReplaceComponent(Args&&... args);

		template <typename TComponent>
		TComponent& GetComponent();

		template <typename TComponent>
		TComponent* TryGetComponent();

		template <typename TComponent>
		bool HasComponent();

		template <typename TComponent>
		auto RemoveComponent();
	};

	template <typename TComponent>
	auto add_component(Entity& entity, const sol::table& comp, sol::this_state s);

	template <typename TComponent>
	auto add_component_default(Entity& entity);

	template <typename TComponent>
	bool has_component(Entity& entity);

	template <typename TComponent>
	auto get_component(Entity& entity, sol::this_state s);

	template <typename TComponent>
	auto remove_component(Entity& entity);

	template <typename TComponent>
	auto copy_component(Entity& entityToCopy, Entity& entityThatCopies);

} // namespace VORTEK_CORE::ECS

#include "Entity.inl"