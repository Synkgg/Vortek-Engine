#include "Core/Scene/SceneManager.h"
#include "Core/Scene/Scene.h"

#include "VORTEKUtilities/VORTEKUtilities.h"
#include "Core/ECS/Components/AllComponents.h"

using namespace VORTEK_CORE::ECS;

namespace VORTEK_CORE
{

SceneManager::SceneManager()
	: m_mapScenes{}
	, m_sCurrentScene{}
	, m_sCurrentTileset{}
{
}

bool SceneManager::AddScene( const std::string& sSceneName, VORTEK_CORE::EMapType eType )
{
	if ( m_mapScenes.contains( sSceneName ) )
	{
		VORTEK_ERROR( "Failed to add new scene object - [{}] already exists.", sSceneName );
		return false;
	}

	auto [ itr, bSuccess ] =
		m_mapScenes.emplace( sSceneName, std::move( std::make_shared<Scene>( sSceneName, eType ) ) );
	return bSuccess;
}

bool SceneManager::HasScene( const std::string& sSceneName )
{
	return m_mapScenes.contains( sSceneName );
}

Scene* SceneManager::GetScene( const std::string& sSceneName )
{
	auto sceneItr = m_mapScenes.find( sSceneName );
	if ( sceneItr == m_mapScenes.end() )
	{
		VORTEK_ERROR( "Failed to get scene object - [{}] does not exist.", sSceneName );
		return nullptr;
	}

	return sceneItr->second.get();
}

Scene* SceneManager::GetCurrentScene()
{
	if ( m_sCurrentScene.empty() )
		return nullptr;

	auto sceneItr = m_mapScenes.find( m_sCurrentScene );
	if ( sceneItr == m_mapScenes.end() )
	{
		VORTEK_ERROR( "Failed to get scene object - [{}] does not exist.", m_sCurrentScene );
		return nullptr;
	}

	return sceneItr->second.get();
}

std::vector<std::string> SceneManager::GetSceneNames() const
{
	return VORTEK_UTIL::GetKeys( m_mapScenes );
}

bool SceneManager::LoadCurrentScene()
{
	if ( auto pCurrentScene = GetCurrentScene() )
	{
		return pCurrentScene->LoadScene();
	}

	return false;
}

bool SceneManager::UnloadCurrentScene()
{
	if ( auto pCurrentScene = GetCurrentScene() )
	{
		return pCurrentScene->UnloadScene();
	}

	return false;
}

bool SceneManager::CheckHasScene( const std::string& sSceneName )
{
	return m_mapScenes.contains( sSceneName );
}

bool SceneManager::ChangeSceneName( const std::string& sOldName, const std::string& sNewName )
{
	return VORTEK_UTIL::KeyChange( m_mapScenes, sOldName, sNewName );
}

void SceneManager::CreateLuaBind( sol::state& lua, SceneManager& sceneManager )
{
	lua.new_usertype<SceneManager>(
		"SceneManager",
		sol::no_constructor,
		"changeScene",
		// TODO: This will still need testing once the runtime has been created.
		[ & ]( const std::string& sSceneName ) {
			auto pCurrentScene = sceneManager.GetCurrentScene();
			if ( !pCurrentScene )
			{
				VORTEK_ERROR( "Failed to change to scene [{}] - Current scene is invalid.", sSceneName );
				return false;
			}

			if ( pCurrentScene->GetSceneName() == sSceneName )
			{
				VORTEK_ERROR( "Failed to load scene [{}] - Scene has already been loaded.", sSceneName );
				return false;
			}

			auto pScene = sceneManager.GetScene( sSceneName );
			if ( !pScene )
			{
				VORTEK_ERROR( "Failed to change to scene [{}] - Scene [{}] is invalid.", sSceneName, sSceneName );
				return false;
			}

			pCurrentScene->GetRegistry().DestroyEntities<ScriptComponent>();
			pCurrentScene->UnloadScene();

			if ( !pScene->IsLoaded() )
			{
				pScene->LoadScene();
			}

			sceneManager.SetCurrentScene( sSceneName );

			return true;
		},
		"getCanvas", // Returns the canvas of the current scene or an empty canvas object.
		[ & ] {
			if ( auto pCurrentScene = sceneManager.GetCurrentScene() )
				return pCurrentScene->GetCanvas();

			return VORTEK_CORE::Canvas{};
		},
		"getDefaultMusic",
		[ & ] {
			if ( auto pCurrentScene = sceneManager.GetCurrentScene() )
				return pCurrentScene->GetDefaultMusicName();

			return std::string{};
		},
		"getCurrentSceneName",
		[ & ] { return sceneManager.GetCurrentSceneName(); } );
}
} // namespace VORTEK_CORE
