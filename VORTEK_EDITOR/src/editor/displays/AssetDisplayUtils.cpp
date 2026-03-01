#include "editor/displays/AssetDisplayUtils.h"
#include "VortekUtilities/VortekUtilities.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "VortekFilesystem/Dialogs/FileDialog.h"
#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "Logger/Logger.h"
#include <imgui.h>

namespace fs = std::filesystem;

#define IMAGE_FILTERS std::vector<const char*>{ "*.png", "*.bmp", "*.jpg" }
#define FONT_FILTERS                                                                                                   \
	std::vector<const char*>{                                                                                          \
		"*.ttf" /* add more font types */                                                                              \
	}
#define MUSIC_FILTERS std::vector<const char*>{ "*.mp3", "*.wav", "*.ogg" }

#define SOUNDFX_FILTERS std::vector<const char*>{ "*.mp3", "*.wav", "*.ogg" }

constexpr const char* IMAGE_DESC = "Image Files (*.png, *.bmp, *.jpg)";
constexpr const char* FONT_DESC = "Fonts (*.ttf)";
constexpr const char* MUSIC_DESC = "Music Files (*.mp3, *.wav, *.ogg)";
constexpr const char* SOUNDFX_DESC = "Soundfx Files (*.mp3, *.wav, *.ogg)";

using namespace Vortek::Filesystem;
using namespace Vortek::Editor;

static const std::map<std::string, Vortek::Core::EMapType> g_mapStringToMapTypes{
	{ "Grid", Vortek::Core::EMapType::Grid }, { "IsoGrid", Vortek::Core::EMapType::IsoGrid } };

namespace
{
class AssetModalCreator
{
  public:
	AssetModalCreator() {}

	bool AddAssetBasedOnType( const std::string& sAssetName, const std::string& sFilepath,
							  Vortek::Utilities::AssetType eAssetType, bool bPixelArt = true, bool bTileset = false,
							  float fontSize = 32.f )
	{
		auto& assetManager = MAIN_REGISTRY().GetAssetManager();
		switch ( eAssetType )
		{
		case Vortek::Utilities::AssetType::TEXTURE:
			return assetManager.AddTexture( sAssetName, sFilepath, bPixelArt, bTileset );
		case Vortek::Utilities::AssetType::FONT: return assetManager.AddFont( sAssetName, sFilepath, fontSize );
		case Vortek::Utilities::AssetType::SOUNDFX: return assetManager.AddSoundFx( sAssetName, sFilepath );
		case Vortek::Utilities::AssetType::MUSIC: return assetManager.AddMusic( sAssetName, sFilepath );
		case Vortek::Utilities::AssetType::SCENE: return false;
		}
		return false;
	}

	std::string CheckForAsset( const std::string& sAssetName, Vortek::Utilities::AssetType eAssetType )
	{
		std::string sError{};
		if ( sAssetName.empty() )
		{
			sError = "Asset name cannot be empty!";
		}
		else if ( eAssetType == Vortek::Utilities::AssetType::SCENE )
		{
			if ( SCENE_MANAGER().HasScene( sAssetName ) )
				sError = fmt::format( "Scene [{}] already exists!", sAssetName );
		}
		else
		{
			if ( MAIN_REGISTRY().GetAssetManager().CheckHasAsset( sAssetName, eAssetType ) )
				sError = fmt::format( "Asset [{}] already exists!", sAssetName );
		}

		return sError;
	}

