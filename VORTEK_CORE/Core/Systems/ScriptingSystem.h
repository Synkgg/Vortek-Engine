#pragma once
#include <sol/sol.hpp>
#include <string>

namespace VORTEK_CORE::ECS
{
	class Registry;
}

namespace VORTEK_CORE::Systems
{
	class ScriptingSystem
	{
	private:
		bool m_bMainLoaded;

	public:
		ScriptingSystem();
		~ScriptingSystem() = default;

		bool LoadMainScript(const std::string& sMainLuaFile, VORTEK_CORE::ECS::Registry& registry, sol::state& lua);
		void Update(VORTEK_CORE::ECS::Registry& registry);
		void Render(VORTEK_CORE::ECS::Registry& registry);

		static void RegisterLuaBindings(sol::state& lua, VORTEK_CORE::ECS::Registry& registry);
		static void RegisterLuaFunctions(sol::state& lua, VORTEK_CORE::ECS::Registry& registry);
	};

} // namespace VORTEK_CORE::Systems