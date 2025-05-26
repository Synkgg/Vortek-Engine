#include "Keyboard.h"
#include <Logger.h>

namespace VORTEK_WINDOWING::Inputs {

    Keyboard::Keyboard()
        :m_mapButtons{ { VORTEK_KEY_BACKSPACE, Button{} }, { VORTEK_KEY_TAB, Button{} }, { VORTEK_KEY_CLEAR, Button{} },
		{ VORTEK_KEY_RETURN, Button{} }, { VORTEK_KEY_PAUSE, Button{} }, { VORTEK_KEY_ESCAPE, Button{} }, { VORTEK_KEY_SPACE, Button{} },
		{ VORTEK_KEY_EXCLAIM, Button{} }, { VORTEK_KEY_QUOTEDBL, Button{} }, { VORTEK_KEY_HASH, Button{} }, { VORTEK_KEY_DOLLAR, Button{} },
		{ VORTEK_KEY_AMPERSAND, Button{} }, { VORTEK_KEY_QUOTE, Button{} }, { VORTEK_KEY_LEFTPAREN, Button{} }, { VORTEK_KEY_RIGHTPAREN, Button{} },
		{ VORTEK_KEY_ASTERISK, Button{} }, { VORTEK_KEY_PLUS, Button{} }, { VORTEK_KEY_COMMA, Button{} }, { VORTEK_KEY_MINUS, Button{} },
		{ VORTEK_KEY_PERIOD, Button{} }, { VORTEK_KEY_SLASH, Button{} }, { VORTEK_KEY_0, Button{} }, { VORTEK_KEY_1, Button{} },
		{ VORTEK_KEY_2, Button{} }, { VORTEK_KEY_3, Button{} }, { VORTEK_KEY_4, Button{} }, { VORTEK_KEY_5, Button{} },
		{ VORTEK_KEY_6, Button{} }, { VORTEK_KEY_7, Button{} }, { VORTEK_KEY_8, Button{} }, { VORTEK_KEY_9, Button{} },
		{ VORTEK_KEY_COLON, Button{} }, { VORTEK_KEY_SEMICOLON, Button{} }, { VORTEK_KEY_LESS, Button{} }, { VORTEK_KEY_EQUALS, Button{} },
		{ VORTEK_KEY_GREATER, Button{} }, { VORTEK_KEY_QUESTION, Button{} }, { VORTEK_KEY_AT, Button{} }, { VORTEK_KEY_LEFTBRACKET, Button{} },
		{ VORTEK_KEY_BACKSLASH, Button{} }, { VORTEK_KEY_RIGHTBRACKET, Button{} }, { VORTEK_KEY_CARET, Button{} }, { VORTEK_KEY_UNDERSCORE, Button{} },
		{ VORTEK_KEY_BACKQUOTE, Button{} }, { VORTEK_KEY_A, Button{} }, { VORTEK_KEY_B, Button{} }, { VORTEK_KEY_C, Button{} },
		{ VORTEK_KEY_D, Button{} }, { VORTEK_KEY_E, Button{} }, { VORTEK_KEY_F, Button{} }, { VORTEK_KEY_G, Button{} },
		{ VORTEK_KEY_H, Button{} }, { VORTEK_KEY_I, Button{} }, { VORTEK_KEY_J, Button{} }, { VORTEK_KEY_K, Button{} },
		{ VORTEK_KEY_L, Button{} }, { VORTEK_KEY_M, Button{} }, { VORTEK_KEY_N, Button{} }, { VORTEK_KEY_O, Button{} },
		{ VORTEK_KEY_P, Button{} }, { VORTEK_KEY_Q, Button{} }, { VORTEK_KEY_R, Button{} }, { VORTEK_KEY_S, Button{} },
		{ VORTEK_KEY_T, Button{} }, { VORTEK_KEY_U, Button{} }, { VORTEK_KEY_V, Button{} }, { VORTEK_KEY_W, Button{} }, 
		{ VORTEK_KEY_X, Button{} }, { VORTEK_KEY_Y, Button{} }, { VORTEK_KEY_Z, Button{} }, { VORTEK_KEY_DELETE, Button{} },
		{ VORTEK_KEY_CAPSLOCK, Button{} }, { VORTEK_KEY_F1, Button{} }, { VORTEK_KEY_F2, Button{} }, { VORTEK_KEY_F3, Button{} },
		{ VORTEK_KEY_F4, Button{} }, { VORTEK_KEY_F5, Button{} }, { VORTEK_KEY_F6, Button{} }, { VORTEK_KEY_F7, Button{} },
		{ VORTEK_KEY_F8, Button{} }, { VORTEK_KEY_F9, Button{} }, { VORTEK_KEY_F10, Button{} }, { VORTEK_KEY_F11, Button{} },
		{ VORTEK_KEY_F12, Button{} }, { VORTEK_KEY_SCROLLOCK, Button{} }, { VORTEK_KEY_PRINTSCREEN, Button{} }, { VORTEK_KEY_INSERT, Button{} },
		{ VORTEK_KEY_HOME, Button{} }, { VORTEK_KEY_PAGEUP, Button{} }, { VORTEK_KEY_PAGEDOWN, Button{} }, { VORTEK_KEY_END, Button{} },
		{ VORTEK_KEY_RIGHT, Button{} }, { VORTEK_KEY_LEFT, Button{} }, { VORTEK_KEY_DOWN, Button{} }, { VORTEK_KEY_UP, Button{} },
		{ VORTEK_KEY_NUMLOCK, Button{} }, { VORTEK_KEY_KP_DIVIDE, Button{} }, { VORTEK_KEY_KP_MULTIPLY, Button{} }, { VORTEK_KEY_KP_MINUS, Button{} },
		{ VORTEK_KEY_KP_PLUS, Button{} }, { VORTEK_KEY_KP_ENTER, Button{} }, { VORTEK_KEY_KP1, Button{} }, { VORTEK_KEY_KP2, Button{} },
		{ VORTEK_KEY_KP3, Button{} }, { VORTEK_KEY_KP4, Button{} }, { VORTEK_KEY_KP5, Button{} }, { VORTEK_KEY_KP6, Button{} },
		{ VORTEK_KEY_KP7, Button{} }, { VORTEK_KEY_KP8, Button{} },	{ VORTEK_KEY_KP9, Button{} }, { VORTEK_KEY_KP0, Button{} },
		{ VORTEK_KEY_KP_PERIOD, Button{} }, { VORTEK_KEY_LCTRL, Button{} }, { VORTEK_KEY_LSHIFT, Button{} }, { VORTEK_KEY_LALT, Button{} },
		{ VORTEK_KEY_RCTRL, Button{} }, { VORTEK_KEY_RSHIFT, Button{} }, { VORTEK_KEY_RALT, Button{} } }
		{

		}

