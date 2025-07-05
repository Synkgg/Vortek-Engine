#include "MenuDisplay.h"
#include "Logger/Logger.h"
#include "VortekFilesystem/Dialogs/FileDialog.h"
#include "Core/Loaders/TilemapLoader.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/CoreUtilities/Prefab.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/MainRegistry.h"

#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"
#include "editor/tools/ToolManager.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "Core/CoreUtilities/SaveProject.h"
#include "editor/loaders/ProjectLoader.h"

#include "Core/Events/EventDispatcher.h"
#include "editor/events/EditorEventTypes.h"

#include "VORTEKUtilities/VORTEKUtilities.h"

#include <imgui.h>
#include <SDL.h>

#include <Notifications/imgui_notify.h>

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

namespace VORTEK_EDITOR
{
	static bool bOpenEditorSettings = false;
	static int selectedTheme = LoadEditorThemeIndex();
	static const char* themes[] = { "ImGui Dark", "ImGui Light", "ImGui Classic", "Vortek Custom" };
	static int activeTabIndex = 0;

	void ApplyThemeByIndex(int index) {
		switch (index)
		{
		case 0: ImGui::StyleColorsDark(); break;
		case 1: ImGui::StyleColorsLight(); break;
		case 2: ImGui::StyleColorsClassic(); break;
		case 3: ImGui::InitDefaultStyles(); break;
		}
	}

