#pragma once

#include <memory>

namespace VORTEK_RENDERING
{
class RectBatchRenderer;
class Camera2D;
} // namespace VORTEK_RENDERING

namespace VORTEK_CORE
{
class Scene;
}

namespace VORTEK_EDITOR
{
class GridSystem
{
  public:
	GridSystem();
	~GridSystem() = default;

	void Update( VORTEK_CORE::Scene& currentScene, VORTEK_RENDERING::Camera2D& camera );

  private:
	void UpdateIso( VORTEK_CORE::Scene& currentScene, VORTEK_RENDERING::Camera2D& camera );

  private:
	std::unique_ptr<VORTEK_RENDERING::RectBatchRenderer> m_pBatchRenderer;
};
} // namespace VORTEK_EDITOR