    void Keyboard::Update()
    {
		for (auto& [key, button] : m_mapButtons)
			button.Reset();
    }

    void Keyboard::OnKeyPressed(int key)
    {
		if (key == KEY_UNKNOWN)
		{
			VORTEK_ERROR("Key [{}] is unknown!", key);
			return;
		}

		auto keyItr = m_mapButtons.find(key);
		if (keyItr == m_mapButtons.end())
		{
			VORTEK_ERROR("Key [{}] does not exist!", key);
			return;
		}
		keyItr->second.Update(true);
    }

    void Keyboard::OnKeyReleased(int key)
    {
		if (key == KEY_UNKNOWN)
		{
			VORTEK_ERROR("Key [{}] is unknown!", key);
			return;
		}

		auto keyItr = m_mapButtons.find(key);
		if (keyItr == m_mapButtons.end())
		{
			VORTEK_ERROR("Key [{}] does not exist!", key);
			return;
		}
		keyItr->second.Update(false);
    }

    const bool Keyboard::IsKeyPressed(int key) const
    {
		if (key == KEY_UNKNOWN)
		{
			VORTEK_ERROR("Key [{}] is unknown!", key);
			return false;
		}

		auto keyItr = m_mapButtons.find(key);
		if (keyItr == m_mapButtons.end())
		{
			VORTEK_ERROR("Key [{}] does not exist!", key);
			return false;
		}
		return keyItr->second.bIsPressed;
    }

    const bool Keyboard::IsKeyJustPressed(int key) const
    {
		if (key == KEY_UNKNOWN)
		{
			VORTEK_ERROR("Key [{}] is unknown!", key);
			return false;
		}

		auto keyItr = m_mapButtons.find(key);
		if (keyItr == m_mapButtons.end())
		{
			VORTEK_ERROR("Key [{}] does not exist!", key);
			return false;
		}
		return keyItr->second.bJustPressed;
    }

    const bool Keyboard::IsKeyJustReleased(int key) const
    {
		if (key == KEY_UNKNOWN)
		{
			VORTEK_ERROR("Key [{}] is unknown!", key);
			return false;
		}

		auto keyItr = m_mapButtons.find(key);
		if (keyItr == m_mapButtons.end())
		{
			VORTEK_ERROR("Key [{}] does not exist!", key);
			return false;
		}
		return keyItr->second.bJustReleased;
    }
}