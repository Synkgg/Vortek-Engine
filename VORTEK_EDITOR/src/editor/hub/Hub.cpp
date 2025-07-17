#include "Hub.h"

#include <Windowing/Window/Window.h>
#include "editor/utilities/imgui/Gui.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/utilities/EditorUtilities.h"
#include "editor/utilities/EditorSettings.h"
#include "editor/loaders/ProjectLoader.h"
#include "VortekFilesystem/Dialogs/FileDialog.h"
#include "Logger/Logger.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "VortekUtilities/HelperUtilities.h"
#include <editor/utilities/fonts/IconsFontAwesome5.h>
#include <Rendering/Essentials/Texture.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL_opengl.h>

#include <filesystem>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace fs = std::filesystem;

constexpr ImVec2 BUTTON_SIZE{ 100.f, 25.f };
constexpr ImVec2 PROJECT_BUTTON_SIZE{ 400.f, 135.f };
std::vector<VORTEK_EDITOR::RecentProject> m_RecentProjects;
const std::string RECENT_PROJECTS_FILE = "recent_projects.txt";

static char g_searchBuffer[ 128 ] = "";

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
	// Ensure default project directory exists
	fs::path projectPath{ m_sNewProjectPath };
	if ( !fs::exists( projectPath ) )
	{
		std::error_code ec;
		if ( !fs::create_directories( projectPath, ec ) )
		{
			VORTEK_ERROR( "HUB - Failed to create directories. Error: {}", ec.message() );
			m_eState = EHubState::Close;
		}
	}
}

Hub::~Hub() = default;

bool Hub::Run()
{
	m_bRunning = Initialize();

	while ( m_bRunning )
	{
		ProcessEvents();
		Update();
		Render();
	}

	const bool isClosed = ( m_eState == EHubState::Close );

	if ( !isClosed )
	{
		SDL_SetWindowBordered( m_Window.GetWindow().get(), SDL_TRUE );
		SDL_SetWindowResizable( m_Window.GetWindow().get(), SDL_TRUE );
		SDL_MaximizeWindow( m_Window.GetWindow().get() );

		std::string windowTitle = "Vortek Engine - ";
		windowTitle += !m_sNewProjectName.empty() ? m_sNewProjectName : m_sPrevProjectName;
		SDL_SetWindowTitle( m_Window.GetWindow().get(), windowTitle.c_str() );
	}

	return !isClosed;
}

bool Hub::Initialize()
{
	VORTEK_EDITOR::LoadEditorConfig();

	LoadRecentProjects();
	return true;
}

void Hub::DrawGui()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration |
							 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	if ( !ImGui::Begin( "##_HudDisplay", nullptr, flags ) )
	{
		ImGui::End();
		return;
	}

	ImGui::SetWindowPos( ImGui::GetMainViewport()->Pos );
	ImGui::SetWindowSize( ImVec2{ m_Width, m_Height } );

	auto pLogo = ASSET_MANAGER().GetTexture( "vortek_logo" );
	if ( pLogo && m_eState != EHubState::OpenProject )
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
		m_eState = EHubState::NewProject;

	ImGui::SameLine();
	if ( ImGui::Button( "Open Project", BUTTON_SIZE ) )
		m_eState = EHubState::OpenProject;

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

	char projectNameBuffer[ 256 ]{};
	char projectPathBuffer[ 256 ]{};

#ifdef _WIN32
	strcpy_s( projectNameBuffer, m_sNewProjectName.c_str() );
	strcpy_s( projectPathBuffer, m_sNewProjectPath.c_str() );
#else
	strcpy( projectNameBuffer, m_sNewProjectName.c_str() );
	strcpy( projectPathBuffer, m_sNewProjectPath.c_str() );
