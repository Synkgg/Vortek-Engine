#include "MenuDisplay.h"
#include "Logger/Logger.h"
#include "../VORTEK_FILESYSTEM/Dialogs/FileDialog.h"
#include "Core/Loaders/TilemapLoader.h"
#include "Core/CoreUtilities/CoreEngineData.h"

#include "../scene/SceneManager.h"
#include "../scene/SceneObject.h"
#include "../tools/ToolManager.h"
#include "../utilities/ImGuiUtils.h"
#include "../utilities/fonts/IconsFontAwesome5.h"

#include <imgui.h>
#include <SDL.h>

namespace VORTEK_EDITOR
{
	void MenuDisplay::Draw()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu(ICON_FA_FILE " File"))
			{
				ImGui::InlineLabel(ICON_FA_FILE_ALT, 32.f);
				if (ImGui::MenuItem("New", "Ctrl + N"))
				{
					VORTEK_LOG("NEW PRESSED");
				}

				ImGui::InlineLabel(ICON_FA_FOLDER_OPEN, 32.f);
				if (ImGui::MenuItem("Open", "Ctrl + O"))
				{
					VORTEK_FILESYSTEM::FileDialog fd{};
					auto file = fd.OpenFileDialog("Open tilemap test", SDL_GetBasePath(), { "*.json" });

					if (!file.empty())
					{
						auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();
						if (pCurrentScene)
						{
							VORTEK_CORE::Loaders::TilemapLoader tl{};
							if (!tl.LoadTilemap(pCurrentScene->GetRegistry(), file, true))
							{
								VORTEK_ERROR("Failed to load tilemap.");
							}
						}
						else
						{
							VORTEK_ERROR("Failed to load tilemap. No active scene.");
						}
					}
				}
				ImGui::InlineLabel(ICON_FA_SAVE, 32.f);
				if (ImGui::MenuItem("Save", "Ctrl + S"))
				{
					VORTEK_FILESYSTEM::FileDialog fd{};
					auto file = fd.SaveFileDialog("Save Tilemap test", SDL_GetBasePath(), { "*.json" });
					if (!file.empty())
					{
						auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();
						if (pCurrentScene)
						{
							VORTEK_CORE::Loaders::TilemapLoader tl{};
							if (!tl.SaveTilemap(pCurrentScene->GetRegistry(), file, true))
							{
								VORTEK_ERROR("Failed to save tilemap.");
							}
						}
						else
						{
							VORTEK_ERROR("Failed to save tilemap. No active scene.");
						}
					}
				}

				ImGui::InlineLabel(ICON_FA_WINDOW_CLOSE, 32.f);
				if (ImGui::MenuItem("Exit"))
				{
					VORTEK_LOG("SHOULD EVENTUALLY EXIT!");
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

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu(ICON_FA_TOOLS " Tools"))
			{
				/*auto& toolManager = TOOL_MANAGER();
				ImGui::InlineLabel( ICON_FA_ARROWS_ALT "Translate" );*/

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu(ICON_FA_VIDEO " Scene"))
			{
				if (auto pCurrentScene = SCENE_MANAGER().GetCurrentScene())
				{
					ImGui::Text("Current Scene");
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
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu(ICON_FA_QUESTION_CIRCLE " Help"))
			{
				if (ImGui::TreeNode("About VORTEK ENGINE"))
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
		}
	}
} // namespace VORTEK_EDITOR