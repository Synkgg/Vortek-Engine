#include "Gamepad.h"
#include <Logger/Logger.h>

namespace VORTEK_WINDOWING::Inputs {

    Gamepad::Gamepad(Controller controller)
        : m_pController{ std::move(controller) }
        , m_mapButtons{
            {VORTEK_GP_BTN_A, Button{}},
            {VORTEK_GP_BTN_B, Button{}},
            {VORTEK_GP_BTN_X, Button{}},
            {VORTEK_GP_BTN_Y, Button{}},
            {VORTEK_GP_BTN_BACK, Button{}},
            {VORTEK_GP_BTN_GUIDE, Button{}},
            {VORTEK_GP_BTN_START, Button{}},
            {VORTEK_GP_BTN_LSTICK, Button{}},
            {VORTEK_GP_BTN_RSTICK, Button{}},
            {VORTEK_GP_BTN_LSHOULDER, Button{}},
            {VORTEK_GP_BTN_RSHOULDER, Button{}},
            {VORTEK_GP_BTN_DPAD_UP, Button{}},
            {VORTEK_GP_BTN_DPAD_DOWN, Button{}},
            {VORTEK_GP_BTN_DPAD_LEFT, Button{}},
            {VORTEK_GP_BTN_DPAD_RIGHT, Button{}},
        }
        , m_InstanceID{ -1 }
        , m_mapAxisValues{
            {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}
        }
        , m_JoystickHatValue{ VORTEK_HAT_CENTERED }
    {
        SDL_Joystick* joystick = SDL_GameControllerGetJoystick(m_pController.get());
        if (!m_pController || !joystick)
            throw("Controller or Joystick were nullptr!");

        m_InstanceID = SDL_JoystickInstanceID(joystick);
        m_sName = std::string{ SDL_JoystickName(joystick) };

        auto num_axis = SDL_JoystickNumAxes(joystick);
        auto num_balls = SDL_JoystickNumBalls(joystick);
        auto num_hats = SDL_JoystickNumHats(joystick);
        auto num_buttons = SDL_JoystickNumButtons(joystick);
    }

    void Gamepad::Update()
    {
        for (auto& [btn, button] : m_mapButtons)
            button.Reset();
    }

    void Gamepad::OnBtnPressed(int btn)
    {
        if (btn == VORTEK_GP_BTN_UNKNOWN)
        {
            VORTEK_ERROR("Gamepad button [{}] is unknown!", btn);
            return;
        }

        auto btnItr = m_mapButtons.find(btn);
        if (btnItr == m_mapButtons.end())
        {
            VORTEK_ERROR("Gamepad button [{}] does not exist!", btn);
            return;
        }

        btnItr->second.Update(true);
    }

    void Gamepad::OnBtnReleased(int btn)
    {
        if (btn == VORTEK_GP_BTN_UNKNOWN)
        {
            VORTEK_ERROR("Gamepad button [{}] is unknown!", btn);
            return;
        }

        auto btnItr = m_mapButtons.find(btn);
        if (btnItr == m_mapButtons.end())
        {
            VORTEK_ERROR("Gamepad button [{}] does not exist!", btn);
            return;
        }

        btnItr->second.Update(false);
    }

    const bool Gamepad::IsBtnPressed(int btn) const
    {
        if (btn == VORTEK_GP_BTN_UNKNOWN)
        {
            VORTEK_ERROR("Gamepad button [{}] is unknown!", btn);
            return false;
        }

        auto btnItr = m_mapButtons.find(btn);
        if (btnItr == m_mapButtons.end())
        {
            VORTEK_ERROR("Gamepad button [{}] does not exist!", btn);
            return false;
        }

        return btnItr->second.bIsPressed;
    }

    const bool Gamepad::IsBtnJustPressed(int btn) const
    {
        if (btn == VORTEK_GP_BTN_UNKNOWN)
        {
            VORTEK_ERROR("Gamepad button [{}] is unknown!", btn);
            return false;
        }

        auto btnItr = m_mapButtons.find(btn);
        if (btnItr == m_mapButtons.end())
        {
            VORTEK_ERROR("Gamepad button [{}] does not exist!", btn);
            return false;
        }

        return btnItr->second.bJustPressed;
    }

    const bool Gamepad::IsBtnJustReleased(int btn) const
    {
        if (btn == VORTEK_GP_BTN_UNKNOWN)
        {
            VORTEK_ERROR("Gamepad button [{}] is unknown!", btn);
            return false;
        }

        auto btnItr = m_mapButtons.find(btn);
        if (btnItr == m_mapButtons.end())
        {
            VORTEK_ERROR("Gamepad button [{}] does not exist!", btn);
            return false;
        }

        return btnItr->second.bJustReleased;
    }

    const bool Gamepad::IsGamepadPresent() const
    {
        return m_pController != nullptr && SDL_NumJoysticks() > 0;
    }

    const Sint16 Gamepad::GetAxisPosition(Uint8 axis)
    {
        auto axisItr = m_mapAxisValues.find(axis);
        if (axisItr == m_mapAxisValues.end())
        {
            VORTEK_ERROR("Axis [{}] does not exist!", axis);
            return 0;
        }

        return axisItr->second;
    }

    void Gamepad::SetAxisPositionValue(Uint8 axis, Sint16 value)
    {
        auto axisItr = m_mapAxisValues.find(axis);
        if (axisItr == m_mapAxisValues.end())
        {
            VORTEK_ERROR("Axis [{}] does not exist!", axis);
            return;
        }

        axisItr->second = value;
    }
}