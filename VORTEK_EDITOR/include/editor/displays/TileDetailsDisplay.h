#pragma once
#include "IDisplay.h"

namespace Vortek::Core
{
class Scene;

namespace ECS
{
struct SpriteComponent;
}
} // namespace Vortek::Core::ECS

namespace Vortek::Editor
{

class TileDetailsDisplay : public IDisplay
{
  public:
	TileDetailsDisplay();
	virtual ~TileDetailsDisplay();

	virtual void Draw() override;

  private:
	void DrawSpriteComponent( Vortek::Core::ECS::SpriteComponent& sprite, Vortek::Core::Scene* pScene );

  private:
	int m_SelectedLayer;
	int m_DeleteLayer;
	std::string m_sRenameLayerBuf;
	bool m_bRename;
	bool m_bDeleteLayer;
};
} // namespace Vortek::Editor
