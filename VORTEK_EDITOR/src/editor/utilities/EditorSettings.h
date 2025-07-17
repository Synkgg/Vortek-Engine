#pragma once

namespace VORTEK_EDITOR
{
// Theme settings
inline int g_SelectedThemeIndex = 3; // Default to Vortek Custom
inline int g_AppliedThemeIndex = 3;

// New Hub UI setting
inline bool g_UseNewHubUI = true; // Default to true

// Functions for loading/saving editor config (implemented in editor_settings.cpp)
void SaveEditorConfig( int selectedTheme, bool useNewHub );
void LoadEditorConfig();
} // namespace VORTEK_EDITOR
