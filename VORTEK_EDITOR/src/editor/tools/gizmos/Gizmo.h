#pragma once
#include "editor/tools/AbstractTool.h"
#include <memory>
#include <entt/entt.hpp>

namespace VORTEK_CORE
{

namespace ECS
{
class Entity;
}

} // namespace VORTEK_CORE

namespace VORTEK_RENDERING
{
class SpriteBatchRenderer;
}

namespace VORTEK_CORE::Events
{
class EventDispatcher;
} // namespace VORTEK_CORE::Events

namespace VORTEK_EDITOR
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

	virtual void Update( VORTEK_CORE::Canvas& canvas ) override;
	virtual void Draw( VORTEK_RENDERING::Camera2D* pCamera ) = 0;

	void SetSelectedEntity( entt::entity entity );
	void Hide();
	void Show();

	VORTEK_CORE::Events::EventDispatcher& GetDispatcher();
	inline bool OverGizmo() const { return !( !m_bHoldingX && !m_bHoldingY && !m_bOverXAxis && !m_bOverYAxis ); }

  protected:
	void Init( const std::string& sXAxisTexture, const std::string& sYAxisTexture );
	virtual void ExamineMousePosition() override;

	float GetDeltaX();
	float GetDeltaY();

	void SetGizmoPosition( VORTEK_CORE::ECS::Entity& selectedEntity );
	virtual void ResetSelectedEntity() override { m_SelectedEntity = entt::null; }

  protected:
	std::unique_ptr<GizmoAxisParams> m_pXAxisParams;
	std::unique_ptr<GizmoAxisParams> m_pYAxisParams;
	std::unique_ptr<VORTEK_RENDERING::SpriteBatchRenderer> m_pBatchRenderer;
	std::unique_ptr<VORTEK_CORE::Events::EventDispatcher> m_pEventDispatcher{ nullptr };

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
	void OnAddComponent( const VORTEK_EDITOR::Events::AddComponentEvent& addCompEvent );
};
} // namespace VORTEK_EDITOR
