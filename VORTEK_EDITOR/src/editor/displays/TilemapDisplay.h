#pragma once
#include "IDisplay.h"
#include <string>
#include <glm/glm.hpp>

namespace VORTEK_RENDERING
{
	class Camera2D;
}

namespace VORTEK_CORE
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
} // namespace VORTEK_CORE

namespace VORTEK_EDITOR
{
	class TilemapDisplay : public IDisplay
	{
	private:
		std::unique_ptr<VORTEK_RENDERING::Camera2D> m_pTilemapCam;
		bool m_bWindowActive;

	private:
		void RenderTilemap();
		void LoadNewScene();
		void PanZoomCamera(const glm::vec2& mousePos);

		void HandleKeyPressedEvent(const VORTEK_CORE::Events::KeyEvent& keyEvent);

		void AddPrefabbedEntityToScene(const VORTEK_CORE::PrefabbedEntity& prefabbed);

	protected:
		virtual void DrawToolbar() override;

	public:
		TilemapDisplay();
		virtual ~TilemapDisplay();

		virtual void Draw() override;
		virtual void Update() override;
	};
} // namespace VORTEK_EDITOR