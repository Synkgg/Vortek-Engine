#pragma once
#include <string>
#include <vector>

namespace VORTEK_UTIL
{
	enum class AssetType;
}

namespace VORTEK_EDITOR
{
	class AssetDisplayUtils
	{
	public:
		AssetDisplayUtils() = delete;
		static std::vector<const char*> GetAssetFileFilters(VORTEK_UTIL::AssetType eAssetType);
		static std::string AddAssetBasedOnType(VORTEK_UTIL::AssetType eAssetType);
		static void OpenAddAssetModalBasedOnType(VORTEK_UTIL::AssetType eAssetType, bool* pbOpen);
	};
}