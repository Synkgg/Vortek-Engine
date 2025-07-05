#pragma once
#include <memory>

namespace VORTEK_RENDERING
{
class Camera2D;
class RectBatchRenderer;
class CircleBatchRenderer;
} // namespace VORTEK_RENDERING

namespace VORTEK_CORE::ECS
{
class Registry;
}

namespace VORTEK_CORE::Systems
{
class RenderShapeSystem
{
  private:
	std::unique_ptr<VORTEK_RENDERING::RectBatchRenderer> m_pRectRenderer;
	std::unique_ptr<VORTEK_RENDERING::CircleBatchRenderer> m_pCircleRenderer;

  public:
	RenderShapeSystem();
	~RenderShapeSystem() = default;

	void Update( VORTEK_CORE::ECS::Registry& registry, VORTEK_RENDERING::Camera2D& camera );
};
} // namespace VORTEK_CORE::Systems
