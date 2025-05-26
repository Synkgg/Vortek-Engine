#pragma once

#include <memory>

namespace VORTEK_RENDERING
{
	class RectBatchRenderer;
	class Camera2D;
}

namespace VORTEK_EDITOR
{
	class GridSystem
	{
	private:
		std::unique_ptr<VORTEK_RENDERING::RectBatchRenderer> m_pBatchRenderer;

	public:
		GridSystem();
		~GridSystem() = default;

		void Update(class SceneObject& currentScene, VORTEK_RENDERING::Camera2D& camera);
	};
} // namespace VORTEK_EDITOR