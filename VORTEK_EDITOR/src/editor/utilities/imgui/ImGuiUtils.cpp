#include "ImGuiUtils.h"
#include "Logger/Logger.h"
#include <imgui_stdlib.h>
#include <unordered_map>
#include "imgui_internal.h"

namespace ImGui
{

void InitDefaultStyles()
{
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	style.TabRounding = 4.f;
	style.TabBorderSize = 1.f;
	style.FrameBorderSize = 0.1f;
	style.FramePadding.x = 4.f;
	style.FramePadding.y = 3.f;
	style.FrameRounding = 2.f;
	style.ScrollbarSize = 10.f;
	style.ScrollbarRounding = 6.f;
	style.WindowBorderSize = 1.f;
	style.WindowPadding.x = 10.f;
	style.WindowPadding.y = 10.f;
	style.WindowRounding = 8.f;
	style.WindowTitleAlign.x = 0.5f;
	style.ItemSpacing.x = 3.f;
	style.ItemInnerSpacing.x = 0.f;
	style.ItemInnerSpacing.y = 4.f;
	style.ButtonTextAlign.x = 0.5f;
	style.ButtonTextAlign.y = 0.5f;
	style.ItemInnerSpacing.x = 2.f;
	style.WindowMenuButtonPosition = ImGuiDir_None;

	auto& colors = ImGui::GetStyle().Colors;
	colors[ ImGuiCol_WindowBg ] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

	// Headers
	colors[ ImGuiCol_Header ] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ ImGuiCol_HeaderHovered ] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ ImGuiCol_HeaderActive ] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Buttons
	colors[ ImGuiCol_Button ] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ ImGuiCol_ButtonHovered ] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ ImGuiCol_ButtonActive ] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Frame BG
	colors[ ImGuiCol_FrameBg ] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ ImGuiCol_FrameBgHovered ] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ ImGuiCol_FrameBgActive ] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Tabs
	colors[ ImGuiCol_Tab ] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ ImGuiCol_TabHovered ] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
	colors[ ImGuiCol_TabActive ] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
	colors[ ImGuiCol_TabUnfocused ] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ ImGuiCol_TabUnfocusedActive ] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

	// Title
	colors[ ImGuiCol_TitleBg ] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ ImGuiCol_TitleBgActive ] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ ImGuiCol_TitleBgCollapsed ] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
}

void ColoredLabel( const std::string& label, const ImVec2& size, const ImVec4& color, const bool bold )
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[ 0 ];

	if ( !bold )
	{
		ImGui::PushFont( boldFont );
		ImGui::PushStyleColor( ImGuiCol_Button, color );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, color );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, color );
		ImGui::Button( label.c_str(), size );
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}
	else
	{
		ImGui::PushFont( boldFont );
		ImGui::PushStyleColor( ImGuiCol_Button, color );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, color );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, color );
		ImGui::Button( label.c_str(), size );
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopFont();
	}
}

void OffsetTextX( const std::string& label, float position )
{
	ImGui::SetCursorPosX( position );
	ImGui::Text( label.c_str() );
}

void AddSpaces( int numSpaces )
{
	VORTEK_ASSERT( numSpaces > 0 && "Number of spaces must be a positive number!" );
	for ( int i = 0; i < numSpaces; ++i )
		ImGui::Spacing();
}

void InlineLabel( const std::string& label, float spaceSize )
{
	ImGui::Text( label.c_str() );
	ImGui::SameLine();
	ImGui::SetCursorPosX( spaceSize );
}

void ActiveButton( const char* label, ImVec2 size )
{
	ImGui::PushStyleColor( ImGuiCol_Button, BUTTON_HELD );
	ImGui::PushStyleColor( ImGuiCol_ButtonHovered, BUTTON_HELD );
	ImGui::PushStyleColor( ImGuiCol_ButtonActive, BUTTON_HELD );
	ImGui::Button( label, size );
	ImGui::PopStyleColor( 3 );
}

void DisabledButton( const char* label, ImVec2 size, const std::string& disabledMsg )
{
	ImGui::BeginDisabled();
	ImGui::Button( label, size );

	if ( !disabledMsg.empty() )
		ImGui::SetItemTooltip( disabledMsg.c_str() );

	ImGui::EndDisabled();
}

void LoadingSpinner( const char* label, float radius, float thickness, const ImU32& color )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if ( window->SkipItems )
		return;

	ImGuiContext& g = *ImGui::GetCurrentContext();
	const ImGuiID id = window->GetID( label );
	const ImVec2 pos = ImGui::GetCursorScreenPos();
	const float size = ( radius + thickness ) * 2.f;
	const ImRect bb( pos, ImVec2{ pos.x + size, pos.y + size } );
	ImGui::ItemSize( bb );
	if ( !ImGui::ItemAdd( bb, id ) )
		return;

	const float time = static_cast<float>( ImGui::GetTime() );
	const int numSegments = 30;
	const float startAngle = time * 4.f;
	const float angleOffset = 2.0f * IM_PI / numSegments;

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList->PathClear();
	for ( int i = 0; i < numSegments; ++i )
	{
		const float a = startAngle + i * angleOffset;
		drawList->PathLineTo(
			ImVec2{ pos.x + radius + std::cos( a ) * radius, pos.y + radius + std::sin( a ) * radius } );
	}
	drawList->PathStroke( color, false, thickness );
}

