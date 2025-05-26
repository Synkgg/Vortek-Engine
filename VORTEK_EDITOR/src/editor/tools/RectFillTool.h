#pragma once
#include "TileTool.h"

namespace VORTEK_RENDERING
{
	class RectBatchRenderer;
	struct Rect;
} // namespace VORTEK_RENDERING

namespace VORTEK_EDITOR
{
	class RectFillTool : public TileTool
	{
	private:
		std::unique_ptr<VORTEK_RENDERING::RectBatchRenderer> m_pShapeRenderer;
		std::unique_ptr<VORTEK_RENDERING::Rect> m_pTileFillRect;
		glm::vec2 m_StartPressPos;

	private:
		void CreateTiles();
		void RemoveTiles();
		void DrawPreview(int dx, int dy);
		void ResetTile();

	public:
		RectFillTool();
		~RectFillTool();

		virtual void Create() override;
		virtual void Draw() override;
	};
} // namespace VORTEK_EDITOR