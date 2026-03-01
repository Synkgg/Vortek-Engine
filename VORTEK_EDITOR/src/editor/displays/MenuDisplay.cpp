#include "editor/displays/MenuDisplay.h"
#include "Logger/Logger.h"
#include "VortekFilesystem/Dialogs/FileDialog.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/CoreUtilities/Prefab.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/MainRegistry.h"

#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"
#include "editor/tools/ToolManager.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "editor/utilities/EditorState.h"
#include "Core/CoreUtilities/ProjectInfo.h"
#include "editor/loaders/ProjectLoader.h"
#include "editor/loaders/TiledMapImporter.h"

#include "Core/Events/EventDispatcher.h"
#include "editor/events/EditorEventTypes.h"

#include "VortekUtilities/VortekUtilities.h"

#include <imgui.h>
#include <SDL.h>

#include <imgui_notify.h>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <fstream>

#include "editor/utilities/EditorSettings.h"

void SaveEditorConfig( int selectedTheme )
{
	rapidjson::Document doc;
	doc.SetObject();

	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
	doc.AddMember( "themeIndex", selectedTheme, allocator );

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer( buffer );
	doc.Accept( writer );

	std::ofstream file( "editor_config.json" );
	if ( file.is_open() )
	{
		file << buffer.GetString();
		file.close();
	}
}

int LoadEditorThemeIndex()
{
	std::ifstream file( "editor_config.json" );
	if ( file.is_open() )
	{
		std::string content( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );

		rapidjson::Document doc;
		if ( !doc.Parse( content.c_str() ).HasParseError() )
		{
			if ( doc.HasMember( "themeIndex" ) && doc[ "themeIndex" ].IsInt() )
			{
				return doc[ "themeIndex" ].GetInt();
			}
		}
	}

	return 3; // Default to Vortek Custom
}

