#include "Core/Scene/Scene.h"
#include "Core/Loaders/TilemapLoader.h"

#include "VORTEKUtilities/VORTEKUtilities.h"
#include "VortekFilesystem/Serializers/JSONSerializer.h"

#include "Core/CoreUtilities/SaveProject.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/MetaUtilities.h"
#include "Core/ECS/MainRegistry.h"

#include "Core/Character/Character.h"
#include "Core/CoreUtilities/Prefab.h"

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <format>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

using namespace VORTEK_FILESYSTEM;
using namespace VORTEK_CORE::Loaders;

namespace VORTEK_CORE
{
Scene::Scene()
	: m_Registry{}
	, m_sSceneName{}
	, m_sTilemapPath{}
	, m_sObjectPath{}
	, m_sSceneDataPath{}
	, m_sDefaultMusic{}
	, m_bSceneLoaded{ false }
	, m_bUsePlayerStart{ false }
	, m_Canvas{}
	, m_eMapType{ EMapType::Grid }
	, m_PlayerStart{ m_Registry, *this }
{
	// Empty Scene
}

Scene::Scene( const std::string& sceneName, EMapType eType )
	: m_Registry{}
	, m_sSceneName{ sceneName }
	, m_sTilemapPath{}
	, m_sObjectPath{}
	, m_sSceneDataPath{}
	, m_sDefaultMusic{}
	, m_bSceneLoaded{ false }
	, m_bUsePlayerStart{ false }
	, m_Canvas{}
	, m_eMapType{ eType }
	, m_PlayerStart{ m_Registry, *this }
{
	auto& pSaveProject = MAIN_REGISTRY().GetContext<std::shared_ptr<VORTEK_CORE::SaveProject>>();
	VORTEK_ASSERT( pSaveProject && "SaveProject must exists here!" );

	std::string sScenePath = std::format( "{}content{}assets{}scenes{}{}",
										  pSaveProject->sProjectPath,
										  PATH_SEPARATOR,
										  PATH_SEPARATOR,
										  PATH_SEPARATOR,
										  m_sSceneName );

	if ( fs::exists( sScenePath ) )
	{
		VORTEK_ERROR( "SCENE ALREADY EXISTS!" );
	}

	std::error_code ec;
	if ( !fs::create_directory( fs::path{ sScenePath }, ec ) )
	{
		VORTEK_ERROR( "Failed to create scene directory [{}] - Error: ", ec.message() );
	}

	m_sTilemapPath = sScenePath + PATH_SEPARATOR + m_sSceneName + "_tilemap.json";
	m_sObjectPath = sScenePath + PATH_SEPARATOR + m_sSceneName + "_objects.json";
	m_sSceneDataPath = sScenePath + PATH_SEPARATOR + m_sSceneName + "_scene_data.json";

	// Create the files
	std::fstream tilemap{};
	tilemap.open( m_sTilemapPath, std::ios::out | std::ios::trunc );
	VORTEK_ASSERT( tilemap.is_open() && "File should have been created and opened." );
	tilemap.close();

	std::fstream objectmap{};
	objectmap.open( m_sObjectPath, std::ios::out | std::ios::trunc );
	VORTEK_ASSERT( objectmap.is_open() && "File should have been created and opened." );
	objectmap.close();

	std::fstream sceneData{};
	sceneData.open( m_sSceneDataPath, std::ios::out | std::ios::trunc );
	VORTEK_ASSERT( sceneData.is_open() && "File should have been created and opened." );
	sceneData.close();

	SaveSceneData(true);
}

bool Scene::LoadScene()
{
	VORTEK_LOG( "LOADED SCENE" );
	if ( m_bSceneLoaded )
	{
		VORTEK_ERROR( "Scene [{}] has already been loaded", m_sSceneName );
		return false;
	}

	if ( !LoadSceneData() )
	{
		VORTEK_ERROR( "Failed to load scene data" );
		return false;
	}

	// Try to load the tilemap and object maps
	auto pTilemapLoader = std::make_unique<TilemapLoader>();
	if ( !pTilemapLoader->LoadTilemap( m_Registry, m_sTilemapPath, true ) )
	{
	}

	// Load scene game objects
	if ( !pTilemapLoader->LoadGameObjects( m_Registry, m_sObjectPath, true ) )
	{
	}

	m_bSceneLoaded = true;
	VORTEK_LOG( "Loaded Scene: {}", m_sSceneName );
	return true;
}

bool Scene::UnloadScene( bool bSaveScene )
{
	if ( !m_bSceneLoaded )
	{
		VORTEK_ERROR( "Scene [{}] has not been loaded", m_sSceneName );
		return false;
	}

	if ( bSaveScene && !SaveSceneData() )
	{
		VORTEK_ERROR( "Failed to unload scene data" );
		return false;
	}

	// Remove all objects in registry
	m_PlayerStart.Unload();
	m_Registry.ClearRegistry();
	m_bSceneLoaded = false;

	return true;
}

int Scene::AddLayer( const std::string& sLayerName, bool bVisible )
{
	if ( !CheckLayerName( sLayerName ) )
	{
		// VORTEK_ERROR( "Failed to add layer [{}] - Already exists.", sLayerName );
		return static_cast<int>( m_LayerParams.size() );
	}

	m_LayerParams.emplace_back( VORTEK_UTIL::SpriteLayerParams{ .sLayerName = sLayerName, .bVisible = bVisible } );
	return static_cast<int>( m_LayerParams.size() );
}

bool Scene::CheckLayerName( const std::string& sLayerName )
{
	return VORTEK_UTIL::CheckContainsValue( m_LayerParams, [ & ]( VORTEK_UTIL::SpriteLayerParams& spriteLayer ) {
		return spriteLayer.sLayerName == sLayerName;
	} );
}

bool Scene::LoadSceneData()
{
	std::error_code ec;
	if ( !fs::exists( m_sSceneDataPath, ec ) )
	{
		VORTEK_ERROR( "Failed to load scene data. Error: {}", ec.message() );
		return false;
	}

	std::ifstream sceneDataFile;
	sceneDataFile.open( m_sSceneDataPath );

	if ( !sceneDataFile.is_open() )
	{
		VORTEK_ERROR( "Failed to open tilemap file [{}]", m_sSceneDataPath );
		return false;
	}

	// The sceneData file could be empty if just created
	if ( sceneDataFile.peek() == std::ifstream::traits_type::eof() )
	{
		// If the sceneData is an empty file, return true. must not have made any changes yet.
		return true;
	}

	std::stringstream ss;
	ss << sceneDataFile.rdbuf();
	std::string contents = ss.str();
	rapidjson::StringStream jsonStr{ contents.c_str() };

	rapidjson::Document doc;
	doc.ParseStream( jsonStr );

	if ( doc.HasParseError() || !doc.IsObject() )
	{
		VORTEK_ERROR( "Failed to load tilemap: File: [{}] is not valid JSON. - {} - {}",
					  m_sSceneDataPath,
					  rapidjson::GetParseError_En( doc.GetParseError() ),
					  doc.GetErrorOffset() );
		return false;
	}

	VORTEK_ASSERT( doc.HasMember( "scene_data" ) && "scene_data member is necessary." );

	const rapidjson::Value& sceneData = doc[ "scene_data" ];

	auto& pSaveProject = MAIN_REGISTRY().GetContext<std::shared_ptr<SaveProject>>();
	VORTEK_ASSERT( pSaveProject && "SaveProject must exists here!" );

	std::string sScenePath = std::format( "{}content{}", pSaveProject->sProjectPath, PATH_SEPARATOR );

	if ( m_sTilemapPath.empty() )
	{
		m_sTilemapPath = sScenePath + sceneData[ "tilemapPath" ].GetString();
	}

	if ( m_sObjectPath.empty() )
	{
		m_sObjectPath = sScenePath + sceneData[ "objectmapPath" ].GetString();
	}

	if ( sceneData.HasMember( "canvas" ) )
	{
		const rapidjson::Value& canvas = sceneData[ "canvas" ];

		m_Canvas.width = canvas[ "width" ].GetInt();
		m_Canvas.height = canvas[ "height" ].GetInt();
		m_Canvas.tileWidth = canvas[ "tileWidth" ].GetInt();
		m_Canvas.tileHeight = canvas[ "tileHeight" ].GetInt();
	}

	if ( sceneData.HasMember( "mapType" ) )
	{
		std::string sMapType = sceneData[ "mapType" ].GetString();
		if ( sMapType == "grid" )
		{
			m_eMapType = VORTEK_CORE::EMapType::Grid;
		}
		else if ( sMapType == "iso" )
		{
			m_eMapType = VORTEK_CORE::EMapType::IsoGrid;
			SetCanvasOffset();
		}
	}

	if ( sceneData.HasMember( "playerStart" ) )
	{
		const rapidjson::Value& playerStart = sceneData[ "playerStart" ];
		if ( playerStart.HasMember( "enabled" ) )
		{
			m_bUsePlayerStart = playerStart[ "enabled" ].GetBool();
		}
		else
		{
			m_bUsePlayerStart = false;
		}

		std::string sPlayerStartPrefab = sceneData[ "playerStart" ][ "character" ].GetString();
		if ( sPlayerStartPrefab != "default" )
		{
			m_PlayerStart.Load( sPlayerStartPrefab );
		}
		else if ( m_bUsePlayerStart && !m_PlayerStart.IsPlayerStartCreated() )
		{
			m_PlayerStart.LoadVisualEntity();
		}

		// Do not set the position if we are not using the player start.
		if ( m_bUsePlayerStart )
		{
			m_PlayerStart.SetPosition( glm::vec2{ sceneData[ "playerStart" ][ "position" ][ "x" ].GetFloat(),
												  sceneData[ "playerStart" ][ "position" ][ "y" ].GetFloat() } );
		}
	}

	if ( sceneData.HasMember( "defaultMusic" ) )
	{
		m_sDefaultMusic = sceneData[ "defaultMusic" ].GetString();
	}

	VORTEK_ASSERT( sceneData.HasMember( "sprite_layers" ) && "Sprite layers must be a part of scene data" );
	const rapidjson::Value& spriteLayers = sceneData[ "sprite_layers" ];
	for ( const auto& layer : spriteLayers.GetArray() )
	{
		std::string sLayerName = layer[ "layerName" ].GetString();
		bool bVisible = layer[ "bVisible" ].GetBool();

		AddLayer( sLayerName, bVisible );
	}

	return true;
}

bool Scene::SaveSceneData(bool bOverride)
{
	/*
	 * Scenes that have not been loaded do not need to be re-saved. They would have been
	 * saved when unloading the scene previously. Only save loaded scenes.
	 */
	if ( !m_bSceneLoaded && !bOverride )
	{
		return true;
	}

	// Check to see if the scene data exists
	fs::path tilemapPath{ m_sSceneDataPath };
	if ( !fs::exists( tilemapPath ) )
	{
		VORTEK_ERROR( "Failed to save scene data - Filepath does not exist [{}]", m_sSceneDataPath );
		return false;
	}

	std::unique_ptr<JSONSerializer> pSerializer{ nullptr };

	try
	{
		pSerializer = std::make_unique<JSONSerializer>( m_sSceneDataPath );
	}
	catch ( const std::exception& ex )
	{
		VORTEK_ERROR( "Failed to save scene data [{}] - [{}]", m_sSceneDataPath, ex.what() );
		return false;
	}

	pSerializer->StartDocument();
	pSerializer->StartNewObject( "scene_data" );

	std::string sTilemapPath = m_sTilemapPath.substr( m_sTilemapPath.find( ASSETS ) );
	std::string sObjectPath = m_sObjectPath.substr( m_sObjectPath.find( ASSETS ) );

	glm::vec2 playerStartPosition = m_bUsePlayerStart ? m_PlayerStart.GetPosition() : glm::vec2{ 0.f };

	pSerializer->AddKeyValuePair( "name", m_sSceneName )
		.AddKeyValuePair( "tilemapPath", sTilemapPath )
		.AddKeyValuePair( "objectmapPath", sObjectPath )
		.AddKeyValuePair( "defaultMusic", m_sDefaultMusic )
		.StartNewObject( "canvas" )
		.AddKeyValuePair( "width", m_Canvas.width )
		.AddKeyValuePair( "height", m_Canvas.height )
		.AddKeyValuePair( "tileWidth", m_Canvas.tileWidth )
		.AddKeyValuePair( "tileHeight", m_Canvas.tileHeight )
		.EndObject() // Canvas
		.AddKeyValuePair( "mapType",
						  ( m_eMapType == VORTEK_CORE::EMapType::Grid ? std::string{ "grid" } : std::string{ "iso" } ) )
		.StartNewObject( "playerStart" )
		.AddKeyValuePair( "enabled", m_bUsePlayerStart )
		.AddKeyValuePair( "character", m_bUsePlayerStart ? m_PlayerStart.GetCharacterName() : std::string{ "default" } )
		.StartNewObject( "position" )
		.AddKeyValuePair( "x", playerStartPosition.x )
		.AddKeyValuePair( "y", playerStartPosition.y )
		.EndObject() // Player start position
		.EndObject() // Player Start
		.StartNewArray( "sprite_layers" );

	for ( const auto& layer : m_LayerParams )
	{
		pSerializer->StartNewObject()
			.AddKeyValuePair( "layerName", layer.sLayerName )
			.AddKeyValuePair( "bVisible", layer.bVisible )
			.EndObject();
	}

	pSerializer->EndArray();  // Sprite Layers
	pSerializer->EndObject(); // Scene  data

	bool bSuccess{ true };
	if ( !pSerializer->EndDocument() )
	{
		bSuccess = false;
	}

	// Try to Save the tilemap
	auto pTilemapLoader = std::make_unique<TilemapLoader>();
	if ( !pTilemapLoader->SaveTilemap( m_Registry, m_sTilemapPath, true ) )
	{
		bSuccess = false;
	}

	// Try to Save scene game objects
	if ( !pTilemapLoader->SaveGameObjects( m_Registry, m_sObjectPath, true ) )
	{
		bSuccess = false;
	}

	return bSuccess;
}

void Scene::SetCanvasOffset()
{
	if ( m_eMapType == VORTEK_CORE::EMapType::Grid )
	{
		m_Canvas.offset = glm::vec2{ 0.f };
		return;
	}

	// TODO: ADD checks to ensure width is 2x height for iso tiles.

	// The width of the tile must be 2x the height
	// Right now we are hard coding it everywhere; however
	// this should automatically change when in iso.
	float doubleTileWidth = m_Canvas.tileWidth * 2.f;

	float halfTileWidth = doubleTileWidth / 2.f;
	float halfTileHeight = m_Canvas.tileHeight / 2.f;

	/* Find the X Offset */
	// Get the Hypotenuse of one tile
	float c1 = sqrt( ( halfTileWidth * halfTileWidth ) + ( halfTileHeight * halfTileHeight ) );

	// Get the length of the entire Y - Side hypotenuse
	float c2 = m_Canvas.width / doubleTileWidth * c1;

	// Find theta
	float theta1 = atan2( halfTileWidth, halfTileHeight );

	// Get the length of offset x
	float offsetX = sin( theta1 ) * c2;
	m_Canvas.offset.x = floor( offsetX );
	// We are assuming that there is no offset in Y currently
	m_Canvas.offset.y = 0.f;
}

void Scene::CreateLuaBind( sol::state& lua )
{
	lua.new_usertype<Canvas>( "Canvas",
							  sol::call_constructor,
							  sol::factories( [] { return Canvas{}; },
											  []( int width, int height, int tileWidth, int tileHeight ) {
												  return Canvas{ .width = width,
																 .height = height,
																 .tileWidth = tileWidth,
																 .tileHeight = tileHeight };
											  } ),
							  "width",
							  &Canvas::width,
							  "height",
							  &Canvas::height,
							  "tileWidth",
							  &Canvas::tileWidth,
							  "tileHeight",
							  &Canvas::tileHeight );
}

} // namespace VORTEK_CORE
