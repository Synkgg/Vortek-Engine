#include "Core/Systems/ScriptingSystem.h"
#include "Core/ECS/Components/ScriptComponent.h"
#include "Core/ECS/Entity.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/ECS/ECSUtils.h"

#include "Core/Scripting/GlmLuaBindings.h"
#include "Core/Scripting/InputManager.h"
#include "Core/Scripting/SoundBindings.h"
#include "Core/Scripting/RendererBindings.h"
#include "Core/Scripting/UserDataBindings.h"
#include "Core/Scripting/ContactListenerBind.h"
#include "Core/Scripting/LuaFilesystemBindings.h"
#include "Core/Scripting/ScriptingUtilities.h"

#include "Core/Resources/AssetManager.h"
#include <Logger/Logger.h>
#include <VORTEKUtilities/Timer.h>
#include <VORTEKUtilities/RandomGenerator.h>
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "Core/CoreUtilities/FollowCamera.h"
#include "Core/CoreUtilities/ProjectInfo.h"

#include "Core/States/State.h"
#include "Core/States/StateStack.h"
#include "Core/States/StateMachine.h"

#include "Core/Events/EngineEventTypes.h"
#include "Core/Events/EventDispatcher.h"

#include "Core/Systems/RenderSystem.h"
#include "Core/Systems/RenderUISystem.h"
#include "Core/Systems/AnimationSystem.h"

#include "Core/Character/Character.h"
#include "VORTEKUtilities/HelperUtilities.h"
#include "VORTEKUtilities/Tween.h"

#include "Core/Scene/Scene.h"

#include "Rendering/Essentials/Texture.h"
#include "Rendering/Essentials/Shader.h"
#include "Rendering/Essentials/Font.h"

#include <filesystem>

namespace fs = std::filesystem;

using namespace VORTEK_CORE::ECS;
using namespace VORTEK_RESOURCES;
using namespace VORTEK_CORE::Events;

