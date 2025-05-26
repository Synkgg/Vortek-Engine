#include "Window.h"
#include <iostream>
#include <Logger.h>

namespace VORTEK_WINDOWING {

	void Window::CreateNewWindow(Uint32 flags)
	{
		m_pWindow = WindowPtr(SDL_CreateWindow(
			m_sTitle.c_str(),
			m_XPos, m_YPos, m_Width, m_Height, flags
		));

		// Check to see if the window was created correctly
		if (!m_pWindow)
		{
			std::string error = SDL_GetError();
			VORTEK_ERROR("Failed to create the Window: {}", error);
		}
	}

	Window::Window()
		: Window("default_window", 640, 480, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, true, 0)
	{
	}

	Window::Window(const std::string title, int width, int height, int x_pos, int y_pos, bool v_sync, Uint32 flags)
		: m_pWindow{ nullptr }, m_GLContext{}, m_sTitle{ title }
		, m_Width{ width }, m_Height{ height }, m_XPos{ x_pos }, m_YPos{ y_pos }, m_WindowFlags{ flags }
	{
		CreateNewWindow(flags);

		// Enable v-sync
		if (v_sync)
		{
			if (!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1"))
				VORTEK_ERROR("Failed to enable VSYNC!");
		}

		VORTEK_LOG("Window Created Successfully!");
	}

	void Window::SetPosition(int x, int y)
	{
		SDL_SetWindowPosition(m_pWindow.get(), x, y);
		m_XPos = x;
		m_YPos = y;
	}

	void Window::SetSize(int width, int height)
	{
		m_Width = width;
		m_Height = height;
	}

	void Window::SetWindowName(const std::string& name)
	{
		m_sTitle = name;
		SDL_SetWindowTitle(m_pWindow.get(), name.c_str());
	}
}