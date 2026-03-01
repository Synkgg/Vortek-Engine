#pragma once

namespace Vortek::Rendering
{
class Camera2D;
class PickingBatchRenderer;
} // namespace Vortek::Rendering

namespace Vortek::Core::ECS
{
class Registry;
}

namespace Vortek::Core::Systems
{
class RenderPickingSystem
{
  private:
	std::unique_ptr<Vortek::Rendering::PickingBatchRenderer> m_pBatchRenderer;

  public:
	RenderPickingSystem();
	~RenderPickingSystem();

	void Update( Vortek::Core::ECS::Registry& registry, Vortek::Rendering::Camera2D& camera );
};
} // namespace Vortek::Core::Systems
