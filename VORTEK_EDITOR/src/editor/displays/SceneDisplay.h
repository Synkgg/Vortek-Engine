#pragma once
#include "IDisplay.h"
#include <Core/ECS/Registry.h>

namespace VORTEK_CORE::Events
{
	struct KeyEvent;
}

namespace VORTEK_EDITOR
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

		void HandleKeyEvent(const VORTEK_CORE::Events::KeyEvent keyEvent);

	private:
		bool m_bPlayScene;
		bool m_bWindowActive;
		bool m_bSceneLoaded;
	};
} // namespace VORTEK_EDITOR