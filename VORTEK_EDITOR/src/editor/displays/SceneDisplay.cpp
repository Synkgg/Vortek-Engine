#include "editor/displays/SceneDisplay.h"
#include "Rendering/Buffers/Framebuffer.h"
#include "Rendering/Core/Camera2D.h"
#include "Rendering/Core/Renderer.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/Systems/AnimationSystem.h"
#include "Core/Systems/RenderSystem.h"
#include "Core/Systems/RenderUISystem.h"
#include "Core/Systems/RenderShapeSystem.h"
#include "Core/Systems/PhysicsSystem.h"
#include "Core/Systems/ScriptingSystem.h"
#include "Core/CoreUtilities/CoreEngineData.h"

#include "Logger/Logger.h"
#include "Logger/CrashLogger.h"

#include "Core/Scripting/CrashLoggerTestBindings.h"
#include "Core/Scripting/ScriptingUtilities.h"

#include "Sounds/MusicPlayer/MusicPlayer.h"
#include "Sounds/SoundPlayer/SoundFxPlayer.h"
#include "Physics/Box2DWrappers.h"
#include "Physics/ContactListener.h"
#include "Core/Resources/AssetManager.h"

#include "editor/utilities/EditorFramebuffers.h"
#include "editor/utilities/EditorUtilities.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"

#include "Core/CoreUtilities/ProjectInfo.h"
#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"

#include "editor/scripting/EditorCoreLuaWrappers.h"

#include "Core/Events/EventDispatcher.h"
#include "Core/Events/EngineEventTypes.h"
#include "Windowing/Inputs/Keys.h"

#include <imgui.h>
#include <thread>

using namespace Vortek::Core::Systems;
using namespace Vortek::Core::ECS;
using namespace Vortek::Rendering;
using namespace Vortek::Physics;

constexpr float TARGET_FRAME_TIME_F = 1.f / 60.f;
constexpr double TARGET_FRAME_TIME = 1.0 / 60.0;

