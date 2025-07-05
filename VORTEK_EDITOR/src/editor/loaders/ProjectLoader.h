#pragma once
#include <string>

namespace VORTEK_CORE
{
struct SaveProject;
}

namespace VORTEK_EDITOR
{

class SaveProject;

class ProjectLoader
{
  public:
	bool CreateNewProject( const std::string& sProjectName, const std::string& sFilepath );
	bool LoadProject( const std::string& sFilepath );
	bool SaveLoadedProject( VORTEK_CORE::SaveProject& save );

  private:
	bool CreateProjectFile( const std::string& sProjectName, const std::string& sFilepath );
	bool CreateMainLuaScript( const std::string& sProjectName, const std::string& sFilepath );
};

} // namespace VORTEK_EDITOR
