#pragma once
#include "IDisplay.h"
#include <TextEditor/TextEditor.h> // Include this so the class knows about TextEditor

namespace VORTEK_EDITOR
{
	class ScriptEditorDisplay : public IDisplay
	{
	public:
		ScriptEditorDisplay();

		void Draw();           // Renders the editor window
		void DrawToolbar();    // Renders Open/Save/Run buttons

	private:
		void LoadScriptFromFile(const std::string& path, TextEditor& editor);
		void SaveScriptToFile(const std::string& path, const TextEditor& editor);

	private:
		TextEditor editor;
	};
}