namespace Vortek::Editor
{
void SceneDisplay::LoadScene()
{
	auto pCurrentScene = SCENE_MANAGER().GetCurrentSceneObject();
	if ( !pCurrentScene )
		return;

	pCurrentScene->CopySceneToRuntime();
	auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();

	const auto& canvas = pCurrentScene->GetCanvas();
	auto pCamera = runtimeRegistry.AddToContext<std::shared_ptr<Camera2D>>(
		std::make_shared<Camera2D>( canvas.width, canvas.height ) );

	auto pPhysicsWorld = runtimeRegistry.AddToContext<Vortek::Physics::PhysicsWorld>(
		std::make_shared<b2World>( b2Vec2{ 0.f, CORE_GLOBALS().GetGravity() } ) );

	auto pContactListener = runtimeRegistry.AddToContext<std::shared_ptr<Vortek::Physics::ContactListener>>(
		std::make_shared<Vortek::Physics::ContactListener>() );

	pPhysicsWorld->SetContactListener( pContactListener.get() );

	// Add the temporary event dispatcher
	runtimeRegistry.AddToContext<std::shared_ptr<Vortek::Core::Events::EventDispatcher>>(
		std::make_shared<Vortek::Core::Events::EventDispatcher>() );

	// Add necessary systems
	auto scriptSystem =
		runtimeRegistry.AddToContext<std::shared_ptr<ScriptingSystem>>( std::make_shared<ScriptingSystem>() );
	runtimeRegistry.AddToContext<std::shared_ptr<MouseGuiInfo>>( std::make_shared<MouseGuiInfo>() );

	auto lua = runtimeRegistry.AddToContext<std::shared_ptr<sol::state>>( std::make_shared<sol::state>() );

	if ( !lua )
	{
		lua = std::make_shared<sol::state>();
	}

	lua->open_libraries( sol::lib::base,
						 sol::lib::math,
						 sol::lib::os,
						 sol::lib::table,
						 sol::lib::io,
						 sol::lib::string,
						 sol::lib::package );

	Vortek::Core::Systems::ScriptingSystem::RegisterLuaBindings( *lua, runtimeRegistry );
	Vortek::Core::Systems::ScriptingSystem::RegisterLuaFunctions( *lua, runtimeRegistry );
	Vortek::Core::Systems::ScriptingSystem::RegisterLuaSystems( *lua, runtimeRegistry );
	// We need to be able to get the editor events from lua. Pass in the main registry to get the main
	// event dispatcher.
	Vortek::Core::Systems::ScriptingSystem::RegisterLuaEvents( *lua, *MAIN_REGISTRY().GetRegistry() );
	LuaCoreBinder::CreateLuaBind( *lua, runtimeRegistry );

	EditorSceneManager::CreateSceneManagerLuaBind( *lua );

	// We need to initialize all of the physics entities
	auto physicsEntities = runtimeRegistry.GetRegistry().view<PhysicsComponent>();
	for ( auto entity : physicsEntities )
	{
		Entity ent{ &runtimeRegistry, entity };

		bool bBoxCollider{ ent.HasComponent<BoxColliderComponent>() };
		bool bCircleCollider{ ent.HasComponent<CircleColliderComponent>() };

		if ( !bBoxCollider && !bCircleCollider )
		{
			VORTEK_ERROR( "Entity must have a box or circle collider component to initialize physics on it." );
			continue;
		}

		auto& physics = ent.GetComponent<PhysicsComponent>();
		auto& physicsAttributes = physics.GetChangableAttributes();

		if ( bBoxCollider )
		{
			const auto& boxCollider = ent.GetComponent<BoxColliderComponent>();
			physicsAttributes.boxSize = glm::vec2{ boxCollider.width, boxCollider.height };
			physicsAttributes.offset = boxCollider.offset;
		}

		else if ( bCircleCollider )
		{
			const auto& circleCollider = ent.GetComponent<CircleColliderComponent>();
			physicsAttributes.radius = circleCollider.radius;
			physicsAttributes.offset = circleCollider.offset;
		}

		const auto& transform = ent.GetComponent<TransformComponent>();
		physicsAttributes.position = transform.position;
		physicsAttributes.scale = transform.scale;
		physicsAttributes.objectData.entityID = static_cast<std::int32_t>( entity );

		physics.Init( pPhysicsWorld, pCamera->GetWidth(), pCamera->GetHeight() );

		/*
		 * Set Filters/Masks/Group Index
		 */
		if ( physics.UseFilters() ) // Right now filters are disabled, since there is no way to set this from the editor
		{
			physics.SetFilterCategory();
			physics.SetFilterMask();

			// Should the group index be set based on the sprite layer?
			physics.SetGroupIndex();
		}
	}

	// Get the main script path
	auto mainScript =
		runtimeRegistry.AddToContext<MainScriptPtr>( std::make_shared<Vortek::Core::Scripting::MainScriptFunctions>() );
	auto& pProjectInfo = MAIN_REGISTRY().GetContext<Vortek::Core::ProjectInfoPtr>();
	if ( !scriptSystem->LoadMainScript( *pProjectInfo, runtimeRegistry, *lua ) )
	{
		VORTEK_ERROR( "Failed to load the main lua script!" );
		return;
	}

	if ( !mainScript->init.valid() )
	{
		VORTEK_ERROR( "Failed to initialize main script. init() function is invalid." );
		return;
	}

	mainScript->init();

	// Setup Crash Tests
	Vortek::Core::Scripting::CrashLoggerTests::CreateLuaBind( *lua );

	// Set the lua state for the crash logger.
	// This is used to log the lua stack trace in case of a crash
	VORTEK_CRASH_LOGGER().SetLuaState( lua->lua_state() );

	m_bSceneLoaded = true;
	m_bPlayScene = true;
}

void SceneDisplay::UnloadScene()
{
	EVENT_DISPATCHER().ClearHandlers<Vortek::Core::Events::GamepadConnectEvent>();
	EVENT_DISPATCHER().ClearHandlers<Vortek::Core::Events::LuaEvent>();

	m_bPlayScene = false;
	m_bSceneLoaded = false;
	auto pCurrentScene = SCENE_MANAGER().GetCurrentSceneObject();

	VORTEK_ASSERT( pCurrentScene && "Current Scene must be Valid." );

	auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();

	runtimeRegistry.ClearRegistry();
	runtimeRegistry.RemoveContext<std::shared_ptr<Camera2D>>();
	runtimeRegistry.RemoveContext<Vortek::Physics::PhysicsWorld>();
	runtimeRegistry.RemoveContext<std::shared_ptr<Vortek::Physics::ContactListener>>();
	runtimeRegistry.RemoveContext<std::shared_ptr<ScriptingSystem>>();
	runtimeRegistry.RemoveContext<std::shared_ptr<Vortek::Core::Events::EventDispatcher>>();
	runtimeRegistry.RemoveContext<MainScriptPtr>();
	runtimeRegistry.RemoveContext<std::shared_ptr<sol::state>>();

	auto& mainRegistry = MAIN_REGISTRY();
	mainRegistry.GetMusicPlayer().Stop();
	mainRegistry.GetSoundPlayer().Stop( -1 );
}

void SceneDisplay::RenderScene() const
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& editorFramebuffers = mainRegistry.GetContext<std::shared_ptr<EditorFramebuffers>>();
	auto& renderer = mainRegistry.GetContext<std::shared_ptr<Vortek::Rendering::Renderer>>();

