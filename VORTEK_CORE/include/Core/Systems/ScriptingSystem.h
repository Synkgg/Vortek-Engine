#pragma once
#include <sol/sol.hpp>

namespace Vortek::Core
{

struct ProjectInfo;

namespace ECS
{
class Registry;
}
} // namespace Vortek::Core

namespace Vortek::Core::Systems
{
class ScriptingSystem
{
  public:
	ScriptingSystem();
	~ScriptingSystem() = default;

	bool LoadMainScript( const std::string& sMainLuaFile, Vortek::Core::ECS::Registry& registry, sol::state& lua );
	bool LoadMainScript( Vortek::Core::ProjectInfo& projectInfo, Vortek::Core::ECS::Registry& registry, sol::state& lua );

	void Update( Vortek::Core::ECS::Registry& registry );
	void Render( Vortek::Core::ECS::Registry& registry );

	static void RegisterLuaBindings( sol::state& lua, Vortek::Core::ECS::Registry& registry );
	static void RegisterLuaFunctions( sol::state& lua, Vortek::Core::ECS::Registry& registry );
	static void RegisterLuaEvents( sol::state& lua, Vortek::Core::ECS::Registry& registry );
	static void RegisterLuaSystems( sol::state& lua, Vortek::Core::ECS::Registry& registry );

  private:
	bool m_bMainLoaded;
};

} // namespace Vortek::Core::Systems
