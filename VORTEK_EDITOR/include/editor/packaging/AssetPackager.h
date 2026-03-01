#pragma once
#include <rapidjson/document.h>

namespace Vortek::Utilities
{
enum class AssetType;
class ThreadPool;
} // namespace Vortek::Utilities

namespace Vortek::Filesystem
{
class LuaSerializer;
}

namespace Vortek::Editor
{
struct AssetPackagerParams
{
	std::string sAssetsPath{};
	std::string sTempFilepath{};
	std::string sDestinationPath{};
	std::string sProjectPath{};
};

struct AssetConversionData
{
	std::string sInAssetFile{};
	std::string sAssetName{};
	Vortek::Utilities::AssetType eType;
	std::optional<float> optFontSize{ std::nullopt };
	std::optional<bool> optPixelArt{ std::nullopt };
};

class AssetPackager
{
  public:
	AssetPackager( const AssetPackagerParams& params, std::shared_ptr<Vortek::Utilities::ThreadPool> pThreadPool );
	~AssetPackager();

	void PackageAssets( const rapidjson::Value& assets );

  private:
	void ConvertAssetToLuaTable( Vortek::Filesystem::LuaSerializer& luaSerializer,
								 const AssetConversionData& conversionData );

	void CreateLuaAssetFiles( const std::string& sProjectPath, const rapidjson::Value& assets );
	bool CompileLuaAssetFiles();
	bool CreateAssetsZip();

	struct AssetPackageStatus
	{
		std::string sError{};
		bool bSuccess{ false };
	};

	AssetPackageStatus SerializeAssetsByType( const rapidjson::Value& assets,
											  const std::filesystem::path& tempAssetsPath,
											  const std::string& sAssetTypeName, const std::string& sContentPath,
											  Vortek::Utilities::AssetType eAssetType );

  private:
	AssetPackagerParams m_Params;
	std::shared_ptr<Vortek::Utilities::ThreadPool> m_pThreadPool;
};

} // namespace Vortek::Editor
