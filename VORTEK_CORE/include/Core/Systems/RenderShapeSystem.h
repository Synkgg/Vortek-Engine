#pragma once

namespace Vortek::Rendering
{
class Camera2D;
class RectBatchRenderer;
class CircleBatchRenderer;
} // namespace Vortek::Rendering

namespace Vortek::Core::ECS
{
class Registry;
}

namespace Vortek::Core::Systems
{
class RenderShapeSystem
{
  private:
	std::unique_ptr<Vortek::Rendering::RectBatchRenderer> m_pRectRenderer;
	std::unique_ptr<Vortek::Rendering::CircleBatchRenderer> m_pCircleRenderer;

  public:
	RenderShapeSystem();
	~RenderShapeSystem() = default;

	void Update( Vortek::Core::ECS::Registry& registry, Vortek::Rendering::Camera2D& camera );
};
} // namespace Vortek::Core::Systems
