#include "EditorSettings.h"

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <fstream>

namespace VORTEK_EDITOR
{

void SaveEditorConfig( int selectedTheme, bool useNewHub )
{
	rapidjson::Document doc;
	doc.SetObject();

	auto& allocator = doc.GetAllocator();
	doc.AddMember( "themeIndex", selectedTheme, allocator );
	doc.AddMember( "useNewHub", useNewHub, allocator );

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer( buffer );
	doc.Accept( writer );

	std::ofstream file( "editor_config.json" );
	if ( file.is_open() )
	{
		file << buffer.GetString();
		file.close();
	}
}

void LoadEditorConfig()
{
	std::ifstream file( "editor_config.json" );
	if ( !file.is_open() )
		return;

	std::string content( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );

	rapidjson::Document doc;
	if ( !doc.Parse( content.c_str() ).HasParseError() )
	{
		if ( doc.HasMember( "themeIndex" ) && doc[ "themeIndex" ].IsInt() )
		{
			g_SelectedThemeIndex = doc[ "themeIndex" ].GetInt();
			g_AppliedThemeIndex = g_SelectedThemeIndex;
		}

		if ( doc.HasMember( "useNewHub" ) && doc[ "useNewHub" ].IsBool() )
		{
			g_UseNewHubUI = doc[ "useNewHub" ].GetBool();
		}
	}
}

} // namespace VORTEK_EDITOR
