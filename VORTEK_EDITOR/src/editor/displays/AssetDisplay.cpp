#include "AssetDisplay.h"
#include "VORTEKUtilities/VORTEKUtilities.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Scripting/InputManager.h"
#include "Core/Resources/AssetManager.h"
#include "Logger/Logger.h"

#include "../utilities/EditorUtilities.h"
#include "../utilities/ImGuiUtils.h"
#include "../utilities/fonts/IconsFontAwesome5.h"
#include "../scene/SceneManager.h"

#include <imgui.h>

constexpr float DEFAULT_ASSET_SIZE = 64.f;
constexpr ImVec2 DRAG_ASSET_SIZE = ImVec2{ 32.f, 32.f };

namespace VORTEK_EDITOR
{
	void AssetDisplay::SetAssetType()
	{
		if (!m_bAssetTypeChanged)
			return;

		if (m_sSelectedType == "TEXTURES")
		{
			m_eSelectedType = VORTEK_UTIL::AssetType::TEXTURE;
			m_sDragSource = std::string{ DROP_TEXTURE_SRC };
		}
		else if (m_sSelectedType == "FONTS")
		{
			m_eSelectedType = VORTEK_UTIL::AssetType::FONT;
			m_sDragSource = std::string{ DROP_FONT_SRC };
		}
		else if (m_sSelectedType == "SOUNDFX")
		{
			m_eSelectedType = VORTEK_UTIL::AssetType::SOUNDFX;
			m_sDragSource = std::string{ DROP_SOUNDFX_SRC };
		}
		else if (m_sSelectedType == "MUSIC")
		{
			m_eSelectedType = VORTEK_UTIL::AssetType::MUSIC;
			m_sDragSource = std::string{ DROP_MUSIC_SRC };
		}
		else if (m_sSelectedType == "SCENES")
		{
			m_eSelectedType = VORTEK_UTIL::AssetType::SCENE;
			m_sDragSource = std::string{ DROP_SCENE_SRC };
		}
		else
		{
			m_eSelectedType = VORTEK_UTIL::AssetType::NO_TYPE;
			m_sDragSource = "NO_ASSET_TYPE";
		}

		m_bAssetTypeChanged = false;
	}

	unsigned int AssetDisplay::GetTextureID(const std::string& sAssetName) const
	{
		auto& assetManager = MAIN_REGISTRY().GetAssetManager();
		switch (m_eSelectedType)
		{
		case VORTEK_UTIL::AssetType::TEXTURE: {
			auto pTexture = assetManager.GetTexture(sAssetName);
			if (pTexture)
				return pTexture->GetID();

			break;
		}
		case VORTEK_UTIL::AssetType::FONT: {
			auto pFont = assetManager.GetFont(sAssetName);
			if (pFont)
				return pFont->GetFontAtlasID();

			break;
		}
		case VORTEK_UTIL::AssetType::SOUNDFX:
		case VORTEK_UTIL::AssetType::MUSIC: {
			auto pTexture = assetManager.GetTexture("music_icon");
			if (pTexture)
				return pTexture->GetID();

			break;
		}
		case VORTEK_UTIL::AssetType::SCENE: {
			auto pTexture = assetManager.GetTexture("scene_icon");
			if (pTexture)
				return pTexture->GetID();

			break;
		}
		}

		return 0;
	}

	bool AssetDisplay::DoRenameAsset(const std::string& sOldName, const std::string& sNewName) const
	{
		if (sNewName.empty())
			return false;

		if (m_eSelectedType == VORTEK_UTIL::AssetType::SCENE)
		{
			// TODO: Change the scene name
		}
		else
		{
			auto& assetManager = MAIN_REGISTRY().GetAssetManager();
			return assetManager.ChangeAssetName(sOldName, sNewName, m_eSelectedType);
		}

		VORTEK_ASSERT(false && "How did it get here??");
		return false;
	}

	void AssetDisplay::CheckRename(const std::string& sCheckName) const
	{
		if (sCheckName.empty())
		{
			ImGui::TextColored(ImVec4{ 1.f, 0.f, 0.f, 1.f }, "Rename text cannot be blank!");
			return;
		}

		bool bHasAsset{ false };

		if (m_eSelectedType == VORTEK_UTIL::AssetType::SCENE)
		{
			// TODO: Check is scene name already exists
		}
		else
		{
			auto& assetManager = MAIN_REGISTRY().GetAssetManager();
			if (assetManager.CheckHasAsset(sCheckName, m_eSelectedType))
				bHasAsset = true;
		}

		if (bHasAsset)
			ImGui::TextColored(ImVec4{ 1.f, 0.f, 0.f, 1.f },
				std::format("Asset name [{}] already exists!", sCheckName).c_str());
	}

	void AssetDisplay::OpenAssetContext(const std::string& sAssetName)
	{
		if (ImGui::Selectable("rename"))
		{
			m_bRename = true;
		}

		if (ImGui::Selectable("delete"))
		{
			if (m_eSelectedType == VORTEK_UTIL::AssetType::SCENE)
			{
				// TODO: Check is scene name already exists
			}
			else
			{
				auto& assetManager = MAIN_REGISTRY().GetAssetManager();
				if (!assetManager.DeleteAsset(sAssetName, m_eSelectedType))
				{
					VORTEK_ERROR("Failed to delete asset {}.", sAssetName);
				}
			}
		}
	}

