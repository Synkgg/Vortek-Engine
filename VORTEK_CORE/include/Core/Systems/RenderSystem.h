#pragma once
#include <sol/sol.hpp>

namespace Vortek::Core::ECS
{
class Registry;
}

namespace Vortek::Rendering
{
class Camera2D;
class SpriteBatchRenderer;
} // namespace Vortek::Rendering

namespace Vortek::Core::Systems
{
class RenderSystem
{
  public:
	RenderSystem();
	~RenderSystem();

	void Update( Vortek::Core::ECS::Registry& registry, Vortek::Rendering::Camera2D& camera );
	static void CreateRenderSystemLuaBind( sol::state& lua, Vortek::Core::ECS::Registry& registry );

  private:
	std::unique_ptr<Vortek::Rendering::SpriteBatchRenderer> m_pBatchRenderer;
};
} // namespace Vortek::Core::Systems
