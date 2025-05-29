#include "Application.h"
#include <glad/glad.h>
#include <Rendering/Core/Renderer.h>

#include <Logger/Logger.h>
#include <Core/ECS/MainRegistry.h>

#include <Core/Resources/AssetManager.h>
#include <Core/CoreUtilities/CoreEngineData.h>

#include <Core/Scripting/InputManager.h>
#include <Core/CoreUtilities/EngineShaders.h>

#include <Windowing/Inputs/Keyboard.h>
#include <Windowing/Inputs/Mouse.h>
#include <Windowing/Inputs/Gamepad.h>
#include <Windowing/Window/Window.h>

#include "editor/displays/MenuDisplay.h"
#include "editor/displays/AssetDisplay.h"
#include "editor/displays/SceneDisplay.h"
#include "editor/displays/SceneHierarchyDisplay.h"
#include "editor/displays/TileDetailsDisplay.h"
#include "editor/displays/TilesetDisplay.h"
#include "editor/displays/TilemapDisplay.h"
#include "editor/displays/LogDisplay.h"
#include "editor/displays/EditorStyleToolDisplay.h"
#include "editor/displays/ContentDisplay.h"

#include "editor/utilities/editor_textures.h"
#include "editor/utilities/EditorFramebuffers.h"
#include "editor/utilities/DrawComponentUtils.h"
#include "editor/utilities/SaveProject.h"
#include "editor/systems/GridSystem.h"

#include "editor/events/EditorEventTypes.h"
#include "Core/Events/EventDispatcher.h"

#include "editor/hub/Hub.h"

// IMGUI
// ===================================
#include <imgui_internal.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>
#include <SDL_opengl.h>
#include "editor/utilities/imgui/Gui.h"
// ===================================

namespace VORTEK_EDITOR
{
	bool Application::Initialize()
	{
		VORTEK_INIT_LOGS(false, true);
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

		// Create the Window
		m_pWindow = std::make_unique<VORTEK_WINDOWING::Window>(
			"Vortek Engine", 800, 600, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, true, SDL_WINDOW_OPENGL);

		/*
		 * SDL Hack - If we create the window as borderless, we lose the icon in the title bar.
		 * Therefore, after testing I found that if we create the window with a border, then hide
		 * the border. When we show the border later on, the icon will be there.
		 */
		SDL_SetWindowBordered(m_pWindow->GetWindow().get(), SDL_FALSE);

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

		if (!Gui::InitImGui(m_pWindow.get()))
		{
			VORTEK_ERROR("Failed to initialize ImGui!");
			return false;
		}

		if (!LoadEditorTextures())
		{
			VORTEK_ERROR("Failed to load the editor textures!");
			return false;
		}

		mainRegistry.AddToContext<std::shared_ptr<SaveProject>>(std::make_shared<SaveProject>());
		m_pHub = std::make_unique<Hub>(*m_pWindow);

		return true;
	}

	bool Application::InitApp()
	{
		if (!LoadShaders())
		{
			VORTEK_ERROR("Failed to load the shaders!");
			return false;
		}

		if (!CreateDisplays())
		{
			VORTEK_ERROR("Failed to create displays.");
			return false;
		}

		if (!ASSET_MANAGER().CreateDefaultFonts())
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

		if (!MAIN_REGISTRY().AddToContext<std::shared_ptr<EditorFramebuffers>>(pEditorFramebuffers))
		{
			VORTEK_ERROR("Failed add the editor frame buffers to registry context!");
			return false;
		}

		pEditorFramebuffers->mapFramebuffers.emplace(FramebufferType::SCENE,
			std::make_shared<VORTEK_RENDERING::Framebuffer>(640, 480, false));

		pEditorFramebuffers->mapFramebuffers.emplace(FramebufferType::TILEMAP,
			std::make_shared<VORTEK_RENDERING::Framebuffer>(640, 480, false));

		if (!MAIN_REGISTRY().AddToContext<std::shared_ptr<GridSystem>>(std::make_shared<GridSystem>()))
		{
			VORTEK_ERROR("Failed add the grid system registry context!");
			return false;
		}

		ADD_EVENT_HANDLER(VORTEK_EDITOR::Events::CloseEditorEvent, &Application::OnCloseEditor, *this);

		// Register Meta Functions
		RegisterEditorMetaFunctions();
		VORTEK_CORE::CoreEngineData::RegisterMetaFunctions();

		return true;
	}