namespace Vortek::Editor
{

static bool bOpenEditorSettings = false;
static int selectedTheme = LoadEditorThemeIndex();
static const char* themes[] = { "ImGui Dark", "ImGui Light", "ImGui Classic", "Vortek Custom" };
static int activeTabIndex = 0;

void ApplyThemeByIndex( int index )
{
	switch ( index )
	{
	case 0: ImGui::StyleColorsDark(); break;
	case 1: ImGui::StyleColorsLight(); break;
	case 2: ImGui::StyleColorsClassic(); break;
	case 3: ImGui::InitDefaultStyles(); break;
	}
}

void MenuDisplay::Draw()
{
	auto& sceneManager = SCENE_MANAGER();
	if ( ImGui::BeginMainMenuBar() )
	{
		if ( ImGui::BeginMenu( ICON_FA_FILE " File" ) )
		{
			ImGui::SeparatorText( "Project" );
			ImGui::InlineLabel( ICON_FA_FILE_ALT, 32.f );
			if ( ImGui::MenuItem( "New", "Ctrl + N" ) )
			{
				VORTEK_ERROR( "New -- Not Implemented" );
			}

			ImGui::InlineLabel( ICON_FA_FOLDER_OPEN, 32.f );
			if ( ImGui::MenuItem( "Open", "Ctrl + O" ) )
			{
				VORTEK_ERROR( "Open -- Not Implemented" );
			}
			ImGui::InlineLabel( ICON_FA_SAVE, 32.f );
			if ( ImGui::MenuItem( "Save All", "Ctrl + S" ) )
			{
				auto& pProjectInfo = MAIN_REGISTRY().GetContext<Vortek::Core::ProjectInfoPtr>();
				VORTEK_ASSERT( pProjectInfo && "Project Info must exist!" );

				// Save entire project
				ProjectLoader pl{};
				if ( !pl.SaveLoadedProject( *pProjectInfo ) )
				{
					auto optProjectFilePath = pProjectInfo->GetProjectFilePath();
					VORTEK_ASSERT( optProjectFilePath && "Project file path not setup correctly." );
					VORTEK_ERROR( "Failed to save project [{}] at file [{}]",
								  pProjectInfo->GetProjectName(),
								  optProjectFilePath->string() );

					ImGui::InsertNotification( { ImGuiToastType_Error, 3000, "Project failed to save!" } );
				}
				else
				{
					ImGui::InsertNotification( { ImGuiToastType_Success, 3000, "Project save was a success!" } );
				}
			}
			ImGui::ItemToolTip( "Saves entire project to disk." );

			ImGui::SeparatorText( "Scenes" );

			if ( auto pCurrentScene = sceneManager.GetCurrentScene() )
			{
				ImGui::InlineLabel( ICON_FA_SAVE, 32.f );
				if ( ImGui::MenuItem( "Save Current Scene As..." ) )
				{
					// TODO: Save a copy of the scene and all of it's entities under a new name.
				}
			}

			ImGui::SeparatorText( "Import" );
			ImGui::InlineLabel( ICON_FA_FILE_IMPORT, 32.f );
			if ( ImGui::MenuItem( "Import Tiled Map" ) )
			{
				Vortek::Filesystem::FileDialog fd{};
				const auto sFilepath =
					fd.OpenFileDialog( "Import Tiled Map", BASE_PATH, { "*.lua", "*.tmx" }, "Tiled Map Files (*.lua, *.tmx)" );

				if ( !sFilepath.empty() )
				{
					if ( !TiledMapImporter::ImportTilemapFromTiled( &SCENE_MANAGER(), sFilepath ) )
					{
						VORTEK_ERROR( "Failed to import tiled map as new scene." );
					}
				}
			}

			ImGui::ItemToolTip( "Import a map from the Tiled Map Editor. Supports tmx and lua map files." );

			ImGui::SeparatorText( "Exit" );
			ImGui::InlineLabel( ICON_FA_WINDOW_CLOSE, 32.f );
			if ( ImGui::MenuItem( "Exit" ) )
			{
				EVENT_DISPATCHER().EmitEvent( Events::CloseEditorEvent{} );
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( ICON_FA_EDIT " Edit" ) )
		{
			ImGui::SeparatorText( "Configuration" );

			auto& coreGlobals = CORE_GLOBALS();
			auto& toolManager = TOOL_MANAGER();

			bool bGridSnap{ toolManager.IsGridSnapEnabled() };
			if ( ImGui::Checkbox( "Enable Gridsnap", &bGridSnap ) )
			{
				toolManager.EnableGridSnap( bGridSnap );
			}

			bool bShowCollision{ coreGlobals.RenderCollidersEnabled() };
			if ( ImGui::Checkbox( "Show Collision", &bShowCollision ) )
			{
				bShowCollision ? coreGlobals.EnableColliderRender() : coreGlobals.DisableColliderRender();
			}

			bool bShowAnimations{ coreGlobals.AnimationRenderEnabled() };
			if ( ImGui::Checkbox( "Show Animations", &bShowAnimations ) )
			{
				bShowAnimations ? coreGlobals.EnableAnimationRender() : coreGlobals.DisableAnimationRender();
			}

			ImGui::SeparatorText( "Editor Configuration" );

			ImGui::InlineLabel( ICON_FA_COG, 32.f );
			if ( ImGui::MenuItem( "Editor Settings", "" ) )
			{
				bOpenEditorSettings = true;
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( ICON_FA_WINDOW_MAXIMIZE " Displays" ) )
		{
			ImGui::SeparatorText( "Displays" );

			auto& pEditorState = MAIN_REGISTRY().GetContext<EditorStatePtr>();
			DrawDisplayItem( *pEditorState, ICON_FA_FILE_ALT " Asset Browser", EDisplay::AssetBrowser );
			DrawDisplayItem( *pEditorState, ICON_FA_FOLDER " Content Browser", EDisplay::ContentBrowser );
			DrawDisplayItem( *pEditorState, ICON_FA_CODE " Script List", EDisplay::ScriptListView );
			DrawDisplayItem( *pEditorState, ICON_FA_ARCHIVE " Game Packager", EDisplay::PackagerView );
			DrawDisplayItem( *pEditorState, ICON_FA_TERMINAL " Console Logger", EDisplay::Console );
			DrawDisplayItem( *pEditorState, ICON_FA_COG " Project Settings", EDisplay::GameSettingsView );
			DrawDisplayItem( *pEditorState, ICON_FA_DESKTOP " Script Editor", EDisplay::ScriptEditor );

			ImGui::Separator();
			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( ICON_FA_VIDEO " Scene" ) )
		{
			if ( auto pCurrentScene = SCENE_MANAGER().GetCurrentScene() )
			{
				ImGui::Text( "Current Scene" );
				ImGui::Separator();
				if ( ImGui::TreeNode( ICON_FA_FILE_IMAGE " Canvas" ) )
				{
					auto& canvas = pCurrentScene->GetCanvas();

					ImGui::InlineLabel( "width" );
					if ( ImGui::InputInt( "##_width", &canvas.width, canvas.tileWidth, canvas.tileWidth ) )
					{
						if ( canvas.width < 640 )
							canvas.width = 640;
					}
					ImGui::ItemToolTip( "Canvas Width - Clamped minimum = 640" );

					ImGui::InlineLabel( "height" );
					if ( ImGui::InputInt( "##_height", &canvas.height, canvas.tileHeight, canvas.tileHeight ) )
					{
						if ( canvas.height < 320 )
							canvas.height = 320;
					}
					ImGui::ItemToolTip( "Canvas Height - Clamped minimum = 320" );

					ImGui::InlineLabel( "tile width" );
					if ( ImGui::InputInt( "##tile_width", &canvas.tileWidth, 8, 8 ) )
					{
						canvas.tileWidth = std::clamp( canvas.tileWidth, 8, 128 );
					}
					ImGui::ItemToolTip( "Tile Width - Range [8 : 128]" );

					ImGui::InlineLabel( "tile height" );
					if ( ImGui::InputInt( "##tile_height", &canvas.tileHeight, 8, 8 ) )
					{
						canvas.tileHeight = std::clamp( canvas.tileHeight, 8, 128 );
					}
					ImGui::ItemToolTip( "Tile Height - Range [8 : 128]" );

					ImGui::TreePop();
				}
				ImGui::Separator();
				if ( ImGui::TreeNode( ICON_FA_COG " Settings" ) )
				{
					bool bChanged{ false };
					bool bPlayerStartEnabled{ pCurrentScene->IsPlayerStartEnabled() };
					ImGui::InlineLabel( "Enable Player Start:" );
					ImGui::SetCursorPosX( 250.f );

					ImGui::ItemToolTip(
						"Enable or Disable the player start.\n"
						"The player start is the character that we want to use when the scene is played." );

					if ( ImGui::Checkbox( "##_enablePlayerStart", &bPlayerStartEnabled ) )
					{
						pCurrentScene->SetPlayerStartEnabled( bPlayerStartEnabled );
						if ( bPlayerStartEnabled )
						{
							pCurrentScene->GetPlayerStart().LoadVisualEntity();
						}
						else
						{
							pCurrentScene->GetPlayerStart().Unload();
						}
					}

					std::string sPlayerStartCharacter{ pCurrentScene->GetPlayerStart().GetCharacterName() };
					auto prefabs =
						Vortek::Utilities::GetKeys( ASSET_MANAGER().GetAllPrefabs() /*, []( auto& prefab ) {
			  return prefab.second->GetType() == Vortek::Core::EPrefabType::Character;
		  } */ );

					ImGui::InlineLabel( ICON_FA_FLAG ICON_FA_GAMEPAD " Player Start Character:" );
					ImGui::SetCursorPosX( 250.f );
					ImGui::ItemToolTip( "The default player to spawn when starting the scene." );
					if ( ImGui::BeginCombo( "##DefaultPlayerStart", sPlayerStartCharacter.c_str() ) )
					{
						for ( const auto& sPrefabName : prefabs )
						{
							if ( ImGui::Selectable( sPrefabName.c_str(), sPrefabName == sPlayerStartCharacter ) )
							{
								sPlayerStartCharacter = sPrefabName;

								bChanged = true;
							}
						}

						ImGui::EndCombo();
					}

					if ( bChanged )
					{
						if ( auto pPrefab = ASSET_MANAGER().GetPrefab( sPlayerStartCharacter ) )
						{
							pCurrentScene->GetPlayerStart().SetCharacter( *pPrefab );
						}
					}

					bChanged = true;

					auto musicNames = Vortek::Utilities::GetKeys( ASSET_MANAGER().GetAllMusic() );
					musicNames.push_back( "None" );

					std::string sDefaultSceneMusic{ pCurrentScene->GetDefaultMusicName() };
					if ( sDefaultSceneMusic.empty() )
						sDefaultSceneMusic = "None";

					ImGui::InlineLabel( ICON_FA_MUSIC " Default Music:" );
					ImGui::SetCursorPosX( 250.f );
					ImGui::ItemToolTip( "Music to play when the scene starts." );
					if ( ImGui::BeginCombo( "##DefaultMusic", sDefaultSceneMusic.c_str() ) )
					{
						for ( const auto& sMusicName : musicNames )
						{
							if ( ImGui::Selectable( sMusicName.c_str(), sMusicName == sDefaultSceneMusic ) )
							{
								sDefaultSceneMusic = sMusicName;
								bChanged = true;
							}
						}

						ImGui::EndCombo();
					}

					if ( bChanged )
					{
						pCurrentScene->SetDefaultMusic( sDefaultSceneMusic );
					}

					ImGui::TreePop();
				}
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( ICON_FA_QUESTION_CIRCLE " Help" ) )
		{
			if ( ImGui::TreeNode( "About Vortek Engine" ) )
			{
				ImGui::Separator();
				ImGui::Text( "Vortek Engine is based on the Youtube Series 'VORTEK2D' From" );
				ImGui::SameLine();
				ImGui::TextLinkOpenURL( "Dustin Clark", "https://github.com/dwjclark11" );
				ImGui::Separator();
				ImGui::AddSpaces( 2 );

				ImGui::SeparatorText( "Helpful Links: " );
				ImGui::TextLinkOpenURL( "Github", "https://github.com/Synkgg/Vortek-Engine" );
				ImGui::TextLinkOpenURL( "YouTube", "https://www.youtube.com/watch?v=dQw4w9WgXcQ" );
				ImGui::TextLinkOpenURL( "Documentation", "https://synkgg.github.io/Vortek-Engine/" );
				ImGui::Separator();
				ImGui::TreePop();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();

		if ( bOpenEditorSettings )
		{
			ImGui::Begin( "Editor Settings",
						  &bOpenEditorSettings,
						  ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse );

			// Split layout into two columns (sidebar tabs + content)
			ImGui::BeginChild( "Sidebar", ImVec2( 150, 300 ), true ); // Left panel for vertical tabs

			static int selectedTab = 0;

			if ( ImGui::Selectable( "Theme", selectedTab == 0 ) )
				selectedTab = 0;
			if ( ImGui::Selectable( "General", selectedTab == 1 ) )
				selectedTab = 1;
			if ( ImGui::Selectable( "Keybinds", selectedTab == 2 ) )
				selectedTab = 2;

			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::BeginChild( "TabContent", ImVec2( 400, 300 ), true ); // Right panel for content
			if ( selectedTab == 0 )
			{
				ImGui::Text( "Theme Settings" );
				ImGui::Separator();
				ImGui::Text( "Editor Theme" );

				ImGui::Combo( "##ThemeCombo", &g_SelectedThemeIndex, themes, IM_ARRAYSIZE( themes ) );

				if ( ImGui::Button( "Apply Theme" ) )
				{
					g_AppliedThemeIndex = g_SelectedThemeIndex; // Set the active one
					ApplyThemeByIndex( g_AppliedThemeIndex );
					Vortek::Editor::SaveEditorConfig( Vortek::Editor::g_SelectedThemeIndex,
													  Vortek::Editor::g_UseNewHubUI );
				}
			}
			else if ( selectedTab == 1 )
			{
				ImGui::Text( "General Settings" );
				ImGui::Separator();

				if ( ImGui::Checkbox( "Use New Hub UI", &Vortek::Editor::g_UseNewHubUI ) )
				{
					// Save config immediately on toggle
					Vortek::Editor::SaveEditorConfig( Vortek::Editor::g_SelectedThemeIndex,
													  Vortek::Editor::g_UseNewHubUI );
				}
			}
			else if ( selectedTab == 2 )
			{
				ImGui::Text( "Keybind Settings" );
			}
			ImGui::EndChild();

			ImGui::End();
		}
	}
}

void MenuDisplay::DrawDisplayItem( EditorState& editorState, const std::string& sDisplayName, const EDisplay eDisplay )
{
	bool bDisplayEnabled{ editorState.IsDisplayOpen( eDisplay ) };
	if ( ImGui::Selectable( sDisplayName.c_str(), false, ImGuiSelectableFlags_DontClosePopups ) )
	{
		bDisplayEnabled = !bDisplayEnabled;
		editorState.SetDisplay( eDisplay, bDisplayEnabled );
	}

	if ( bDisplayEnabled )
	{
		ImGui::SameLine();
		ImGui::SetCursorPosX( 150.f );
		ImGui::Text( ICON_FA_CHECK );
	}
}

} // namespace Vortek::Editor
