#pragma once
#include "../ECS/Registry.h"

namespace VORTEK_RENDERING
{
	class Camera2D;
	class SpriteBatchRenderer;
	class TextBatchRenderer;
} // namespace VORTEK_RENDERING

namespace VORTEK_CORE::Systems
{
	class RenderUISystem
	{
	private:
		std::unique_ptr<VORTEK_RENDERING::SpriteBatchRenderer> m_pSpriteRenderer;
		std::unique_ptr<VORTEK_RENDERING::TextBatchRenderer> m_pTextRenderer;
		std::unique_ptr<VORTEK_RENDERING::Camera2D> m_pCamera2D;

	public:
		RenderUISystem();
		~RenderUISystem();

		void Update(VORTEK_CORE::ECS::Registry& registry);
	};
} // namespace VORTEK_CORE::Systems