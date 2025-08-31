#pragma once
#include <SDL2/SDL.h>

namespace sol
{
class state;
}

namespace VORTEK_WINDOWING
{
class Window;
}

namespace VORTEK_CORE
{
struct GameConfig;
}

namespace VORTEK_UTIL
{
enum class AssetType;
struct VEAsset;
} // namespace VORTEK_UTIL

namespace VORTEK_ENGINE
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
	std::unique_ptr<VORTEK_WINDOWING::Window> m_pWindow;
	std::unique_ptr<VORTEK_CORE::GameConfig> m_pGameConfig;
	std::unordered_map<VORTEK_UTIL::AssetType, std::vector<std::unique_ptr<VORTEK_UTIL::VEAsset>>> m_mapVEAssets;
	SDL_Event m_Event;
	bool m_bRunning;
};
} // namespace VORTEK_ENGINE
