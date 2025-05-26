#pragma once
#include <Windowing/Inputs/Keyboard.h>
#include <Windowing/Inputs/Mouse.h>
#include <Windowing/Inputs/Gamepad.h>
#include <memory>
#include <sol/sol.hpp>
#include "../ECS/Registry.h"

using namespace VORTEK_WINDOWING::Inputs;
constexpr int MAX_CONTROLLERS = 4;

#define INPUT_MANAGER() VORTEK_CORE::InputManager::GetInstance()

namespace VORTEK_CORE
{
	class InputManager
	{
	private:
		std::unique_ptr<Keyboard> m_pKeyboard;
		std::unique_ptr<Mouse> m_pMouse;
		std::map<int, std::shared_ptr<Gamepad>> m_mapGameControllers;

	private:
		InputManager();
		~InputManager() = default;
		InputManager(const InputManager&) = delete;
		InputManager& operator=(const InputManager&) = delete;

	private:
		static void RegisterLuaKeyNames(sol::state& lua);
		static void RegisterMouseBtnNames(sol::state& lua);
		static void RegisterGamepadBtnNames(sol::state& lua);

	public:
		static InputManager& GetInstance();
		static void CreateLuaInputBindings(sol::state& lua, VORTEK_CORE::ECS::Registry& registry);

		inline Keyboard& GetKeyboard() { return *m_pKeyboard; }
		inline Mouse& GetMouse() { return *m_pMouse; }

		inline std::map<int, std::shared_ptr<Gamepad>>& GetControllers() { return m_mapGameControllers; }
		std::shared_ptr<Gamepad> GetController(int index);

		bool AddGamepad(Sint32 gamepadIndex);
		bool RemoveGamepad(Sint32 gamepadID);
		void GamepadBtnPressed(const SDL_Event& event);
		void GamepadBtnReleased(const SDL_Event& event);
		void GamepadAxisValues(const SDL_Event& event);
		void GamepadHatValues(const SDL_Event& event);
		void UpdateGamepads();
	};
} // namespace VORTEK_CORE