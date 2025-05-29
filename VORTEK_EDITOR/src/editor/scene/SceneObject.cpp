#include "SceneObject.h"
#include "VORTEKUtilities/VORTEKUtilities.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/MetaUtilities.h"
#include <format>
#include "Core/ECS/MainRegistry.h"
#include "../utilities/SaveProject.h"

#include "Core/Loaders/TilemapLoader.h"

#include "../VORTEK_FILESYSTEM/Serializers/JSONSerializer.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

using namespace VORTEK_FILESYSTEM;
using namespace VORTEK_CORE::ECS;
using namespace VORTEK_CORE::Loaders;
using namespace entt::literals;

namespace VORTEK_EDITOR
{
	SceneObject::SceneObject(const std::string& sceneName)
		: m_Registry{}
		, m_RuntimeRegistry{}
		, m_sSceneName{ sceneName }
		, m_sTilemapPath{ "" }
		, m_sObjectPath{ "" }
		, m_sSceneDataPath{ "" }
		, m_Canvas{}
		, m_CurrentLayer{ 0 }
	{
		auto& pSaveProject = MAIN_REGISTRY().GetContext<std::shared_ptr<SaveProject>>();
		VORTEK_ASSERT(pSaveProject && "SaveProject must exists here!");

		std::string sScenePath = std::format("{}content{}assets{}scenes{}{}",
			pSaveProject->sProjectPath,
			PATH_SEPARATOR,
			PATH_SEPARATOR,
			PATH_SEPARATOR,
			m_sSceneName);

		if (fs::exists(sScenePath))
		{
			VORTEK_ERROR("SCENE ALREADY EXISTS!");
		}

		std::error_code ec;
		if (!fs::create_directory(fs::path{ sScenePath }, ec))
		{
			VORTEK_ERROR("Failed to create scene directory [{}] - Error: ", ec.message());
		}

		m_sTilemapPath = sScenePath + PATH_SEPARATOR + m_sSceneName + "_tilemap.json";
		m_sObjectPath = sScenePath + PATH_SEPARATOR + m_sSceneName + "_objects.json";
		m_sSceneDataPath = sScenePath + PATH_SEPARATOR + m_sSceneName + "_scene_data.json";

		// Create the files
		std::fstream tilemap{};
		tilemap.open(m_sTilemapPath, std::ios::out | std::ios::trunc);
		VORTEK_ASSERT(tilemap.is_open() && "File should have been created and opened.");
		tilemap.close();

		std::fstream objectmap{};
		objectmap.open(m_sObjectPath, std::ios::out | std::ios::trunc);
		VORTEK_ASSERT(objectmap.is_open() && "File should have been created and opened.");
		objectmap.close();

		std::fstream sceneData{};
		sceneData.open(m_sSceneDataPath, std::ios::out | std::ios::trunc);
		VORTEK_ASSERT(sceneData.is_open() && "File should have been created and opened.");
		sceneData.close();
	}

	SceneObject::SceneObject(const std::string& sceneName, const std::string& sceneData)
		: m_Registry{}
		, m_RuntimeRegistry{}
		, m_sSceneName{ sceneName }
		, m_sTilemapPath{}
		, m_sObjectPath{}
		, m_sSceneDataPath{ sceneData }
		, m_Canvas{}
		, m_CurrentLayer{ 0 }
	{
		// We need to load the scene data from the json file!
		if (!LoadSceneData())
		{
			VORTEK_ERROR("Failed to load scene data");
			return;
		}

		// Verify that the tilemap and objectmap files exist.
		if (!fs::exists(fs::path{ m_sTilemapPath }))
		{
			// Log Some error??
		}

		if (!fs::exists(fs::path{ m_sObjectPath }))
		{
			// Log Some error??
		}
	}

	void SceneObject::CopySceneToRuntime()
	{
		auto& registryToCopy = m_Registry.GetRegistry();

		for (auto entityToCopy : registryToCopy.view<entt::entity>(entt::exclude<ScriptComponent>))
		{
			entt::entity newEntity = m_RuntimeRegistry.CreateEntity();

			// Copy the components of the entity to the new entity
			for (auto&& [id, storage] : registryToCopy.storage())
			{
				if (!storage.contains(entityToCopy))
					continue;

				VORTEK_CORE::Utils::InvokeMetaFunction(
					id, "copy_component"_hs, Entity{ m_Registry, entityToCopy }, Entity{ m_RuntimeRegistry, newEntity });
			}
		}
	}

