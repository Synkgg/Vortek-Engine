#pragma once
#include <SDL2/SDL.h>

namespace sol
{
class state;
}

namespace Vortek::Windowing
{
class Window;
}

namespace Vortek::Core
{
struct GameConfig;
}

namespace Vortek::Utilities
{
enum class AssetType;
struct Asset;
} // namespace Vortek::Utilities

namespace Vortek::Engine
{
class RuntimeApp
{
  public:
	RuntimeApp();
	~RuntimeApp();

	void Run();

  private:
	void Initialize();

	bool LoadShaders();
	bool LoadConfig( sol::state& lua );
	bool LoadRegistryContext();
	void LoadBindings();
	bool LoadScripts();
	bool LoadPhysics();
	bool LoadZip();

	void ProcessEvents();
	void Update();
	void Render();

	void CleanUp();

  private:
	std::unique_ptr<Vortek::Windowing::Window> m_pWindow;
	std::unique_ptr<Vortek::Core::GameConfig> m_pGameConfig;
	std::unordered_map<Vortek::Utilities::AssetType, std::vector<std::unique_ptr<Vortek::Utilities::Asset>>> m_mapVEAssets;
	SDL_Event m_Event;
	bool m_bRunning;
	/*
	   Hack to deal with a change in allocated channels.
	   The channels are automatically allocated in the music player in
	   the main registry; however, the game config could have more or less
	   channels to allocate. This needs to be done after the config and registry
	   have already been initialized.
	*/
	int m_DeltaAllocatedChannels;
};
} // namespace Vortek::Engine
