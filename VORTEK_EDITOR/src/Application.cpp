#include "Application.h"
#include <glad/glad.h>
#include <Rendering/Core/Renderer.h>

#include <Logger/Logger.h>
#include <Core/ECS/MainRegistry.h>

#include <Core/Resources/AssetManager.h>
#include <Core/CoreUtilities/CoreEngineData.h>

#include <Core/Scripting/InputManager.h>
#include <Windowing/Inputs/Keyboard.h>
#include <Windowing/Inputs/Mouse.h>
#include <Windowing/Inputs/Gamepad.h>
#include <Windowing/Window/Window.h>

// IMGUI TESTING
// ===================================
#include <imgui_internal.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>
#include <SDL_opengl.h>
// ===================================

#include "editor/displays/MenuDisplay.h"
#include "editor/displays/AssetDisplay.h"
#include "editor/displays/SceneDisplay.h"
#include "editor/displays/SceneHierarchyDisplay.h"
#include "editor/displays/TileDetailsDisplay.h"
#include "editor/displays/TilesetDisplay.h"
#include "editor/displays/TilemapDisplay.h"
#include "editor/displays/LogDisplay.h"
#include "editor/displays/EditorStyleToolDisplay.h"

#include "editor/utilities/editor_textures.h"
#include "editor/utilities/EditorFramebuffers.h"
#include "editor/utilities/ImGuiUtils.h"
#include "editor/utilities/DrawComponentUtils.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"

#include "editor/systems/GridSystem.h"

// TODO: This needs to be removed. Scenes are added by default for testing.
#include "editor/scene/SceneManager.h"


namespace VORTEK_EDITOR
{
	bool Application::Initialize()
	{
		VORTEK_INIT_LOGS(false, true);
		// TODO: LOAD CORE ENGINE DATA
		// Init SDL
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			std::string error = SDL_GetError();
			VORTEK_ERROR("Failed to initialize SDL: {0}", error);
			return false;
		}

		// Set up OpenGL
		if (SDL_GL_LoadLibrary(NULL) != 0)
		{
			std::string error = SDL_GetError();
			VORTEK_ERROR("Failed to Open GL Library: {0}", error);
			return false;
		}

		// Set the OpenGL attributes
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		// Set the number of bits per channel
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

		SDL_DisplayMode displayMode;
		SDL_GetCurrentDisplayMode(0, &displayMode);

		// Create the Window
		m_pWindow = std::make_unique<VORTEK_WINDOWING::Window>("VORTEK 2D",
			displayMode.w,
			displayMode.h,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			true,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
			SDL_WINDOW_MOUSE_CAPTURE | SDL_WINDOW_MAXIMIZED);

		if (!m_pWindow->GetWindow())
		{
			VORTEK_ERROR("Failed to create the window!");
			return false;
		}

		// Create the openGL context
		m_pWindow->SetGLContext(SDL_GL_CreateContext(m_pWindow->GetWindow().get()));

		// Initialize Glad
		if (gladLoadGLLoader(SDL_GL_GetProcAddress) == 0)
		{
			std::cout << "Failed to Initialize Glad" << std::endl;

			VORTEK_ERROR("Failed to LoadGL --> GLAD");
			return false;
		}

		if (!m_pWindow->GetGLContext())
		{
			std::string error = SDL_GetError();
			VORTEK_ERROR("Failed to create OpenGL context: {0}", error);
			return false;
		}

		if ((SDL_GL_MakeCurrent(m_pWindow->GetWindow().get(), m_pWindow->GetGLContext())) != 0)
		{
			std::string error = SDL_GetError();
			VORTEK_ERROR("Failed to make OpenGL context current: {0}", error);
			return false;
		}

		SDL_GL_SetSwapInterval(1);


		auto& mainRegistry = MAIN_REGISTRY();
		if (!mainRegistry.Initialize())
		{
			VORTEK_ERROR("Failed to initialize the Main Registry!");
			return false;
		}

		if (!InitImGui())
		{
			VORTEK_ERROR("Failed to initialize ImGui!");
			return false;
		}

		if (!LoadShaders())
		{
			VORTEK_ERROR("Failed to load the shaders!");
			return false;
		}

		if (!LoadEditorTextures())
		{
			VORTEK_ERROR("Failed to load the editor textures!");
			return false;
		}