	void SceneObject::ClearRuntimeScene()
	{
		m_RuntimeRegistry.ClearRegistry();
	}

	void SceneObject::AddNewLayer()
	{
		m_LayerParams.emplace_back(
			VORTEK_UTIL::SpriteLayerParams{ .sLayerName = std::format("NewLayer_{}", m_CurrentLayer++) });
	}

	void SceneObject::AddLayer(const std::string& sLayerName, bool bVisible)
	{
		if (!CheckLayerName(sLayerName))
		{
			//VORTEK_ERROR( "Failed to add layer [{}] - Already exists.", sLayerName );
			return;
		}

		m_LayerParams.emplace_back(VORTEK_UTIL::SpriteLayerParams{ .sLayerName = sLayerName, .bVisible = bVisible });
	}

	bool SceneObject::CheckLayerName(const std::string& sLayerName)
	{
		return VORTEK_UTIL::CheckContainsValue(m_LayerParams, [&](VORTEK_UTIL::SpriteLayerParams& spriteLayer) {
			return spriteLayer.sLayerName == sLayerName;
			});
	}

	bool SceneObject::LoadScene()
	{
		if (m_bSceneLoaded)
		{
			VORTEK_ERROR("Scene [{}] has already been loaded", m_sSceneName);
			return false;
		}

		if (!LoadSceneData())
		{
			VORTEK_ERROR("Failed to load scene data");
			return false;
		}

		// Try to load the tilemap and object maps
		auto pTilemapLoader = std::make_unique<TilemapLoader>();
		if (!pTilemapLoader->LoadTilemap(m_Registry, m_sTilemapPath, true))
		{
		}

		// Load scene game objects
		if (!pTilemapLoader->LoadGameObjects(m_Registry, m_sObjectPath, true))
		{
		}

		m_bSceneLoaded = true;
		return true;
	}

	bool SceneObject::UnloadScene()
	{
		if (!m_bSceneLoaded)
		{
			VORTEK_ERROR("Scene [{}] has not been loaded", m_sSceneName);
			return false;
		}

		if (!SaveSceneData())
		{
			VORTEK_ERROR("Failed to unload scene data");
			return false;
		}

		// Remove all objects in registry
		m_Registry.ClearRegistry();

		m_bSceneLoaded = false;
		return false;
	}

	bool SceneObject::SaveScene()
	{
		return SaveSceneData();
	}

