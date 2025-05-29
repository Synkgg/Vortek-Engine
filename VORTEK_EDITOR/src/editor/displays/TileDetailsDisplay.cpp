#include "TileDetailsDisplay.h"

#include "../utilities/imgui/ImGuiUtils.h"
#include "../utilities/DrawComponentUtils.h"
#include "../utilities/fonts/IconsFontAwesome5.h"
#include "../scene/SceneManager.h"
#include "../scene/SceneObject.h"
#include "../tools/ToolManager.h"
#include "../tools/TileTool.h"

#include "Core/ECS/MainRegistry.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/Resources/AssetManager.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "Logger/Logger.h"

#include <imgui.h>
#include <ranges>

using namespace VORTEK_CORE::ECS;

namespace VORTEK_EDITOR
{
	void TileDetailsDisplay::DrawSpriteComponent(VORTEK_CORE::ECS::SpriteComponent& sprite, SceneObject* pScene)
	{
		bool bChanged{ false };

		ImGui::SeparatorText("Sprite");
		ImGui::PushID(entt::type_hash<SpriteComponent>::value());
		if (ImGui::TreeNodeEx("##SpriteTree", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::AddSpaces(2);

			ImGui::InlineLabel("texture: ");
			ImGui::TextColored(ImVec4{ 0.f, 1.f, 0.f, 1.f }, sprite.texture_name.c_str());

			std::string sLayer{ "" };

			// Sets the layer description
			if (sprite.layer >= 0 && sprite.layer < pScene->GetLayerParams().size())
			{
				sLayer = pScene->GetLayerParams()[sprite.layer].sLayerName;
			}

			ImGui::InlineLabel("layer: ");
			ImGui::TextColored(ImVec4{ 0.f, 1.f, 0.f, 1.f }, sLayer.c_str());

			ImGui::AddSpaces(2);

			// Color picker
			ImVec4 col = { sprite.color.r / 255.f, sprite.color.g / 255.f, sprite.color.b / 255.f, sprite.color.a / 255.f };
			ImGui::InlineLabel("color");
			if (ImGui::ColorEdit4("##color", &col.x, IMGUI_COLOR_PICKER_FLAGS))
			{
				sprite.color.r = static_cast<GLubyte>(col.x * 255.f);
				sprite.color.g = static_cast<GLubyte>(col.y * 255.f);
				sprite.color.b = static_cast<GLubyte>(col.z * 255.f);
				sprite.color.a = static_cast<GLubyte>(col.w * 255.f);
			}

			ImGui::AddSpaces(2);

			ImGui::PushItemWidth(120.f);
			ImGui::InlineLabel("width");
			if (ImGui::InputFloat("##width", &sprite.width, 8.f, 8.f))
			{
				sprite.width = std::clamp(sprite.width, 8.f, 1366.f);
				bChanged = true;
			}

			ImGui::InlineLabel("height");
			if (ImGui::InputFloat("##height", &sprite.height, 8.f, 8.f))
			{
				sprite.height = std::clamp(sprite.height, 8.f, 768.f);
				bChanged = true;
			}

			ImGui::TreePop();
			ImGui::PopItemWidth();
		}

		ImGui::PopID();

		if (bChanged)
		{
			auto pTileTool = SCENE_MANAGER().GetToolManager().GetActiveTool();
			if (pTileTool)
			{
				// Setting the sprite rect handles the UVs as well.
				pTileTool->SetSpriteRect(glm::vec2{ sprite.width, sprite.height });
				// We should also adjust the tileset here so when we change the width/height
				// It will show in the tileset.
			}
			else // In reality, this should never get here, should probably assert instead.
			{
				auto pTexture = MAIN_REGISTRY().GetAssetManager().GetTexture(sprite.texture_name);
				if (!pTexture)
					return;

				VORTEK_CORE::GenerateUVs(sprite, pTexture->GetWidth(), pTexture->GetHeight());
			}
		}
	}

	TileDetailsDisplay::TileDetailsDisplay()
		: m_SelectedLayer{ -1 }
		, m_sRenameLayerBuf{ "" }
		, m_bRename{ false }
	{
	}

	TileDetailsDisplay::~TileDetailsDisplay()
	{
	}

	void TileDetailsDisplay::Draw()
	{
		auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();
		auto& toolManager = SCENE_MANAGER().GetToolManager();

		auto pActiveTool = toolManager.GetActiveTool();
		if (!pActiveTool)
			return;

		if (!ImGui::Begin("Tile Details") || !pCurrentScene)
		{
			ImGui::End();
			return;
		}

		auto& tileData = pActiveTool->GetTileData();

		// Transform
		DrawComponentsUtil::DrawComponentInfo(tileData.transform);

		ImGui::AddSpaces(2);
		// Sprite
		DrawSpriteComponent(tileData.sprite, pCurrentScene.get());

		ImGui::AddSpaces(2);
		ImGui::Separator();
		ImGui::AddSpaces(2);

		ImGui::InlineLabel("Box Collider");
		ImGui::Checkbox("##Box_Collider", &tileData.bCollider);
		if (tileData.bCollider)
		{
			DrawComponentsUtil::DrawComponentInfo(tileData.boxCollider);
		}

		ImGui::AddSpaces(2);
		ImGui::Separator();
		ImGui::AddSpaces(2);

		ImGui::InlineLabel("Circle Collider");
		ImGui::Checkbox("##Cicle_Collider", &tileData.bCircle);
		if (tileData.bCircle)
		{
			DrawComponentsUtil::DrawComponentInfo(tileData.circleCollider);
		}

		ImGui::AddSpaces(2);
		ImGui::Separator();
		ImGui::AddSpaces(2);

		ImGui::InlineLabel("Animation");
		ImGui::Checkbox("##Animation", &tileData.bAnimation);
		if (tileData.bAnimation)
		{
			DrawComponentsUtil::DrawComponentInfo(tileData.animation);
		}

		ImGui::AddSpaces(2);
		ImGui::Separator();
		ImGui::AddSpaces(2);

		ImGui::InlineLabel("Physics");
		ImGui::Checkbox("##Physics", &tileData.bPhysics);
		if (tileData.bPhysics)
		{
			DrawComponentsUtil::DrawComponentInfo(tileData.physics);
		}

		ImGui::AddSpaces(2);
		ImGui::Separator();
		ImGui::AddSpaces(2);

		ImGui::End();

		if (ImGui::Begin("Tile Layers"))
		{
			ImGui::SeparatorText("Tile Layers");
			auto& spriteLayers = pCurrentScene->GetLayerParams();

			std::string sCheckName{ m_sRenameLayerBuf.data() };

			if (ImGui::Button(ICON_FA_PLUS_CIRCLE " Add Layer"))
			{
				pCurrentScene->AddNewLayer();
			}
			ImGui::ItemToolTip("Add Layer");

			ImGui::AddSpaces(2);
			ImGui::Separator();
			ImGui::AddSpaces(2);

			float itemWidth{ ImGui::GetWindowWidth() - 64.f };
			auto rView = spriteLayers | std::ranges::views::reverse;

			for (auto rit = rView.begin(); rit != rView.end(); rit++)
			{
				int n = std::distance(rit, rView.end()) - 1;

				auto& spriteLayer = *rit;
				bool bIsSelected{ m_SelectedLayer == n };

				ImGui::Selectable(spriteLayer.sLayerName.c_str(), bIsSelected, 0, ImVec2{ itemWidth, 20.f });

				if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
				{
					int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? 1 : -1);
					if (n_next >= 0 && n_next < rView.size())
					{
						// We need to swap the sprite layers here, not the reverse view.
						// Because the indexes are not reversed anymore.
						std::swap(spriteLayers[n], spriteLayers[n_next]);

						auto spriteView = pCurrentScene->GetRegistry().GetRegistry().view<SpriteComponent, TileComponent>();

						for (auto entity : spriteView)
						{
							auto& sprite = spriteView.get<SpriteComponent>(entity);
							if (sprite.layer == n)
							{
								sprite.layer = n_next;
							}
							else if (sprite.layer == n_next)
							{
								sprite.layer = n;
							}
						}

						m_SelectedLayer = n_next;
						tileData.sprite.layer = n_next;

						ImGui::ResetMouseDragDelta();
					}
				}

				// Set the current selected layer
				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0) && !m_bRename)
				{
					m_SelectedLayer = n;
					tileData.sprite.layer = n;
				}

				// Start the rename
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && !m_bRename && bIsSelected)
				{
					m_bRename = true;
					m_sRenameLayerBuf.clear();
					m_sRenameLayerBuf = spriteLayer.sLayerName;
				}

