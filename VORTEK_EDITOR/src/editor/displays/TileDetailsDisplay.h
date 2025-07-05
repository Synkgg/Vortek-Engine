#pragma once
#include "IDisplay.h"
#include <string>

namespace VORTEK_CORE
{
	class Scene;

	namespace ECS
	{
		struct SpriteComponent;
	}
} // namespace VORTEK_CORE::ECS

namespace VORTEK_EDITOR
{

	class TileDetailsDisplay : public IDisplay
	{
	private:
		int m_SelectedLayer;
		std::string m_sRenameLayerBuf;
		bool m_bRename;

	private:
		void DrawSpriteComponent(VORTEK_CORE::ECS::SpriteComponent& sprite, VORTEK_CORE::Scene* pScene);

	public:
		TileDetailsDisplay();
		virtual ~TileDetailsDisplay();

		virtual void Draw() override;
	};
} // namespace VORTEK_EDITOR