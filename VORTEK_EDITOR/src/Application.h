#pragma once
#include <memory>
#include <SDL.h>

namespace VORTEK_WINDOWING
{
	class Window;
}

namespace VORTEK_EDITOR
{
	class Application
	{
	private:
		std::unique_ptr<VORTEK_WINDOWING::Window> m_pWindow;

		SDL_Event m_Event;
		bool m_bIsRunning;

	private:
		bool Initialize();
		bool LoadShaders();
		bool LoadEditorTextures();

		void ProcessEvents();
		void Update();
		void Render();

		void SetDarkThemeColors();

		void CleanUp();

		bool CreateDisplays();
		bool InitImGui();
		void Begin();
		void End();
		void RenderImGui();
		void RegisterEditorMetaFunctions();

		Application();
		~Application() = default;
		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

	public:
		static Application& GetInstance();

		void Run();
	};
} // namespace VORTEK_EDITOR