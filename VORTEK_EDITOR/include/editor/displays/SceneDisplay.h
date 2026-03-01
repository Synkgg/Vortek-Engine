#pragma once
#include "IDisplay.h"

namespace Vortek::Core::Events
{
struct KeyEvent;
}

namespace Vortek::Editor
{
class SceneDisplay : public IDisplay
{
  public:
	SceneDisplay();
	~SceneDisplay() = default;

	virtual void Draw() override;
	virtual void Update() override;

  protected:
	virtual void DrawToolbar() override;

  private:
	void LoadScene();
	void UnloadScene();
	void RenderScene() const;

	void HandleKeyEvent( const Vortek::Core::Events::KeyEvent keyEvent );

  private:
	bool m_bPlayScene;
	bool m_bWindowActive;
	bool m_bSceneLoaded;
};
} // namespace Vortek::Editor