namespace VORTEK_CORE::Systems
{

ScriptingSystem::ScriptingSystem()
	: m_bMainLoaded{ false }
{
}

bool ScriptingSystem::LoadMainScript( const std::string& sMainLuaFile, VORTEK_CORE::ECS::Registry& registry,
									  sol::state& lua )
{
	std::error_code ec;
	if ( !fs::exists( sMainLuaFile, ec ) )
	{
		VORTEK_ERROR( "Error loading the main lua script: {}", ec.message() );
		return false;
	}

	try
	{
		auto result = lua.safe_script_file( sMainLuaFile );
	}
	catch ( const sol::error& err )
	{
		VORTEK_ERROR( "Error loading the main lua script: {}", err.what() );
		return false;
	}

	sol::table main_lua = lua[ "main" ];
	sol::optional<sol::table> bUpdateExists = main_lua[ 1 ];
	if ( bUpdateExists == sol::nullopt )
	{
		VORTEK_ERROR( "There is no update function in main.lua" );
		return false;
	}

	sol::table update_script = main_lua[ 1 ];
	sol::function update = update_script[ "update" ];

	sol::optional<sol::table> bRenderExists = main_lua[ 2 ];
	if ( bRenderExists == sol::nullopt )
	{
		VORTEK_ERROR( "There is no render function in main.lua" );
		return false;
	}

	sol::table render_script = main_lua[ 2 ];
	sol::function render = render_script[ "render" ];

	VORTEK_CORE::ECS::Entity mainLuaScript{ registry, "main_script", "" };
	mainLuaScript.AddComponent<VORTEK_CORE::ECS::ScriptComponent>(
		VORTEK_CORE::ECS::ScriptComponent{ .update = update, .render = render } );

	m_bMainLoaded = true;
	return true;
}

bool ScriptingSystem::LoadMainScript( VORTEK_CORE::ProjectInfo& projectInfo, VORTEK_CORE::ECS::Registry& registry,
									  sol::state& lua )
{
	auto optScriptListPath = projectInfo.GetScriptListPath();
	VORTEK_ASSERT( optScriptListPath && "Script List Path not setup correctly in project info." );
	auto optContentPath = projectInfo.TryGetFolderPath( VORTEK_CORE::EProjectFolderType::Content );
	VORTEK_ASSERT( optContentPath && "Content Path not setup correctly in project info." );

	// Try to load script list files
	if ( fs::exists( *optScriptListPath ) && fs::exists( *optContentPath ) )
	{
		try
		{
			sol::state scriptLua;
			auto result = scriptLua.safe_script_file( optScriptListPath->string() );
			if ( !result.valid() )
			{
				sol::error err = result;
				throw err;
			}

			sol::optional<sol::table> scriptList = scriptLua[ "ScriptList" ];
			if ( !scriptList )
			{
				VORTEK_ERROR( "Failed to load script list. Missing \"ScriptList\" table." );
				return false;
			}

			for ( const auto& [ _, script ] : *scriptList )
			{
				try
				{
					fs::path scriptPath = *optContentPath / script.as<std::string>();
					auto result = lua.safe_script_file( scriptPath.string() );
					if ( !result.valid() )
					{
						sol::error error = result;
						throw error;
					}
				}
				catch ( const sol::error& error )
				{
					VORTEK_ERROR( "Failed to load script: {}, Error: {}", script.as<std::string>(), error.what() );
					return false;
				}
			}
		}
		catch ( const sol::error& error )
		{
			VORTEK_ERROR( "Error Loading script_list.lua: {}", error.what() );
			return false;
		}
	}

	auto optMainLuaScript = projectInfo.GetMainLuaScriptPath();
	VORTEK_ASSERT( optMainLuaScript && "Main lua script has not been set correctly in project info." );

	return LoadMainScript( optMainLuaScript->string(), registry, lua );
}

void ScriptingSystem::Update( VORTEK_CORE::ECS::Registry& registry )
{
	if ( !m_bMainLoaded )
	{
		VORTEK_ERROR( "Main lua script has not been loaded!" );
		return;
	}

	auto mainScript = FindEntityByTag( registry, "main_script" );
	if ( mainScript == entt::null )
	{
		VORTEK_ERROR( "Failed to run main Update script. Entity does not exist." );
		return;
	}

	Entity scriptEnt{ registry, mainScript };

	if ( auto* pScript = scriptEnt.TryGetComponent<ScriptComponent>() )
	{
		auto error = pScript->update();
		if ( !error.valid() )
		{
			sol::error err = error;
			VORTEK_ERROR( "Error running the Update script: {0}", err.what() );
		}
	}

	if ( auto* pLua = registry.TryGetContext<std::shared_ptr<sol::state>>() )
	{
		( *pLua )->collect_garbage();
	}
}

void ScriptingSystem::Render( VORTEK_CORE::ECS::Registry& registry )
{
	if ( !m_bMainLoaded )
	{
		VORTEK_ERROR( "Main lua script has not been loaded!" );
		return;
	}

	auto mainScript = FindEntityByTag( registry, "main_script" );
	if ( mainScript == entt::null )
	{
		VORTEK_ERROR( "Failed to run main render script. Entity does not exist." );
		return;
	}

	Entity scriptEnt{ registry, mainScript };

	if ( auto* pScript = scriptEnt.TryGetComponent<ScriptComponent>() )
	{
		auto error = pScript->render();
		if ( !error.valid() )
		{
			sol::error err = error;
			VORTEK_ERROR( "Error running the Render script: {0}", err.what() );
		}
	}

	if ( auto* pLua = registry.TryGetContext<std::shared_ptr<sol::state>>() )
	{
		( *pLua )->collect_garbage();
	}
}

auto create_timer = []( sol::state& lua ) {
	using namespace VORTEK_UTIL;
	lua.new_usertype<Timer>( "Timer",
							 sol::call_constructor,
							 sol::constructors<Timer()>(),
							 "start",
							 &Timer::Start,
							 "stop",
							 &Timer::Stop,
							 "pause",
							 &Timer::Pause,
							 "resume",
							 &Timer::Resume,
							 "is_paused",
							 &Timer::IsPaused,
							 "is_running",
							 &Timer::IsRunning,
							 "elapsed_ms",
							 &Timer::ElapsedMS,
							 "elapsed_sec",
							 &Timer::ElapsedSec,
							 "restart",
							 []( Timer& timer ) {
								 if ( timer.IsRunning() )
									 timer.Stop();

								 timer.Start();
							 } );
};

auto createTweenLuaBind = []( sol::state& lua ) {
	using namespace VORTEK_UTIL;

	// We only need to expose the easing function type to the user not how it was implemented.
	lua.new_enum<EEasingFunc>( "EasingFuncType",
							   { { "Linear", EEasingFunc::LINEAR },
								 { "EaseInQuad", EEasingFunc::EASE_IN_QUAD },
								 { "EaseOutQuad", EEasingFunc::EASE_OUT_QUAD },
								 { "EaseInSine", EEasingFunc::EASE_IN_SINE },
								 { "EaseOutSine", EEasingFunc::EASE_OUT_SINE },
								 { "EaseInOutSine", EEasingFunc::EASE_IN_OUT_SINE },
								 { "EaseOutElastic", EEasingFunc::EASE_OUT_ELASTIC },
								 { "EaseInElastic", EEasingFunc::EASE_IN_ELASTIC },
								 { "EaseInOutElastic", EEasingFunc::EASE_IN_OUT_ELASTIC },
								 { "EaseInExponential", EEasingFunc::EASE_IN_EXPONENTIAL },
								 { "EaseOutExponential", EEasingFunc::EASE_OUT_EXPONENTIAL },
								 { "EaseInOutExponential", EEasingFunc::EASE_IN_OUT_EXPONENTIAL },
								 { "EaseInBound", EEasingFunc::EASE_IN_BOUNCE },
								 { "EaseOutBound", EEasingFunc::EASE_OUT_BOUNCE },
								 { "EaseInOutBound", EEasingFunc::EASE_IN_OUT_BOUNCE },
								 { "EaseInCirc", EEasingFunc::EASE_IN_CIRC },
								 { "EaseOutCirc", EEasingFunc::EASE_OUT_CIRC },
								 { "EaseInOutCirc", EEasingFunc::EASE_IN_OUT_CIRC } } );

	lua.new_usertype<Tween>( "Tween",
							 sol::call_constructor,
							 sol::constructors<Tween(), Tween( float, float, float, EEasingFunc )>(),
							 "update",
							 &Tween::Update,
							 "totalDistance",
							 &Tween::TotalDistance,
							 "currentValue",
							 &Tween::CurrentValue,
							 "isFinished",
							 &Tween::IsFinished );
};

auto create_lua_logger = []( sol::state& lua ) {
	auto& logger = VORTEK_LOGGER::Logger::GetInstance();

	lua.new_usertype<VORTEK_LOGGER::Logger>(
		"Logger",
		sol::no_constructor,
		"log",
		[ & ]( const std::string_view message ) { logger.LuaLog( message ); },
		"warn",
		[ & ]( const std::string_view message ) { logger.LuaWarn( message ); },
		"error",
		[ & ]( const std::string_view message ) { logger.LuaError( message ); } );

	auto logResult = lua.safe_script( R"(
				function ZZZ_Log(message, ...)
					Logger.log(string.format(message, ...))
				end
			)" );

	if ( !logResult.valid() )
	{
		VORTEK_ERROR( "Failed to initialize lua logs" );
	}

	auto warnResult = lua.safe_script( R"(
				function ZZZ_Warn(message, ...)
					Logger.warn(string.format(message, ...))
				end
			)" );

	if ( !warnResult.valid() )
	{
		VORTEK_ERROR( "Failed to initialize lua warnings" );
	}

	auto errorResult = lua.safe_script( R"(
				function ZZZ_Error(message, ...)
					Logger.error(string.format(message, ...))
				end
			)" );

	if ( !errorResult.valid() )
	{
		VORTEK_ERROR( "Failed to initialize lua errors" );
	}

	lua.set_function( "log", []( const std::string& message, const sol::variadic_args& args, sol::this_state s ) {
		try
		{
			sol::state_view L = s;
			sol::protected_function log = L[ "ZZZ_Log" ];
			auto result = log( message, args );
			if ( !result.valid() )
			{
				sol::error error = result;
				throw error;
			}
		}
		catch ( const sol::error& error )
		{
			VORTEK_ERROR( "Failed to get lua log: {}", error.what() );
		}
	} );

	lua.set_function( "warn", []( const std::string& message, const sol::variadic_args& args, sol::this_state s ) {
		try
		{
			sol::state_view L = s;
			sol::protected_function warn = L[ "ZZZ_Warn" ];
			auto result = warn( message, args );
			if ( !result.valid() )
			{
				sol::error error = result;
				throw error;
			}
		}
		catch ( const sol::error& error )
		{
			VORTEK_ERROR( "Failed to get lua warning: {}", error.what() );
		}
	} );

	lua.set_function( "error", []( const std::string& message, const sol::variadic_args& args, sol::this_state s ) {
		try
		{
			sol::state_view L = s;
			sol::protected_function err = L[ "ZZZ_Error" ];
			auto result = err( message, args );
			if ( !result.valid() )
			{
				sol::error error = result;
				throw error;
			}
		}
		catch ( const sol::error& error )
		{
			VORTEK_ERROR( "Failed to get lua errors: {}", error.what() );
		}
	} );

	auto assertResult = lua.safe_script( R"(
				S2D_assert = assert
				assert = function(arg1, message, ...)
					if not arg1 then 
						Logger.error(string.format(message, ...))
					end 
					S2D_assert(arg1)
				end
			)" );
};

void ScriptingSystem::RegisterLuaBindings( sol::state& lua, VORTEK_CORE::ECS::Registry& registry )
{
	VORTEK_CORE::Scripting::GLMBindings::CreateGLMBindings( lua );
	VORTEK_CORE::InputManager::CreateLuaInputBindings( lua, registry );
	VORTEK_RESOURCES::AssetManager::CreateLuaAssetManager( lua );
	VORTEK_CORE::Scripting::SoundBinder::CreateSoundBind( lua );
	VORTEK_CORE::Scripting::RendererBinder::CreateRenderingBind( lua, registry );
	VORTEK_CORE::Scripting::UserDataBinder::CreateLuaUserData( lua );
	VORTEK_CORE::Scripting::ContactListenerBinder::CreateLuaContactListener( lua, registry.GetRegistry() );
	VORTEK_CORE::Scripting::LuaFilesystem::CreateLuaFileSystemBind( lua );
	VORTEK_CORE::Scripting::ScriptingHelpers::CreateLuaHelpers( lua );

	VORTEK_CORE::FollowCamera::CreateLuaFollowCamera( lua, registry );
	VORTEK_CORE::Character::CreateCharacterLuaBind( lua, registry );

	create_timer( lua );
	create_lua_logger( lua );
	createTweenLuaBind( lua );

	VORTEK_CORE::State::CreateLuaStateBind( lua );
	VORTEK_CORE::StateStack::CreateLuaStateStackBind( lua );
	VORTEK_CORE::StateMachine::CreateLuaStateMachine( lua );

	Registry::CreateLuaRegistryBind( lua, registry );
	Entity::CreateLuaEntityBind( lua, registry );
	TransformComponent::CreateLuaTransformBind( lua );
	SpriteComponent::CreateSpriteLuaBind( lua );
	AnimationComponent::CreateAnimationLuaBind( lua );
	BoxColliderComponent::CreateLuaBoxColliderBind( lua );
	CircleColliderComponent::CreateLuaCircleColliderBind( lua );
	PhysicsComponent::CreatePhysicsLuaBind( lua, registry.GetRegistry() );
	TextComponent::CreateLuaTextBindings( lua );
	RigidBodyComponent::CreateRigidBodyBind( lua );
	UIComponent::CreateLuaBind( lua );
}

void ScriptingSystem::RegisterLuaFunctions( sol::state& lua, VORTEK_CORE::ECS::Registry& registry )
{
	auto& mainRegistry = MAIN_REGISTRY();

	lua.set_function( "RunScript", [ & ]( const std::string& path ) {
		try
		{
			auto& engine = CoreEngineData::GetInstance();
			std::string ProjectPath = engine.GetProjectPath();

			lua.safe_script_file( ProjectPath + "content/" + path );
		}
		catch ( const sol::error& error )
		{
			VORTEK_ERROR( "Error loading Lua Script: {}", error.what() );
			return false;
		}

		return true;
	} );

	lua.set_function( "LoadScriptTable", [ & ]( const sol::table& scriptList ) {
		if ( !scriptList.valid() )
		{
			VORTEK_ERROR( "Failed to load script list: Table is invalid." );
			return;
		}

		for ( const auto& [ index, script ] : scriptList )
		{
			try
			{
				auto result = lua.safe_script_file( script.as<std::string>() );
				if ( !result.valid() )
				{
					sol::error error = result;
					throw error;
				}
			}
			catch ( const sol::error& error )
			{
				VORTEK_ERROR( "Failed to load script: {}, Error: {}", script.as<std::string>(), error.what() );
				return;
			}
		}
	} );

	lua.set_function( "GetTicks", [] { return SDL_GetTicks(); } );

	auto& assetManager = mainRegistry.GetAssetManager();

	// clang-format off
		lua.set_function("MeasureText",
			sol::overload(
				[&](const std::string& text, const std::string& fontName)
				{
					const auto& pFont = assetManager.GetFont(fontName);
					if (!pFont)
					{
						VORTEK_ERROR("Failed to get font [{}] - Does not exist in asset manager!", fontName);
						return -1.f;
					}

					glm::vec2 position{ 0.f }, temp_pos{ position };
					for (const auto& character : text)
						pFont->GetNextCharPos(character, temp_pos);

					return std::abs((position - temp_pos).x);
				},
				[&](const TextComponent& text, const TransformComponent& transform)
				{
					return VORTEK_CORE::GetTextBlockSize(text, transform, assetManager);
				}
			)
		);

	// clang-format on

	auto& engine = CoreEngineData::GetInstance();
	lua.set_function( "DeltaTime", [ & ] { return engine.GetDeltaTime(); } );
	lua.set_function( "WindowWidth", [ & ] { return engine.WindowWidth(); } );
	lua.set_function( "WindowHeight", [ & ] { return engine.WindowHeight(); } );

	// Physics Enable functions
	lua.set_function( "DisablePhysics", [ & ] { engine.DisablePhysics(); } );
	lua.set_function( "EnablePhysics", [ & ] { engine.EnablePhysics(); } );
	lua.set_function( "IsPhysicsEnabled", [ & ] { return engine.IsPhysicsEnabled(); } );

	// Render Colliders Enable functions
	lua.set_function( "DisableCollisionRendering", [ & ] { engine.DisableColliderRender(); } );
	lua.set_function( "EnableCollisionRendering", [ & ] { engine.EnableColliderRender(); } );
	lua.set_function( "CollisionRenderingEnabled", [ & ] { return engine.RenderCollidersEnabled(); } );

	// Animation Enable functions
	lua.set_function( "DisableAnimationRendering", [ & ] { engine.DisableAnimationRender(); } );
	lua.set_function( "EnableAnimationRendering", [ & ] { engine.EnableAnimationRender(); } );
	lua.set_function( "AnimationRenderingEnabled", [ & ] { return engine.AnimationRenderEnabled(); } );

	lua.set_function( "GetProjectPath", [ & ] { return engine.GetProjectPath(); } );

	lua.new_usertype<VORTEK_UTIL::RandomIntGenerator>(
		"RandomInt",
		sol::call_constructor,
		sol::constructors<VORTEK_UTIL::RandomIntGenerator( uint32_t, uint32_t ), VORTEK_UTIL::RandomIntGenerator()>(),
		"get_value",
		&VORTEK_UTIL::RandomIntGenerator::GetValue );

	lua.new_usertype<VORTEK_UTIL::RandomFloatGenerator>(
		"RandomFloat",
		sol::call_constructor,
		sol::constructors<VORTEK_UTIL::RandomFloatGenerator( float, float ), VORTEK_UTIL::RandomFloatGenerator()>(),
		"get_value",
		&VORTEK_UTIL::RandomFloatGenerator::GetValue );

	lua.set_function( "EntityInView", [ & ]( const TransformComponent& transform, float width, float height ) {
		auto& camera = registry.GetContext<std::shared_ptr<VORTEK_RENDERING::Camera2D>>();
		return VORTEK_CORE::EntityInView( transform, width, height, *camera );
	} );

	Scene::CreateLuaBind( lua );
}

void ScriptingSystem::RegisterLuaEvents( sol::state& lua, VORTEK_CORE::ECS::Registry& registry )
{
	auto* pDispatcher = registry.TryGetContext<std::shared_ptr<EventDispatcher>>();
	VORTEK_ASSERT( pDispatcher && "There must be at least one registered dispatcher." );

	LuaEventBinder::CreateLuaEventBindings( lua );
	EventDispatcher::RegisterMetaEventFuncs<ContactEvent>();
	EventDispatcher::RegisterMetaEventFuncs<KeyEvent>();
	EventDispatcher::RegisterMetaEventFuncs<LuaEvent>();
	EventDispatcher::CreateEventDispatcherLuaBind( lua, **pDispatcher );
}

void ScriptingSystem::RegisterLuaSystems( sol::state& lua, VORTEK_CORE::ECS::Registry& registry )
{
	VORTEK_CORE::Systems::RenderSystem::CreateRenderSystemLuaBind( lua, registry );
	VORTEK_CORE::Systems::RenderUISystem::CreateRenderUISystemLuaBind( lua );
	VORTEK_CORE::Systems::AnimationSystem::CreateAnimationSystemLuaBind( lua, registry );
}

} // namespace VORTEK_CORE::Systems
