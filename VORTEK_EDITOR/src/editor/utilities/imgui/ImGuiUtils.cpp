#include "ImGuiUtils.h"
#include "Logger/Logger.h"
#include <imgui_stdlib.h>

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

} // namespace ImGui
