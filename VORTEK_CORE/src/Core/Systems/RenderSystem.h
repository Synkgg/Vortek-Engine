#pragma once
#include "../ECS/Registry.h"

namespace VORTEK_RENDERING
{
	class Camera2D;
	class SpriteBatchRenderer;
} // namespace VORTEK_RENDERING

namespace VORTEK_UTIL
{
	struct SpriteLayerParams;
}

namespace VORTEK_CORE::Systems
{
	class RenderSystem
	{
	private:
		std::unique_ptr<VORTEK_RENDERING::SpriteBatchRenderer> m_pBatchRenderer;

	public:
		RenderSystem();
		~RenderSystem() = default;
		void Update(VORTEK_CORE::ECS::Registry& registry, VORTEK_RENDERING::Camera2D& camera,
			const std::vector<VORTEK_UTIL::SpriteLayerParams>& layerFilters = {});
	};
} // namespace VORTEK_CORE::Systems