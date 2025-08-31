#pragma once
#include <sol/sol.hpp>

namespace VORTEK_CORE::ECS
{
class Registry;
}

namespace VORTEK_RENDERING
{
class Camera2D;
class SpriteBatchRenderer;
} // namespace VORTEK_RENDERING

namespace VORTEK_CORE::Systems
{
class RenderSystem
{
  private:
	std::unique_ptr<VORTEK_RENDERING::SpriteBatchRenderer> m_pBatchRenderer;

  public:
	RenderSystem();
	~RenderSystem();

	void Update( VORTEK_CORE::ECS::Registry& registry, VORTEK_RENDERING::Camera2D& camera );

	static void CreateRenderSystemLuaBind( sol::state& lua, VORTEK_CORE::ECS::Registry& registry );
};
} // namespace VORTEK_CORE::Systems
