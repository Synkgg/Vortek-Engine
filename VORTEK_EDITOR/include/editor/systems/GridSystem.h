#pragma once

namespace Vortek::Rendering
{
class RectBatchRenderer;
class Camera2D;
} // namespace Vortek::Rendering

namespace Vortek::Core
{
class Scene;
}

namespace Vortek::Editor
{
class GridSystem
{
  public:
	GridSystem();
	~GridSystem() = default;

	void Update( Vortek::Core::Scene& currentScene, Vortek::Rendering::Camera2D& camera );

  private:
	void UpdateIso( Vortek::Core::Scene& currentScene, Vortek::Rendering::Camera2D& camera );

  private:
	std::unique_ptr<Vortek::Rendering::RectBatchRenderer> m_pBatchRenderer;
};
} // namespace Vortek::Editor