	auto& renderSystem = mainRegistry.GetRenderSystem();
	auto& renderUISystem = mainRegistry.GetRenderUISystem();
	auto& renderShapeSystem = mainRegistry.GetRenderShapeSystem();

	const auto& fb = editorFramebuffers->mapFramebuffers[ FramebufferType::SCENE ];

	fb->Bind();
	renderer->SetViewport( 0, 0, fb->Width(), fb->Height() );
	renderer->SetClearColor( 0.f, 0.f, 0.f, 1.f );
	renderer->ClearBuffers( true, true, false );

	auto pCurrentScene = SCENE_MANAGER().GetCurrentSceneObject();

	if ( pCurrentScene && m_bPlayScene )
	{
		auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();
		auto& camera = runtimeRegistry.GetContext<std::shared_ptr<Camera2D>>();
		renderSystem.Update( runtimeRegistry, *camera );

		if ( CORE_GLOBALS().RenderCollidersEnabled() )
		{
			renderShapeSystem.Update( runtimeRegistry, *camera );
		}

		renderUISystem.Update( runtimeRegistry );

		// Add Render Script stuff after everything???
		auto& scriptSystem = runtimeRegistry.GetContext<std::shared_ptr<Vortek::Core::Systems::ScriptingSystem>>();
		scriptSystem->Render( runtimeRegistry );
	}

	fb->Unbind();
	fb->CheckResize();
}

void SceneDisplay::HandleKeyEvent( const Vortek::Core::Events::KeyEvent keyEvent )
{
	if ( m_bSceneLoaded )
	{
		if ( keyEvent.eType == Vortek::Core::Events::EKeyEventType::Released )
		{
			if ( keyEvent.key == VORTEK_KEY_ESCAPE )
			{
				UnloadScene();
			}
		}
	}

	// Send double dispatch events to the scene dispatcher.
	auto pCurrentScene = SCENE_MANAGER().GetCurrentSceneObject();
	if ( !pCurrentScene )
		return;

	auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();

	if ( auto* pEventDispatcher =
			 runtimeRegistry.TryGetContext<std::shared_ptr<Vortek::Core::Events::EventDispatcher>>() )
	{
		if ( !pEventDispatcher->get()->HasHandlers<Vortek::Core::Events::KeyEvent>() )
			return;

		pEventDispatcher->get()->EmitEvent( keyEvent );
	}
}

