#pragma once
#include "AbstractTool.h"
#include "Core/ECS/Entity.h"

namespace VORTEK_RENDERING
{
	class SpriteBatchRenderer;
}

namespace VORTEK_EDITOR
{
	class TileTool : public AbstractTool
	{
	private:
		glm::vec2 m_MouseRect;
		glm::vec2 m_GridCoords;

		bool m_bGridSnap;

	protected:
		std::shared_ptr<VORTEK_RENDERING::SpriteBatchRenderer> m_pBatchRenderer;
		std::shared_ptr<struct Tile> m_pMouseTile;

	protected:
		uint32_t CheckForTile(const glm::vec2& position);

		VORTEK_CORE::ECS::Entity CreateEntity();
		VORTEK_CORE::ECS::Entity CreateEntity(uint32_t id);

		void DrawMouseSprite();
		virtual void ExamineMousePosition() override;

	public:
		TileTool();
		virtual ~TileTool() = default;

		virtual void Create() = 0;
		virtual void Draw() = 0;

		virtual void Update(Canvas& canvas) override;

		void ClearMouseTextureData();

		void LoadSpriteTextureData(const std::string& textureName);

		const std::string& GetSpriteTexture() const;

		void SetSpriteUVs(int startX, int startY);
		void SetSpriteRect(const glm::vec2& spriteRect);

		const bool SpriteValid() const;

		const bool CanDrawOrCreate() const;

		inline void EnableGridSnap() { m_bGridSnap = true; }
		inline void DisableGridSnap() { m_bGridSnap = false; }

		inline Tile& GetTileData() { return *m_pMouseTile; }
	};
} // namespace VORTEK_EDITOR