#endif

	ImGui::SetCursorPosX( 128.f );
	if ( ImGui::InputText( "##ProjectName", projectNameBuffer, sizeof( projectNameBuffer ) ) )
		m_sNewProjectName = projectNameBuffer;

	ImGui::OffsetTextX( "Location", 32.f );
	ImGui::SameLine();
	ImGui::SetCursorPosX( 128.f );
	if ( ImGui::InputText( "##ProjectPath", projectPathBuffer, sizeof( projectPathBuffer ) ) )
		m_sNewProjectPath = projectPathBuffer;

	ImGui::SameLine();
	if ( ImGui::Button( "Browse" ) )
	{
		VORTEK_FILESYSTEM::FileDialog fileDialog{};
		const auto selectedPath = fileDialog.SelectFolderDialog( "Open", BASE_PATH );
		if ( !selectedPath.empty() )
		{
#ifdef _WIN32
			strcpy_s( projectPathBuffer, selectedPath.c_str() );
#else
			strcpy( projectPathBuffer, selectedPath.c_str() );
#endif
			m_sNewProjectPath = selectedPath;
		}
	}

	if ( !m_sNewProjectName.empty() && !m_sNewProjectPath.empty() )
	{
		fs::path projectPath{ m_sNewProjectPath };

		if ( fs::exists( projectPath ) )
		{
			if ( IsReservedPathOrFile( projectPath ) )
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
					ProjectLoader loader{};
					if ( !loader.CreateNewProject( m_sNewProjectName, m_sNewProjectPath ) )
					{
						// TODO: Show an error dialog/message here
					}
					else
					{
						std::string fullProjectPath =
							m_sNewProjectPath + "\\" + m_sNewProjectName + "\\VORTEK_ENGINE\\" + m_sNewProjectName + ".veprj";
						SaveRecentProject( fullProjectPath );

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
	if ( VORTEK_EDITOR::g_UseNewHubUI )
	{
		DrawNewOpenProject();
	}
	else
	{
		DrawOldOpenProject();
	}
}

void Hub::DrawOldOpenProject()
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

			ImGui::SetCursorPosX( ( scrollWidth - PROJECT_BUTTON_SIZE.x ) * 0.5f );

			if ( ImGui::BeginChild( "ProjectTile", PROJECT_BUTTON_SIZE, true ) )
			{
				ImVec2 childStart = ImGui::GetCursorScreenPos();

				// Title first, so we get its position
				ImGui::Text( "%s", fs::path( project.path ).stem().string().c_str() );
				ImVec2 titleStart = ImGui::GetItemRectMin();

				// Position red X to the left of the edge (more spacing)
				ImVec2 alignedCloseBtnPos = ImVec2( childStart.x + PROJECT_BUTTON_SIZE.x - 45, // ← More to the left
													titleStart.y );
				ImGui::SetCursorScreenPos( alignedCloseBtnPos );

				ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.7f, 0.1f, 0.1f, 1.0f ) );
				ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 1.0f, 0.2f, 0.2f, 1.0f ) );
				ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.8f, 0.0f, 0.0f, 1.0f ) );

				ImGui::PushID( "close_btn" );
				if ( ImGui::Button( "X", ImVec2( 20, 20 ) ) )
				{
					// Remove from list
					m_RecentProjects.erase( std::remove_if( m_RecentProjects.begin(),
															m_RecentProjects.end(),
															[ & ]( const VORTEK_EDITOR::RecentProject& p ) {
																return p.path == project.path;
															} ),
											m_RecentProjects.end() );

					std::ofstream file( RECENT_PROJECTS_FILE, std::ios::trunc );
					for ( const auto& entry : m_RecentProjects )
						file << entry.path << "|" << entry.lastOpened << "\n";

					ImGui::PopID();
					ImGui::PopStyleColor( 3 );
					ImGui::EndChild();
					continue;
				}
				ImGui::PopID();

				if ( ImGui::IsItemHovered() )
					ImGui::SetTooltip( "Remove from recent projects" );

				ImGui::PopStyleColor( 3 );

				ImGui::TextDisabled( "Last edited: %s", ConvertTo12HourFormat( project.lastOpened ).c_str() );

				fs::path fullPath( project.path );
				std::string parentDir = fullPath.has_parent_path() ? fullPath.parent_path().filename().string() : "";
				std::string folderName = fullPath.filename().string();

				std::string displayPath;
				if ( !parentDir.empty() )
					displayPath = parentDir + " / " + folderName;
				else
					displayPath = folderName;

				ImGui::PushStyleColor( ImGuiCol_Text, ImGui::GetStyleColorVec4( ImGuiCol_TextDisabled ) );
				ImGui::Text( "%s", displayPath.c_str() );
				ImGui::PopStyleColor();

				if ( ImGui::IsItemHovered() )
				{
					ImGui::SetTooltip( "%s", fullPath.string().c_str() );
				}

				ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 10 );
				if ( ImGui::Button( "Open", ImVec2( 100, 35 ) ) )
				{
					TryOpenProject( project.path );
				}
			}
			ImGui::EndChild();

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

