#include "Windowing/Inputs/Button.h"

void VORTEK_WINDOWING::Inputs::Button::Update( bool bPressed )
{
	bJustPressed = !bIsPressed && bPressed;
	bJustReleased = bIsPressed && !bPressed;
	bIsPressed = bPressed;
}

void VORTEK_WINDOWING::Inputs::Button::Reset()
{
	bJustPressed = false;
	bJustReleased = false;
}
