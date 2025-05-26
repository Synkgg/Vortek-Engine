#include "SceneObject.h"
#include "VORTEKUtilities/VORTEKUtilities.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/MetaUtilities.h"
#include <format>

using namespace VORTEK_CORE::ECS;
using namespace entt::literals;

namespace VORTEK_EDITOR
{
	SceneObject::SceneObject(const std::string& sceneName)
		: registry{}
		, m_RuntimeRegistry{}
		, m_sSceneName{ sceneName }
		, m_Canvas{}
		, m_CurrentLayer{ 0 }
	{
	}

	void SceneObject::CopySceneToRuntime()
	{
		auto& registryToCopy = registry.GetRegistry();

		for (auto entityToCopy : registryToCopy.view<entt::entity>(entt::exclude<ScriptComponent>))
		{
			entt::entity newEntity = m_RuntimeRegistry.CreateEntity();

			// Copy the components of the entity to the new entity
			for (auto&& [id, storage] : registryToCopy.storage())
			{
				if (!storage.contains(entityToCopy))
					continue;

				VORTEK_CORE::Utils::InvokeMetaFunction(
					id, "copy_component"_hs, Entity{ registry, entityToCopy }, Entity{ m_RuntimeRegistry, newEntity });
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

	bool SceneObject::CheckLayerName(const std::string& sLayerName)
	{
		return VORTEK_UTIL::CheckContainsValue(m_LayerParams, [&](VORTEK_UTIL::SpriteLayerParams& spriteLayer) {
			return spriteLayer.sLayerName == sLayerName;
			});
	}

} // namespace VORTEK_EDITOR