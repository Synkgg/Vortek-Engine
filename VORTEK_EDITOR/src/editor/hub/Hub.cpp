#include "editor/hub/Hub.h"

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

#include <VortekUtilities/VortekUtilities.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL_opengl.h>

#include <fstream>
#include <iomanip>
#include <sstream>

using namespace Vortek::Utilities;

constexpr ImVec2 BUTTON_SIZE{ 100.f, 25.f };
constexpr ImVec2 PROJECT_BUTTON_SIZE{ 400.f, 135.f };
std::vector<Vortek::Editor::RecentProject> m_RecentProjects;
const std::string RECENT_PROJECTS_FILE = "recent_projects.txt";

static char g_searchBuffer[ 128 ] = "";

constexpr float HUB_HEADER_HEIGHT = 60.f;

namespace Vortek::Editor
{

namespace ButtonStyles
{
constexpr ImU32 Primary = IM_COL32( 70, 120, 255, 255 );
constexpr ImU32 PrimaryHover = IM_COL32( 90, 140, 255, 255 );
constexpr ImU32 PrimaryActive = IM_COL32( 60, 105, 230, 255 );

constexpr ImU32 Secondary = IM_COL32( 55, 55, 55, 255 );
constexpr ImU32 SecondaryHover = IM_COL32( 75, 75, 75, 255 );
constexpr ImU32 SecondaryActive = IM_COL32( 65, 65, 65, 255 );

constexpr ImVec2 PrimarySize = { 260.f, 45.f };
constexpr ImVec2 SecondarySize = { 180.f, 45.f };
}

static bool PrimaryButton( const char* label, ImVec2 size )
{
	ImGui::PushStyleColor( ImGuiCol_Button, ButtonStyles::Primary );
	ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ButtonStyles::PrimaryHover );
	ImGui::PushStyleColor( ImGuiCol_ButtonActive, ButtonStyles::PrimaryActive );

	bool pressed = ImGui::Button( label, size );

	ImGui::PopStyleColor( 3 );
	return pressed;
}

static bool SecondaryButton( const char* label, ImVec2 size )
{
	ImGui::PushStyleColor( ImGuiCol_Button, ButtonStyles::Secondary );
	ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ButtonStyles::SecondaryHover );
	ImGui::PushStyleColor( ImGuiCol_ButtonActive, ButtonStyles::SecondaryActive );

	bool pressed = ImGui::Button( label, size );

	ImGui::PopStyleColor( 3 );
	return pressed;
}

