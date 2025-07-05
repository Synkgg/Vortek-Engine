#include "Core/Scripting/InputManager.h"
#include <Logger/Logger.h>
#include <VORTEKUtilities/SDL_Wrappers.h>
#include <glm/glm.hpp>

#include <Rendering/Core/Camera2D.h>

namespace VORTEK_CORE
{

InputManager::InputManager()
	: m_pKeyboard{ std::make_unique<Keyboard>() }
	, m_pMouse{ std::make_unique<Mouse>() }
{
}

void InputManager::RegisterLuaKeyNames( sol::state& lua )
{
	// ==================================================================
	// Register Typewriter Keys
	// ==================================================================
	lua.set( "KEY_A", VORTEK_KEY_A );
	lua.set( "KEY_B", VORTEK_KEY_B );
	lua.set( "KEY_C", VORTEK_KEY_C );
	lua.set( "KEY_D", VORTEK_KEY_D );
	lua.set( "KEY_E", VORTEK_KEY_E );
	lua.set( "KEY_F", VORTEK_KEY_F );
	lua.set( "KEY_G", VORTEK_KEY_G );
	lua.set( "KEY_H", VORTEK_KEY_H );
	lua.set( "KEY_I", VORTEK_KEY_I );
	lua.set( "KEY_J", VORTEK_KEY_J );
	lua.set( "KEY_K", VORTEK_KEY_K );
	lua.set( "KEY_L", VORTEK_KEY_L );
	lua.set( "KEY_M", VORTEK_KEY_M );
	lua.set( "KEY_N", VORTEK_KEY_N );
	lua.set( "KEY_O", VORTEK_KEY_O );
	lua.set( "KEY_P", VORTEK_KEY_P );
	lua.set( "KEY_Q", VORTEK_KEY_Q );
	lua.set( "KEY_R", VORTEK_KEY_R );
	lua.set( "KEY_S", VORTEK_KEY_S );
	lua.set( "KEY_T", VORTEK_KEY_T );
	lua.set( "KEY_U", VORTEK_KEY_U );
	lua.set( "KEY_V", VORTEK_KEY_V );
	lua.set( "KEY_W", VORTEK_KEY_W );
	lua.set( "KEY_X", VORTEK_KEY_X );
	lua.set( "KEY_Y", VORTEK_KEY_Y );
	lua.set( "KEY_Z", VORTEK_KEY_Z );

	lua.set( "KEY_0", VORTEK_KEY_0 );
	lua.set( "KEY_1", VORTEK_KEY_1 );
	lua.set( "KEY_2", VORTEK_KEY_2 );
	lua.set( "KEY_3", VORTEK_KEY_3 );
	lua.set( "KEY_4", VORTEK_KEY_4 );
	lua.set( "KEY_5", VORTEK_KEY_5 );
	lua.set( "KEY_6", VORTEK_KEY_6 );
	lua.set( "KEY_7", VORTEK_KEY_7 );
	lua.set( "KEY_8", VORTEK_KEY_8 );
	lua.set( "KEY_9", VORTEK_KEY_9 );

	lua.set( "KEY_ENTER", VORTEK_KEY_RETURN );
	lua.set( "KEY_BACKSPACE", VORTEK_KEY_BACKSPACE );
	lua.set( "KEY_ESC", VORTEK_KEY_ESCAPE );
	lua.set( "KEY_SPACE", VORTEK_KEY_SPACE );
	lua.set( "KEY_LCTRL", VORTEK_KEY_LCTRL );
	lua.set( "KEY_RCTRL", VORTEK_KEY_RCTRL );
	lua.set( "KEY_LALT", VORTEK_KEY_LALT );
	lua.set( "KEY_RALT", VORTEK_KEY_RALT );
	lua.set( "KEY_LSHIFT", VORTEK_KEY_LSHIFT );
	lua.set( "KEY_RSHIFT", VORTEK_KEY_RSHIFT );

	// ==================================================================
	//  Register Punctuation Keys
	// ==================================================================
	lua.set( "KEY_COLON", VORTEK_KEY_COLON );
	lua.set( "KEY_SEMICOLON", VORTEK_KEY_SEMICOLON );
	lua.set( "KEY_QUOTE", VORTEK_KEY_QUOTE );
	lua.set( "KEY_BACKQUOTE", VORTEK_KEY_BACKQUOTE );
	lua.set( "KEY_CARET", VORTEK_KEY_CARET );
	lua.set( "KEY_UNDERSCORE", VORTEK_KEY_UNDERSCORE );
	lua.set( "KEY_RIGHTBRACKET", VORTEK_KEY_RIGHTBRACKET );
	lua.set( "KEY_LEFTBRACKET", VORTEK_KEY_LEFTBRACKET );
	lua.set( "KEY_SLASH", VORTEK_KEY_SLASH );
	lua.set( "KEY_ASTERISK", VORTEK_KEY_ASTERISK );
	lua.set( "KEY_LEFTPAREN", VORTEK_KEY_LEFTPAREN );
	lua.set( "KEY_RIGHTPAREN", VORTEK_KEY_RIGHTPAREN );
	lua.set( "KEY_QUESTION", VORTEK_KEY_QUESTION );
	lua.set( "KEY_AMPERSAND", VORTEK_KEY_AMPERSAND );
	lua.set( "KEY_DOLLAR", VORTEK_KEY_DOLLAR );
	lua.set( "KEY_EXCLAIM", VORTEK_KEY_EXCLAIM );
	lua.set( "KEY_BACKSLASH", VORTEK_KEY_BACKSLASH );

	// ==================================================================
	//  Register Function Keys
	// ==================================================================
	lua.set( "KEY_F1", VORTEK_KEY_F1 );
	lua.set( "KEY_F2", VORTEK_KEY_F2 );
	lua.set( "KEY_F3", VORTEK_KEY_F3 );
	lua.set( "KEY_F4", VORTEK_KEY_F4 );
	lua.set( "KEY_F5", VORTEK_KEY_F5 );
	lua.set( "KEY_F6", VORTEK_KEY_F6 );
	lua.set( "KEY_F7", VORTEK_KEY_F7 );
	lua.set( "KEY_F8", VORTEK_KEY_F8 );
	lua.set( "KEY_F9", VORTEK_KEY_F9 );
	lua.set( "KEY_F10", VORTEK_KEY_F10 );
	lua.set( "KEY_F11", VORTEK_KEY_F11 );
	lua.set( "KEY_F12", VORTEK_KEY_F12 );

	// ==================================================================
	// Register Cursor Control Keys
	// ==================================================================
	lua.set( "KEY_UP", VORTEK_KEY_UP );
	lua.set( "KEY_RIGHT", VORTEK_KEY_RIGHT );
	lua.set( "KEY_DOWN", VORTEK_KEY_DOWN );
	lua.set( "KEY_LEFT", VORTEK_KEY_LEFT );

	// ==================================================================
	// Register Numeric Keypad Keys
	// ==================================================================
	lua.set( "KP_KEY_0", VORTEK_KEY_KP0 );
	lua.set( "KP_KEY_1", VORTEK_KEY_KP1 );
	lua.set( "KP_KEY_2", VORTEK_KEY_KP2 );
	lua.set( "KP_KEY_3", VORTEK_KEY_KP3 );
	lua.set( "KP_KEY_4", VORTEK_KEY_KP4 );
	lua.set( "KP_KEY_5", VORTEK_KEY_KP5 );
	lua.set( "KP_KEY_6", VORTEK_KEY_KP6 );
	lua.set( "KP_KEY_7", VORTEK_KEY_KP7 );
	lua.set( "KP_KEY_8", VORTEK_KEY_KP8 );
	lua.set( "KP_KEY_9", VORTEK_KEY_KP9 );

	lua.set( "KP_KEY_DIVIDE", VORTEK_KEY_KP_DIVIDE );
	lua.set( "KP_KEY_MULTIPLY", VORTEK_KEY_KP_MULTIPLY );
	lua.set( "KP_KEY_MINUS", VORTEK_KEY_KP_MINUS );
	lua.set( "KP_KEY_PLUS", VORTEK_KEY_KP_PLUS );
	lua.set( "KP_KEY_ENTER", VORTEK_KEY_KP_ENTER );
	lua.set( "KP_KEY_PERIOD", VORTEK_KEY_KP_PERIOD );

	lua.set( "KEY_NUM_LOCK", VORTEK_KEY_NUMLOCK );

	// Adding windows specific keys here for convenience
#ifdef _WIN32
	lua.set( "KEY_LWIN", VORTEK_KEY_LWIN );
	lua.set( "KEY_RWIN", VORTEK_KEY_RWIN );
#else
	lua.set( "KEY_LGUI", VORTEK_KEY_LGUI );
	lua.set( "KEY_RGUI", VORTEK_KEY_RGUI );
#endif
}

void InputManager::RegisterMouseBtnNames( sol::state& lua )
{
	lua.set( "LEFT_BTN", VORTEK_MOUSE_LEFT );
	lua.set( "MIDDLE_BTN", VORTEK_MOUSE_MIDDLE );
	lua.set( "RIGHT_BTN", VORTEK_MOUSE_RIGHT );
}

void InputManager::RegisterGamepadBtnNames( sol::state& lua )
{
	lua.set( "GP_BTN_A", VORTEK_GP_BTN_A );
	lua.set( "GP_BTN_B", VORTEK_GP_BTN_B );
	lua.set( "GP_BTN_X", VORTEK_GP_BTN_X );
	lua.set( "GP_BTN_Y", VORTEK_GP_BTN_Y );

	lua.set( "GP_BTN_BACK", VORTEK_GP_BTN_BACK );
	lua.set( "GP_BTN_GUIDE", VORTEK_GP_BTN_GUIDE );
	lua.set( "GP_BTN_START", VORTEK_GP_BTN_START );

	lua.set( "GP_LSTICK", VORTEK_GP_BTN_LSTICK );
	lua.set( "GP_RSTICK", VORTEK_GP_BTN_RSTICK );
	lua.set( "GP_LSHOULDER", VORTEK_GP_BTN_LSHOULDER );
	lua.set( "GP_RSHOULDER", VORTEK_GP_BTN_RSHOULDER );

	lua.set( "DPAD_UP", VORTEK_GP_BTN_DPAD_UP );
	lua.set( "DPAD_DOWN", VORTEK_GP_BTN_DPAD_DOWN );
	lua.set( "DPAD_LEFT", VORTEK_GP_BTN_DPAD_LEFT );
	lua.set( "DPAD_RIGHT", VORTEK_GP_BTN_DPAD_RIGHT );

	lua.set( "AXIS_X1", 0 );
	lua.set( "AXIS_Y1", 1 );
	lua.set( "AXIS_X2", 2 );
	lua.set( "AXIS_Y2", 3 );

	// Bottom triggers
	lua.set( "AXIS_Z1", 4 );
	lua.set( "AXIS_Z2", 5 );
}

InputManager& InputManager::GetInstance()
{
	static InputManager instance{};
	return instance;
}

void InputManager::CreateLuaInputBindings( sol::state& lua, VORTEK_CORE::ECS::Registry& registry )
{
	RegisterLuaKeyNames( lua );
	RegisterMouseBtnNames( lua );
	RegisterGamepadBtnNames( lua );

	auto& inputManager = GetInstance();
	auto& keyboard = inputManager.GetKeyboard();
	auto& camera = registry.GetContext<std::shared_ptr<VORTEK_RENDERING::Camera2D>>();

	lua.new_usertype<Keyboard>(
		"Keyboard",
		sol::no_constructor,
		"just_pressed",
		[ & ]( int key ) { return keyboard.IsKeyJustPressed( key ); },
		"just_released",
		[ & ]( int key ) { return keyboard.IsKeyJustReleased( key ); },
		"pressed",
		[ & ]( int key ) { return keyboard.IsKeyPressed( key ); },
		"pressed_keys",
		[ & ]() {
			std::vector<int> keys;
			for ( const auto& [ key, button ] : keyboard.GetButtonMap() )
			{
				if ( button.bIsPressed )
					keys.push_back( key );
			}
			return keys;
		} );

	/*
	 * In order for this to work in the editor, we need to take into account
	 * the imgui window position, to get the world position.
	 * There is new bindings inside the editor that handles this.
	 */
	#ifndef IN_VORTEK_EDITOR
	auto& mouse = inputManager.GetMouse();

	lua.new_usertype<Mouse>(
		"Mouse",
		sol::no_constructor,
		"just_pressed",
		[ & ]( int btn ) { return mouse.IsBtnJustPressed( btn ); },
		"just_released",
		[ & ]( int btn ) { return mouse.IsBtnJustReleased( btn ); },
		"pressed",
		[ & ]( int btn ) { return mouse.IsBtnPressed( btn ); },
		"screen_position",
		[ & ]() {
			auto [ x, y ] = mouse.GetMouseScreenPosition();
			return glm::vec2{ x, y };
		},
		"world_position",
		[ & ]() {
			auto [ x, y ] = mouse.GetMouseScreenPosition();
			return camera->ScreenCoordsToWorld( glm::vec2{ x, y } );
		},
		"wheel_x",
		[ & ]() { return mouse.GetMouseWheelX(); },
		"wheel_y",
		[ & ]() { return mouse.GetMouseWheelY(); } );
	#endif // IN_VORTEK_EDITOR

	lua.new_usertype<Gamepad>(
		"Gamepad",
		sol::no_constructor,
		"just_pressed",
		[ & ]( int index, int btn ) {
			auto gamepad = inputManager.GetController( index );
			if ( !gamepad )
			{
				VORTEK_ERROR( "Invalid Gamepad index [{}] provided or gamepad is not plugged in!", index );
				return false;
			}
			return gamepad->IsBtnJustPressed( btn );
		},
		"just_released",
		[ & ]( int index, int btn ) {
			auto gamepad = inputManager.GetController( index );
			if ( !gamepad )
			{
				VORTEK_ERROR( "Invalid Gamepad index [{}] provided or gamepad is not plugged in!", index );
				return false;
			}
			return gamepad->IsBtnJustReleased( btn );
		},
		"pressed",
		[ & ]( int index, int btn ) {
			auto gamepad = inputManager.GetController( index );
			if ( !gamepad )
			{
				VORTEK_ERROR( "Invalid Gamepad index [{}] provided or gamepad is not plugged in!", index );
				return false;
			}
			return gamepad->IsBtnPressed( btn );
		},
		"get_axis_position",
		[ & ]( int index, int axis ) {
			auto gamepad = inputManager.GetController( index );
			if ( !gamepad )
			{
				VORTEK_ERROR( "Invalid Gamepad index [{}] provided or gamepad is not plugged in!", index );
				return Sint16{ 0 };
			}
			return gamepad->GetAxisPosition( axis );
		},
		"get_hat_value",
		[ & ]( int index ) {
			auto gamepad = inputManager.GetController( index );
			if ( !gamepad )
			{
				VORTEK_ERROR( "Invalid Gamepad index [{}] provided or gamepad is not plugged in!", index );
				return Uint8{ 0 };
			}
			return gamepad->GetJoystickHatValue();
		} );
}
std::shared_ptr<Gamepad> InputManager::GetController( int index )
{
	auto gamepadItr = m_mapGameControllers.find( index );
	if ( gamepadItr == m_mapGameControllers.end() )
	{
		VORTEK_ERROR( "Failed to get gamepad at index [{}] -- Does not exist!", index );
		return nullptr;
	}

	return gamepadItr->second;
}

bool InputManager::AddGamepad( Sint32 gamepadIndex )
{
	if ( m_mapGameControllers.size() >= MAX_CONTROLLERS )
	{
		VORTEK_ERROR( "Trying to add too many controllers! Max Controllers allowed = {}", MAX_CONTROLLERS );
		return false;
	}

	std::shared_ptr<Gamepad> gamepad{ nullptr };
	try
	{
		gamepad =
			std::make_shared<Gamepad>( std::move( make_shared_controller( SDL_GameControllerOpen( gamepadIndex ) ) ) );
	}
	catch ( ... )
	{
		std::string error{ SDL_GetError() };
		VORTEK_ERROR( "Failed to Open gamepad device -- {}", error );
		return false;
	}

	for ( int i = 1; i <= MAX_CONTROLLERS; i++ )
	{
		if ( m_mapGameControllers.contains( i ) )
			continue;

		m_mapGameControllers.emplace( i, std::move( gamepad ) );
		VORTEK_LOG( "Gamepad [{}] was added at index [{}]", gamepadIndex, i );
		return true;
	}

	VORTEK_ASSERT( false && "Failed to add the new controller!" );
	VORTEK_ERROR( "Failed to add the new controller!" );
	return false;
}

bool InputManager::RemoveGamepad( Sint32 gamepadID )
{
	auto gamepadRemoved = std::erase_if(
		m_mapGameControllers, [ & ]( auto& gamepad ) { return gamepad.second->CheckJoystickID( gamepadID ); } );

	if ( gamepadRemoved > 0 )
	{
		VORTEK_LOG( "Gamepad Removed -- [{}]", gamepadID );
		return true;
	}

	VORTEK_ASSERT( false && "Failed to remove Gamepad must not have been mapped!" );
	VORTEK_ERROR( "Failed to remove Gamepad [{}] must not have been mapped!", gamepadID );
	return false;
}

void InputManager::GamepadBtnPressed( const SDL_Event& event )
{
	for ( const auto& [ index, gamepad ] : m_mapGameControllers )
	{
		if ( gamepad && gamepad->CheckJoystickID( event.jdevice.which ) )
		{
			gamepad->OnBtnPressed( event.cbutton.button );
			break;
		}
	}
}

void InputManager::GamepadBtnReleased( const SDL_Event& event )
{
	for ( const auto& [ index, gamepad ] : m_mapGameControllers )
	{
		if ( gamepad && gamepad->CheckJoystickID( event.jdevice.which ) )
		{
			gamepad->OnBtnReleased( event.cbutton.button );
			break;
		}
	}
}
void InputManager::GamepadAxisValues( const SDL_Event& event )
{
	for ( const auto& [ index, gamepad ] : m_mapGameControllers )
	{
		if ( gamepad && gamepad->CheckJoystickID( event.jdevice.which ) )
		{
			gamepad->SetAxisPositionValue( event.jaxis.axis, event.jaxis.value );
			break;
		}
	}
}

void InputManager::GamepadHatValues( const SDL_Event& event )
{
	for ( const auto& [ index, gamepad ] : m_mapGameControllers )
	{
		if ( gamepad && gamepad->CheckJoystickID( event.jdevice.which ) )
		{
			gamepad->SetJoystickHatValue( event.jhat.value );
			break;
		}
	}
}

void InputManager::UpdateGamepads()
{
	for ( const auto& [ index, gamepad ] : m_mapGameControllers )
	{
		if ( gamepad )
			gamepad->Update();
	}
}
} // namespace VORTEK_CORE
