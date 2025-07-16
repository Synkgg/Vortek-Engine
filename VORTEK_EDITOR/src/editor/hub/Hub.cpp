#include "Hub.h"
#include <Windowing/Window/Window.h>
#include "editor/utilities/imgui/Gui.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/utilities/EditorUtilities.h"
#include "editor/loaders/ProjectLoader.h"
#include "VortekFilesystem/Dialogs/FileDialog.h"
#include "Logger/Logger.h"

#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"

#include "VortekUtilities/HelperUtilities.h"

#include <Rendering/Essentials/Texture.h>

// IMGUI
// ===================================
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL_opengl.h>
// ===================================

#include <filesystem>

namespace fs = std::filesystem;

constexpr ImVec2 BUTTON_SIZE = ImVec2{ 100.f, 25.f };
constexpr ImVec2 PROJECT_BUTTON_SIZE = ImVec2{ 400.f, 115.f };

std::vector<VORTEK_EDITOR::RecentProject> m_RecentProjects;
const std::string RECENT_PROJECTS_FILE = "recent_projects.txt";

namespace VORTEK_EDITOR
{

Hub::Hub( VORTEK_WINDOWING::Window& window )
	: m_Window{ window }
	, m_bRunning{ false }
	, m_bLoadError{ false }
	, m_Event{}
	, m_eState{ EHubState::Default }
	, m_Width{ static_cast<float>( m_Window.GetWidth() ) }
	, m_Height{ static_cast<float>( m_Window.GetHeight() ) }
	, m_sNewProjectName{}
	, m_sNewProjectPath{ DEFAULT_PROJECT_PATH }
	, m_sPrevProjectPath{}
	, m_sPrevProjectName{}
{
	fs::path projectPath{ m_sNewProjectPath };
	if ( !fs::exists( projectPath ) )
	{
		std::error_code ec;
		if ( !fs::create_directories( projectPath, ec ) )
		{
			VORTEK_ERROR( "HUB - Failed to create directories. Error: {}", ec.message() );
			m_eState = EHubState::Close;
			// TODO: Close and actually log an error.
		}
	}
}

Hub::~Hub()
{
}

bool Hub::Run()
{
	m_bRunning = Initialize();

	while ( m_bRunning )
	{
		ProcessEvents();
		Update();
		Render();
	}

	bool bClosed{ m_eState == EHubState::Close };

	if ( !bClosed )
	{
		SDL_SetWindowBordered( m_Window.GetWindow().get(), SDL_TRUE );
		SDL_SetWindowResizable( m_Window.GetWindow().get(), SDL_TRUE );
		SDL_MaximizeWindow( m_Window.GetWindow().get() );

		std::string sTitle{ "Vortek Engine - " };
		sTitle += !m_sNewProjectName.empty() ? m_sNewProjectName : m_sPrevProjectName;
		SDL_SetWindowTitle( m_Window.GetWindow().get(), sTitle.c_str() );
	}


	return !bClosed;
}

bool Hub::Initialize()
{
	LoadRecentProjects(); // ← ADD THIS
	return true;
}

void Hub::DrawGui()
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration |
								   ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	if ( !ImGui::Begin( "##_HudDisplay", nullptr, windowFlags ) )
	{
		ImGui::End();
		return;
	}

	ImGui::SetWindowPos( ImGui::GetMainViewport()->Pos );
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImGui::SetWindowSize( ImVec2{ m_Width, m_Height } );

	// Draw the Vortek Engine LOGO
	auto pLogo = ASSET_MANAGER().GetTexture( "vortek_logo" );
	if ( pLogo )
	{
		ImGui::SetCursorPos( ImVec2{ m_Width * 0.55f - pLogo->GetWidth(), 50.f } );
		ImGui::Image(
			(ImTextureID)(intptr_t)pLogo->GetID(),
			ImVec2{ static_cast<float>( pLogo->GetWidth() * 1.5f ), static_cast<float>( pLogo->GetHeight() * 1.5f ) } );
	}

	switch ( m_eState )
	{
	case EHubState::Default: DrawDefault(); break;
	case EHubState::NewProject: DrawNewProject(); break;
	case EHubState::OpenProject: DrawOpenProject(); break;
	default: break;
	}

	ImGui::End();
}

void Hub::DrawDefault()
{
	ImGui::SetCursorPos( ImVec2{ m_Width * 0.5f - BUTTON_SIZE.x, 300.f } );
	if ( ImGui::Button( "New Project", BUTTON_SIZE ) )
	{
		m_eState = EHubState::NewProject;
	}

	ImGui::SameLine();
	if ( ImGui::Button( "Open Project", BUTTON_SIZE ) )
	{
		m_eState = EHubState::OpenProject;
	}

	ImGui::SetCursorPos( ImVec2{ m_Width - ( BUTTON_SIZE.x + 16.f ), m_Height - ( BUTTON_SIZE.y + 16.f ) } );
	if ( ImGui::Button( "Close", BUTTON_SIZE ) )
	{
		m_eState = EHubState::Close;
		m_bRunning = false;
	}
}

