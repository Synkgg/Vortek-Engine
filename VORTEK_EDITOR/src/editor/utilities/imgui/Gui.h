#pragma once
#include <SDL.h>

namespace VORTEK_WINDOWING
{
	class Window;
}

namespace VORTEK_EDITOR
{
	class Gui
	{
	public:
		Gui() = delete;

		static bool InitImGui(VORTEK_WINDOWING::Window* pWindow);
		static void Begin();
		static void End(VORTEK_WINDOWING::Window* pWindow);
		static void ShowImGuiDemo();
		void SetDarkThemeColors();

	private:
		inline static bool m_bInitialized{ false };
	};
} // namespace VORTEK _EDITOR