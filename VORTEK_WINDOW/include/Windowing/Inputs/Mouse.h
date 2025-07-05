#pragma once
#include "Button.h"
#include "MouseButtons.h"
#include <map>

namespace VORTEK_WINDOWING::Inputs
{
class Mouse
{
  public:
	Mouse() = default;
	~Mouse() = default;

	void Update();
	void OnBtnPressed( int btn );
	void OnBtnReleased( int btn );

	const bool IsBtnPressed( int btn ) const;
	const bool IsBtnJustPressed( int btn ) const;
	const bool IsBtnJustReleased( int btn ) const;

	const std::tuple<int, int> GetMouseScreenPosition();

	inline void SetMouseWheelX( int wheel ) { m_WheelX = wheel; }
	inline void SetMouseWheelY( int wheel ) { m_WheelY = wheel; }
	inline void SetMouseMoving( bool moving ) { m_bMouseMoving = moving; }

	inline const int GetMouseWheelX() const { return m_WheelX; }
	inline const int GetMouseWheelY() const { return m_WheelY; }
	inline const bool IsMouseMoving() const { return m_bMouseMoving; }

  private:
	std::map<int, Button> m_mapButtons{
		{ VORTEK_MOUSE_LEFT, Button{} },
		{ VORTEK_MOUSE_MIDDLE, Button{} },
		{ VORTEK_MOUSE_RIGHT, Button{} },
	};

	int m_X{ 0 };
	int m_Y{ 0 };
	int m_WheelX{ 0 };
	int m_WheelY{ 0 };
	bool m_bMouseMoving{ false };
};
} // namespace VORTEK_WINDOWING::Inputs
