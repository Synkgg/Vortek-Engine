#pragma once
#include <sol/sol.hpp>
#include <string>

namespace VORTEK_CORE
{
struct SaveProject;

namespace ECS
{
class Registry;
}
}

namespace VORTEK_CORE::Systems
{
class ScriptingSystem
{
  public:
	ScriptingSystem();
	~ScriptingSystem() = default;

	bool LoadMainScript( const std::string& sMainLuaFile, VORTEK_CORE::ECS::Registry& registry, sol::state& lua );
	bool LoadMainScript( const VORTEK_CORE::SaveProject& save, VORTEK_CORE::ECS::Registry& registry, sol::state& lua );
	void Update( VORTEK_CORE::ECS::Registry& registry );
	void Render( VORTEK_CORE::ECS::Registry& registry );

	static void RegisterLuaBindings( sol::state& lua, VORTEK_CORE::ECS::Registry& registry );
	static void RegisterLuaFunctions( sol::state& lua, VORTEK_CORE::ECS::Registry& registry );
	static void RegisterLuaEvents( sol::state& lua, VORTEK_CORE::ECS::Registry& registry );
	static void RegisterLuaSystems( sol::state& lua, VORTEK_CORE::ECS::Registry& registry );

  private:
	bool m_bMainLoaded;
};

} // namespace VORTEK_CORE::Systems