	void AssetDisplay::DrawSelectedAssets()
	{
		auto& mainRegistry = MAIN_REGISTRY();
		auto& assetManager = mainRegistry.GetAssetManager();

		std::vector<std::string> assetNames{};

		if (m_eSelectedType == VORTEK_UTIL::AssetType::SCENE)
		{
			assetNames = SCENE_MANAGER().GetSceneNames();
		}
		else
			assetNames = assetManager.GetAssetKeyNames(m_eSelectedType);

		if (assetNames.empty())
			return;

		float windowWidth = ImGui::GetWindowWidth();

		int numCols = static_cast<int>((windowWidth - m_AssetSize) / m_AssetSize);
		int numRows = static_cast<int>(assetNames.size() / (numCols <= 1 ? 1 : numCols) + 1);

		if (!numCols || !numRows)
			return;

		ImGuiTableFlags tableFlags{ 0 };
		tableFlags |= ImGuiTableFlags_SizingFixedFit;

		int k{ 0 }, id{ 0 };

		auto assetItr = assetNames.begin();

		if (ImGui::BeginTable("Assets", numCols, tableFlags))
		{
			for (int row = 0; row < numRows; row++)
			{
				ImGui::TableNextRow();
				for (int col = 0; col < numCols; col++)
				{
					if (assetItr == assetNames.end())
						break;

					ImGui::PushID(k++);
					ImGui::TableSetColumnIndex(col);

					bool bSelectedAsset{ m_SelectedID == id };

					if (bSelectedAsset)
						ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
							ImGui::GetColorU32(ImVec4{ 0.f, 0.9f, 0.f, 0.3f }));

					GLuint textureID{ GetTextureID(*assetItr) };
					std::string sCheckName{ m_sRenameBuf.data() };

					if (textureID == 0)
						break;

					ImGui::ImageButton(std::format("##{}", k).c_str(), (ImTextureID)(intptr_t)textureID, ImVec2{m_AssetSize, m_AssetSize});

					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0) && !m_bRename)
						m_SelectedID = id;

					auto sAssetName = (*assetItr).c_str();

					if (bSelectedAsset && ImGui::BeginPopupContextItem())
					{
						OpenAssetContext(*assetItr);

						ImGui::EndPopup();
					}

					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload(m_sDragSource.c_str(),
							sAssetName,
							(strlen(sAssetName) + 1) * sizeof(char),
							ImGuiCond_Once);

						ImGui::Image((ImTextureID)(intptr_t)textureID, DRAG_ASSET_SIZE);
						ImGui::EndDragDropSource();
					}

					if (!m_bRename || !bSelectedAsset)
						ImGui::Text(sAssetName);

					if (m_bRename && bSelectedAsset)
					{
						ImGui::SetKeyboardFocusHere();
						if (ImGui::InputText(
							"##rename", m_sRenameBuf.data(), 255, ImGuiInputTextFlags_EnterReturnsTrue))
						{
							if (!DoRenameAsset(*assetItr, sCheckName))
							{
								VORTEK_ERROR("Failed to change asset name.");
							}

							m_sRenameBuf.clear();
							m_bRename = false;
						}
						else if (m_bRename && ImGui::IsKeyPressed(ImGuiKey_Escape))
						{
							m_sRenameBuf.clear();
							m_bRename = false;
						}
					}

					if (!m_bRename && bSelectedAsset && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
					{
						m_sRenameBuf.clear();
						m_sRenameBuf = *assetItr;
						m_bRename = true;
					}

					if (m_bRename && bSelectedAsset)
					{
						if (sAssetName != sCheckName)
							CheckRename(sCheckName);
					}

					++id;
					++assetItr;
					ImGui::PopID();
				}
			}

			ImGui::EndTable();
		}
	}

	AssetDisplay::AssetDisplay()
		: m_bAssetTypeChanged{ true }
		, m_bRename{ false }
		, m_bWindowSelected{ false }
		, m_bWindowHovered{ false }
		, m_sSelectedAssetName{ "" }
		, m_sSelectedType{ "TEXTURES" }
		, m_sDragSource{ "" }
		, m_sRenameBuf{ "" }
		, m_eSelectedType{ VORTEK_UTIL::AssetType::TEXTURE }
		, m_AssetSize{ DEFAULT_ASSET_SIZE }
		, m_SelectedID{ -1 }
	{
		SetAssetType();
	}

	void AssetDisplay::Draw()
	{
		if (!ImGui::Begin("Assets"))
		{
			ImGui::End();
			return;
		}

		DrawToolbar();

		if (ImGui::BeginChild("##AssetTable",
			ImVec2{ 0.f, 0.f },
			NULL,
			ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_ChildWindow))
		{
			m_bWindowHovered = ImGui::IsWindowHovered();
			m_bWindowSelected = ImGui::IsWindowFocused();

			DrawSelectedAssets();

			ImGui::EndChild();
		}

		ImGui::End();
	}

	void AssetDisplay::Update()
	{
	}

	void AssetDisplay::DrawToolbar()
	{

		auto& mainRegistry = MAIN_REGISTRY();
		auto& assetManager = mainRegistry.GetAssetManager();

		ImGui::PushStyleColor(ImGuiCol_Button, BLACK_TRANSPARENT);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, BLACK_TRANSPARENT);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, BLACK_TRANSPARENT);
		ImGui::Button("Asset Type");
		ImGui::PopStyleColor(3);

		ImGui::SameLine(0.f, 10.f);
		if (ImGui::BeginCombo("##AssetType", m_sSelectedType.c_str()))
		{

			for (const auto& sAssetType : m_SelectableTypes)
			{
				bool bIsSelected = m_sSelectedType == sAssetType;
				if (ImGui::Selectable(sAssetType.c_str(), bIsSelected))
				{
					m_bAssetTypeChanged = true;
					m_sSelectedType = sAssetType;
					m_SelectedID = -1;
					SetAssetType();
				}

				if (bIsSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::Separator();
	}
} // namespace VORTEK_EDITOR