	void MenuDisplay::Draw()
	{
		static bool bThemeInitialized = false;

		if ( !bThemeInitialized )
		{
			g_SelectedThemeIndex = LoadEditorThemeIndex();
			g_AppliedThemeIndex = g_SelectedThemeIndex;
			ApplyThemeByIndex( g_AppliedThemeIndex );
			bThemeInitialized = true;
		}

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu(ICON_FA_FILE " File"))
			{
				ImGui::InlineLabel(ICON_FA_FILE_ALT, 32.f);
				if (ImGui::MenuItem("New", "Ctrl + N"))
				{
					VORTEK_ERROR("New -- Not Implemented");
				}

				ImGui::InlineLabel(ICON_FA_FOLDER_OPEN, 32.f);
				if (ImGui::MenuItem("Open", "Ctrl + O"))
				{
					VORTEK_ERROR("Open -- Not Implemented");
				}
				ImGui::InlineLabel(ICON_FA_SAVE, 32.f);
				if (ImGui::MenuItem("Save", "Ctrl + S"))
				{
					auto& pSaveProject = MAIN_REGISTRY().GetContext<std::shared_ptr<VORTEK_CORE::SaveProject>>();
					VORTEK_ASSERT(pSaveProject && "Save Project must exist!");
					// Save entire project
					ProjectLoader pl{};
					if (pl.SaveLoadedProject(*pSaveProject))
					{
						ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Project save was a success!" });
					}
					else
					{
						VORTEK_ERROR("Failed to save project [{}] at file [{}]",
							pSaveProject->sProjectName,
							pSaveProject->sProjectFilePath);

						ImGui::InsertNotification({ ImGuiToastType_Error, 3000, "Project failed to save!" });
					}
				}

				ImGui::InlineLabel(ICON_FA_COG, 32.f);
				if (ImGui::MenuItem("Editor Settings", "SOON"))
				{
					bOpenEditorSettings = true;
				}

				ImGui::InlineLabel(ICON_FA_WINDOW_CLOSE, 32.f);
				if (ImGui::MenuItem("Exit"))
				{
					EVENT_DISPATCHER().EmitEvent(Events::CloseEditorEvent{});
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu(ICON_FA_EDIT " Edit"))
			{
				auto& coreGlobals = CORE_GLOBALS();

				static bool bGridSnap{ true };
				if (ImGui::Checkbox("Enable Gridsnap", &bGridSnap))
				{
					SCENE_MANAGER().GetToolManager().EnableGridSnap(bGridSnap);
				}

				static bool bShowCollision{ false };
				if (ImGui::Checkbox("Show Collision", &bShowCollision))
				{
					if (bShowCollision)
						coreGlobals.EnableColliderRender();
					else
						coreGlobals.DisableColliderRender();
				}

				static bool bShowAnimations{ true };
				if (ImGui::Checkbox("Show Animations", &bShowAnimations))
				{
					if (bShowAnimations)
						coreGlobals.EnableAnimationRender();
					else
						coreGlobals.DisableAnimationRender();
				}

				if (ImGui::TreeNode("Project Settings"))
				{
					// TODO: Add specific Project settings
					/*
					 * Desired Settings
					 * - Window Size
					 * - Window Position
					 * - Window flags
					 *
					 */
					ImGui::TreePop();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu(ICON_FA_WINDOW_MAXIMIZE " Displays"))
			{
				// TODO: Open and close specific displays

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu(ICON_FA_VIDEO " Scene"))
			{
				if (auto pCurrentScene = SCENE_MANAGER().GetCurrentScene())
				{
					ImGui::Text("Current Scene");
					ImGui::Separator();
					if (ImGui::TreeNode("Canvas"))
					{
						auto& canvas = pCurrentScene->GetCanvas();

						ImGui::InlineLabel("width");
						if (ImGui::InputInt("##_width", &canvas.width, canvas.tileWidth, canvas.tileWidth))
						{
							if (canvas.width < 640)
								canvas.width = 640;
						}
						ImGui::ItemToolTip("Canvas Width - Clamped minimum = 640");

						ImGui::InlineLabel("height");
						if (ImGui::InputInt("##_height", &canvas.height, canvas.tileHeight, canvas.tileHeight))
						{
							if (canvas.height < 320)
								canvas.height = 320;
						}
						ImGui::ItemToolTip("Canvas Height - Clamped minimum = 320");

						ImGui::InlineLabel("tile width");
						if (ImGui::InputInt("##tile_width", &canvas.tileWidth, 8, 8))
						{
							canvas.tileWidth = std::clamp(canvas.tileWidth, 8, 128);
						}
						ImGui::ItemToolTip("Tile Width - Range [8 : 128]");

						ImGui::InlineLabel("tile height");
						if (ImGui::InputInt("##tile_height", &canvas.tileHeight, 8, 8))
						{
							canvas.tileHeight = std::clamp(canvas.tileHeight, 8, 128);
						}
						ImGui::ItemToolTip("Tile Height - Range [8 : 128]");

						ImGui::TreePop();
					}
					ImGui::Separator();
					if (ImGui::TreeNode("Settings"))
					{
						bool bChanged{ false };
						bool bPlayerStartEnabled{ pCurrentScene->IsPlayerStartEnabled() };
						ImGui::InlineLabel("Enable Player Start:");
						ImGui::SetCursorPosX(250.f);

						ImGui::ItemToolTip(
							"Enable or Disable the player start.\n"
							"The player start is the character that we want to use when the scene is played.");

						if (ImGui::Checkbox("##_enablePlayerStart", &bPlayerStartEnabled))
						{
							pCurrentScene->SetPlayerStartEnabled(bPlayerStartEnabled);
							if (bPlayerStartEnabled)
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
							VORTEK_UTIL::GetKeys(ASSET_MANAGER().GetAllPrefabs() /*, []( auto& prefab ) {
				  return prefab.second->GetType() == VORTEK_CORE::EPrefabType::Character;
			  } */);

						ImGui::InlineLabel(ICON_FA_FLAG ICON_FA_GAMEPAD " Player Start Character:");
						ImGui::SetCursorPosX(250.f);
						ImGui::ItemToolTip("The default player to spawn when starting the scene.");
						if (ImGui::BeginCombo("##DefaultPlayerStart", sPlayerStartCharacter.c_str()))
						{
							for (const auto& sPrefabName : prefabs)
							{
								if (ImGui::Selectable(sPrefabName.c_str(), sPrefabName == sPlayerStartCharacter))
								{
									sPlayerStartCharacter = sPrefabName;

									bChanged = true;
								}
							}

							ImGui::EndCombo();
						}

						if (bChanged)
						{
							if (auto pPrefab = ASSET_MANAGER().GetPrefab(sPlayerStartCharacter))
							{
								pCurrentScene->GetPlayerStart().SetCharacter(*pPrefab);
							}
						}

						bChanged = true;

						auto musicNames = VORTEK_UTIL::GetKeys(ASSET_MANAGER().GetAllMusic());
						musicNames.push_back("None");

						std::string sDefaultSceneMusic{ pCurrentScene->GetDefaultMusicName() };
						if (sDefaultSceneMusic.empty())
							sDefaultSceneMusic = "None";

						ImGui::InlineLabel(ICON_FA_MUSIC " Default Music:");
						ImGui::SetCursorPosX(250.f);
						ImGui::ItemToolTip("Music to play when the scene starts.");
						if (ImGui::BeginCombo("##DefaultMusic", sDefaultSceneMusic.c_str()))
						{
							for (const auto& sMusicName : musicNames)
							{
								if (ImGui::Selectable(sMusicName.c_str(), sMusicName == sDefaultSceneMusic))
								{
									sDefaultSceneMusic = sMusicName;
									bChanged = true;
								}
							}

							ImGui::EndCombo();
						}

						if (bChanged)
						{
							pCurrentScene->SetDefaultMusic(sDefaultSceneMusic);
						}

						ImGui::TreePop();
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu(ICON_FA_COG " Settings"))
			{
				if (ImGui::TreeNode("Physics"))
				{
					ImGui::Separator();
					auto& coreGlobals = CORE_GLOBALS();
					bool bEnablePhysics = coreGlobals.IsPhysicsEnabled();
					ImGui::InlineLabel("Enable Physics", 176.f);
					if (ImGui::Checkbox("##enable_physics", &bEnablePhysics))
					{
						if (bEnablePhysics)
							coreGlobals.EnablePhysics();
						else
							coreGlobals.DisablePhysics();
					}

					int32 velocityIterations = coreGlobals.GetVelocityIterations();
					int32 positionIterations = coreGlobals.GetPositionIterations();
					float gravity = coreGlobals.GetGravity();
					ImGui::InlineLabel("Gravity", 176.f);
					if (ImGui::InputFloat("##Gravity", &gravity, .1f, .1f, "%.1f"))
					{
						coreGlobals.SetGravity(gravity);
					}

					ImGui::InlineLabel("Velocity Iterations", 176.f);
					if (ImGui::InputInt("##VelocityIterations", &velocityIterations, 1, 1))
					{
						coreGlobals.SetVelocityIterations(velocityIterations);
					}

					ImGui::InlineLabel("Position Iterations", 176.f);
					if (ImGui::InputInt("##PositionIterations", &positionIterations, 1, 1))
					{
						coreGlobals.SetPositionIterations(positionIterations);
					}
					ImGui::TreePop();
				}

				auto& coreGlobals = CORE_GLOBALS();
				bool bChanged{ false };
				std::string sGameType{ coreGlobals.GetGameTypeStr(coreGlobals.GetGameType()) };
				VORTEK_CORE::EGameType eGameType{ coreGlobals.GetGameType() };

				ImGui::InlineLabel(ICON_FA_GAMEPAD " Game Type:");
				ImGui::SetCursorPosX(250.f);
				ImGui::ItemToolTip("The type of game this is going to be.");

				if (ImGui::BeginCombo("##DefaultMusic", sGameType.c_str()))
				{
					for (const auto& [eType, sTypeStr] : coreGlobals.GetGameTypesMap())
					{
						if (ImGui::Selectable(sTypeStr.c_str(), sTypeStr == sGameType))
						{
							sGameType = sTypeStr;
							eGameType = eType;
							bChanged = true;
						}
					}

					ImGui::EndCombo();
				}

				if (bChanged)
				{
					coreGlobals.SetGameType(eGameType);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu(ICON_FA_QUESTION_CIRCLE " Help"))
			{
				if (ImGui::TreeNode("About Vortek Engine"))
				{
					ImGui::Separator();
					ImGui::Text("Vortek Engine is based on the Youtube Series 'VORTEK2D' From");
					ImGui::SameLine();
					ImGui::TextLinkOpenURL("Dustin Clark", "https://github.com/dwjclark11");
					ImGui::Separator();
					ImGui::AddSpaces(2);
					ImGui::Text("Helpful Links: ");
					ImGui::TextLinkOpenURL("Github", "https://github.com/Synkgg/Vortek-Engine");
					ImGui::TextLinkOpenURL("YouTube", "https://www.youtube.com/watch?v=dQw4w9WgXcQ");
					ImGui::TextLinkOpenURL("Documentation", "https://synkgg.github.io/Vortek-Engine/");
					ImGui::Separator();
					ImGui::TreePop();
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();

			if (bOpenEditorSettings)
			{
				ImGui::Begin("Editor Settings", &bOpenEditorSettings, ImGuiWindowFlags_AlwaysAutoResize |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoCollapse);

				// Split layout into two columns (sidebar tabs + content)
				ImGui::BeginChild("Sidebar", ImVec2(150, 300), true); // Left panel for vertical tabs

				static int selectedTab = 0;

				if (ImGui::Selectable("Theme", selectedTab == 0))
					selectedTab = 0;
				if (ImGui::Selectable("General", selectedTab == 1))
					selectedTab = 1;
				if (ImGui::Selectable("Keybinds", selectedTab == 2))
					selectedTab = 2;

				ImGui::EndChild();

				ImGui::SameLine();

				ImGui::BeginChild("TabContent", ImVec2(400, 300), true); // Right panel for content
				if (selectedTab == 0)
				{
					ImGui::Text("Theme Settings");
					ImGui::Separator();
					ImGui::Text("Editor Theme");

					ImGui::Combo( "##ThemeCombo", &g_SelectedThemeIndex, themes, IM_ARRAYSIZE( themes ) );

					if ( ImGui::Button( "Apply Theme" ) )
					{
						g_AppliedThemeIndex = g_SelectedThemeIndex; // Set the active one
						ApplyThemeByIndex( g_AppliedThemeIndex );
						SaveEditorConfig( g_AppliedThemeIndex );
					}
				}
				else if (selectedTab == 1)
				{
					ImGui::Text("General Settings");
				}
				else if (selectedTab == 2)
				{
					ImGui::Text("Keybind Settings");
				}
				ImGui::EndChild();

				ImGui::End();
			}
		}
	}
} // namespace VORTEK_EDITOR
