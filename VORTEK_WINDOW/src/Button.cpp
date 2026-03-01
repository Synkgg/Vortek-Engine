#include "Windowing/Inputs/Button.h"

void Vortek::Windowing::Inputs::Button::Update( bool bPressed )
{
	bJustPressed = !bIsPressed && bPressed;
	bJustReleased = bIsPressed && !bPressed;
	bIsPressed = bPressed;
}

void Vortek::Windowing::Inputs::Button::Reset()
{
	bJustPressed = false;
	bJustReleased = false;
}
