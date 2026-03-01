#pragma once
#include <sol/sol.hpp>
#include "Core/Scene/SceneManager.h"

#define SCENE_MANAGER() Vortek::Editor::EditorSceneManager::GetInstance()
#define COMMAND_MANAGER() SCENE_MANAGER().GetCommandManager()
#define TOOL_MANAGER() SCENE_MANAGER().GetToolManager()

namespace Vortek::Core
{
enum class EMapType;
namespace Events
{
class EventDispatcher;
}
} // namespace Vortek::Core

namespace Vortek::Editor
{
class ToolManager;
class SceneObject;
class CommandManager;

class EditorSceneManager : public Vortek::Core::SceneManager
{
  public:
	static EditorSceneManager& GetInstance();

	virtual bool AddScene( const std::string& sSceneName, Vortek::Core::EMapType eType ) override;
	bool AddSceneObject( const std::string& sSceneName, const std::string& sSceneData );
	bool DeleteScene( const std::string& sSceneName );

	ToolManager& GetToolManager();
	CommandManager& GetCommandManager();
	Vortek::Core::Events::EventDispatcher& GetDispatcher();

	void SetTileset( const std::string& sTileset );

	SceneObject* GetCurrentSceneObject();

	bool SaveAllScenes();

	bool CheckTagName( const std::string& sTagName );

	void UpdateScenes();

	std::string GetSceneFilepath( const std::string& sSceneName );

	inline const std::map<std::string, std::shared_ptr<Vortek::Core::Scene>>& GetAllScenes() const { return m_mapScenes; }

	inline const std::string& GetCurrentTileset() const { return m_sCurrentTileset; }

	static void CreateSceneManagerLuaBind( sol::state& lua );

  private:
	std::unique_ptr<ToolManager> m_pToolManager{ nullptr };
	std::unique_ptr<CommandManager> m_pCommandManager{ nullptr };

	std::unique_ptr<Vortek::Core::Events::EventDispatcher> m_pSceneDispatcher{ nullptr };

  private:
	EditorSceneManager();
	virtual ~EditorSceneManager() = default;
	EditorSceneManager( const EditorSceneManager& ) = delete;
	EditorSceneManager& operator=( const EditorSceneManager& ) = delete;
};
} // namespace Vortek::Editor

#define ADD_SWE_HANDLER( Event, Func, Handler )                                                                        \
	{                                                                                                                  \
		for ( auto& pDispatcher : TOOL_MANAGER().GetDispatchers() )                                                    \
		{                                                                                                              \
			if ( !pDispatcher )                                                                                        \
				continue;                                                                                              \
			pDispatcher->AddHandler<Event, Func>( Handler );                                                           \
		}                                                                                                              \
	}
