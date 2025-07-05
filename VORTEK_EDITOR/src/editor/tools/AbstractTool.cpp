#include "AbstractTool.h"
#include "Logger/Logger.h"
#include "Core/Scripting/InputManager.h"
#include "Core/ECS/Registry.h"
#include "Rendering/Core/Camera2D.h"
#include "editor/utilities/EditorUtilities.h"
#include "editor/scene/SceneObject.h"

using namespace VORTEK_CORE;

namespace VORTEK_EDITOR
{

void AbstractTool::UpdateMouseWorldCoords()
{
	m_MouseScreenCoords = m_GUICursorCoords - m_GUIRelativeCoords;
	if ( !m_pCamera )
		return;

	m_MouseWorldCoords = m_pCamera->ScreenCoordsToWorld( m_MouseScreenCoords );
}

void AbstractTool::CheckOutOfBounds( const VORTEK_CORE::Canvas& canvas )
{
	if ( !m_pCurrentScene )
	{
		m_bOutOfBounds = true;
		return;
	}

	if ( m_pCurrentScene->GetMapType() == EMapType::Grid )
	{
		auto boundsWidth{ canvas.width - ( canvas.tileWidth * 0.5f ) };
		auto boundsHeight{ canvas.height - ( canvas.tileHeight * 0.5f ) };

		if ( m_MouseWorldCoords.x <= boundsWidth && m_MouseWorldCoords.y <= boundsHeight &&
			 m_MouseWorldCoords.x >= 0.f && m_MouseWorldCoords.y >= 0.f )
		{
			m_bOutOfBounds = false;
		}
		else
		{
			m_bOutOfBounds = true;
		}
	}
	else
	{
		const auto& canvas = m_pCurrentScene->GetCanvas();
		int xNumTiles = canvas.height / canvas.tileWidth - 1;
		int yNumTiles = canvas.width / ( canvas.tileHeight * 2.f ) - 1;
		if ( m_GridCoords.x >= 0 && m_GridCoords.y >= 0 && m_GridCoords.x <= xNumTiles && m_GridCoords.y <= yNumTiles )
		{
			m_bOutOfBounds = false;
		}
		else
		{
			m_bOutOfBounds = true;
		}
	}
}

bool AbstractTool::MouseBtnJustPressed( EMouseButton eButton )
{
	return INPUT_MANAGER().GetMouse().IsBtnJustPressed( static_cast<int>( eButton ) );
}

bool AbstractTool::MouseBtnJustReleased( EMouseButton eButton )
{
	return INPUT_MANAGER().GetMouse().IsBtnJustReleased( static_cast<int>( eButton ) );
}

bool AbstractTool::MouseBtnPressed( EMouseButton eButton )
{
	return INPUT_MANAGER().GetMouse().IsBtnPressed( static_cast<int>( eButton ) );
}

bool AbstractTool::MouseMoving()
{
	return INPUT_MANAGER().GetMouse().IsMouseMoving();
}

AbstractTool::AbstractTool()
	: m_MouseScreenCoords{ 0.f }
	, m_MouseWorldCoords{ 0.f }
	, m_GUICursorCoords{ 0.f }
	, m_GUIRelativeCoords{ 0.f }
	, m_WindowPos{ 0.f }
	, m_WindowSize{ 0.f }
	, m_bActivated{ false }
	, m_bOutOfBounds{ false }
	, m_bOverTilemapWindow{ false }
	, m_GridCoords{ 0.f }
{
}

void AbstractTool::Update( VORTEK_CORE::Canvas& canvas )
{
	UpdateMouseWorldCoords();
	CheckOutOfBounds( canvas );
}

bool AbstractTool::SetupTool( SceneObject* pSceneObject, VORTEK_RENDERING::Camera2D* pCamera )
{
	if ( !pSceneObject )
	{
		VORTEK_ERROR( "Failed to setup tool - SceneObject was nullptr." );
		return false;
	}

	if ( !pSceneObject->GetRegistryPtr() )
	{
		VORTEK_ERROR( "Failed to setup tool - Registry was nullptr." );
		return false;
	}

	if ( !pCamera )
	{
		VORTEK_ERROR( "Failed to setup tool - Camera was nullptr." );
		return false;
	}
	m_pCurrentScene = pSceneObject;
	m_pCamera = pCamera;
	m_pRegistry = pSceneObject->GetRegistryPtr();

	ResetSelectedEntity();

	return true;
}

} // namespace VORTEK_EDITOR