void SceneDisplay::DrawToolbar()
{
	const float toolbarHeight = 58.0f;
	const float buttonSize = 42.0f;
	const float spacing = 16.0f;

	// ===== FPS BUFFER =====
	static std::array<float, 120> frameTimes{};
	static int frameIndex = 0;
	static bool bufferFilled = false;

	float avgFPS = 0.0f;
	float avgMS = 0.0f;

	if ( m_bSceneLoaded )
	{
		float dt = CORE_GLOBALS().GetDeltaTime();
		frameTimes[ frameIndex ] = dt;

		frameIndex++;
		if ( frameIndex >= frameTimes.size() )
		{
			frameIndex = 0;
			bufferFilled = true;
		}

		int count = bufferFilled ? frameTimes.size() : frameIndex;

		float total = 0.0f;
		for ( int i = 0; i < count; i++ )
			total += frameTimes[ i ];

		float avgDt = count > 0 ? total / count : 0.0f;

		avgFPS = avgDt > 0.0f ? ( 1.0f / avgDt ) : 0.0f;
		avgMS = avgDt * 1000.0f;
	}
	else
	{
		frameTimes.fill( 0.0f );
		frameIndex = 0;
		bufferFilled = false;
	}

	// ===============================
	ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, { 15.f, 10.f } );
	ImGui::PushStyleColor( ImGuiCol_ChildBg, ImVec4( 0.07f, 0.07f, 0.08f, 1.f ) );

	ImGui::BeginChild( "##SceneToolbar",
					   ImVec2( 0, toolbarHeight ),
					   false,
					   ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );

	float windowWidth = ImGui::GetWindowWidth();
	float contentHeight = ImGui::GetContentRegionAvail().y;

	// Vertical center helper
	float centerY = ( toolbarHeight - buttonSize ) * 0.5f;

	// ================= LEFT =================
	if ( m_bSceneLoaded )
	{
		ImGui::SetCursorPosY( ( toolbarHeight - ImGui::GetTextLineHeight() ) * 0.5f );
		ImGui::TextColored( ImVec4( 0.25f, 0.85f, 0.35f, 1.f ), ICON_FA_PLAY "  PLAY TESTING" );
	}

	// ================= CENTER =================
	float totalWidth = ( buttonSize * 2.0f ) + spacing;
	float centerStart = ( windowWidth - totalWidth ) * 0.5f;

	ImGui::SetCursorPosX( centerStart );
	ImGui::SetCursorPosY( centerY );

	ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 10.f );
	ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, { 10.f, 10.f } );

	// PLAY
	if ( !m_bSceneLoaded )
	{
		ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.2f, 0.6f, 0.2f, 1.f ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.3f, 0.8f, 0.3f, 1.f ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.2f, 0.6f, 0.2f, 1.f ) );

		if ( ImGui::Button( ICON_FA_PLAY, { buttonSize, buttonSize } ) )
			LoadScene();

		ImGui::PopStyleColor( 3 );
	}
	else
	{
		ImGui::BeginDisabled();
		ImGui::Button( ICON_FA_PLAY, { buttonSize, buttonSize } );
		ImGui::EndDisabled();
	}

	ImGui::SameLine( 0, spacing );

	// STOP
	if ( m_bSceneLoaded )
	{
		ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.65f, 0.2f, 0.2f, 1.f ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.85f, 0.3f, 0.3f, 1.f ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.65f, 0.2f, 0.2f, 1.f ) );

		if ( ImGui::Button( ICON_FA_STOP, { buttonSize, buttonSize } ) )
			UnloadScene();

		ImGui::PopStyleColor( 3 );
	}
	else
	{
		ImGui::BeginDisabled();
		ImGui::Button( ICON_FA_STOP, { buttonSize, buttonSize } );
		ImGui::EndDisabled();
	}

	ImGui::PopStyleVar( 2 );

	// ================= RIGHT =================

	char statsBuffer[ 64 ];
	sprintf_s( statsBuffer, "FPS %3.0f | %6.2f ms", avgFPS, avgMS );

	float textWidth = ImGui::CalcTextSize( statsBuffer ).x;

	ImGui::SetCursorPosX( windowWidth - textWidth - 15.0f ); // respect padding
	ImGui::SetCursorPosY( ( toolbarHeight - ImGui::GetTextLineHeight() ) * 0.5f );

	ImGui::TextUnformatted( statsBuffer );

	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

SceneDisplay::SceneDisplay()
	: m_bPlayScene{ false }
	, m_bWindowActive{ false }
	, m_bSceneLoaded{ false }
{
	ADD_EVENT_HANDLER( Vortek::Core::Events::KeyEvent, &SceneDisplay::HandleKeyEvent, *this );
}

