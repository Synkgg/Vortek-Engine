#include "RotateGizmo.h"
#include "editor/utilities/EditorUtilities.h"
#include "Rendering/Core/BatchRenderer.h"
#include "Rendering/Core/Camera2D.h"

#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "Core/CoreUtilities/CoreUtilities.h"

#include "Core/ECS/Entity.h"

#include <Rendering/Essentials/Shader.h>
#include <Rendering/Essentials/Texture.h>

#include "Logger/Logger.h"

using namespace VORTEK_CORE::ECS;

VORTEK_EDITOR::RotateGizmo::RotateGizmo()
	: Gizmo( GizmoAxisParams{ .axisColor = XAXIS_GIZMO_COLOR,
							  .axisHoveredColor = XAXIS_HOVERED_GIZMO_COLOR,
							  .axisDisabledColor = GRAYED_OUT_GIZMO_COLOR },
			 GizmoAxisParams{}, true )
{
	Init( "rotate_tool", "" );
}

void VORTEK_EDITOR::RotateGizmo::Update( VORTEK_CORE::Canvas& canvas )
{
	Gizmo::Update( canvas );

	if ( m_SelectedEntity == entt::null || !m_pRegistry )
	{
		Hide();
		return;
	}

	Show();

	Entity selectedEntity{ *m_pRegistry, m_SelectedEntity };
	auto& selectedTransform = selectedEntity.GetComponent<TransformComponent>();

	float deltaX{ GetDeltaX() };
	if ( deltaX != 0.f )
	{
		selectedTransform.rotation += deltaX;

		// Clamp the values between 0 and 360.
		if ( selectedTransform.rotation < 0.f )
		{
			selectedTransform.rotation = 360.f + selectedTransform.rotation;
		}
		else if ( selectedTransform.rotation > 360.f )
		{
			selectedTransform.rotation = selectedTransform.rotation - 360.f;
		}

		selectedTransform.bDirty = true;
	}

	SetGizmoPosition( selectedEntity );

	ExamineMousePosition();
}

void VORTEK_EDITOR::RotateGizmo::Draw( VORTEK_RENDERING::Camera2D* pCamera )
{
	if ( m_bHidden )
		return;

	auto pShader = MAIN_REGISTRY().GetAssetManager().GetShader( "basic" );
	if ( !pShader )
		return;

	pShader->Enable();
	glm::mat4 camMat{ 1.f };
	if ( m_bUIComponent && pCamera )
	{
		camMat = pCamera->GetCameraMatrix();
	}
	else
	{
		camMat = m_pCamera->GetCameraMatrix();
	}
	pShader->SetUniformMat4( "uProjection", camMat );

	m_pBatchRenderer->Begin();
	const auto& xAxisSprite = m_pXAxisParams->sprite;
	const auto& xAxisTransform = m_pXAxisParams->transform;

	if ( !xAxisSprite.bHidden )
	{
		glm::vec4 xAxisPosition{ xAxisTransform.position.x,
								 xAxisTransform.position.y,
								 xAxisSprite.width * xAxisTransform.scale.x,
								 xAxisSprite.height * xAxisTransform.scale.y };

		glm::vec4 xAxisUVs{ xAxisSprite.uvs.u, xAxisSprite.uvs.v, xAxisSprite.uvs.uv_width, xAxisSprite.uvs.uv_height };

		const auto pXAxisTexture = MAIN_REGISTRY().GetAssetManager().GetTexture( xAxisSprite.sTextureName );
		if ( pXAxisTexture )
		{
			m_pBatchRenderer->AddSprite(
				xAxisPosition, xAxisUVs, pXAxisTexture->GetID(), 99, glm::mat4{ 1.f }, xAxisSprite.color );
		}
	}

	m_pBatchRenderer->End();
	m_pBatchRenderer->Render();

	pShader->Disable();
}
