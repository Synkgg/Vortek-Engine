#pragma once
#include <rapidjson/document.h>

namespace Vortek::Core
{
class ProjectInfo;
struct GameConfig;
} // namespace Vortek::Core

namespace Vortek::Utilities
{
class ThreadPool;
}

namespace Vortek::Editor
{
struct PackageData
{
	std::unique_ptr<Vortek::Core::ProjectInfo> pProjectInfo{ nullptr };
	std::unique_ptr<Vortek::Core::GameConfig> pGameConfig{ nullptr };
	std::string sTempDataPath{};
	std::string sAssetFilepath{};
	std::string sFinalDestination{};
};

struct PackagingProgress
{
	float percent{ 0.f };
	std::string sMessage{};
};

class Packager
{
  public:
	Packager( std::unique_ptr<PackageData> pData, std::shared_ptr<Vortek::Utilities::ThreadPool> pThreadPool );
	~Packager();

	bool Completed() const;
	bool HasError() const;

	PackagingProgress GetProgress() const;

	void FinishPackaging();

  private:
	void RunPackager();
	void UpdateProgress( float percent, std::string_view message );
	std::string CreateConfigFile( const std::string& sTempFilepath );
	std::string CreateAssetDefsFile( const std::string& sTempFilepath, const rapidjson::Value& assets );
	std::vector<std::string> CreateSceneFiles( const std::string& sTempFilepath, const rapidjson::Value& scenes );
	void CopyFilesToDestination();
	void CopyAssetsToDestination();

  private:
	std::unique_ptr<PackageData> m_pPackageData;
	std::thread m_PackageThread;
	std::atomic_bool m_bPackaging;
	std::atomic_bool m_bHasError;
	mutable std::shared_mutex m_ProgressMutex;
	PackagingProgress m_Progress;

	std::shared_ptr<Vortek::Utilities::ThreadPool> m_pThreadPool;
};

} // namespace Vortek::Editor
