#pragma once
#include <string>
#include <entt/entt.hpp>

namespace Vortek::Core::ECS
{
struct Identification
{
	/* Name identifier for the owner object. Eventually these will be unique. */
	std::string name{ "GameObject" };
	/* The group name that the owner belongs to. */
	std::string group{};
	/* The underlying entity id of the owner. */
	uint32_t entity_id{ entt::null };
};
} // namespace Vortek::Core::ECS