	bool SceneObject::LoadSceneData()
	{
		std::error_code ec;
		if (!fs::exists(m_sSceneDataPath, ec))
		{
			VORTEK_ERROR("Failed to load scene data. Error: {}", ec.message());
			return false;
		}

		std::ifstream sceneDataFile;
		sceneDataFile.open(m_sSceneDataPath);

		if (!sceneDataFile.is_open())
		{
			VORTEK_ERROR("Failed to open tilemap file [{}]", m_sSceneDataPath);
			return false;
		}

		// The sceneData file could be empty if just created
		if (sceneDataFile.peek() == std::ifstream::traits_type::eof())
		{
			// If the sceneData is an empty file, return true. must not have made any changes yet.
			return true;
		}

		std::stringstream ss;
		ss << sceneDataFile.rdbuf();
		std::string contents = ss.str();
		rapidjson::StringStream jsonStr{ contents.c_str() };

		rapidjson::Document doc;
		doc.ParseStream(jsonStr);

		if (doc.HasParseError() || !doc.IsObject())
		{
			VORTEK_ERROR("Failed to load tilemap: File: [{}] is not valid JSON. - {} - {}",
				m_sSceneDataPath,
				rapidjson::GetParseError_En(doc.GetParseError()),
				doc.GetErrorOffset());
			return false;
		}

		VORTEK_ASSERT(doc.HasMember("scene_data") && "scene_data member is necessary.");

		const rapidjson::Value& sceneData = doc["scene_data"];

		auto& pSaveProject = MAIN_REGISTRY().GetContext<std::shared_ptr<SaveProject>>();
		VORTEK_ASSERT(pSaveProject && "SaveProject must exists here!");

		std::string sScenePath = std::format("{}content{}", pSaveProject->sProjectPath, PATH_SEPARATOR);

		if (m_sTilemapPath.empty())
		{
			m_sTilemapPath = sScenePath + sceneData["tilemapPath"].GetString();
		}

		if (m_sObjectPath.empty())
		{
			m_sObjectPath = sScenePath + sceneData["objectmapPath"].GetString();
		}

		if (sceneData.HasMember("canvas"))
		{
			const rapidjson::Value& canvas = sceneData["canvas"];

			m_Canvas.width = canvas["width"].GetInt();
			m_Canvas.height = canvas["height"].GetInt();
			m_Canvas.tileWidth = canvas["tileWidth"].GetInt();
			m_Canvas.tileHeight = canvas["tileHeight"].GetInt();
		}

		VORTEK_ASSERT(sceneData.HasMember("sprite_layers") && "Sprite layers must be a part of scene data");
		const rapidjson::Value& spriteLayers = sceneData["sprite_layers"];
		for (const auto& layer : spriteLayers.GetArray())
		{
			std::string sLayerName = layer["layerName"].GetString();
			bool bVisible = layer["bVisible"].GetBool();

			AddLayer(sLayerName, bVisible);
		}

		return true;
	}

	bool SceneObject::SaveSceneData()
	{
		// Check to see if the scene data exists
		fs::path tilemapPath{ m_sSceneDataPath };
		if (!fs::exists(tilemapPath))
		{
			VORTEK_ERROR("Failed to save scene data - Filepath does not exist [{}]", m_sSceneDataPath);
			return false;
		}

		std::unique_ptr<JSONSerializer> pSerializer{ nullptr };

		try
		{
			pSerializer = std::make_unique<JSONSerializer>(m_sSceneDataPath);
		}
		catch (const std::exception& ex)
		{
			VORTEK_ERROR("Failed to save scene data [{}] - [{}]", m_sSceneDataPath, ex.what());
			return false;
		}

		pSerializer->StartDocument();
		pSerializer->StartNewObject("scene_data");

		std::string sTilemapPath = m_sTilemapPath.substr(m_sTilemapPath.find(ASSETS));
		std::string sObjectPath = m_sObjectPath.substr(m_sObjectPath.find(ASSETS));

		pSerializer->AddKeyValuePair("name", m_sSceneName)
			.AddKeyValuePair("tilemapPath", sTilemapPath)
			.AddKeyValuePair("objectmapPath", sObjectPath)
			.StartNewObject("canvas")
			.AddKeyValuePair("width", m_Canvas.width)
			.AddKeyValuePair("height", m_Canvas.height)
			.AddKeyValuePair("tileWidth", m_Canvas.tileWidth)
			.AddKeyValuePair("tileHeight", m_Canvas.tileHeight)
			.EndObject() // Canvas
			.StartNewArray("sprite_layers");

		for (const auto& layer : m_LayerParams)
		{
			pSerializer->StartNewObject()
				.AddKeyValuePair("layerName", layer.sLayerName)
				.AddKeyValuePair("bVisible", layer.bVisible)
				.EndObject();
		}

		pSerializer->EndArray();  // Sprite Layers
		pSerializer->EndObject(); // Scene  data

		bool bSuccess{ true };
		if (!pSerializer->EndDocument())
		{
			bSuccess = false;
		}

		// Try to Save the tilemap
		auto pTilemapLoader = std::make_unique<TilemapLoader>();
		if (!pTilemapLoader->SaveTilemap(m_Registry, m_sTilemapPath, true))
		{
			bSuccess = false;
		}

		// Try to Save scene game objects
		if (!pTilemapLoader->SaveGameObjects(m_Registry, m_sObjectPath, true))
		{
			bSuccess = false;
		}

		return bSuccess;
	}

} // namespace VORTEK_EDITOR