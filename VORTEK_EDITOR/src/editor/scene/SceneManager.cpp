#include "SceneManager.h"
#include "SceneObject.h"
#include "VORTEKUtilities/VORTEKUtilities.h"
#include "editor/tools/ToolManager.h"
#include "editor/tools/TileTool.h"
#include "editor/tools/gizmos/Gizmo.h"
#include "editor/commands/CommandManager.h"
#include "editor/loaders/ProjectLoader.h"

#include "Core/Events/EventDispatcher.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/CoreUtilities/SaveProject.h"
#include "Core/CoreUtilities/CoreUtilities.h"

#include "Logger/Logger.h"

#include <filesystem>

namespace fs = std::filesystem;

using namespace VORTEK_CORE::ECS;

namespace VORTEK_EDITOR
{
EditorSceneManager& EditorSceneManager::GetInstance()
{
	static EditorSceneManager instance{};
	return instance;
}

bool EditorSceneManager::AddScene( const std::string& sSceneName, VORTEK_CORE::EMapType eType )
{
	if ( m_mapScenes.contains( sSceneName ) )
	{
		VORTEK_ERROR( "Failed to add new scene object - [{}] already exists.", sSceneName );
		return false;
	}

	auto [ itr, bSuccess ] =
		m_mapScenes.emplace( sSceneName, std::move( std::make_shared<SceneObject>( sSceneName, eType ) ) );
	return bSuccess;
}

bool EditorSceneManager::AddSceneObject( const std::string& sSceneName, const std::string& sSceneData )
{
	if ( m_mapScenes.contains( sSceneName ) )
	{
		VORTEK_ERROR( "Failed to add new scene object - [{}] already exists.", sSceneName );
		return false;
	}

	auto [ itr, bSuccess ] =
		m_mapScenes.emplace( sSceneName, std::move( std::make_shared<SceneObject>( sSceneName, sSceneData ) ) );
	return bSuccess;
}

bool EditorSceneManager::DeleteScene( const std::string& sSceneName )
{
	auto sceneItr = m_mapScenes.find( sSceneName );
	if ( sceneItr == m_mapScenes.end() )
	{
		VORTEK_ERROR( "Failed to delete scene [{}] - Does not exist in the scene manager.", sSceneName );
		return false;
	}

	// Check to see if the scene is loaded. We do not want to delete a loaded scene.
	if ( sceneItr->second->IsLoaded() )
	{
		VORTEK_ERROR( "Failed to delete scene [{}] - Loaded scenes cannot be delete. Please unload the scene if you "
					  "want to delete it.",
					  sSceneName );
		return false;
	}

	const std::string& sDataPath{ sceneItr->second->GetSceneDataPath() };
	fs::path dataPath{ sDataPath };

	if ( fs::exists( dataPath.parent_path() ) && fs::is_directory( dataPath.parent_path() ) )
	{
		if ( dataPath.parent_path().stem().string() == sSceneName )
		{
			std::error_code ec;
			if ( !fs::remove_all( dataPath.parent_path(), ec ) )
			{
				VORTEK_ERROR( "Failed to delete scene [{}] and remove files.", sSceneName, ec.message() );
				return false;
			}
		}
	}

	// Recheck if the path exists
	if ( fs::exists( dataPath.parent_path() ) )
	{
		VORTEK_ERROR( "Failed to delete scene [{}] and remove files.", sSceneName );
		return false;
	}

	if ( m_mapScenes.erase( sSceneName ) > 0 )
	{
		auto& pSaveProject = MAIN_REGISTRY().GetContext<std::shared_ptr<VORTEK_CORE::SaveProject>>();
		VORTEK_ASSERT( pSaveProject && "Save Project must exist!" );
		// Save entire project
		ProjectLoader pl{};
		if ( !pl.SaveLoadedProject( *pSaveProject ) )
		{
			VORTEK_ERROR(
				"Failed to save project [{}] at file [{}] after deleting scene. Please ensure the scene files "
				"have been removed.",
				pSaveProject->sProjectName,
				pSaveProject->sProjectFilePath );

			return false;
		}
	}

	return true;
}

ToolManager& EditorSceneManager::GetToolManager()
{
	if ( !m_pToolManager )
		m_pToolManager = std::make_unique<ToolManager>();

	VORTEK_ASSERT( m_pToolManager && "Tool manager must be valid" );

	return *m_pToolManager;
}

CommandManager& EditorSceneManager::GetCommandManager()
{
	if ( !m_pCommandManager )
		m_pCommandManager = std::make_unique<CommandManager>();

	VORTEK_ASSERT( m_pCommandManager && "Command manager must be valid" );

	return *m_pCommandManager;
}

VORTEK_CORE::Events::EventDispatcher& EditorSceneManager::GetDispatcher()
{
	if ( !m_pSceneDispatcher )
		m_pSceneDispatcher = std::make_unique<VORTEK_CORE::Events::EventDispatcher>();

	VORTEK_ASSERT( m_pSceneDispatcher && "Event Dispatcher must be valid" );

	return *m_pSceneDispatcher;
}

void EditorSceneManager::SetTileset( const std::string& sTileset )
{
	m_sCurrentTileset = sTileset;

	if ( !m_pToolManager )
		return;

	m_pToolManager->SetToolsCurrentTileset( sTileset );
}

SceneObject* EditorSceneManager::GetCurrentSceneObject()
{
	if ( auto pCurrentScene = dynamic_cast<SceneObject*>( GetCurrentScene() ) )
		return pCurrentScene;

	return nullptr;
}

bool EditorSceneManager::SaveAllScenes()
{
	bool bSuccess{ true };
	for ( const auto& [ sName, pScene ] : m_mapScenes )
	{
		if ( !pScene->SaveScene() )
		{
			VORTEK_ERROR( "Failed to save scene [{}]", sName );
			bSuccess = false;
		}
	}

	return bSuccess;
}

bool EditorSceneManager::CheckTagName( const std::string& sTagName )
{
	if ( auto pScene = GetCurrentScene() )
	{
		auto pSceneObject = dynamic_cast<SceneObject*>( pScene );
		return pSceneObject->CheckTagName( sTagName );
	}

	return false;
}

void EditorSceneManager::UpdateScenes()
{
	if ( auto pCurrentScene = GetCurrentSceneObject() )
	{
		VORTEK_CORE::UpdateDirtyEntities( pCurrentScene->GetRegistry() );
		VORTEK_CORE::UpdateDirtyEntities( pCurrentScene->GetRuntimeRegistry() );
	}
}

EditorSceneManager::EditorSceneManager()
	: VORTEK_CORE::SceneManager()
{
}

void EditorSceneManager::CreateSceneManagerLuaBind( sol::state& lua )
{
	auto& sceneManager = SCENE_MANAGER();

	// clang-format off
		lua.new_usertype<EditorSceneManager>(
			"SceneManager",
			sol::no_constructor,
			"changeScene",
			[&](const std::string& sSceneName) {
				auto pCurrentScene = sceneManager.GetCurrentSceneObject();
				if (!pCurrentScene)
				{
					VORTEK_ERROR("Failed to change to scene [{}] - Current scene is invalid.", sSceneName);
					return false;
				}

				auto* pRuntimeData = pCurrentScene->GetRuntimeData();
				VORTEK_ASSERT(pRuntimeData && "Runtime Data was not initialized.");
				if (pRuntimeData->sSceneName == sSceneName)
				{
					VORTEK_ERROR("Failed to load scene [{}] - Scene has already been loaded.", sSceneName);
					return false;
				}

				auto pScene = sceneManager.GetScene(sSceneName);
				if (!pScene)
				{
					VORTEK_ERROR("Failed to change to scene [{}] - Scene [{}] is invalid.", sSceneName, sSceneName);
					return false;
				}

				pCurrentScene->GetRuntimeRegistry().DestroyEntities<ScriptComponent>();
				if (!pScene->IsLoaded())
				{
					pScene->LoadScene();
				}

				auto pSceneObject = dynamic_cast<SceneObject*>(pScene);
				VORTEK_ASSERT(pSceneObject && "Scene Must be a valid Scene Object If run in the editor!");
				if (!pSceneObject)
				{
					VORTEK_ERROR("Failed to load scene [{}] - Scene is not a valid SceneObject.", sSceneName);

					return pScene->UnloadScene(false);
				}

				pCurrentScene->CopySceneToRuntime(*pSceneObject);

				return pScene->UnloadScene(false);
			},
			"getCanvas", // Returns the canvas of the current scene or an empty canvas object.
			[&] {
				if (auto pCurrentScene = sceneManager.GetCurrentSceneObject())
				{
					auto* pRuntimeData = pCurrentScene->GetRuntimeData();
					return pRuntimeData ? pRuntimeData->canvas : VORTEK_CORE::Canvas{};
				}

				return VORTEK_CORE::Canvas{};
			},
			"getDefaultMusic",
			[&] {
				if (auto pCurrentScene = sceneManager.GetCurrentSceneObject())
				{
					auto* pRuntimeData = pCurrentScene->GetRuntimeData();
					return pRuntimeData ? pRuntimeData->sDefaultMusic : "";
				}

				return std::string{};
			},
			"getCurrentSceneName", [&]
			{
				if (auto pCurrentScene = sceneManager.GetCurrentSceneObject())
				{
					auto* pRuntimeData = pCurrentScene->GetRuntimeData();
					return pRuntimeData ? pRuntimeData->sSceneName : "";
				}

				return std::string{};
			}
		);
	// clang-format on
}

} // namespace VORTEK_EDITOR
