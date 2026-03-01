#pragma once

namespace Vortek::Utilities
{
enum class AssetType;
}

namespace Vortek::Editor
{
	class AssetDisplayUtils
	{
	  public:
		AssetDisplayUtils() = delete;
		static std::vector<const char*> GetAssetFileFilters( Vortek::Utilities::AssetType eAssetType );
		static const char* GetAssetDescriptionByType( Vortek::Utilities::AssetType eAssetType );
		static std::string AddAssetBasedOnType( Vortek::Utilities::AssetType eAssetType );
		static void OpenAddAssetModalBasedOnType( Vortek::Utilities::AssetType eAssetType, bool* pbOpen );
	};
}
