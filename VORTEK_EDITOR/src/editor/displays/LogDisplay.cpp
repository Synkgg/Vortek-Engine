#include "LogDisplay.h"
#include "Logger/Logger.h"
#include <ranges>
#include <imgui_cpp/imgui_stdlib.h>
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "editor/utilities/EditorSettings.h"

bool LogIncremented = false;
bool WarnIncremented = false;
bool ErrorIncremented = false;

bool ShowAll = true;

namespace VORTEK_EDITOR
{
	void LogDisplay::GetLogs()
	{
		const auto& logs{ VORTEK_GET_LOGS() };
		if (logs.size() == m_LogIndex)
			return;

		for (size_t i = m_LogIndex; i < logs.size(); ++i)
		{
			const auto& log = logs[i];
			int oldTextSize = m_TextBuffer.size();
			m_TextBuffer.append(log.log.c_str());
			m_TextBuffer.append("\n");
			m_TextOffsets.push_back(oldTextSize);
		}

		m_LogIndex = static_cast<int>(logs.size());
	}

	LogDisplay::LogDisplay()
		: m_sSearchQuery{}
		, m_bAutoScroll{ true }
		, m_bShowInfo{ true }
		, m_bShowWarn{ true }
		, m_bShowError{ true }
		, m_LogIndex{ 0 }
		, m_Logs { 0 }
		, m_Warns { 0 }
		, m_Errors { 0 }
	{
	}

	void LogDisplay::Clear()
	{
		m_TextBuffer.clear();
		m_TextOffsets.clear();
		m_LogIndex = 0;
		m_Logs = 0;
		m_Warns = 0;
		m_Errors = 0;
		LogIncremented = false;
		WarnIncremented = false;
		ErrorIncremented = false;
	}

	void LogDisplay::Draw()
	{
		if (!ImGui::Begin(ICON_FA_TERMINAL " Console"))
		{
			ImGui::End();
			return;
		}

		GetLogs();

		// Logging Controls
		ImGui::SetNextItemWidth( 150 );
		ImGui::BeginGroup();
		if ( ImGui::BeginCombo( "##Options", "Options", 0 ) )
		{
			if ( ImGui::Checkbox( "Show All", &ShowAll ) )
			{
				m_bShowInfo = m_bShowWarn = m_bShowError = ShowAll;
			}

			ImGui::Separator();

			if ( ImGui::Checkbox( "INFO", &m_bShowInfo ) )
				ShowAll = false;
			ImGui::SameLine();
			ImGui::Text( "(%d)", m_Logs );
			if ( ImGui::Checkbox( "WARN", &m_bShowWarn ) )
				ShowAll = false;
			ImGui::SameLine();
			ImGui::Text( "(%d)", m_Warns );
			if ( ImGui::Checkbox( "ERROR", &m_bShowError ) )
				ShowAll = false;
			ImGui::SameLine();
			ImGui::Text( "(%d)", m_Errors );

			ImGui::Separator();
			ImGui::Checkbox( "Auto-scroll", &m_bAutoScroll );

			ImGui::EndCombo();
		}
		ImGui::EndGroup();

		// Right: Search + buttons
		ImGui::SameLine( ImGui::GetContentRegionAvail().x - 265 ); // Adjust width to fit all right-side controls
		ImGui::SetNextItemWidth(200.f);
		ImGui::InputTextWithHint("##Search", "Filter...", &m_sSearchQuery);

		ImGui::SameLine(0.f, 10.f);

		// Position where you want the vertical line
		ImVec2 pos = ImGui::GetCursorScreenPos();
		float height = 25.0f; // height of the separator
		float thickness = 1.0f;

		ImGui::GetWindowDrawList()->AddLine(
			ImVec2(pos.x, pos.y),
			ImVec2(pos.x, pos.y + height),
			IM_COL32(200, 200, 200, 255), // color (light gray)
			thickness
		);

		// Add a dummy to advance the cursor
		ImGui::Dummy(ImVec2(thickness, height));
		ImGui::SameLine(0.f, 10.f);

		if (ImGui::Button(ICON_FA_BROOM "##Clear"))
		{
			Clear();
			VORTEK_CLEAR_LOGS();
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			ImGui::SetTooltip("Clear");
		ImGui::SameLine(0.f, 5.f);

		if (ImGui::Button(ICON_FA_COPY "##Copy"))
		{
			ImGui::LogToClipboard();
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			ImGui::SetTooltip("Copy");

		ImGui::Separator();

		if (ImGui::BeginChild("scrolling", ImVec2{ 0.f, 0.f }, false, ImGuiWindowFlags_HorizontalScrollbar))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.f, 0.f });

			int logs = 0;
			int warns = 0;
			int errors = 0;

			for (int i = 0; i < m_TextOffsets.Size; ++i)
			{
				const char* line_start = m_TextBuffer.begin() + m_TextOffsets[i];
				const char* line_end = (i + 1 < m_TextOffsets.Size)
					? (m_TextBuffer.begin() + m_TextOffsets[i + 1] - 1)
					: m_TextBuffer.end();

				std::string_view text{ line_start, line_end };

				// Apply Filters
				bool bIsInfo { text.find("INFO") != std::string_view::npos };
				bool bIsWarn { text.find("WARN") != std::string_view::npos };
				bool bIsError { text.find("ERROR") != std::string_view::npos };

				// Always count log types
				if ( bIsInfo ) logs++;
				if ( bIsWarn ) warns++;
				if ( bIsError ) errors++;

				// Apply filter for drawing
				if ( ( bIsInfo && !m_bShowInfo ) || ( bIsWarn && !m_bShowWarn ) || ( bIsError && !m_bShowError ) )
					continue;

				if ( !m_sSearchQuery.empty() && text.find( m_sSearchQuery ) == std::string_view::npos )
					continue;

				ImVec4 color{ 1.f, 1.f, 1.f, 1.f };
				if (bIsInfo)
				{
					if ( g_AppliedThemeIndex == 1 )
						color = ImVec4{ 0.f, 0.f, 0.f, 1.f };
				}
				else if ( bIsError )
					color = ImVec4{ 1.f, 0.f, 0.f, 1.f };
				else if ( bIsWarn )
					color = ImVec4{ 1.f, 1.f, 0.f, 1.f };


				ImGui::PushStyleColor(ImGuiCol_Text, color);
				ImGui::TextUnformatted(line_start, line_end);
				ImGui::PopStyleColor();
			}

			// Store after loop
			m_Logs = logs;
			m_Warns = warns;
			m_Errors = errors;

			ImGui::PopStyleVar();

			if (m_bAutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.f);

			ImGui::EndChild();
		}
		ImGui::End();
	}
} // namespace VORTEK_EDITOR
