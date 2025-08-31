#pragma once
#include "IDisplay.h"

namespace VORTEK_CORE
{
class Scene;

namespace ECS
{
struct SpriteComponent;
}
} // namespace VORTEK_CORE

namespace VORTEK_EDITOR
{

class TileDetailsDisplay : public IDisplay
{
  public:
	TileDetailsDisplay();
	virtual ~TileDetailsDisplay();

	virtual void Draw() override;

  private:
	void DrawSpriteComponent( VORTEK_CORE::ECS::SpriteComponent& sprite, VORTEK_CORE::Scene* pScene );

  private:
	int m_SelectedLayer;
	int m_DeleteLayer;
	std::string m_sRenameLayerBuf;
	bool m_bRename;
	bool m_bDeleteLayer;
};
} // namespace VORTEK_EDITOR