		if (!CreateDisplays())
		{
			VORTEK_ERROR("Failed to create displays.");
			return false;
		}

		if (!mainRegistry.GetAssetManager().CreateDefaultFonts())
		{
			VORTEK_ERROR("Failed to create default fonts");
			return false;
		}

		auto pEditorFramebuffers = std::make_shared<EditorFramebuffers>();

		if (!pEditorFramebuffers)
		{
			VORTEK_ERROR("Failed to create editor frame buffers!");
			return false;
		}

		if (!mainRegistry.AddToContext<std::shared_ptr<EditorFramebuffers>>(pEditorFramebuffers))
		{
			VORTEK_ERROR("Failed add the editor frame buffers to registry context!");
			return false;
		}

		pEditorFramebuffers->mapFramebuffers.emplace(FramebufferType::SCENE,
			std::make_shared<VORTEK_RENDERING::Framebuffer>(640, 480, false));

		pEditorFramebuffers->mapFramebuffers.emplace(FramebufferType::TILEMAP,
			std::make_shared<VORTEK_RENDERING::Framebuffer>(640, 480, false));

		if (!mainRegistry.AddToContext<std::shared_ptr<GridSystem>>(std::make_shared<GridSystem>()))
		{
			VORTEK_ERROR("Failed add the grid system registry context!");
			return false;
		}

		// Register Meta Functions
		RegisterEditorMetaFunctions();
		VORTEK_CORE::CoreEngineData::RegisterMetaFunctions();

		// TEST SCENES -- TODO: Remove These after testing
		SCENE_MANAGER().AddScene("DefaultScene");
		SCENE_MANAGER().AddScene("NewScene");

