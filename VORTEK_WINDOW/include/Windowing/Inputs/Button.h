#pragma once

namespace Vortek::Windowing::Inputs
{
struct Button
{
	bool bIsPressed{ false };
	bool bJustPressed{ false };
	bool bJustReleased{ false };

	void Update( bool bPressed );
	void Reset();
};
} // namespace Vortek::Windowing::Inputs
