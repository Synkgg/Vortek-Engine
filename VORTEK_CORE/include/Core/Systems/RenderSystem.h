#pragma once
#include "Core/ECS/Registry.h"

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

	/*
	 * @brief Loops through all of the entities in the registry that have a sprite
	 * and transform component. Applies all the necessary transformations and adds them
	 * to a Batch to be rendered.
	 */
	void Update( VORTEK_CORE::ECS::Registry& registry, VORTEK_RENDERING::Camera2D& camera,
				 const std::vector<VORTEK_UTIL::SpriteLayerParams>& layerFilters = {} );

	static void CreateRenderSystemLuaBind( sol::state& lua, VORTEK_CORE::ECS::Registry& registry );
};
} // namespace VORTEK_CORE::Systems
