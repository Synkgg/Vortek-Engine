#pragma once
#include "IDisplay.h"

#include <vector>
#include <string>
#include <filesystem>

namespace VORTEK_FILESYSTEM
{
	class DirectoryWatcher;
}

namespace VORTEK_EDITOR
{
	class ScriptDisplay : public IDisplay
	{
	public:
		ScriptDisplay();
		virtual ~ScriptDisplay();

		virtual void Draw() override;
		virtual void Update() override;

	private:
		void GenerateScriptList();
		void WriteScriptListToFile();
		void OnFileChanged(const std::filesystem::path& path, bool bModified);

	private:
		std::vector<std::string> m_ScriptList;
		std::vector<std::string> m_Scripts;
		std::string m_sScriptsDirectory;
		int m_Selected;
		bool m_bScriptsChanged;
		bool m_bListScripts;

		std::unique_ptr<VORTEK_FILESYSTEM::DirectoryWatcher> m_pDirWatcher;
		std::atomic_bool m_bFilesChanged;
	};

} // namespace VORTEK_EDITOR