void Hub::DrawNewOpenProject()
{
	constexpr float headerHeight = 60.f;
	constexpr float searchWidth = 200.f;
	constexpr float buttonWidth = 100.f;
	constexpr float buttonHeight = 30.f;

	std::string searchQuery = g_searchBuffer;

	// Header
	ImGui::SetCursorPos( ImVec2{ 20.f, 20.f } );
	ImGui::Text( "Projects" );

	// Right-aligned controls: Search, Open..., New Project...
	const float spacing = ImGui::GetStyle().ItemSpacing.x;
	const float totalWidth = searchWidth + buttonWidth * 2 + spacing * 2;

	ImGui::SetCursorPos( ImVec2{ m_Width - totalWidth - 20.f, 20.f } );
	ImGui::PushItemWidth( searchWidth );
	ImGui::InputTextWithHint( "##Search", "Search projects...", g_searchBuffer, IM_ARRAYSIZE( g_searchBuffer ) );
	ImGui::PopItemWidth();

	ImGui::SameLine();
	if ( ImGui::Button( "Open...", ImVec2{ buttonWidth, buttonHeight } ) )
	{
		VORTEK_FILESYSTEM::FileDialog fileDialog;
		const auto selectedPath = fileDialog.OpenFileDialog( "Open Project", "", { "*.veprj" } );
		if ( !selectedPath.empty() )
			TryOpenProject( selectedPath );
	}

	ImGui::SameLine();
	if ( ImGui::Button( "New Project...", ImVec2{ buttonWidth, buttonHeight } ) )
		m_eState = EHubState::NewProject;

	// Project list
	ImGui::SetCursorPos( ImVec2{ 20.f, headerHeight + 10.f } );
	ImGui::BeginChild( "ProjectList", ImVec2{ m_Width - 40.f, m_Height - headerHeight - 80.f }, true );

	ImGui::Columns( 4, "project_columns", false );
	ImGui::SetColumnWidth( 0, 350.f );
	ImGui::SetColumnWidth( 1, 200.f );
	ImGui::SetColumnWidth( 2, 120.f );
	ImGui::SetColumnWidth( 3, 50.f );

	ImGui::TextDisabled( "Name" );
	ImGui::NextColumn();
	ImGui::TextDisabled( "Modified" );
	ImGui::NextColumn();
	ImGui::TextDisabled( "Editor Version" );
	ImGui::NextColumn();
	ImGui::NextColumn(); // Options column header is empty
	ImGui::Separator();

	if ( m_RecentProjects.empty() )
	{
		ImGui::TextDisabled( "No recent projects loaded!" );
	}
	else
	{
		for ( const auto& project : m_RecentProjects )
		{
			fs::path fullPath = project.path;
			std::string name = fullPath.stem().string();
			std::string path = fullPath.string();

			// Filter projects based on search query
			if ( !searchQuery.empty() )
			{
				std::string combined = name + " " + path;
				if ( combined.find( searchQuery ) == std::string::npos )
					continue;
			}

			ImGui::PushID( project.path.c_str() );

			const float rowStartY = ImGui::GetCursorPosY() + 25.f;

			// Column 0: Icon + Name + Path
			ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 2.f );
			ImGui::Text( ICON_FA_FOLDER );
			ImGui::SameLine();

			ImGui::BeginGroup();
			ImGui::Text( "%s", name.c_str() );
			ImGui::PushStyleColor( ImGuiCol_Text, ImGui::GetStyleColorVec4( ImGuiCol_TextDisabled ) );
			ImGui::Text( "%s", path.c_str() );
			if ( ImGui::IsItemHovered() )
				ImGui::SetTooltip( "%s", fullPath.string().c_str() );
			ImGui::PopStyleColor();
			ImGui::EndGroup();
			ImGui::NextColumn();

			// Column 1: Last Modified
			ImGui::SetCursorPosY( rowStartY );
			ImGui::BeginGroup();
			ImGui::Text( "%s", ConvertTo12HourFormat( project.lastOpened ).c_str() );
			ImGui::EndGroup();
			ImGui::NextColumn();

			// Column 2: Editor Version (stubbed)
			ImGui::SetCursorPosY( rowStartY );
			ImGui::TextDisabled( "v1.0.0" );
			ImGui::NextColumn();

			// Column 3: Options Button
			ImGui::SetCursorPosY( rowStartY );
			if ( ImGui::Button( ICON_FA_COG "##options" ) )
				ImGui::OpenPopup( "OptionsMenu" );

			if ( ImGui::BeginPopup( "OptionsMenu" ) )
			{
				if ( ImGui::MenuItem( "Open" ) )
					TryOpenProject( path );

				if ( ImGui::MenuItem( "Remove from list" ) )
				{
					m_RecentProjects.erase(
						std::remove_if( m_RecentProjects.begin(),
										m_RecentProjects.end(),
										[ & ]( const RecentProject& p ) { return p.path == path; } ),
						m_RecentProjects.end() );

					// Update recent projects file
					std::ofstream file( RECENT_PROJECTS_FILE, std::ios::trunc );
					for ( const auto& entry : m_RecentProjects )
						file << entry.path << "|" << entry.lastOpened << "\n";
				}

				ImGui::EndPopup();
			}

			ImGui::NextColumn();
			ImGui::PopID();
		}
	}

	ImGui::EndChild();

	// Cancel button bottom-right
	ImGui::SetCursorPos( ImVec2{ m_Width - BUTTON_SIZE.x - 20.f, m_Height - BUTTON_SIZE.y - 20.f } );
	if ( ImGui::Button( "Cancel", BUTTON_SIZE ) )
	{
		m_eState = EHubState::Default;
		m_sPrevProjectName.clear();
		m_sPrevProjectPath.clear();
	}
}

