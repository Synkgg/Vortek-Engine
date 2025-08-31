#pragma once

namespace VORTEK_CORE
{
namespace ECS
{
class Registry;
}
} // namespace VORTEK_CORE

namespace VORTEK_RENDERING
{
class Camera2D;
class SpriteBatchRenderer;
} // namespace VORTEK_RENDERING

namespace VORTEK_UTIL
{
struct SpriteLayerParams;
}

namespace VORTEK_EDITOR
{

class EditorRenderSystem
{
  public:
	EditorRenderSystem();
	~EditorRenderSystem();

	/*
	 * @brief Loops through all of the entities in the registry that have a sprite
	 * and transform component. Applies all the necessary transformations and adds them
	 * to a Batch to be rendered.
	 */
	void Update( VORTEK_CORE::ECS::Registry& registry, VORTEK_RENDERING::Camera2D& camera,
				 const std::vector<VORTEK_UTIL::SpriteLayerParams>& layerFilters = {} );

  private:
	std::unique_ptr<VORTEK_RENDERING::SpriteBatchRenderer> m_pBatchRenderer;
};
} // namespace VORTEK_EDITOR

using EditorRenderSystemPtr = std::shared_ptr<VORTEK_EDITOR::EditorRenderSystem>;
