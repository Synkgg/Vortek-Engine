#pragma once
#include "editor/tools/AbstractTool.h"
#include <entt/entt.hpp>

namespace Vortek::Core
{

namespace ECS
{
class Entity;
}

} // namespace Vortek::Core

namespace Vortek::Rendering
{
class SpriteBatchRenderer;
}

namespace Vortek::Core::Events
{
class EventDispatcher;
} // namespace Vortek::Core::Events

namespace Vortek::Editor
{
namespace Events
{
struct AddComponentEvent;
}

struct GizmoAxisParams;

class Gizmo : public AbstractTool
{
  public:
	Gizmo();
	Gizmo( const GizmoAxisParams& xAxisParams, const GizmoAxisParams& yAxisParams, bool bOneAxis );
	virtual ~Gizmo();

	virtual void Update( Vortek::Core::Canvas& canvas ) override;
	virtual void Draw( Vortek::Rendering::Camera2D* pCamera ) = 0;

	void SetSelectedEntity( entt::entity entity );
	void Hide();
	void Show();

	Vortek::Core::Events::EventDispatcher& GetDispatcher();
	inline bool OverGizmo() const { return !( !m_bHoldingX && !m_bHoldingY && !m_bOverXAxis && !m_bOverYAxis ); }

  protected:
	void Init( const std::string& sXAxisTexture, const std::string& sYAxisTexture );
	virtual void ExamineMousePosition() override;

	float GetDeltaX();
	float GetDeltaY();

	void SetGizmoPosition( Vortek::Core::ECS::Entity& selectedEntity );
	virtual void ResetSelectedEntity() override { m_SelectedEntity = entt::null; }

  protected:
	std::unique_ptr<GizmoAxisParams> m_pXAxisParams;
	std::unique_ptr<GizmoAxisParams> m_pYAxisParams;
	std::unique_ptr<Vortek::Rendering::SpriteBatchRenderer> m_pBatchRenderer;
	std::unique_ptr<Vortek::Core::Events::EventDispatcher> m_pEventDispatcher{ nullptr };

	entt::entity m_SelectedEntity;
	glm::vec2 m_LastMousePos;

	bool m_bOverXAxis;
	bool m_bOverYAxis;
	bool m_bHoldingX;
	bool m_bHoldingY;
	bool m_bHidden;
	bool m_bOnlyOneAxis;
	bool m_bUIComponent;

  private:
	void OnAddComponent( const Vortek::Editor::Events::AddComponentEvent& addCompEvent );
};
} // namespace Vortek::Editor
