#include "Editor/displays/ScriptEditorDisplay.h"
#include "Logger/Logger.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "editor/utilities/EditorState.h"
#include <VortekFilesystem/Dialogs/FileDialog.h>
#include "Core/ECS/MainRegistry.h"
#include <regex>

using namespace Vortek::Filesystem;

namespace Vortek::Editor
{

// ======================================================
// Helpers
// ======================================================

// Extract all user-defined variables from Lua text
static std::unordered_set<std::string> ExtractUserVariables( const std::string& text )
{
	std::unordered_set<std::string> vars;
	std::regex localVarPattern( R"(\blocal\s+([A-Za-z_][A-Za-z0-9_]*)\b)" );
	std::regex globalVarPattern( R"(\b([A-Za-z_][A-Za-z0-9_]*)\s*=)" );

	std::smatch match;
	std::string::const_iterator searchStart( text.cbegin() );

	// Local variables
	while ( std::regex_search( searchStart, text.cend(), match, localVarPattern ) )
	{
		vars.insert( match[ 1 ].str() );
		searchStart = match.suffix().first;
	}

	// Global variables (non-local)
	searchStart = text.cbegin();
	while ( std::regex_search( searchStart, text.cend(), match, globalVarPattern ) )
	{
		std::string var = match[ 1 ].str();
		if ( var != "local" && var != "function" && var != "for" && var != "if" && var != "while" && var != "return" &&
			 var != "then" && var != "end" )
			vars.insert( var );
		searchStart = match.suffix().first;
	}

	return vars;
}

// Create a Lua language definition with user variables included
static TextEditor::LanguageDefinition CreateDynamicLuaLanguage( const std::unordered_set<std::string>& userVars )
{
	auto lang = TextEditor::LanguageDefinition::Lua();

	for ( const auto& var : userVars )
	{
		lang.mIdentifiers.insert( { var, TextEditor::Identifier() } );
	}

	return lang;
}

// Red-colored identifiers
static TextEditor::Palette GetCustomPalette()
{
	auto palette = TextEditor::GetDarkPalette();
	palette[ (int)TextEditor::PaletteIndex::Identifier ] = 0xFF4040FF; // Red
	return palette;
}

// ======================================================
// ScriptEditorDisplay
// ======================================================

ScriptEditorDisplay::ScriptEditorDisplay()
{
}

void ScriptEditorDisplay::DrawToolbar()
{
	if ( ImGui::Button( "Save" ) )
	{
		if ( m_CurrentTab >= 0 && m_CurrentTab < (int)m_OpenScripts.size() )
		{
			SaveScriptToFile( m_OpenScripts[ m_CurrentTab ].FilePath, m_OpenScripts[ m_CurrentTab ].Editor );
			m_OpenScripts[ m_CurrentTab ].bIsDirty = false;
		}
	}
}

void ScriptEditorDisplay::OpenScript( const std::string& filepath )
{
	// Check if already open
	for ( size_t i = 0; i < m_OpenScripts.size(); i++ )
	{
		if ( m_OpenScripts[ i ].FilePath == filepath )
		{
			m_CurrentTab = (int)i;
			return;
		}
	}

	// Create new editor
	ScriptTab script{};
	script.FilePath = filepath;

	// Initial setup
	script.Editor.SetLanguageDefinition( TextEditor::LanguageDefinition::Lua() );
	script.Editor.SetPalette( GetCustomPalette() );
	script.Editor.SetShowWhitespaces( false );
	script.Editor.SetTabSize( 4 );

	LoadScriptFromFile( filepath, script.Editor );

	// Initial variable parsing
	std::string text = script.Editor.GetText();
	auto userVars = ExtractUserVariables( text );
	auto lang = CreateDynamicLuaLanguage( userVars );
	script.Editor.SetLanguageDefinition( lang );

	m_OpenScripts.push_back( std::move( script ) );
	m_CurrentTab = (int)m_OpenScripts.size() - 1;
}

void ScriptEditorDisplay::LoadScriptFromFile( const std::string& path, TextEditor& editor )
{
	std::ifstream file( path );
	if ( !file.good() )
	{
		VORTEK_ERROR( "Failed to load script: " + path );
		return;
	}

	std::string content( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );
	editor.SetText( content );
}

void ScriptEditorDisplay::SaveScriptToFile( const std::string& path, const TextEditor& editor )
{
	std::ofstream file( path );
	if ( !file.good() )
	{
		VORTEK_ERROR( "Failed to save script: " + path );
		return;
	}

	file << editor.GetText();
}

void ScriptEditorDisplay::Draw()
{
	if ( auto& pEditorState = MAIN_REGISTRY().GetContext<EditorStatePtr>() )
	{
		if ( !pEditorState->IsDisplayOpen( EDisplay::ScriptEditor ) )
		{
			return;
		}
	}

	if ( !ImGui::Begin( ICON_FA_CODE " Script Editor" ) )
	{
		ImGui::End();
		return;
	}

	DrawToolbar();
	ImGui::Separator();

	if ( ImGui::BeginTabBar( "ScriptTabs" ) )
	{
		for ( int i = 0; i < (int)m_OpenScripts.size(); i++ )
		{
			auto& script = m_OpenScripts[ i ];
			bool bOpen = true;

			std::string tabName = std::filesystem::path( script.FilePath ).filename().string();
			if ( script.bIsDirty )
				tabName += " *";

			if ( ImGui::BeginTabItem( tabName.c_str(), &bOpen ) )
			{
				m_CurrentTab = i;

				script.Editor.Render( ( "Editor" + std::to_string( i ) ).c_str() );

				if ( script.Editor.IsTextChanged() )
				{
					script.bIsDirty = true;

					// Live variable parsing + recolor
					std::string text = script.Editor.GetText();
					auto userVars = ExtractUserVariables( text );
					auto lang = script.Editor.GetLanguageDefinition(); // make a copy
					auto baseLang = TextEditor::LanguageDefinition::Lua();

					// Merge default and user identifiers
					for ( const auto& var : userVars )
					{
						if ( baseLang.mKeywords.count( var ) == 0 )
							lang.mIdentifiers.insert( { var, TextEditor::Identifier() } );
					}

					// Apply the modified definition
					script.Editor.SetLanguageDefinition( lang );
				}

				ImGui::EndTabItem();
			}

			if ( !bOpen )
			{
				m_OpenScripts.erase( m_OpenScripts.begin() + i );
				if ( m_CurrentTab >= i )
					m_CurrentTab--;
				i--;
			}
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}

} // namespace VORTEK_EDITOR
