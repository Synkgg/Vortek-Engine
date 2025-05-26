#include "TileTool.h"
#include "Logger/Logger.h"
#include "../utilities/EditorUtilities.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "Rendering/Core/BatchRenderer.h"
#include "Rendering/Core/Camera2D.h"
#include "../scene/SceneObject.h"

constexpr int MOUSE_SPRITE_LAYER = 10;

using namespace VORTEK_CORE::ECS;

namespace VORTEK_EDITOR
{
	uint32_t TileTool::CheckForTile(const glm::vec2& position)
	{
		if (!m_pRegistry)
			return entt::null;

		auto tileView = m_pRegistry->GetRegistry().view<TileComponent, TransformComponent>();

		for (auto entity : tileView)
		{
			Entity tile{ *m_pRegistry, entity };
			const auto& transform = tile.GetComponent<TransformComponent>();
			const auto& sprite = tile.GetComponent<SpriteComponent>();

			if (position.x >= transform.position.x &&
				position.x < transform.position.x + sprite.width * transform.scale.x &&
				position.y >= transform.position.y &&
				position.y < transform.position.y + sprite.height * transform.scale.y &&
				m_pMouseTile->sprite.layer == sprite.layer)
			{
				return static_cast<uint32_t>(entity);
			}
		}

		return entt::null;
	}

	VORTEK_CORE::ECS::Entity TileTool::CreateEntity()
	{
		VORTEK_ASSERT(m_pRegistry && "The registry must be valid to create an entity.");
		return VORTEK_CORE::ECS::Entity{ *m_pRegistry, "", "" };
	}

	VORTEK_CORE::ECS::Entity TileTool::CreateEntity(uint32_t id)
	{
		VORTEK_ASSERT(m_pRegistry && "The registry must be valid to create an entity.");
		return VORTEK_CORE::ECS::Entity{ *m_pRegistry, static_cast<entt::entity>(id) };
	}

	void TileTool::DrawMouseSprite()
	{
		m_pBatchRenderer->Begin();
		const auto& sprite = m_pMouseTile->sprite;
		const auto& transform = m_pMouseTile->transform;

		glm::vec4 position{ transform.position.x,
							transform.position.y,
							sprite.width * transform.scale.x,
							sprite.height * transform.scale.y };

		glm::vec4 uvs{ sprite.uvs.u, sprite.uvs.v, sprite.uvs.uv_width, sprite.uvs.uv_height };

		const auto pTexture = MAIN_REGISTRY().GetAssetManager().GetTexture(sprite.texture_name);
		if (pTexture)
			m_pBatchRenderer->AddSprite(
				position, uvs, pTexture->GetID(), MOUSE_SPRITE_LAYER, glm::mat4{ 1.f }, sprite.color);

		m_pBatchRenderer->End();
		m_pBatchRenderer->Render();
	}

	void TileTool::ExamineMousePosition()
	{
		if (!m_pCamera)
			return;

		const auto& mouseWorldPos = GetMouseWorldCoords();
		auto& transform = m_pMouseTile->transform;

		const float cameraScale = m_pCamera->GetScale();
		bool bIsOffset{ true };

		if (m_bGridSnap)
		{
			glm::vec2 mouseGrid{ mouseWorldPos.x / (m_MouseRect.x * transform.scale.x) * cameraScale,
								 mouseWorldPos.y / (m_MouseRect.y * transform.scale.y) * cameraScale };

			auto scaledGridToCamX = std::floor(mouseGrid.x / cameraScale);
			auto scaledGridToCamY = std::floor(mouseGrid.y / cameraScale);
			transform.position.x = scaledGridToCamX * m_MouseRect.x * transform.scale.x;
			transform.position.y = scaledGridToCamY * m_MouseRect.y * transform.scale.y;

			m_GridCoords.x = scaledGridToCamX;
			m_GridCoords.y = scaledGridToCamY;

			SetMouseWorldCoords(transform.position);
		}
		else
		{
			transform.position = bIsOffset ? mouseWorldPos - m_MouseRect * 0.5f : mouseWorldPos;
		}
	}

	TileTool::TileTool()
		: AbstractTool()
		, m_MouseRect{ 16.f }
		, m_GridCoords{ 0.f }
		, m_bGridSnap{ true }
		, m_pBatchRenderer{ std::make_shared<VORTEK_RENDERING::SpriteBatchRenderer>() }
		, m_pMouseTile{ std::make_shared<Tile>() }
	{
	}

	void TileTool::Update(Canvas& canvas)
	{
		AbstractTool::Update(canvas);
		ExamineMousePosition();
	}

	void TileTool::ClearMouseTextureData()
	{
		m_pMouseTile->sprite = SpriteComponent{};
	}

	void TileTool::LoadSpriteTextureData(const std::string& textureName)
	{
		// We need to get the current sprite layer to ensure that if we change tilesets,
		// the layer we are drawing on does not reset.
		int currentLayer = m_pMouseTile->sprite.layer;

		m_pMouseTile->sprite = SpriteComponent{ .width = m_MouseRect.x,
												.height = m_MouseRect.y,
												.color = VORTEK_RENDERING::Color{ 255, 255, 255, 255 },
												.start_x = 0,
												.start_y = 0,
												.layer = currentLayer,
												.texture_name = textureName };

		auto pTexture = MAIN_REGISTRY().GetAssetManager().GetTexture(textureName);
		VORTEK_ASSERT(pTexture && "Texture must exist");
		VORTEK_CORE::GenerateUVs(m_pMouseTile->sprite, pTexture->GetWidth(), pTexture->GetHeight());
	}

	const std::string& TileTool::GetSpriteTexture() const
	{
		return m_pMouseTile->sprite.texture_name;
	}

	void TileTool::SetSpriteUVs(int startX, int startY)
	{
		m_pMouseTile->sprite.start_x = startX;
		m_pMouseTile->sprite.start_y = startY;
		m_pMouseTile->sprite.uvs.u = startX * m_pMouseTile->sprite.uvs.uv_width;
		m_pMouseTile->sprite.uvs.v = startY * m_pMouseTile->sprite.uvs.uv_height;
	}

	void TileTool::SetSpriteRect(const glm::vec2& spriteRect)
	{
		if (m_pMouseTile->sprite.texture_name.empty())
			return;

		m_MouseRect = spriteRect;
		auto& sprite = m_pMouseTile->sprite;
		sprite.width = m_MouseRect.x;
		sprite.height = m_MouseRect.y;

		auto pTexture = MAIN_REGISTRY().GetAssetManager().GetTexture(sprite.texture_name);
		VORTEK_ASSERT(pTexture && "Texture Must exist.");
		VORTEK_CORE::GenerateUVs(sprite, pTexture->GetWidth(), pTexture->GetHeight());
	}

	const bool TileTool::SpriteValid() const
	{
		return !m_pMouseTile->sprite.texture_name.empty();
	}

	const bool TileTool::CanDrawOrCreate() const
	{
		return IsActivated() && !OutOfBounds() && IsOverTilemapWindow() && SpriteValid() && m_pCurrentScene &&
			m_pCurrentScene->HasTileLayers();
	}
} // namespace VORTEK_EDITOR