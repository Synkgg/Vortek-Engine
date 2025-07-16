#pragma once
#include <string>

namespace VORTEK_CORE
{
class ProjectInfo;
}

namespace VORTEK_EDITOR
{

class ProjectLoader
{
  public:
	bool CreateNewProject( const std::string& sProjectName, const std::string& sFilepath );
	bool LoadProject( const std::string& sFilepath );
	bool SaveLoadedProject( const VORTEK_CORE::ProjectInfo& projectInfo );

  private:
	bool CreateProjectFile( const std::string& sProjectName, const std::string& sFilepath );
	bool CreateMainLuaScript( const std::string& sProjectName, const std::string& sFilepath );
	bool CreateScriptListFile();
};

} // namespace VORTEK_EDITOR
