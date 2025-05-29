#include "AssetDisplayUtils.h"
#include "VORTEKUtilities/VORTEKUtilities.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "../VORTEK_FILESYSTEM/Dialogs/FileDialog.h"
#include "../scene/SceneManager.h"
#include "Logger/Logger.h"
#include <imgui.h>
#include <filesystem>

namespace fs = std::filesystem;

#define IMAGE_FILTERS                                                                                                  \
	std::vector<const char*>                                                                                           \
	{                                                                                                                  \
		"*.png", "*.bmp", "*.jpg"                                                                                      \
	}
#define FONT_FILTERS                                                                                                   \
	std::vector<const char*>                                                                                           \
	{                                                                                                                  \
		"*.ttf" /* add more font types */                                                                              \
	}
#define MUSIC_FILTERS                                                                                                  \
	std::vector<const char*>                                                                                           \
	{                                                                                                                  \
		"*.mp3", "*.wav", "*.ogg"                                                                                      \
	}

#define SOUNDFX_FILTERS                                                                                                \
	std::vector<const char*>                                                                                           \
	{                                                                                                                  \
		"*.mp3", "*.wav", "*.ogg"                                                                                      \
	}

using namespace VORTEK_FILESYSTEM;

namespace
{
	class AssetModalCreator
	{
	public:
		AssetModalCreator() {}

		bool AddAssetBasedOnType(const std::string& sAssetName, const std::string& sFilepath,
			VORTEK_UTIL::AssetType eAssetType, bool bPixelArt = true, bool bTileset = false,
			float fontSize = 32.f)
		{
			auto& assetManager = MAIN_REGISTRY().GetAssetManager();
			switch (eAssetType)
			{
			case VORTEK_UTIL::AssetType::TEXTURE:
				return assetManager.AddTexture(sAssetName, sFilepath, bPixelArt, bTileset);
			case VORTEK_UTIL::AssetType::FONT: return assetManager.AddFont(sAssetName, sFilepath, fontSize);
			case VORTEK_UTIL::AssetType::SOUNDFX: return assetManager.AddSoundFx(sAssetName, sFilepath);
			case VORTEK_UTIL::AssetType::MUSIC: return assetManager.AddMusic(sAssetName, sFilepath);
			case VORTEK_UTIL::AssetType::SCENE: return false;
			}
			return false;
		}

		std::string CheckForAsset(const std::string& sAssetName, VORTEK_UTIL::AssetType eAssetType)
		{
			std::string sError{ "" };
			if (sAssetName.empty())
			{
				sError = "Asset name cannot be empty!";
			}
			else if (eAssetType == VORTEK_UTIL::AssetType::SCENE)
			{
				if (SCENE_MANAGER().HasScene(sAssetName))
					sError = std::format("Scene [{}] already exists!", sAssetName);
			}
			else
			{
				if (MAIN_REGISTRY().GetAssetManager().CheckHasAsset(sAssetName, eAssetType))
					sError = std::format("Asset [{}] already exists!", sAssetName);
			}

			return sError;
		}

