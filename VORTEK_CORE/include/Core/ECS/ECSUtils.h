#pragma once
#include <entt/entt.hpp>

namespace Vortek::Core::ECS
{
class Registry;

entt::entity FindEntityByTag( Registry& registry, const std::string& sTag );

} // namespace Vortek::Core::ECS
