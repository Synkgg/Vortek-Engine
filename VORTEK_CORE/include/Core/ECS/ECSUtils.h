#pragma once
#include <string>
#include <entt/entt.hpp>

namespace VORTEK_CORE::ECS
{
class Registry;

entt::entity FindEntityByTag( Registry& registry, const std::string& sTag );

} // namespace VORTEK_CORE::ECS
