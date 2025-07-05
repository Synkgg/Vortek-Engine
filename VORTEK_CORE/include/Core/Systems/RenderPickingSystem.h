#pragma once
#include <memory>

namespace VORTEK_RENDERING
{
class Camera2D;
class PickingBatchRenderer;
} // namespace VORTEK_RENDERING

namespace VORTEK_CORE::ECS
{
class Registry;
}

namespace VORTEK_CORE::Systems
{
class RenderPickingSystem
{
  private:
	std::unique_ptr<VORTEK_RENDERING::PickingBatchRenderer> m_pBatchRenderer;

  public:
	RenderPickingSystem();
	~RenderPickingSystem();

	void Update( VORTEK_CORE::ECS::Registry& registry, VORTEK_RENDERING::Camera2D& camera );
};
} // namespace VORTEK_CORE::Systems
