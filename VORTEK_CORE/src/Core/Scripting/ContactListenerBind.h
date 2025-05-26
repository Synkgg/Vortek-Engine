#pragma once
#include <sol/sol.hpp>
#include <Physics/ContactListener.h>
#include <entt/entt.hpp>

namespace VORTEK_CORE::Scripting {
    class ContactListenerBinder
    {
    private:
        static std::tuple<sol::object, sol::object> GetUserData(VORTEK_PHYSICS::ContactListener& contactListener, sol::this_state s);

    public:
        static void CreateLuaContactListener(sol::state& lua, entt::registry& registry);
    };
}