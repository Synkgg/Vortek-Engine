#pragma once
#include "../AbstractTool.h"
#include <memory>
#include <entt.hpp>

namespace VORTEK_CORE::ECS
{
	class Entity;
}

namespace VORTEK_RENDERING
{
	class SpriteBatchRenderer;
}

namespace VORTEK_EDITOR
{

	struct GizmoAxisParams;
	struct Canvas;

	class Gizmo : public AbstractTool
	{
	public:
		Gizmo();
		Gizmo(const GizmoAxisParams& xAxisParams, const GizmoAxisParams& yAxisParams, bool bOneAxis);
		virtual ~Gizmo();

		virtual void Update(Canvas& canvas) override;
		virtual void Draw() = 0;

		void SetSelectedEntity(entt::entity entity);
		void Hide();
		void Show();

		inline bool OverGizmo() const { return false; }

	protected:
		void Init(const std::string& sXAxisTexture, const std::string& sYAxisTexture);
		virtual void ExamineMousePosition() override;

		float GetDeltaX();
		float GetDeltaY();

		void SetGizmoPosition(VORTEK_CORE::ECS::Entity& selectedEntity);

		virtual void ResetSelectedEntity() override { m_SelectedEntity = entt::null; }

	protected:
		std::unique_ptr<GizmoAxisParams> m_pXAxisParams;
		std::unique_ptr<GizmoAxisParams> m_pYAxisParams;
		std::unique_ptr<VORTEK_RENDERING::SpriteBatchRenderer> m_pBatchRenderer;

		entt::entity m_SelectedEntity;
		glm::vec2 m_LastMousePos;

		bool m_bOverXAxis;
		bool m_bOverYAxis;
		bool m_bHoldingX;
		bool m_bHoldingY;
		bool m_bHidden;
		bool m_bOnlyOneAxis;

	};
} // namespace VORTEK_EDITOR