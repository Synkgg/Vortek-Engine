#include "ScriptEditorDisplay.h"
#include "Logger/Logger.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include <VortekFilesystem/Dialogs/FileDialog.h>
#include <fstream>
#include <sstream>
#include <string>

using namespace VORTEK_FILESYSTEM;

std::string OpenedFile;

namespace VORTEK_EDITOR
{

	ScriptEditorDisplay::ScriptEditorDisplay()
	{
		auto lang = TextEditor::LanguageDefinition::Lua();
		editor.SetLanguageDefinition(lang);
		editor.SetPalette(TextEditor::GetDarkPalette());
		editor.SetShowWhitespaces(false);
		editor.SetTabSize( 8 );
	}

	void ScriptEditorDisplay::DrawToolbar()
	{
		if (ImGui::Button("Open")) {
			VORTEK_FILESYSTEM::FileDialog fd{};
			const auto sFilepath = fd.OpenFileDialog("Open", "", { "*.lua" });

			LoadScriptFromFile(sFilepath, editor);
			OpenedFile = sFilepath;
		}

		ImGui::SameLine();

		if (ImGui::Button("Save")) {
			SaveScriptToFile(OpenedFile, editor);
		}
	}

	void ScriptEditorDisplay::LoadScriptFromFile(const std::string& path, TextEditor& editor)
	{
		std::ifstream file(path);
		if (!file.good())
		{
			VORTEK_ERROR("Failed to load script: " + path);
			return;
		}

		std::string content((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());
		editor.SetText(content);
	}

	void ScriptEditorDisplay::SaveScriptToFile(const std::string& path, const TextEditor& editor)
	{
		std::ofstream file(path);
		if (!file.good())
		{
			VORTEK_ERROR("Failed to save script: " + path);
			return;
		}

		file << editor.GetText();
	}

	void ScriptEditorDisplay::Draw()
	{
		if (!ImGui::Begin(ICON_FA_DESKTOP " Script Editor"))
		{
			ImGui::End();
			return;
		}

		// Draw the toolbar
		DrawToolbar();

		// Add spacing between toolbar and editor
		ImGui::Separator();

		// Render the code editor
		editor.Render("LuaScriptEditor");

		ImGui::End();
	}
} // namespace VORTEK_EDITOR