	void AddSceneModal( bool* pbOpen )
	{
		if ( *pbOpen )
			ImGui::OpenPopup( "Add New Scene" );

		if ( ImGui::BeginPopupModal( "Add New Scene" ) )
		{
			ImGui::InlineLabel( "Name" );
			static std::string sAssetName{};
			ImGui::InputText( "##assetName", sAssetName.data(), 255 );

			static std::vector<std::string> mapTypes{ "Grid", "IsoGrid" };
			static std::string sMapType{ "Grid" };
			static Vortek::Core::EMapType eSelectedType{ Vortek::Core::EMapType::Grid };

			ImGui::InlineLabel( "Map Type" );
			if ( ImGui::BeginCombo( "##Map Type", sMapType.c_str() ) )
			{
				for ( const auto& [ sMapStr, eMapType ] : g_mapStringToMapTypes )
				{
					if ( ImGui::Selectable( sMapStr.c_str(), sMapStr == sMapType ) )
					{
						sMapType = sMapStr;
						eSelectedType = eMapType;
					}

					ImGui::ItemToolTip( "{}",
										eMapType == Vortek::Core::EMapType::IsoGrid
											? "Warning! IsoGrid maps are not fully supported."
											: "2D Grid tile map." );
				}

				ImGui::EndCombo();
			}

			std::string sCheckName{ sAssetName.data() };
			std::string sNameError{ CheckForAsset( sCheckName, Vortek::Utilities::AssetType::SCENE ) };

			if ( sNameError.empty() )
			{
				if ( ImGui::Button( "Ok" ) )
				{
					if ( !SCENE_MANAGER().AddScene( sCheckName, eSelectedType ) )
					{
						VORTEK_ERROR( "Failed to add new scene [{}]", sCheckName );
					}

					sAssetName.clear();

					*pbOpen = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
			}
			else
			{
				ImGui::TextColored( ImVec4{ 1.f, 0.f, 0.f, 1.f }, sNameError.c_str() );
			}

			if ( eSelectedType == Vortek::Core::EMapType::IsoGrid )
			{
				ImGui::TextColored( ImVec4{ 1.f, 1.f, 0.f, 1.f }, "IsoGrid maps are not fully supported yet!" );
			}

			// We always want to be able to cancel
			if ( ImGui::Button( "Cancel" ) )
			{
				sAssetName.clear();
				*pbOpen = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
	void AddAssetModal( Vortek::Utilities::AssetType eAssetType, bool* pbOpen )
	{
		std::string sAssetType{ Vortek::Editor::AssetDisplayUtils::AddAssetBasedOnType( eAssetType ) };

		if ( *pbOpen )
			ImGui::OpenPopup( sAssetType.c_str() );

		if ( ImGui::BeginPopupModal( sAssetType.c_str() ) )
		{
			static std::string sAssetName{};
			static std::string sFilepath{};
			static bool bTileset{ false };
			static bool bPixelArt{ false };
			static float fontSize{ 32.f };

			std::string sCheckName{ sAssetName.data() };
			std::string sCheckFilepath{ sFilepath.data() };

			ImGui::InputText( "##assetName", sAssetName.data(), 255 );
			std::string sNameError{ CheckForAsset( sCheckName, eAssetType ) };
			ImGui::InputText( "##filepath", sFilepath.data(), 255 );
			ImGui::SameLine();
			if ( ImGui::Button( "Browse" ) )
			{
				FileDialog fd{};
				sFilepath =
					fd.OpenFileDialog(
						"Open", BASE_PATH, 
						Vortek::Editor::AssetDisplayUtils::GetAssetFileFilters( eAssetType ),
						Vortek::Editor::AssetDisplayUtils::GetAssetDescriptionByType( eAssetType )
					);

				if ( !sFilepath.empty() )
				{
					fs::path path{ sFilepath };
					sAssetName = path.stem().string();
				}
			}

			if ( eAssetType == Vortek::Utilities::AssetType::TEXTURE )
			{
				ImGui::Checkbox( "Pixel Art", &bPixelArt );
				ImGui::Checkbox( "Tileset", &bTileset );
			}
			else if ( eAssetType == Vortek::Utilities::AssetType::FONT )
			{
				ImGui::InputFloat( "Font Size", &fontSize, 1.f, 1.f, "%.1f" );
			}

			if ( sNameError.empty() )
			{
				if ( ImGui::Button( "Ok" ) )
				{
					if ( fs::exists( fs::path{ sCheckFilepath } ) )
					{
						if ( !AddAssetBasedOnType(
								 sCheckName, sCheckFilepath, eAssetType, bPixelArt, bTileset, fontSize ) )
						{
							VORTEK_ERROR( "Failed to add new texture!" );
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
				ImGui::TextColored( ImVec4{ 1.f, 0.f, 0.f, 1.f }, sNameError.c_str() );
			}

			// We always want to be able to cancel
			if ( ImGui::Button( "Cancel" ) )
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

namespace Vortek::Editor
{
std::vector<const char*> AssetDisplayUtils::GetAssetFileFilters( Vortek::Utilities::AssetType eAssetType )
{
	switch ( eAssetType )
	{
	case Vortek::Utilities::AssetType::TEXTURE: return IMAGE_FILTERS;
	case Vortek::Utilities::AssetType::FONT: return FONT_FILTERS;
	case Vortek::Utilities::AssetType::SOUNDFX: return SOUNDFX_FILTERS;
	case Vortek::Utilities::AssetType::MUSIC: return MUSIC_FILTERS;
	}

	return {};
}

const char* AssetDisplayUtils::GetAssetDescriptionByType( Vortek::Utilities::AssetType eAssetType )
{
	switch ( eAssetType )
	{
	case Vortek::Utilities::AssetType::TEXTURE: return IMAGE_DESC;
	case Vortek::Utilities::AssetType::FONT: return FONT_DESC;
	case Vortek::Utilities::AssetType::SOUNDFX: return SOUNDFX_DESC;
	case Vortek::Utilities::AssetType::MUSIC: return MUSIC_DESC;
	}

	return "Files";
}

std::string AssetDisplayUtils::AddAssetBasedOnType( Vortek::Utilities::AssetType eAssetType )
{
	switch ( eAssetType )
	{
	case Vortek::Utilities::AssetType::TEXTURE: return "Add Texture";
	case Vortek::Utilities::AssetType::FONT: return "Add Font";
	case Vortek::Utilities::AssetType::SOUNDFX: return "Add SoundFx";
	case Vortek::Utilities::AssetType::MUSIC: return "Add Music";
	case Vortek::Utilities::AssetType::SCENE: return "Add Scene";
	default: VORTEK_ASSERT( false && "Type has not been implemented!" ); return {};
	}
}

void AssetDisplayUtils::OpenAddAssetModalBasedOnType( Vortek::Utilities::AssetType eAssetType, bool* pbOpen )
{
	VORTEK_ASSERT( eAssetType != Vortek::Utilities::AssetType::NO_TYPE && "The asset type must be set!" );
	static AssetModalCreator md{};
	if ( eAssetType == Vortek::Utilities::AssetType::SCENE )
	{
		md.AddSceneModal( pbOpen );
	}
	else
	{
		md.AddAssetModal( eAssetType, pbOpen );
	}
}
} // namespace Vortek::Editor
