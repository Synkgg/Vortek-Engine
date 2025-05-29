#pragma once
#include "Core/ECS/Entity.h"
#include "../utilities/EditorUtilities.h"
#include "../../../../VORTEK_UTILITIES/VortekUtilities/HelperUtilities.h"

namespace VORTEK_EDITOR
{
	class SceneObject
	{
	public:
		SceneObject(const std::string& sceneName);
		SceneObject(const std::string& sceneName, const std::string& sceneData);
		~SceneObject() = default;

		void CopySceneToRuntime();
		void ClearRuntimeScene();

		void AddNewLayer();
		void AddLayer(const std::string& sLayerName, bool bVisible);
		bool CheckLayerName(const std::string& sLayerName);

		bool LoadScene();
		bool UnloadScene();
		bool SaveScene();

		inline bool HasTileLayers() const { return !m_LayerParams.empty(); }
		inline std::vector<VORTEK_UTIL::SpriteLayerParams>& GetLayerParams() { return m_LayerParams; }

		inline Canvas& GetCanvas() { return m_Canvas; }
		inline const std::string& GetName() { return m_sSceneName; }
		inline const std::string& GetSceneDataPath() { return m_sSceneDataPath; }
		inline VORTEK_CORE::ECS::Registry& GetRegistry() { return m_Registry; }
		inline VORTEK_CORE::ECS::Registry* GetRegistryPtr() { return &m_Registry; }
		inline VORTEK_CORE::ECS::Registry& GetRuntimeRegistry() { return m_RuntimeRegistry; }

	private:
		bool LoadSceneData();
		bool SaveSceneData();

	private:
		VORTEK_CORE::ECS::Registry m_Registry;
		VORTEK_CORE::ECS::Registry m_RuntimeRegistry;

		std::string m_sSceneName;
		std::string m_sTilemapPath;
		std::string m_sObjectPath;
		std::string m_sSceneDataPath;

		Canvas m_Canvas;
		std::vector<VORTEK_UTIL::SpriteLayerParams> m_LayerParams;
		int m_CurrentLayer;
		bool m_bSceneLoaded{ false };
	};

} // namespace VORTEK_EDITOR