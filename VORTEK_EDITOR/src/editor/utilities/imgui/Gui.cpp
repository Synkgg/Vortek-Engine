#include "Gui.h"
#include "Logger/Logger.h"
#include "../fonts/IconsFontAwesome5.h"
#include "../fonts/editor_fonts.h"
#include <Windowing/Window/Window.h>
#include "../fonts/fonts_editor.h"

// IMGUI 
// ===================================
#include "ImGuiUtils.h"
#include "imgui.h"
#include <imgui_internal.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL_opengl.h>
// ===================================

namespace VORTEK_EDITOR
{
	bool Gui::InitImGui(VORTEK_WINDOWING::Window* pWindow)
	{
		if (m_bInitialized)
			return false;

		const char* glslVersion = "#version 450";
		IMGUI_CHECKVERSION();

		if (!ImGui::CreateContext())
		{
			VORTEK_ERROR("Failed to create ImGui Context");
			return false;
		}

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		io.ConfigWindowsMoveFromTitleBarOnly = true;

		io.Fonts->AddFontFromMemoryTTF(OpenSans_ExtraBold, sizeof(OpenSans_ExtraBold), 18.0f, NULL);

		io.Fonts->AddFontDefault();
		io.FontDefault = io.Fonts->AddFontFromMemoryTTF(OpenSans_Regular, sizeof(OpenSans_Regular), 18.0f, NULL);
		float baseFontSize = 16.0f;
		float iconFontSize = baseFontSize * 2.0f / 3.0f;

		// merge in icons from Font Awesome
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;
		icons_config.GlyphMinAdvanceX = iconFontSize;
		io.Fonts->AddFontFromMemoryTTF(fa_solid_900, fa_solid_900_size, baseFontSize, &icons_config, icons_ranges);
		io.ConfigViewportsNoDecoration = false;

		if (!ImGui_ImplSDL2_InitForOpenGL(pWindow->GetWindow().get(), pWindow->GetGLContext()))
		{
			VORTEK_ERROR("Failed to intialize ImGui SDL2 for OpenGL!");
			return false;
		}

		if (!ImGui_ImplOpenGL3_Init(glslVersion))
		{
			VORTEK_ERROR("Failed to intialize ImGui OpenGL3!");
			return false;
		}

		ImGui::InitDefaultStyles();

		return true;
	}
	void Gui::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
	}

	void Gui::End(VORTEK_WINDOWING::Window* pWindow)
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			SDL_Window* backupCurrentWindow = SDL_GL_GetCurrentWindow();
			SDL_GLContext backupCurrentContext = SDL_GL_GetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			SDL_GL_MakeCurrent(backupCurrentWindow, backupCurrentContext);
		}
	}

	void Gui::ShowImGuiDemo()
	{
		ImGui::ShowDemoWindow();
	}

} // namespace VORTEK_EDITOR
