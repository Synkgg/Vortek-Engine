#pragma once
#include "IDisplay.h"
#include <glm/glm.hpp>

namespace Vortek::Rendering
{
class Camera2D;
}

namespace Vortek::Core
{

struct PrefabbedEntity;

namespace ECS
{
class Entity;
}
namespace Events
{
struct KeyEvent;
}
} // namespace Vortek::Core

namespace Vortek::Editor
{
class TilemapDisplay : public IDisplay
{
  public:
	TilemapDisplay();
	virtual ~TilemapDisplay();

	virtual void Draw() override;
	virtual void Update() override;

  protected:
	virtual void DrawToolbar() override;

  private:
	void RenderTilemap();
	void LoadNewScene();
	void PanZoomCamera( const glm::vec2& mousePos );

	void HandleKeyPressedEvent( const Vortek::Core::Events::KeyEvent& keyEvent );
	void AddPrefabbedEntityToScene( const Vortek::Core::PrefabbedEntity& prefabbed );

  private:
	std::unique_ptr<Vortek::Rendering::Camera2D> m_pTilemapCam;
	bool m_bWindowActive;
};
} // namespace Vortek::Editor
