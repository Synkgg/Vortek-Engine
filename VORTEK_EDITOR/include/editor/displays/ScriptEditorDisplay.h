#pragma once
#include "IDisplay.h"
#include <TextEditor.h>
#include <string>
#include <vector>

namespace Vortek::Editor
{
class ScriptEditorDisplay : public IDisplay
{
  public:
	ScriptEditorDisplay();

	void Draw() override; // Renders the editor window
	void DrawToolbar();	  // Renders Open/Save buttons

	// Called from ContentDisplay when a Lua file is opened
	void OpenScript( const std::string& filepath );

  private:
	struct ScriptTab
	{
		std::string FilePath;
		TextEditor Editor;
		bool bIsDirty = false;
	};

	std::vector<ScriptTab> m_OpenScripts;
	int m_CurrentTab = -1; // Index of active tab

	void LoadScriptFromFile( const std::string& path, TextEditor& editor );
	void SaveScriptToFile( const std::string& path, const TextEditor& editor );
};
} // namespace VORTEK_EDITOR
