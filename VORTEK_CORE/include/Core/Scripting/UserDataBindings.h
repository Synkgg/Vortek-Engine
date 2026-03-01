#pragma once
#include <sol/sol.hpp>

namespace Vortek::Physics
{
struct UserData;
}

namespace Vortek::Core::Scripting
{

struct UserDataBinder
{
	static void CreateLuaUserData( sol::state& lua );

	template <typename DATA>
	static void register_meta_user_data();
};

template <typename DATA>
auto create_user_data( const sol::table& data, sol::this_state s );

template <typename DATA>
auto set_user_data( Vortek::Physics::UserData* pUserData, const sol::table& data, sol::this_state s );

template <typename DATA>
auto get_user_data( Vortek::Physics::UserData& userData, sol::this_state s );

} // namespace Vortek::Core::Scripting
#include "UserDataBindings.inl"
