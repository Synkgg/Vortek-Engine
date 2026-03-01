#pragma once
#include <sol/sol.hpp>
#include <entt/entt.hpp>

namespace Vortek::Physics 
{
class ContactListener;
}

namespace Vortek::Core::Scripting
{
class ContactListenerBinder
{
  private:
	static std::tuple<sol::object, sol::object> GetUserData( Vortek::Physics::ContactListener& contactListener,
															 sol::this_state s );

  public:
	static void CreateLuaContactListener( sol::state& lua, entt::registry& registry );
};
} // namespace Vortek::Core::Scripting