		void AddSceneModal(bool* pbOpen)
		{
			if (*pbOpen)
				ImGui::OpenPopup("Add New Scene");

			if (ImGui::BeginPopupModal("Add New Scene"))
			{
				static std::string sAssetName{ "" };
				ImGui::InputText("##assetName", sAssetName.data(), 255);

				std::string sCheckName{ sAssetName.data() };
				std::string sNameError{ CheckForAsset(sCheckName, VORTEK_UTIL::AssetType::SCENE) };

				if (sNameError.empty())
				{
					if (ImGui::Button("Ok"))
					{
						if (!SCENE_MANAGER().AddScene(sCheckName))
						{
							VORTEK_ERROR("Failed to add new scene [{}]", sCheckName);
						}

						sAssetName.clear();

						*pbOpen = false;
						ImGui::CloseCurrentPopup();
					}
					ImGui::SameLine();
				}
				else
				{
					ImGui::TextColored(ImVec4{ 1.f, 0.f, 0.f, 1.f }, sNameError.c_str());
				}

				// We always want to be able to cancel
				if (ImGui::Button("Cancel"))
				{
					sAssetName.clear();
					*pbOpen = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}
		void AddAssetModal(VORTEK_UTIL::AssetType eAssetType, bool* pbOpen)
		{
			std::string sAssetType{ VORTEK_EDITOR::AssetDisplayUtils::AddAssetBasedOnType(eAssetType) };

			if (*pbOpen)
				ImGui::OpenPopup(sAssetType.c_str());

			if (ImGui::BeginPopupModal(sAssetType.c_str()))
			{
				static std::string sAssetName{ "" };
				static std::string sFilepath{ "" };
				static bool bTileset{ false };
				static bool bPixelArt{ false };
				static float fontSize{ 32.f };

				std::string sCheckName{ sAssetName.data() };
				std::string sCheckFilepath{ sFilepath.data() };

				ImGui::InputText("##assetName", sAssetName.data(), 255);
				std::string sNameError{ CheckForAsset(sCheckName, eAssetType) };
				ImGui::InputText("##filepath", sFilepath.data(), 255);
				ImGui::SameLine();
				if (ImGui::Button("Browse"))
				{
					FileDialog fd{};
					sFilepath =
						fd.OpenFileDialog("Open", "", VORTEK_EDITOR::AssetDisplayUtils::GetAssetFileFilters(eAssetType));

					if (!sFilepath.empty())
					{
						fs::path path{ sFilepath };
						sAssetName = path.stem().string();
					}
				}

				if (eAssetType == VORTEK_UTIL::AssetType::TEXTURE)
				{
					ImGui::Checkbox("Pixel Art", &bPixelArt);
					ImGui::Checkbox("Tileset", &bTileset);
				}
				else if (eAssetType == VORTEK_UTIL::AssetType::FONT)
				{
					ImGui::InputFloat("Font Size", &fontSize, 1.f, 1.f, "%.1f");
				}

				if (sNameError.empty())
				{
					if (ImGui::Button("Ok"))
					{
						if (fs::exists(fs::path{ sCheckFilepath }))
						{
							if (!AddAssetBasedOnType(
								sCheckName, sCheckFilepath, eAssetType, bPixelArt, bTileset, fontSize))
							{
								VORTEK_ERROR("Failed to add new texture!");
							}

							bTileset = false;
							bPixelArt = false;
							sAssetName.clear();
							sFilepath.clear();
							*pbOpen = false;
							ImGui::CloseCurrentPopup();
						}
						else
						{
							// TODO: Add filepath error!
						}
					}
					ImGui::SameLine();
				}
				else
				{
					ImGui::TextColored(ImVec4{ 1.f, 0.f, 0.f, 1.f }, sNameError.c_str());
				}

				// We always want to be able to cancel
				if (ImGui::Button("Cancel"))
				{
					bTileset = false;
					bPixelArt = false;
					sAssetName.clear();
					sFilepath.clear();
					*pbOpen = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}
	};

} // namespace

namespace VORTEK_EDITOR
{
	std::vector<const char*> AssetDisplayUtils::GetAssetFileFilters(VORTEK_UTIL::AssetType eAssetType)
	{
		switch (eAssetType)
		{
		case VORTEK_UTIL::AssetType::TEXTURE: return IMAGE_FILTERS;
		case VORTEK_UTIL::AssetType::FONT: return FONT_FILTERS;
		case VORTEK_UTIL::AssetType::SOUNDFX: return SOUNDFX_FILTERS;
		case VORTEK_UTIL::AssetType::MUSIC: return MUSIC_FILTERS;
		}

		return {};
	}
	std::string AssetDisplayUtils::AddAssetBasedOnType(VORTEK_UTIL::AssetType eAssetType)
	{
		switch (eAssetType)
		{
		case VORTEK_UTIL::AssetType::TEXTURE: return "Add Texture";
		case VORTEK_UTIL::AssetType::FONT: return "Add Font";
		case VORTEK_UTIL::AssetType::SOUNDFX: return "Add SoundFx";
		case VORTEK_UTIL::AssetType::MUSIC: return "Add Music";
		case VORTEK_UTIL::AssetType::SCENE: return "Add Scene";
		default: VORTEK_ASSERT(false && "Type has not been implemented!"); return {};
		}
	}

	void AssetDisplayUtils::OpenAddAssetModalBasedOnType(VORTEK_UTIL::AssetType eAssetType, bool* pbOpen)
	{
		VORTEK_ASSERT(eAssetType != VORTEK_UTIL::AssetType::NO_TYPE && "The asset type must be set!");
		static AssetModalCreator md{};
		if (eAssetType == VORTEK_UTIL::AssetType::SCENE)
		{
			md.AddSceneModal(pbOpen);
		}
		else
		{
			md.AddAssetModal(eAssetType, pbOpen);
		}
	}
} // namespace VORTEK_EDITOR