#pragma once
#include <entt.hpp>
#include <sol/sol.hpp>

namespace VORTEK_CORE::ECS
{

	struct Relationship
	{
		entt::entity self{ entt::null };
		/* The first child of the entity. */
		entt::entity firstChild{ entt::null };
		/* The previous sibling entity in the hierarchy for the parent. */
		entt::entity prevSibling{ entt::null };
		/* The next sibling entity in the hierarchy for the parent. */
		entt::entity nextSibling{ entt::null };
		/* The parent entity. Not all entities will have parents. */
		entt::entity parent{ entt::null };

		static void CreateRelationshipLuaBind(sol::state& lua);
	};

	class Entity;

	struct RelationshipUtils
	{
		static bool IsAParentOf(Entity& entityA, Entity& entityB);
		static void SetSiblingLinks(Entity& firstChild, Relationship& childRelationship);
		static void RemoveAndDelete(Entity& entityToRemove);
	};

} // namespace VORTEK_CORE::ECS