Hub::Hub( Vortek::Windowing::Window& window )
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
	Vortek::Editor::LoadEditorConfig();

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

	ImGui::SetWindowPos( ImGui::GetMainViewport()->Pos );
	ImGui::SetWindowSize( ImVec2{ m_Width, m_Height } );

	DrawCloseButton(); // ← add this line

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
	const float centerX = m_Width * 0.5f;

	// Title
	ImGui::SetCursorPosY( HUB_HEADER_HEIGHT + 40.f );

	// center dynamically based on text size
	ImGui::PushFont( ImGui::GetIO().Fonts->Fonts[ 1 ] ); // optional if using custom fonts

	ImGui::SetWindowFontScale( 2.5f ); // increase size
	const char* title = "VORTEK ENGINE";
	float titleWidth = ImGui::CalcTextSize( title ).x;

	ImGui::SetCursorPosX( centerX - titleWidth * 0.5f );
	ImGui::Text( "%s", title );

	ImGui::SetWindowFontScale( 1.0f );
	ImGui::PopFont();

	ImGui::Spacing();
	ImGui::Spacing();

	// Subtitle
	ImGui::SetCursorPosX( centerX - 80.f );
	ImGui::TextDisabled( "Recent Projects" );

	ImGui::Spacing();
	ImGui::Spacing();

	// Project cards
	const float cardWidth = 600.f;
	const float cardHeight = 70.f;

	for ( const auto& project : m_RecentProjects )
	{
		fs::path path( project.path );
		std::string name = path.stem().string();

		ImGui::SetCursorPosX( centerX - cardWidth * 0.5f );

		ImGui::PushID( project.path.c_str() );

		ImGui::PushStyleColor( ImGuiCol_ChildBg, IM_COL32( 36, 36, 36, 255 ) );
		ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 8.f );

		if ( ImGui::BeginChild( "ProjectCard", ImVec2( cardWidth, cardHeight ), true ) )
		{
			bool hovered = ImGui::IsWindowHovered();
			bool clicked = hovered && ImGui::IsMouseClicked( 0 );

			ImVec2 pos = ImGui::GetWindowPos();
			ImVec2 size = ImGui::GetWindowSize();
			ImVec2 end = ImVec2( pos.x + size.x, pos.y + size.y );

			ImU32 color = hovered ? IM_COL32( 60, 60, 60, 255 ) : IM_COL32( 36, 36, 36, 255 );

			ImGui::GetWindowDrawList()->AddRectFilled( pos, end, color, 8.f );

			ImGui::SetCursorPos( ImVec2( 20, 12 ) );
			ImGui::Text( "%s", name.c_str() );

			ImGui::SetCursorPos( ImVec2( 20, 35 ) );
			ImGui::TextDisabled( "%s", StringUtils::ConvertTo12HourFormat( project.lastOpened ).c_str() );

			if ( clicked )
			{
				TryOpenProject( project.path );
			}
		}

		ImGui::EndChild();

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		ImGui::Spacing();

		ImGui::PopID();
	}

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	float totalWidth = ButtonStyles::PrimarySize.x + 15.f + ButtonStyles::SecondarySize.x;

	float startX = centerX - totalWidth * 0.5f;

	ImGui::SetCursorPosX( startX );

	if ( PrimaryButton( ICON_FA_PLUS " Create New Project", ButtonStyles::PrimarySize ) )
		m_eState = EHubState::NewProject;

	ImGui::SameLine( 0, 15.f );

	if ( SecondaryButton( ICON_FA_FOLDER_OPEN " Open Project", ButtonStyles::SecondarySize ) )
		m_eState = EHubState::OpenProject;
}

void Hub::DrawNewProject()
{
	const float panelWidth = 600.f;
	const float panelHeight = 300.f;

	const float startX = m_Width * 0.5f - panelWidth * 0.5f;
	const float startY = HUB_HEADER_HEIGHT + ( m_Height - HUB_HEADER_HEIGHT ) * 0.5f - panelHeight * 0.5f;

	ImGui::SetCursorPos( ImVec2( startX, startY ) );

	ImGui::PushStyleColor( ImGuiCol_ChildBg, IM_COL32( 32, 32, 32, 255 ) );
	ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 10.f );

	ImGui::BeginChild( "CreateProjectPanel", ImVec2( panelWidth, panelHeight ), true );

	ImGui::Text( "Create New Project" );

	ImGui::Spacing();

	char nameBuffer[ 256 ];
	char pathBuffer[ 256 ];

#ifdef _WIN32
	strcpy_s( nameBuffer, m_sNewProjectName.c_str() );
	strcpy_s( pathBuffer, m_sNewProjectPath.c_str() );
#else
	strcpy( nameBuffer, m_sNewProjectName.c_str() );
	strcpy( pathBuffer, m_sNewProjectPath.c_str() );
#endif

	ImGui::Text( "Project Name" );
	ImGui::InputText( "##Name", nameBuffer, sizeof( nameBuffer ) );
	m_sNewProjectName = nameBuffer;

	ImGui::Spacing();

	ImGui::Text( "Location" );
	ImGui::InputText( "##Path", pathBuffer, sizeof( pathBuffer ) );
	m_sNewProjectPath = pathBuffer;

	ImGui::SameLine();

	if ( ImGui::Button( "Browse" ) )
	{
		Vortek::Filesystem::FileDialog dialog;
		auto path = dialog.SelectFolderDialog( "Select Folder", BASE_PATH );
		if ( !path.empty() )
			m_sNewProjectPath = path;
	}

	ImGui::Spacing();
	ImGui::Spacing();

	float totalWidth = 200.f + 15.f + 140.f;
	float buttonStartX = panelWidth * 0.5f - totalWidth * 0.5f;

	ImGui::SetCursorPosX( buttonStartX );

	if ( PrimaryButton( "Create Project", { 200.f, 40.f } ) )
	{
		ProjectLoader loader;

		if ( loader.CreateNewProject( m_sNewProjectName, m_sNewProjectPath ) )
		{
			std::string full =
				m_sNewProjectPath + "\\" + m_sNewProjectName + "\\VORTEK_ENGINE\\" + m_sNewProjectName + ".veprj";

			SaveRecentProject( full );

			m_bRunning = false;
			m_eState = EHubState::CreateNew;
		}
	}

	ImGui::SameLine( 0, 15.f );

	if ( SecondaryButton( "Cancel", { 140.f, 40.f } ) )
		m_eState = EHubState::Default;

	ImGui::EndChild();

	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}

