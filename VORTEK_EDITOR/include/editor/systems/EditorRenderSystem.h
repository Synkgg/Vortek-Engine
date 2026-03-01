#pragma once

namespace Vortek::Core
{
namespace ECS
{
class Registry;
}
} // namespace Vortek::Core

namespace Vortek::Rendering
{
class Camera2D;
class SpriteBatchRenderer;
} // namespace Vortek::Rendering

namespace Vortek::Utilities
{
struct SpriteLayerParams;
}

namespace Vortek::Editor
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
	void Update( Vortek::Core::ECS::Registry& registry, Vortek::Rendering::Camera2D& camera,
				 const std::vector<Vortek::Utilities::SpriteLayerParams>& layerFilters = {} );

  private:
	std::unique_ptr<Vortek::Rendering::SpriteBatchRenderer> m_pBatchRenderer;
};
} // namespace Vortek::Editor

using EditorRenderSystemPtr = std::shared_ptr<Vortek::Editor::EditorRenderSystem>;