void Hub::DrawNewProject()
{
	ImGui::SetCursorPosY( 300.f );
	ImGui::OffsetTextX( "New Project", 32.f );
	ImGui::SameLine();

	char newProjectBuffer[ 256 ]{ 0 };
	char newProjectPathBuffer[ 256 ]{ 0 };
#ifdef _WIN32
	strcpy_s( newProjectBuffer, m_sNewProjectName.c_str() );
	strcpy_s( newProjectPathBuffer, m_sNewProjectPath.c_str() );
#else
	strcpy( newProjectBuffer, m_sNewProjectName.c_str() );
	strcpy( newProjectPathBuffer, m_sNewProjectPath.c_str() );
#endif
	ImGui::SetCursorPosX( 128.f );
	if ( ImGui::InputText( "##ProjectName", newProjectBuffer, sizeof( newProjectBuffer ) ) )
	{
		m_sNewProjectName = std::string{ newProjectBuffer };
	}

	ImGui::OffsetTextX( "Location", 32.f );
	ImGui::SameLine();
	ImGui::SetCursorPosX( 128.f );
	if ( ImGui::InputText( "##ProjectPath", newProjectPathBuffer, sizeof( newProjectPathBuffer ) ) )
	{
		m_sNewProjectPath = std::string{ newProjectPathBuffer };
	}
	ImGui::SameLine();
	if ( ImGui::Button( "Browse" ) )
	{
		VORTEK_FILESYSTEM::FileDialog fd{};
		const auto sFilepath = fd.SelectFolderDialog( "Open", BASE_PATH );
		if ( !sFilepath.empty() )
		{
#ifdef _WIN32
			strcpy_s( newProjectPathBuffer, sFilepath.c_str() );
#else
			strcpy( newProjectPathBuffer, sFilepath.c_str() );
#endif
			m_sNewProjectPath = sFilepath;
		}
	}

	if ( !m_sNewProjectName.empty() && !m_sNewProjectPath.empty() )
	{
		fs::path projectPath{ m_sNewProjectPath };

		if ( fs::exists( projectPath ) )
		{
			if ( IsReservedPathOrFile( fs::path{ m_sNewProjectPath } ) )

			{
				ImGui::TextColored( ImVec4{ 1.f, 0.f, 0.f, 1.f },
									"Path [%s] is a reserved path. Please change paths.",
									m_sNewProjectPath.c_str() );
			}
			else
			{
				ImGui::SetCursorPosX( 128.f );
				if ( ImGui::Button( "Create", BUTTON_SIZE ) )
				{
					// Create the project
					ProjectLoader pl{};
					if ( !pl.CreateNewProject( m_sNewProjectName, m_sNewProjectPath ) )
					{
						// TODO: show an error
					}
					else
					{

						m_bRunning = false;
						m_eState = EHubState::CreateNew;
					}
				}
			}
		}
	}

	ImGui::SetCursorPos( ImVec2{ m_Width - ( BUTTON_SIZE.x + 16.f ), m_Height - ( BUTTON_SIZE.y + 16.f ) } );
	if ( ImGui::Button( "Cancel", BUTTON_SIZE ) )
	{
		m_eState = EHubState::Default;
		m_sNewProjectName.clear();
		m_sNewProjectPath = DEFAULT_PROJECT_PATH;
	}
}