void Hub::DrawOpenProject()
{
	if ( Vortek::Editor::g_UseNewHubUI )
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
															[ & ]( const Vortek::Editor::RecentProject& p ) {
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

				ImGui::TextDisabled( "Last edited: %s",
									 StringUtils::ConvertTo12HourFormat( project.lastOpened ).c_str() );

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
		Vortek::Filesystem::FileDialog fd;
		const auto sFilepath = fd.OpenFileDialog( "Open", BASE_PATH, { "*.veprj" }, "Vortek Engine Project (*.veprj)" );

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
	constexpr float buttonHeight = 25.f;

	std::string searchQuery = g_searchBuffer;

	// Header
	ImGui::SetCursorPos( ImVec2{ 20.f, HUB_HEADER_HEIGHT - 0.f } );
	ImGui::Text( "Projects" );

	// Right-aligned controls: Search, Open..., New Project...
	const float spacing = ImGui::GetStyle().ItemSpacing.x;
	const float totalWidth = searchWidth + buttonWidth * 2 + spacing * 2;

	ImGui::SetCursorPos( ImVec2{ m_Width - totalWidth - 60.f, HUB_HEADER_HEIGHT - 0.f } );
	ImGui::PushItemWidth( searchWidth );
	ImGui::InputTextWithHint( "##Search", "Search projects...", g_searchBuffer, IM_ARRAYSIZE( g_searchBuffer ) );
	ImGui::PopItemWidth();

	ImGui::SameLine();

	if ( SecondaryButton( "Open...", { 110.f, 30.f } ) )
	{
		Vortek::Filesystem::FileDialog fd;
		auto selectedPath = fd.OpenFileDialog( "Open", BASE_PATH, { "*.veprj", "*.s2dprj" });
		if ( !selectedPath.empty() )
			TryOpenProject( selectedPath );
	}

	ImGui::SameLine();

	if ( PrimaryButton( "New Project...", { 140.f, 30.f } ) )
		m_eState = EHubState::NewProject;

	// Project list
	ImGui::SetCursorPos( ImVec2{ 20.f, HUB_HEADER_HEIGHT + 60.f } );

	const float cancelHeight = BUTTON_SIZE.y + 20.f;

	ImGui::BeginChild( "ProjectList", ImVec2{ m_Width - 40.f, m_Height - headerHeight - 80.f - cancelHeight }, true );

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
			ImGui::Text( "%s", StringUtils::ConvertTo12HourFormat( project.lastOpened ).c_str() );
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

	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::SetCursorPosX( ImGui::GetWindowContentRegionMax().x - BUTTON_SIZE.x - 20.f );

	if ( SecondaryButton( "Cancel", BUTTON_SIZE ) )
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
											[ & ]( const Vortek::Editor::RecentProject& p ) { return p.path == path; } ),
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

void Hub::DrawCloseButton()
{
	const float size = 32.f;
	const float padding = 10.f;

	ImGui::SetCursorPos( ImVec2( m_Width - size - padding, HUB_HEADER_HEIGHT * 0.5f - size * 0.5f ) );

	ImGui::PushStyleColor( ImGuiCol_Button, IM_COL32( 60, 60, 60, 255 ) );
	ImGui::PushStyleColor( ImGuiCol_ButtonHovered, IM_COL32( 200, 60, 60, 255 ) );
	ImGui::PushStyleColor( ImGuiCol_ButtonActive, IM_COL32( 160, 40, 40, 255 ) );

	if ( ImGui::Button( ICON_FA_TIMES, ImVec2( size, size ) ) )
	{
		m_eState = EHubState::Close;
		m_bRunning = false;
	}

	ImGui::PopStyleColor( 3 );

	if ( ImGui::IsItemHovered() )
		ImGui::SetTooltip( "Close Hub" );
}

} // namespace Vortek::Editor