	bool Application::LoadShaders()
	{
		auto& mainRegistry = MAIN_REGISTRY();
		auto& assetManager = mainRegistry.GetAssetManager();

		if (!assetManager.AddShaderFromMemory(
			"basic", VORTEK_CORE::Shaders::basicShaderVert, VORTEK_CORE::Shaders::basicShaderFrag))
		{
			VORTEK_ERROR("Failed to add the basic shader to the asset manager");
			return false;
		}

		if (!assetManager.AddShaderFromMemory(
			"color", VORTEK_CORE::Shaders::colorShaderVert, VORTEK_CORE::Shaders::colorShaderFrag))
		{
			VORTEK_ERROR("Failed to add the color shader to the asset manager");
			return false;
		}

		if (!assetManager.AddShaderFromMemory(
			"circle", VORTEK_CORE::Shaders::circleShaderVert, VORTEK_CORE::Shaders::circleShaderFrag))
		{
			VORTEK_ERROR("Failed to add the color shader to the asset manager");
			return false;
		}

		if (!assetManager.AddShaderFromMemory(
			"font", VORTEK_CORE::Shaders::fontShaderVert, VORTEK_CORE::Shaders::fontShaderFrag))
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

			// ====== Content Display Textures Start ======
		if (!assetManager.AddTextureFromMemory(
			"S2D_file_icon", file_icon, sizeof(file_icon) / sizeof(file_icon[0])))
		{
			VORTEK_ERROR("Failed to load texture [file_icon] from memory.");
			return false;
		}

		assetManager.GetTexture("S2D_file_icon")->SetIsEditorTexture(true);

		if (!assetManager.AddTextureFromMemory(
			"S2D_music_icon", music_icon, sizeof(music_icon) / sizeof(music_icon[0])))
		{
			VORTEK_ERROR("Failed to load texture [music_icon] from memory.");
			return false;
		}

		assetManager.GetTexture("S2D_music_icon")->SetIsEditorTexture(true);

		if (!assetManager.AddTextureFromMemory(
			"S2D_folder_icon", folder_icon, sizeof(folder_icon) / sizeof(folder_icon[0])))
		{
			VORTEK_ERROR("Failed to load texture [folder_icon] from memory.");
			return false;
		}

		assetManager.GetTexture("S2D_folder_icon")->SetIsEditorTexture(true);

		if (!assetManager.AddTextureFromMemory(
			"S2D_image_icon", image_icon, sizeof(image_icon) / sizeof(image_icon[0])))
		{
			VORTEK_ERROR("Failed to load texture [image_icon] from memory.");
			return false;
		}

		assetManager.GetTexture("S2D_image_icon")->SetIsEditorTexture(true);
		// ====== Content Display Textures End   ======

		if (!assetManager.AddTextureFromMemory("vortek_logo", vortek_logo, vortek_logo_size))
		{
			VORTEK_ERROR("Failed to load texture [vortek_logo] from memory.");
			return false;
		}

		assetManager.GetTexture("vortek_logo")->SetIsEditorTexture(true);

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
			case SDL_DROPFILE: {
				EVENT_DISPATCHER().EmitEvent(VORTEK_EDITOR::Events::FileEvent{
					.eAction = Events::EFileAction::FileDropped, .sFilepath = std::string{ m_Event.drop.file } });

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
		Gui::Begin();
		RenderDisplays();
		Gui::End(m_pWindow.get());

		SDL_GL_SwapWindow(m_pWindow->GetWindow().get());
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

		auto pContentDisplay = std::make_unique<ContentDisplay>();
		if (!pContentDisplay)
		{
			VORTEK_ERROR("Failed to Create Content Display!");
			return false;
		}

		pDisplayHolder->displays.push_back(std::move(pMenuDisplay));
		pDisplayHolder->displays.push_back(std::move(pSceneDisplay));
		pDisplayHolder->displays.push_back(std::move(pSceneHierarchyDisplay));
		pDisplayHolder->displays.push_back(std::move(pLogDisplay));
		pDisplayHolder->displays.push_back(std::move(pTileDetailsDisplay));
		pDisplayHolder->displays.push_back(std::move(pTilesetDisplay));
		pDisplayHolder->displays.push_back(std::move(pTilemapDisplay));
		pDisplayHolder->displays.push_back(std::move(pAssetDisplay));
		pDisplayHolder->displays.push_back(std::move(pContentDisplay));

		return true;
	}

	void Application::InitDisplays()
	{
		const auto dockSpaceId = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

		if (static auto firstTime = true; firstTime) [[unlikely]]
		{
			firstTime = false;

			ImGui::DockBuilderRemoveNode(dockSpaceId);
			ImGui::DockBuilderAddNode(dockSpaceId);

			auto centerNodeId = dockSpaceId;
			const auto leftNodeId =
				ImGui::DockBuilderSplitNode(centerNodeId, ImGuiDir_Left, 0.2f, nullptr, &centerNodeId);

			auto RightNodeId = ImGui::DockBuilderSplitNode(centerNodeId, ImGuiDir_Right, 0.3f, nullptr, &centerNodeId);

			const auto LogNodeId =
				ImGui::DockBuilderSplitNode(centerNodeId, ImGuiDir_Down, 0.25f, nullptr, &centerNodeId);

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
			ImGui::DockBuilderDockWindow("Content Browser", LogNodeId);

			ImGui::DockBuilderFinish(dockSpaceId);
		}
	}

	void Application::RenderDisplays()
	{
		InitDisplays();

		auto& mainRegistry = MAIN_REGISTRY();
		auto& pDisplayHolder = mainRegistry.GetContext<std::shared_ptr<DisplayHolder>>();

		for (const auto& pDisplay : pDisplayHolder->displays)
		{
			pDisplay->Draw();
		}
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

	void Application::OnCloseEditor(VORTEK_EDITOR::Events::CloseEditorEvent& close)
	{
		// TODO: Maybe add a check for save??
		m_bIsRunning = false;
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

		if (!m_pHub || !m_pHub->Run())
		{
			// If it makes it here, the app is closing.
			return;
		}

		InitApp();

		while (m_bIsRunning)
		{
			ProcessEvents();
			Update();
			Render();
		}

		CleanUp();
	}
} // namespace VORTEK_EDITOR