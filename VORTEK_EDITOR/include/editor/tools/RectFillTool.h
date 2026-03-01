#pragma once
#include "TileTool.h"

namespace Vortek::Rendering
{
class RectBatchRenderer;
struct Rect;
} // namespace Vortek::Rendering

namespace Vortek::Editor
{
class RectFillTool : public TileTool
{
  private:
	std::unique_ptr<Vortek::Rendering::RectBatchRenderer> m_pShapeRenderer;
	std::unique_ptr<Vortek::Rendering::Rect> m_pTileFillRect;
	glm::vec2 m_StartPressPos;

  private:
	void CreateTiles();
	void RemoveTiles();
	void DrawPreview( int dx, int dy );
	void ResetTile();

  public:
	RectFillTool();
	~RectFillTool();

	virtual void Create() override;
	virtual void Draw() override;
};
} // namespace Vortek::Editor