void Hub::ProcessEvents()
{
	while ( SDL_PollEvent( &m_Event ) )
	{
		ImGui_ImplSDL2_ProcessEvent( &m_Event );

		if ( m_Event.type == SDL_QUIT )
			m_bRunning = false;
	}
}

void Hub::Update()
{
	// Currently no logic needed here.
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

	ProjectLoader loader;
	if ( loader.LoadProject( m_sPrevProjectPath ) )
	{
		SaveRecentProject( m_sPrevProjectPath );
		m_bRunning = false;
		m_eState = EHubState::OpenProject;
		return true;
	}

	VORTEK_ERROR( "Failed to load project: {}", m_sPrevProjectPath );
	return false;
}

void Hub::LoadRecentProjects()
{
	m_RecentProjects.clear();

	std::ifstream file( RECENT_PROJECTS_FILE );
	if ( !file.is_open() )
		return;

	std::vector<RecentProject> validProjects;
	std::string line;
	while ( std::getline( file, line ) )
	{
		if ( line.empty() )
			continue;

		auto delimiterPos = line.find( '|' );
		if ( delimiterPos == std::string::npos )
			continue;

		std::string path = line.substr( 0, delimiterPos );
		std::string lastOpened = line.substr( delimiterPos + 1 );

		if ( fs::exists( path ) )
		{
			validProjects.push_back( { path, lastOpened } );
		}
		// If file doesn't exist, we skip adding it => removing invalid entries
	}
	file.close();

	// Overwrite the recent projects file with only valid entries
	std::ofstream outFile( RECENT_PROJECTS_FILE, std::ios::trunc );
	for ( const auto& entry : validProjects )
		outFile << entry.path << "|" << entry.lastOpened << "\n";
	outFile.close();

	m_RecentProjects = std::move( validProjects );
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

	// Removed project count limit here

	std::ofstream file( RECENT_PROJECTS_FILE, std::ios::trunc );
	for ( const auto& entry : m_RecentProjects )
		file << entry.path << "|" << entry.lastOpened << "\n";
}

std::string Hub::ConvertTo12HourFormat( const std::string& time )
{
	// Expecting format: "YYYY-MM-DD HH:MM:SS"
	if ( time.size() < 16 )
		return time; // Invalid format; return original

	int hour = std::stoi( time.substr( 11, 2 ) );
	int minute = std::stoi( time.substr( 14, 2 ) );

	const char* ampm = ( hour >= 12 ) ? "PM" : "AM";
	hour = hour % 12;
	if ( hour == 0 )
		hour = 12;

	std::ostringstream oss;
	oss << time.substr( 0, 11 ); // date part "YYYY-MM-DD "
	oss << std::setw( 2 ) << std::setfill( '0' ) << hour << ":";
	oss << std::setw( 2 ) << std::setfill( '0' ) << minute << " ";
	oss << ampm;

	return oss.str();
}

} // namespace VORTEK_EDITOR
