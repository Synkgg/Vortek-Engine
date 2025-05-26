#pragma once
#include "IDisplay.h"
#include <Core/ECS/Registry.h>

namespace VORTEK_EDITOR
{
	class SceneDisplay : public IDisplay
	{
	private:
		bool m_bPlayScene, m_bSceneLoaded;

	private:
		void LoadScene();
		void UnloadScene();
		void RenderScene() const;

	protected:
		virtual void DrawToolbar() override;

	public:
		SceneDisplay();
		~SceneDisplay() = default;

		virtual void Draw() override;
		virtual void Update() override;
	};
} // namespace VORTEK_EDITOR