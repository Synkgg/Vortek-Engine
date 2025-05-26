#include "RenderSystem.h"
#include "../Resources/AssetManager.h"
#include "../ECS/Components/SpriteComponent.h"
#include "../ECS/Components/TransformComponent.h"
#include "../ECS/MainRegistry.h"
#include "../CoreUtilities/CoreUtilities.h"
#include <Rendering/Core/Camera2D.h>
#include <Rendering/Essentials/Shader.h>
#include <Rendering/Core/BatchRenderer.h>

#include "VORTEKUtilities/HelperUtilities.h"

#include <Logger/Logger.h>

#include <ranges>

using namespace VORTEK_CORE::ECS;
using namespace VORTEK_RENDERING;
using namespace VORTEK_RESOURCES;

namespace VORTEK_CORE::Systems
{
	RenderSystem::RenderSystem()
		: m_pBatchRenderer{ std::make_unique<SpriteBatchRenderer>() }
	{
	}

	void RenderSystem::Update(VORTEK_CORE::ECS::Registry& registry, VORTEK_RENDERING::Camera2D& camera,
		const std::vector<VORTEK_UTIL::SpriteLayerParams>& layerFilters)
	{
		auto& mainRegistry = MAIN_REGISTRY();
		auto& assetManager = mainRegistry.GetAssetManager();

		const auto& spriteShader = assetManager.GetShader("basic");
		auto cam_mat = camera.GetCameraMatrix();

		if (spriteShader->ShaderProgramID() == 0)
		{
			VORTEK_ERROR("Sprite shader program has not been set correctly!");
			return;
		}

		// enable the shader
		spriteShader->Enable();
		spriteShader->SetUniformMat4("uProjection", cam_mat);

		m_pBatchRenderer->Begin();

		auto spriteView = registry.GetRegistry().view<SpriteComponent, TransformComponent>();
		std::function<bool(entt::entity)> filterFunc;

		// Check to see if the layers are visible, if not, filter them out.
		if (layerFilters.empty())
		{
			filterFunc = [](entt::entity) { return true; };
		}
		else
		{
			filterFunc = [&](entt::entity entity) {
				// We only want to filter tiles
				if (!registry.GetRegistry().all_of<TileComponent>(entity))
					return true;

				const auto& sprite = spriteView.get<SpriteComponent>(entity);
				if (sprite.layer >= 0 && sprite.layer < layerFilters.size())
				{
					return layerFilters[sprite.layer].bVisible;
				}

				return false;
				};
		}

		for (const auto& entity : std::views::filter(spriteView, filterFunc))
		{
			const auto& transform = spriteView.get<TransformComponent>(entity);
			const auto& sprite = spriteView.get<SpriteComponent>(entity);

			if (!VORTEK_CORE::EntityInView(transform, sprite.width, sprite.height, camera))
				continue;

			if (sprite.texture_name.empty() || sprite.bHidden)
				continue;

			const auto& pTexture = assetManager.GetTexture(sprite.texture_name);
			if (!pTexture)
			{
				VORTEK_ERROR("Texture [{0}] was not created correctly!", sprite.texture_name);
				return;
			}

			glm::vec4 spriteRect{ transform.position.x, transform.position.y, sprite.width, sprite.height };
			glm::vec4 uvRect{ sprite.uvs.u, sprite.uvs.v, sprite.uvs.uv_width, sprite.uvs.uv_height };

			glm::mat4 model = VORTEK_CORE::RSTModel(transform, sprite.width, sprite.height);

			m_pBatchRenderer->AddSprite(spriteRect, uvRect, pTexture->GetID(), sprite.layer, model, sprite.color);
		}

		m_pBatchRenderer->End();
		m_pBatchRenderer->Render();

		spriteShader->Disable();
	}
} // namespace VORTEK_CORE::Systems