void SceneDisplay::Draw()
{
	static bool pOpen{ true };
	if ( !ImGui::Begin( ICON_FA_IMAGE " Scene", &pOpen ) )
	{
		ImGui::End();
		return;
	}

	DrawToolbar();
	RenderScene();

	if ( ImGui::BeginChild( "##SceneChild",
							ImVec2( 0.f, 0.f ),
							false,
							ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse ) )
	{
		m_bWindowActive = ImGui::IsWindowFocused();

		auto& editorFramebuffers = MAIN_REGISTRY().GetContext<std::shared_ptr<EditorFramebuffers>>();
		const auto& fb = editorFramebuffers->mapFramebuffers[ FramebufferType::SCENE ];

		// 👇 Get exact available region INSIDE the child
		ImVec2 avail = ImGui::GetContentRegionAvail();

		// 👇 Resize BEFORE rendering image
		if ( fb->Width() != (int)avail.x || fb->Height() != (int)avail.y )
			fb->Resize( (int)avail.x, (int)avail.y );

		// Render scene after resize
		RenderScene();

		ImGui::Image( (ImTextureID)(intptr_t)fb->GetTextureID(), avail, ImVec2( 0.f, 1.f ), ImVec2( 1.f, 0.f ) );

		ImGui::EndChild();
	}

	ImGui::End();
}

void SceneDisplay::Update()
{
	if ( !m_bPlayScene )
		return;

	auto pCurrentScene = SCENE_MANAGER().GetCurrentSceneObject();
	if ( !pCurrentScene )
		return;

	auto& runtimeRegistry = pCurrentScene->GetRuntimeRegistry();

	auto& mainRegistry = MAIN_REGISTRY();
	auto& coreGlobals = CORE_GLOBALS();

	double dt = coreGlobals.GetDeltaTime();
	coreGlobals.UpdateDeltaTime();

	// Clamp delta time to the target frame rate
	if ( dt < TARGET_FRAME_TIME )
	{
		std::this_thread::sleep_for( std::chrono::duration<double>( TARGET_FRAME_TIME - dt ) );
	}

	auto& camera = runtimeRegistry.GetContext<std::shared_ptr<Vortek::Rendering::Camera2D>>();
	if ( !camera )
	{
		VORTEK_ERROR( "Failed to get the camera from the registry context!" );
		return;
	}

	camera->Update();

	auto& scriptSystem = runtimeRegistry.GetContext<std::shared_ptr<Vortek::Core::Systems::ScriptingSystem>>();
	scriptSystem->Update( runtimeRegistry );

	if ( coreGlobals.IsPhysicsEnabled() )
	{
		auto& pPhysicsWorld = runtimeRegistry.GetContext<Vortek::Physics::PhysicsWorld>();
		pPhysicsWorld->Step(
			TARGET_FRAME_TIME_F, coreGlobals.GetVelocityIterations(), coreGlobals.GetPositionIterations() );
		pPhysicsWorld->ClearForces();

		auto& dispatch = runtimeRegistry.GetContext<std::shared_ptr<Vortek::Core::Events::EventDispatcher>>();

		// If there are no listeners for contact events, don't emit event
		if ( dispatch->HasHandlers<Vortek::Core::Events::ContactEvent>() )
		{
			if ( auto& pContactListener = runtimeRegistry.GetContext<std::shared_ptr<ContactListener>>() )
			{
				auto pUserDataA = pContactListener->GetUserDataA();
				auto pUserDataB = pContactListener->GetUserDataB();

				// Only emit contact event if both contacts are valid
				if ( pUserDataA && pUserDataB )
				{
					try
					{
						auto ObjectA = std::any_cast<ObjectData>( pUserDataA->userData );
						auto ObjectB = std::any_cast<ObjectData>( pUserDataB->userData );

						dispatch->EmitEvent(
							Vortek::Core::Events::ContactEvent{ .objectA = ObjectA, .objectB = ObjectB } );
					}
					catch ( const std::bad_any_cast& e )
					{
						VORTEK_ERROR( "Failed to cast to object data - {}", e.what() );
					}
				}
			}
		}
	}

	auto& pPhysicsSystem = mainRegistry.GetPhysicsSystem();
	pPhysicsSystem.Update( runtimeRegistry );

	auto& animationSystem = mainRegistry.GetAnimationSystem();
	animationSystem.Update( runtimeRegistry, *camera );

	runtimeRegistry.ClearPendingEntities();
}
} // namespace Vortek::Editor
