#include "Core/Events/EngineEventTypes.h"
#include "Core/Events/EventDispatcher.h"

namespace VORTEK_CORE::Events
{
void LuaEventBinder::CreateLuaEventBindings( sol::state& lua )
{
	lua.new_enum<EKeyEventType>( "KeyEventType",
								 {
									 { "Pressed", EKeyEventType::Pressed },
									 { "Released", EKeyEventType::Released },
									 { "NoType", EKeyEventType::NoType },
								 } );

	lua.new_usertype<ContactEvent>( "ContactEvent",
									"type_id",
									entt::type_hash<ContactEvent>::value,
									sol::call_constructor,
									sol::factories( [] { return ContactEvent{}; },
													[]( VORTEK_PHYSICS::ObjectData a, VORTEK_PHYSICS::ObjectData b ) {
														return ContactEvent{ .objectA = a, .objectB = b };
													} ),
									"objectA",
									&ContactEvent::objectA,
									"objectB",
									&ContactEvent::objectB );

	lua.new_usertype<KeyEvent>(
		"KeyEvent",
		"type_id",
		entt::type_hash<KeyEvent>::value,
		sol::call_constructor,
		sol::factories( [] { return KeyEvent{}; },
						[]( int key, EKeyEventType eType ) { return KeyEvent{ .key = key, .eType = eType }; } ),
		"key",
		&KeyEvent::key,
		"type",
		&KeyEvent::eType );

	lua.new_usertype<LuaEvent>(
		"LuaEvent",
		"type_id",
		entt::type_hash<LuaEvent>::value,
		sol::call_constructor,
		sol::factories( [] { return LuaEvent{}; }, []( const sol::object& data ) { return LuaEvent{ .data = data }; } ),
		"data",
		&LuaEvent::data );

	lua.new_usertype<LuaHandler<ContactEvent>>(
		"ContactEventHandler",
		"type_id",
		entt::type_hash<LuaHandler<ContactEvent>>::value,
		"event_type",
		entt::type_hash<ContactEvent>::value,
		sol::call_constructor,
		sol::factories( []( const sol::function& func ) { return LuaHandler<ContactEvent>{ .callback = func }; } ),
		"release",
		&LuaHandler<ContactEvent>::ReleaseConnection );

	lua.new_usertype<LuaHandler<KeyEvent>>(
		"KeyEventHandler",
		"type_id",
		entt::type_hash<LuaHandler<KeyEvent>>::value,
		"event_type",
		entt::type_hash<KeyEvent>::value,
		sol::call_constructor,
		sol::factories( []( const sol::function& func ) { return LuaHandler<KeyEvent>{ .callback = func }; } ),
		"release",
		&LuaHandler<KeyEvent>::ReleaseConnection );

	lua.new_usertype<LuaHandler<LuaEvent>>(
		"LuaEventHandler",
		"type_id",
		entt::type_hash<LuaHandler<LuaEvent>>::value,
		"event_type",
		entt::type_hash<LuaEvent>::value,
		sol::call_constructor,
		sol::factories( []( const sol::function& func ) { return LuaHandler<LuaEvent>{ .callback = func }; } ),
		"release",
		&LuaHandler<LuaEvent>::ReleaseConnection );
}
} // namespace VORTEK_CORE::Events
