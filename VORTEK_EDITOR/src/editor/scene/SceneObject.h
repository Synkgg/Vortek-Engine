#pragma once
#include "Core/ECS/Entity.h"
#include "../utilities/EditorUtilities.h"
#include "VORTEKUtilities/HelperUtilities.h"

namespace VORTEK_EDITOR
{
	class SceneObject
	{
	private:
		VORTEK_CORE::ECS::Registry registry, m_RuntimeRegistry;
		std::string m_sSceneName;
		Canvas m_Canvas;
		std::vector<VORTEK_UTIL::SpriteLayerParams> m_LayerParams;
		int m_CurrentLayer;

	public:
		SceneObject(const std::string& sceneName);
		~SceneObject() = default;

		void CopySceneToRuntime();
		void ClearRuntimeScene();

		void AddNewLayer();
		bool CheckLayerName(const std::string& sLayerName);

		bool HasTileLayers() const { return !m_LayerParams.empty(); }

		inline std::vector<VORTEK_UTIL::SpriteLayerParams>& GetLayerParams() { return m_LayerParams; }

		inline Canvas& GetCanvas() { return m_Canvas; }
		inline const std::string& GetName() { return m_sSceneName; }
		inline VORTEK_CORE::ECS::Registry& GetRegistry() { return registry; }
		inline VORTEK_CORE::ECS::Registry* GetRegistryPtr() { return &registry; }
		inline VORTEK_CORE::ECS::Registry& GetRuntimeRegistry() { return m_RuntimeRegistry; }
	};
} // namespace VORTEK_EDITOR