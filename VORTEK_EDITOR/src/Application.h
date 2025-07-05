#pragma once
#include <memory>
#include <SDL.h>

namespace VORTEK_WINDOWING
{
class Window;
}

namespace VORTEK_EDITOR::Events
{
struct CloseEditorEvent;
}

namespace VORTEK_EDITOR
{
class Application
{
  private:
	std::unique_ptr<VORTEK_WINDOWING::Window> m_pWindow;
	std::unique_ptr<class Hub> m_pHub;

	SDL_Event m_Event;
	bool m_bIsRunning;

  private:
	bool Initialize();
	bool InitApp();
	bool LoadShaders();
	bool LoadEditorTextures();

	void ProcessEvents();
	void Update();
	void UpdateInputs();
	void Render();

	void CleanUp();
	bool CreateDisplays();

	void InitDisplays();
	void RenderDisplays();

	void RegisterEditorMetaFunctions();

	void OnCloseEditor( VORTEK_EDITOR::Events::CloseEditorEvent& close );

	Application();
	~Application() = default;
	Application( const Application& ) = delete;
	Application& operator=( const Application& ) = delete;

  public:
	static Application& GetInstance();

	void Run();
};
} // namespace VORTEK_EDITOR