				bool bCheckPassed{ pCurrentScene->CheckLayerName(sCheckName) };

				ImGui::SameLine();
				ImGui::PushID(n);
				ImGui::PushStyleColor(ImGuiCol_Button, BLACK_TRANSPARENT);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, BLACK_TRANSPARENT);
				if (ImGui::Button(spriteLayer.bVisible ? ICON_FA_EYE : ICON_FA_EYE_SLASH, { 24.f, 24.f }))
				{
					spriteLayer.bVisible = !spriteLayer.bVisible;
				}

				ImGui::PopStyleColor(2);

				if (m_bRename && bIsSelected)
				{
					ImGui::SetKeyboardFocusHere();
					if (ImGui::InputText(
						"##rename", m_sRenameLayerBuf.data(), 255, ImGuiInputTextFlags_EnterReturnsTrue) &&
						bCheckPassed)
					{
						spriteLayer.sLayerName = sCheckName;
						m_sRenameLayerBuf.clear();
						m_bRename = false;
					}
					else if (m_bRename && ImGui::IsKeyPressed(ImGuiKey_Escape))
					{
						m_sRenameLayerBuf.clear();
						m_bRename = false;
					}
				}

				// We want to display an error to the user if the name already exists. Try to prevent duplicate
				// layer names.
				if (!bCheckPassed && bIsSelected)
				{
					ImGui::TextColored(ImVec4{ 1.f, 0.f, 0.f, 1.f },
						format("{} - Already exists.", sCheckName).c_str());
				}

				ImGui::PopID();
			}

			ImGui::End();
		}
	}

} // namespace VORTEK_EDITOR