		return true;
	}

	bool Application::LoadShaders()
	{
		auto& mainRegistry = MAIN_REGISTRY();
		auto& assetManager = mainRegistry.GetAssetManager();

		if (!assetManager.AddShader("basic", "assets/shaders/basicShader.vert", "assets/shaders/basicShader.frag"))
		{
			VORTEK_ERROR("Failed to add the basic shader to the asset manager");
			return false;
		}

		if (!assetManager.AddShader("color", "assets/shaders/colorShader.vert", "assets/shaders/colorShader.frag"))
		{
			VORTEK_ERROR("Failed to add the color shader to the asset manager");
			return false;
		}

		if (!assetManager.AddShader("circle", "assets/shaders/circleShader.vert", "assets/shaders/circleShader.frag"))
		{
			VORTEK_ERROR("Failed to add the color shader to the asset manager");
			return false;
		}

		if (!assetManager.AddShader("font", "assets/shaders/fontShader.vert", "assets/shaders/fontShader.frag"))
		{
			VORTEK_ERROR("Failed to add the font shader to the asset manager");
			return false;
		}

		return true;
	}

	bool Application::LoadEditorTextures()
	{
		auto& mainRegistry = MAIN_REGISTRY();
		auto& assetManager = mainRegistry.GetAssetManager();

		if (!assetManager.AddTextureFromMemory(
			"play_button", play_button, sizeof(play_button) / sizeof(play_button[0])))
		{
			VORTEK_ERROR("Failed to load texture [play_button] from memory.");
			return false;
		}

		assetManager.GetTexture("play_button")->SetIsEditorTexture(true);

		if (!assetManager.AddTextureFromMemory(
			"stop_button", stop_button, sizeof(stop_button) / sizeof(stop_button[0])))
		{
			VORTEK_ERROR("Failed to load texture [stop_button] from memory.");
			return false;
		}

		assetManager.GetTexture("stop_button")->SetIsEditorTexture(true);

		if (!assetManager.AddTextureFromMemory(
			"music_icon", music_icon, sizeof(music_icon) / sizeof(music_icon[0])))
		{
			VORTEK_ERROR("Failed to load texture [music_icon] from memory.");
			return false;
		}

		assetManager.GetTexture("music_icon")->SetIsEditorTexture(true);

		if (!assetManager.AddTextureFromMemory(
			"scene_icon", scene_icon, sizeof(scene_icon) / sizeof(scene_icon[0])))
		{
			VORTEK_ERROR("Failed to load texture [scene_icon] from memory.");
			return false;
		}

		assetManager.GetTexture("scene_icon")->SetIsEditorTexture(true);

		// ====== Gizmo Textures Start ======

		if (!assetManager.AddTextureFromMemory("x_axis_translate", x_axis_arrow, x_axis_arrow_size))
		{
			VORTEK_ERROR("Failed to load texture [x_axis_translate] from memory.");
			return false;
		}

		assetManager.GetTexture("x_axis_translate")->SetIsEditorTexture(true);

		if (!assetManager.AddTextureFromMemory("y_axis_translate", y_axis_arrow, y_axis_arrow_size))
		{
			VORTEK_ERROR("Failed to load texture [y_axis_translate] from memory.");
			return false;
		}

		assetManager.GetTexture("y_axis_translate")->SetIsEditorTexture(true);

		if (!assetManager.AddTextureFromMemory("x_axis_scale", x_axis_scale, x_axis_scale_size))
		{
			VORTEK_ERROR("Failed to load texture [x_axis_scale] from memory.");
			return false;
		}

		assetManager.GetTexture("x_axis_scale")->SetIsEditorTexture(true);

		if (!assetManager.AddTextureFromMemory("y_axis_scale", y_axis_scale, y_axis_scale_size))
		{
			VORTEK_ERROR("Failed to load texture [y_axis_scale] from memory.");
			return false;
		}

		assetManager.GetTexture("y_axis_scale")->SetIsEditorTexture(true);

		if (!assetManager.AddTextureFromMemory("rotate_tool", rotate_tool, rotate_tool_size))
		{
			VORTEK_ERROR("Failed to load texture [rotate_tool] from memory.");
			return false;
		}

		assetManager.GetTexture("rotate_tool")->SetIsEditorTexture(true);

		// ====== Gizmo Textures End   ======

		return true;
	}

	void Application::ProcessEvents()
	{
		auto& inputManager = VORTEK_CORE::InputManager::GetInstance();
		auto& keyboard = inputManager.GetKeyboard();
		auto& mouse = inputManager.GetMouse();

		// Process Events
		while (SDL_PollEvent(&m_Event))
		{
			ImGui_ImplSDL2_ProcessEvent(&m_Event);

			switch (m_Event.type)
			{
			case SDL_QUIT: m_bIsRunning = false; break;
			case SDL_KEYDOWN: keyboard.OnKeyPressed(m_Event.key.keysym.sym); break;
			case SDL_KEYUP: keyboard.OnKeyReleased(m_Event.key.keysym.sym); break;
			case SDL_MOUSEBUTTONDOWN: mouse.OnBtnPressed(m_Event.button.button); break;
			case SDL_MOUSEBUTTONUP: mouse.OnBtnReleased(m_Event.button.button); break;
			case SDL_MOUSEWHEEL:
				mouse.SetMouseWheelX(m_Event.wheel.x);
				mouse.SetMouseWheelY(m_Event.wheel.y);
				break;
			case SDL_MOUSEMOTION: mouse.SetMouseMoving(true); break;
			case SDL_CONTROLLERBUTTONDOWN: inputManager.GamepadBtnPressed(m_Event); break;
			case SDL_CONTROLLERBUTTONUP: inputManager.GamepadBtnReleased(m_Event); break;
			case SDL_CONTROLLERDEVICEADDED: inputManager.AddGamepad(m_Event.jdevice.which); break;
			case SDL_CONTROLLERDEVICEREMOVED: inputManager.RemoveGamepad(m_Event.jdevice.which); break;
			case SDL_JOYAXISMOTION: inputManager.GamepadAxisValues(m_Event); break;
			case SDL_JOYHATMOTION: inputManager.GamepadHatValues(m_Event); break;
			case SDL_WINDOWEVENT: {
				switch (m_Event.window.event)
				{
				case SDL_WINDOWEVENT_SIZE_CHANGED: m_pWindow->SetSize(m_Event.window.data1, m_Event.window.data2); break;
				default: break;
				}
				break;
			}
			default: break;
			}
		}
	}

	void Application::Update()
	{
		auto& engineData = VORTEK_CORE::CoreEngineData::GetInstance();
		engineData.UpdateDeltaTime();

		auto& mainRegistry = MAIN_REGISTRY();
		auto& displayHolder = mainRegistry.GetContext<std::shared_ptr<DisplayHolder>>();

		for (const auto& pDisplay : displayHolder->displays)
			pDisplay->Update();

		// Update inputs
		auto& inputManager = VORTEK_CORE::InputManager::GetInstance();
		auto& keyboard = inputManager.GetKeyboard();
		keyboard.Update();
		auto& mouse = inputManager.GetMouse();
		mouse.Update();

		inputManager.UpdateGamepads();
	}

	void Application::Render()
	{
		Begin();
		RenderImGui();
		End();

		SDL_GL_SwapWindow(m_pWindow->GetWindow().get());
	}

	void Application::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}

	void Application::CleanUp()
	{
		SDL_Quit();
	}

	bool Application::CreateDisplays()
	{
		auto& mainRegistry = MAIN_REGISTRY();

		auto pDisplayHolder = std::make_shared<DisplayHolder>();

		if (!mainRegistry.AddToContext<std::shared_ptr<DisplayHolder>>(pDisplayHolder))
		{
			VORTEK_ERROR("Failed to add the display holder to the main registry.");
			return false;
		}

		auto pMenuDisplay = std::make_unique<MenuDisplay>();
		if (!pMenuDisplay)
		{
			VORTEK_ERROR("Failed to Create Menu Display!");
			return false;
		}

		auto pSceneDisplay = std::make_unique<SceneDisplay>();
		if (!pSceneDisplay)
		{
			VORTEK_ERROR("Failed to Create Scene Display!");
			return false;
		}

		auto pSceneHierarchyDisplay = std::make_unique<SceneHierarchyDisplay>();
		if (!pSceneHierarchyDisplay)
		{
			VORTEK_ERROR("Failed to Create pSceneHierarchyDisplay !");
			return false;
		}

		auto pLogDisplay = std::make_unique<LogDisplay>();
		if (!pLogDisplay)
		{
			VORTEK_ERROR("Failed to Create Log Display!");
			return false;
		}

		auto pTilesetDisplay = std::make_unique<TilesetDisplay>();
		if (!pTilesetDisplay)
		{
			VORTEK_ERROR("Failed to Create TilesetDisplay!");
			return false;
		}

		auto pTilemapDisplay = std::make_unique<TilemapDisplay>();
		if (!pTilemapDisplay)
		{
			VORTEK_ERROR("Failed to Create TilemapDisplay!");
			return false;
		}

		auto pTileDetailsDisplay = std::make_unique<TileDetailsDisplay>();
		if (!pTileDetailsDisplay)
		{
			VORTEK_ERROR("Failed to Create TileDetailsDisplay!");
			return false;
		}

		auto pAssetDisplay = std::make_unique<AssetDisplay>();
		if (!pAssetDisplay)
		{
			VORTEK_ERROR("Failed to Create AssetDisplay!");
			return false;
		}

		// auto pEditorStylesDisplay = std::make_unique<EditorStyleToolDisplay>();
	// if ( !pEditorStylesDisplay )
	//{
	//	VORTEK_ERROR( "Failed to Create Editor Styles Display!" );
	//	return false;
	// }

		pDisplayHolder->displays.push_back(std::move(pMenuDisplay));
		pDisplayHolder->displays.push_back(std::move(pSceneDisplay));
		pDisplayHolder->displays.push_back(std::move(pSceneHierarchyDisplay));
		pDisplayHolder->displays.push_back(std::move(pLogDisplay));
		pDisplayHolder->displays.push_back(std::move(pTileDetailsDisplay));
		pDisplayHolder->displays.push_back(std::move(pTilesetDisplay));
		pDisplayHolder->displays.push_back(std::move(pTilemapDisplay));
		pDisplayHolder->displays.push_back(std::move(pAssetDisplay));
		// pDisplayHolder->displays.push_back( std::move( pEditorStylesDisplay ) );

		return true;
	}

	bool Application::InitImGui()
	{
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

		io.FontDefault = io.Fonts->AddFontFromFileTTF("./assets/fonts/opensans/OpenSans-Regular.ttf", 18.0f);

		float baseFontSize = 16.0f;
		float iconFontSize = baseFontSize * 2.0f / 3.0f;

		// merge in icons from Font Awesome
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;
		icons_config.GlyphMinAdvanceX = iconFontSize;
		icons_config.GlyphOffset = ImVec2{ 0.f, 2.f };
		io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, baseFontSize, &icons_config, icons_ranges);

		ImGui::StyleColorsDark();
		SetDarkThemeColors();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		if (!ImGui_ImplSDL2_InitForOpenGL(m_pWindow->GetWindow().get(), m_pWindow->GetGLContext()))
		{
			VORTEK_ERROR("Failed to initialize ImGui SDL2 for OpenGL!");
			return false;
		}

		if (!ImGui_ImplOpenGL3_Init(glslVersion))
		{
			VORTEK_ERROR("Failed to initialize ImGui OpenGL3!");
			return false;
		}

		ImGui::InitDefaultStyles();

		return true;
	}

	void Application::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
	}

	void Application::End()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			SDL_GLContext backupContext = SDL_GL_GetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();

			SDL_GL_MakeCurrent(m_pWindow->GetWindow().get(), backupContext);
		}
	}

	void Application::RenderImGui()
	{
		const auto dockSpaceId = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
		if (static auto firstTime = true; firstTime) [[unlikely]]
		{
			firstTime = false;

			ImGui::DockBuilderRemoveNode(dockSpaceId);
			ImGui::DockBuilderAddNode(dockSpaceId);

			auto centerNodeId = dockSpaceId;

			const auto leftNodeId = ImGui::DockBuilderSplitNode(centerNodeId, ImGuiDir_Left, 0.2f, nullptr, &centerNodeId);

			auto RightNodeId = ImGui::DockBuilderSplitNode(centerNodeId, ImGuiDir_Right, 0.3f, nullptr, &centerNodeId);

			const auto LogNodeId = ImGui::DockBuilderSplitNode(centerNodeId, ImGuiDir_Down, 0.25f, nullptr, &centerNodeId);

			auto TileLayerId = ImGui::DockBuilderSplitNode(RightNodeId, ImGuiDir_Down, 0.4f, nullptr, &RightNodeId);

			ImGui::DockBuilderDockWindow("Object Details", RightNodeId);
			ImGui::DockBuilderDockWindow("Tileset", RightNodeId);
			ImGui::DockBuilderDockWindow("Tile Details", RightNodeId);
			ImGui::DockBuilderDockWindow("Tile Layers", TileLayerId);
			ImGui::DockBuilderDockWindow("Scene Hierarchy", leftNodeId);
			ImGui::DockBuilderDockWindow("Scene", centerNodeId);
			ImGui::DockBuilderDockWindow("Tilemap Editor", centerNodeId);
			ImGui::DockBuilderDockWindow("Assets", LogNodeId);
			ImGui::DockBuilderDockWindow("Logs", LogNodeId);

			ImGui::DockBuilderFinish(dockSpaceId);
		}

		auto& mainRegistry = MAIN_REGISTRY();
		auto& pDisplayHolder = mainRegistry.GetContext<std::shared_ptr<DisplayHolder>>();

		for (const auto& pDisplay : pDisplayHolder->displays)
		{
			pDisplay->Draw();
		}

		//ImGui::ShowDemoWindow();
	}

	void Application::RegisterEditorMetaFunctions()
	{
		DrawComponentsUtil::RegisterUIComponent<VORTEK_CORE::ECS::Identification>();
		DrawComponentsUtil::RegisterUIComponent<VORTEK_CORE::ECS::TransformComponent>();
		DrawComponentsUtil::RegisterUIComponent<VORTEK_CORE::ECS::SpriteComponent>();
		DrawComponentsUtil::RegisterUIComponent<VORTEK_CORE::ECS::AnimationComponent>();
		DrawComponentsUtil::RegisterUIComponent<VORTEK_CORE::ECS::PhysicsComponent>();
		DrawComponentsUtil::RegisterUIComponent<VORTEK_CORE::ECS::TextComponent>();
		DrawComponentsUtil::RegisterUIComponent<VORTEK_CORE::ECS::RigidBodyComponent>();
		DrawComponentsUtil::RegisterUIComponent<VORTEK_CORE::ECS::BoxColliderComponent>();
		DrawComponentsUtil::RegisterUIComponent<VORTEK_CORE::ECS::CircleColliderComponent>();
	}

	Application::Application()
		: m_pWindow{ nullptr }
		, m_Event{}
		, m_bIsRunning{ true }
	{
	}

	Application& Application::GetInstance()
	{
		static Application app{};
		return app;
	}

	void Application::Run()
	{
		if (!Initialize())
		{
			VORTEK_ERROR("Initialization Failed!");
			return;
		}

		while (m_bIsRunning)
		{
			ProcessEvents();
			Update();
			Render();
		}

		CleanUp();
	}
} // namespace VORTEK_EDITOR
