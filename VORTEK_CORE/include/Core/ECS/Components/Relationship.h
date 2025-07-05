#pragma once
#include <entt/entt.hpp>
#include <sol/sol.hpp>

namespace VORTEK_CORE::ECS
{

/*
 * Relationship
 * Simple struct for an Unconstrained Hierarchy.
 * The number of children for the parent is not known and is not clamped.
 * Treated like an implicit doubly-linked list with the benefit of
 * no dynamic allocations or reallocations.
 */
struct Relationship
{
	/*
	Easy way to get owning entity. This is set when entity is created.
	Not necessary to have, can be removed if desired.
	*/
	entt::entity self{ entt::null };
	/* The first child of the entity. */
	entt::entity firstChild{ entt::null };
	/* The previous sibling entity in the hierarchy for the parent. */
	entt::entity prevSibling{ entt::null };
	/* The next sibling entity in the hierarchy for the parent. */
	entt::entity nextSibling{ entt::null };
	/* The parent entity. Not all entities will have parents. */
	entt::entity parent{ entt::null };

	static void CreateRelationshipLuaBind( sol::state& lua );
};

class Entity;

struct RelationshipUtils
{
	static bool IsAParentOf( Entity& entityA, Entity& entityB );
	static void SetSiblingLinks( Entity& firstChild, Relationship& childRelationship );
	static void RemoveAndDelete( Entity& entityToRemove, std::vector<std::string>& entitiesRemoved );
};

} // namespace VORTEK_CORE::ECS
