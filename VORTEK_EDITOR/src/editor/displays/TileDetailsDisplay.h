#pragma once
#include "IDisplay.h"
#include <string>

namespace VORTEK_CORE::ECS
{
	struct SpriteComponent;
}

namespace VORTEK_EDITOR
{

	class SceneObject;

	class TileDetailsDisplay : public IDisplay
	{
	private:
		int m_SelectedLayer;
		std::string m_sRenameLayerBuf;
		bool m_bRename;

	private:
		void DrawSpriteComponent(VORTEK_CORE::ECS::SpriteComponent& sprite, SceneObject* pScene);

	public:
		TileDetailsDisplay();
		virtual ~TileDetailsDisplay();

		virtual void Draw() override;
	};
} // namespace VORTEK_EDITOR