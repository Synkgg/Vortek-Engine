#pragma once
#include "Core/ECS/Registry.h"
#include <sol/sol.hpp>

namespace Vortek::Rendering
{
class Camera2D;
class SpriteBatchRenderer;
class TextBatchRenderer;
} // namespace Vortek::Rendering

namespace Vortek::Core::Systems
{
class RenderUISystem
{
  private:
	std::unique_ptr<Vortek::Rendering::SpriteBatchRenderer> m_pSpriteRenderer;
	std::unique_ptr<Vortek::Rendering::TextBatchRenderer> m_pTextRenderer;
	std::unique_ptr<Vortek::Rendering::Camera2D> m_pCamera2D;

  public:
	RenderUISystem();
	~RenderUISystem();

	void Update( Vortek::Core::ECS::Registry& registry );
	inline Vortek::Rendering::Camera2D* GetCamera() { return m_pCamera2D.get(); }

	static void CreateRenderUISystemLuaBind( sol::state& lua );

};
} // namespace Vortek::Core::Systems