void Hub::DrawOpenProject()
{
	constexpr float scrollWidth = 500.f;
	constexpr float scrollHeight = 400.f;
	const float titleY = 50.f;

	// Title
	ImGui::SetCursorPosY( titleY );
	const char* heading = "Recent Projects";
	ImGui::SetCursorPosX( ( m_Width - ImGui::CalcTextSize( heading ).x ) * 0.5f );
	ImGui::Text( "%s", heading );
	ImGui::Spacing();

	const float centerX = ( m_Width - scrollWidth ) * 0.5f;
	const float centerY = ( m_Height - scrollHeight ) * 0.5f;
	ImGui::SetCursorPos( ImVec2( centerX, centerY ) );

	ImGui::BeginChild(
		"RecentProjectsScroll", ImVec2( scrollWidth, scrollHeight ), true, ImGuiWindowFlags_AlwaysVerticalScrollbar );

	if ( m_RecentProjects.empty() )
	{
		ImGui::TextDisabled( "No recent projects found." );
	}
	else
	{
		const float projectHeight = PROJECT_BUTTON_SIZE.y + ImGui::GetStyle().ItemSpacing.y;
		const float contentHeight = projectHeight * m_RecentProjects.size();

		if ( contentHeight < scrollHeight )
			ImGui::SetCursorPosY( ImGui::GetCursorPosY() + ( scrollHeight - contentHeight ) * 0.5f );

		for ( const auto& project : m_RecentProjects )
		{
			ImGui::PushID( project.path.c_str() );
			ImGui::PushStyleColor( ImGuiCol_ChildBg, IM_COL32( 40, 40, 40, 255 ) );
			ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 10, 10 ) );

			ImGui::SetCursorPosX( ( scrollWidth - PROJECT_BUTTON_SIZE.x ) * 0.5f );

			if ( ImGui::BeginChild( "ProjectTile", PROJECT_BUTTON_SIZE, true ) )
			{
				ImGui::Text( "%s", fs::path( project.path ).stem().string().c_str() );
				ImGui::TextDisabled( "Last edited: %s", project.lastOpened.c_str() );

				ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 15 );
				if ( ImGui::Button( "Open", ImVec2( 100, 35 ) ) )
				{
					TryOpenProject( project.path );
				}
			}
			ImGui::EndChild();

			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			ImGui::PopID();
			ImGui::Spacing();
		}
	}

	ImGui::EndChild();

	// Browse + Cancel buttons
	ImGui::SetCursorPos( ImVec2( centerX + scrollWidth - BUTTON_SIZE.x * 2 - 15, centerY + scrollHeight + 10 ) );

	if ( ImGui::Button( "Browse", BUTTON_SIZE ) )
	{
		VORTEK_FILESYSTEM::FileDialog fd;
		const auto sFilepath = fd.OpenFileDialog( "Open Project", "", { "*.veprj" } );

		if ( !sFilepath.empty() )
		{
			TryOpenProject( sFilepath );
		}
	}

	ImGui::SameLine();

	if ( ImGui::Button( "Cancel", BUTTON_SIZE ) )
	{
		m_eState = EHubState::Default;
		m_sPrevProjectName.clear();
		m_sPrevProjectPath.clear();
	}
}

void Hub::ProcessEvents()
{
	// Process Events
	while ( SDL_PollEvent( &m_Event ) )
	{
		ImGui_ImplSDL2_ProcessEvent( &m_Event );

		switch ( m_Event.type )
		{
		case SDL_QUIT: m_bRunning = false; break;
		case SDL_KEYDOWN:		  // keyboard.OnKeyPressed( m_Event.key.keysym.sym ); break;
		case SDL_KEYUP:			  // keyboard.OnKeyReleased( m_Event.key.keysym.sym ); break;
		case SDL_MOUSEBUTTONDOWN: // mouse.OnBtnPressed( m_Event.button.button ); break;
		case SDL_MOUSEBUTTONUP:	  // mouse.OnBtnReleased( m_Event.button.button ); break;
		default: break;
		}
	}
}

void Hub::Update()
{
}

void Hub::Render()
{
	Gui::Begin();
	DrawGui();
	Gui::End( &m_Window );

	SDL_GL_SwapWindow( m_Window.GetWindow().get() );
}

bool Hub::TryOpenProject( const std::string& path )
{
	if ( !fs::exists( path ) )
	{
		VORTEK_ERROR( "Project path does not exist: {}", path );
		return false;
	}

	m_sPrevProjectPath = path;
	m_sPrevProjectName = fs::path( path ).stem().string();

	ProjectLoader pl;
	if ( pl.LoadProject( m_sPrevProjectPath ) )
	{
		SaveRecentProject( m_sPrevProjectPath );
		m_bRunning = false;
		m_eState = EHubState::OpenProject;
		return true;
	}
	else
	{
		VORTEK_ERROR( "Failed to load project: {}", m_sPrevProjectPath );
		return false;
	}
}

void Hub::LoadRecentProjects()
{
	m_RecentProjects.clear();
	std::ifstream file( RECENT_PROJECTS_FILE );
	std::string line;

	while ( std::getline( file, line ) )
	{
		if ( line.empty() )
			continue;

		auto delimiterPos = line.find( '|' );
		if ( delimiterPos == std::string::npos )
			continue; // invalid line format

		std::string path = line.substr( 0, delimiterPos );
		std::string lastOpened = line.substr( delimiterPos + 1 );

		if ( fs::exists( path ) )
		{
			m_RecentProjects.push_back( { path, lastOpened } );
		}
	}
}

void Hub::SaveRecentProject( const std::string& path )
{
	// Remove old entry if it exists
	m_RecentProjects.erase( std::remove_if( m_RecentProjects.begin(),
											m_RecentProjects.end(),
											[ & ]( const VORTEK_EDITOR::RecentProject& p ) { return p.path == path; } ),
							m_RecentProjects.end() );

	// Insert with current time as lastOpened
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t( now );
	char timeStr[ 64 ];
	std::strftime( timeStr, sizeof( timeStr ), "%Y-%m-%d %H:%M:%S", std::localtime( &now_c ) );

	m_RecentProjects.insert( m_RecentProjects.begin(), { path, timeStr } );

	if ( m_RecentProjects.size() > 5 )
		m_RecentProjects.resize( 5 );

	std::ofstream file( RECENT_PROJECTS_FILE, std::ios::trunc );
	for ( const auto& entry : m_RecentProjects )
		file << entry.path << "|" << entry.lastOpened << "\n";
}


} // namespace VORTEK_EDITOR