void ActiveImageButton( const char* buttonId, ImTextureID textureID, ImVec2 size )
{
	ImGui::PushStyleColor( ImGuiCol_Button, BUTTON_HELD );
	ImGui::PushStyleColor( ImGuiCol_ButtonHovered, BUTTON_HELD );
	ImGui::PushStyleColor( ImGuiCol_ButtonActive, BUTTON_HELD );
	ImGui::ImageButton( buttonId, textureID, size );
	ImGui::PopStyleColor( 3 );
}

void DisabledImageButton( const char* buttonId, ImTextureID textureID, ImVec2 size, const std::string& disabledMsg )
{
	ImGui::BeginDisabled();
	ImGui::ImageButton( buttonId, textureID, size );

	if ( !disabledMsg.empty() )
		ImGui::SetItemTooltip( disabledMsg.c_str() );

	ImGui::EndDisabled();
}

void InputTextReadOnly( const std::string& sLabel, std::string* sInputText )
{
	ImVec4 textDisabled = ImGui::GetStyle().Colors[ ImGuiCol_TextDisabled ];
	ImVec4 bgHovered = ImGui::GetStyle().Colors[ ImGuiCol_FrameBgHovered ];
	ImVec4 dimmedBg = ImVec4( bgHovered.x, bgHovered.y, bgHovered.z, bgHovered.w * 0.5f );

	ImGui::PushStyleColor( ImGuiCol_FrameBg, dimmedBg );
	ImGui::PushStyleColor( ImGuiCol_Text, textDisabled );
	ImGui::InputText( sLabel.c_str(), sInputText, ImGuiInputTextFlags_ReadOnly );

	ImGui::PopStyleColor( 2 );
}

static std::unordered_map<std::string, ImFont*> g_mapImGuiFonts;

ImFont* GetFont( const std::string& sFontName )
{
	auto fontItr = g_mapImGuiFonts.find( sFontName );
	if ( fontItr == g_mapImGuiFonts.end() )
	{
		VORTEK_ERROR( "Failed to get font [{}] - Does not exist.", sFontName );
		ImFont* pFont = ImGui::GetIO().Fonts->Fonts[ 0 ];
		return pFont;
	}

	return fontItr->second;
}

bool AddFont( const std::string& sFontName, ImFont* pFont, float fontSize )
{
	if ( g_mapImGuiFonts.contains( sFontName ) )
	{
		VORTEK_ERROR( "Failed to add font [{}] to imgui fonts. Already exists.", sFontName );
		return false;
	}

	if ( !pFont )
	{
		VORTEK_ERROR( "Failed to add font [{}] to imgui fonts.", sFontName );
		return false;
	}

	return g_mapImGuiFonts.emplace( sFontName, pFont ).second;
}

bool AddFontFromFile( const std::string sFontName, const std::string& sFontFile, float fontSize )
{
	if ( g_mapImGuiFonts.contains( sFontName ) )
	{
		VORTEK_ERROR( "Failed to add font [{}] to imgui fonts. Already exists.", sFontName );
		return false;
	}

	ImGuiIO& io = ImGui::GetIO();
	ImFont* pFont =
		io.Fonts->AddFontFromFileTTF( sFontFile.c_str(), fontSize, nullptr, io.Fonts->GetGlyphRangesDefault() );

	if ( !pFont )
	{
		VORTEK_ERROR( "Failed to add font [{}] to imgui fonts.", sFontName );
		return false;
	}

	return g_mapImGuiFonts.emplace( sFontName, pFont ).second;
}

bool AddFontFromMemory( const std::string& sFontName, void* fontData, float dataSize, float fontSize )
{
	if ( g_mapImGuiFonts.contains( sFontName ) )
	{
		VORTEK_ERROR( "Failed to add font [{}] to imgui fonts. Already exists.", sFontName );
		return false;
	}

	ImGuiIO& io = ImGui::GetIO();
	ImFont* pFont =
		io.Fonts->AddFontFromMemoryTTF( fontData, dataSize, fontSize, nullptr, io.Fonts->GetGlyphRangesDefault() );

	if ( !pFont )
	{
		VORTEK_ERROR( "Failed to add font [{}] to imgui fonts.", sFontName );
		return false;
	}

	return g_mapImGuiFonts.emplace( sFontName, pFont ).second;
}

} // namespace ImGui
