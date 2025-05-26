#include "ScriptingSystem.h"
#include "../ECS/Entity.h"
#include "../ECS/MainRegistry.h"

#include "../Scripting/GlmLuaBindings.h"
#include "../Scripting/InputManager.h"
#include "../Scripting/SoundBindings.h"
#include "../Scripting/RendererBindings.h"
#include "../Scripting/UserDataBindings.h"
#include "../Scripting/ContactListenerBind.h"

#include "../Resources/AssetManager.h"
#include <Logger/Logger.h>
#include <VORTEKUtilities/Timer.h>
#include <VORTEKUtilities/RandomGenerator.h>
#include "../CoreUtilities/CoreEngineData.h"
#include "../CoreUtilities/CoreUtilities.h"
#include "../CoreUtilities/FollowCamera.h"

#include "../States/State.h"
#include "../States/StateStack.h"
#include "../States/StateMachine.h"

using namespace VORTEK_CORE::ECS;
using namespace VORTEK_RESOURCES;

namespace VORTEK_CORE::Systems
{

	ScriptingSystem::ScriptingSystem()
		: m_bMainLoaded{ false }
	{
	}

	bool ScriptingSystem::LoadMainScript(VORTEK_CORE::ECS::Registry& registry, sol::state& lua)
	{
		try
		{
			auto result = lua.safe_script_file("./assets/scripts/main.lua");
		}
		catch (const sol::error& err)
		{
			VORTEK_ERROR("Error loading the main lua script: {0}", err.what());
			return false;
		}

		sol::table main_lua = lua["main"];
		sol::optional<sol::table> bUpdateExists = main_lua[1];
		if (bUpdateExists == sol::nullopt)
		{
			VORTEK_ERROR("There is no update function in main.lua");
			return false;
		}

		sol::table update_script = main_lua[1];
		sol::function update = update_script["update"];

		sol::optional<sol::table> bRenderExists = main_lua[2];
		if (bRenderExists == sol::nullopt)
		{
			VORTEK_ERROR("There is no render function in main.lua");
			return false;
		}

		sol::table render_script = main_lua[2];
		sol::function render = render_script["render"];

		VORTEK_CORE::ECS::Entity mainLuaScript{ registry, "main_script", "" };
		mainLuaScript.AddComponent<VORTEK_CORE::ECS::ScriptComponent>(
			VORTEK_CORE::ECS::ScriptComponent{ .update = update, .render = render });

		m_bMainLoaded = true;
		return true;
	}

	void ScriptingSystem::Update(VORTEK_CORE::ECS::Registry& registry)
	{
		if (!m_bMainLoaded)
		{
			VORTEK_ERROR("Main lua script has not been loaded!");
			return;
		}

		auto view = registry.GetRegistry().view<VORTEK_CORE::ECS::ScriptComponent>();

		for (const auto& entity : view)
		{
			VORTEK_CORE::ECS::Entity ent{ registry, entity };
			if (ent.GetName() != "main_script")
				continue;

			auto& script = ent.GetComponent<VORTEK_CORE::ECS::ScriptComponent>();
			auto error = script.update(entity);
			if (!error.valid())
			{
				sol::error err = error;
				VORTEK_ERROR("Error running the Update script: {0}", err.what());
			}
		}

		auto& lua = registry.GetContext<std::shared_ptr<sol::state>>();
		if (lua) lua->collect_garbage();
	}

	void ScriptingSystem::Render(VORTEK_CORE::ECS::Registry& registry)
	{
		if (!m_bMainLoaded)
		{
			VORTEK_ERROR("Main lua script has not been loaded!");
			return;
		}

		auto view = registry.GetRegistry().view<VORTEK_CORE::ECS::ScriptComponent>();

		for (const auto& entity : view)
		{
			VORTEK_CORE::ECS::Entity ent{ registry, entity };
			if (ent.GetName() != "main_script")
				continue;

			auto& script = ent.GetComponent<VORTEK_CORE::ECS::ScriptComponent>();
			auto error = script.render(entity);
			if (!error.valid())
			{
				sol::error err = error;
				VORTEK_ERROR("Error running the Render script: {0}", err.what());
			}
		}

		auto& lua = registry.GetContext<std::shared_ptr<sol::state>>();
		if(lua)	lua->collect_garbage();
	}

	auto create_timer = [](sol::state& lua) {
		using namespace VORTEK_UTIL;
		lua.new_usertype<Timer>("Timer",
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
			[](Timer& timer) {
				if (timer.IsRunning())
					timer.Stop();

				timer.Start();
			});
		};

	auto create_lua_logger = [&](sol::state& lua) {
		auto& logger = VORTEK_LOGGER::Logger::GetInstance();

		lua.new_usertype<VORTEK_LOGGER::Logger>(
			"Logger",
			sol::no_constructor,
			"log",
			[&](const std::string_view message) { logger.LuaLog(message); },
			"warn",
			[&](const std::string_view message) { logger.LuaWarn(message); },
			"error",
			[&](const std::string_view message) { logger.LuaError(message); });

		auto logResult = lua.safe_script(R"(
				function ZZZ_Log(message, ...)
					Logger.log(string.format(message, ...))
				end
			)");

		if (!logResult.valid())
		{
			VORTEK_ERROR("Failed to initialize lua logs");
		}

		auto warnResult = lua.safe_script(R"(
				function ZZZ_Warn(message, ...)
					Logger.warn(string.format(message, ...))
				end
			)");

		if (!warnResult.valid())
		{
			VORTEK_ERROR("Failed to initialize lua warnings");
		}

		auto errorResult = lua.safe_script(R"(
				function ZZZ_Error(message, ...)
					Logger.error(string.format(message, ...))
				end
			)");

		if (!errorResult.valid())
		{
			VORTEK_ERROR("Failed to initialize lua errors");
		}

		lua.set_function("log", [](const std::string& message, const sol::variadic_args& args, sol::this_state s) {
			try
			{
				sol::state_view L = s;
				sol::protected_function log = L["ZZZ_Log"];
				auto result = log(message, args);
				if (!result.valid())
				{
					sol::error error = result;
					throw error;
				}
			}
			catch (const sol::error& error)
			{
				VORTEK_ERROR("Failed to get lua log: {}", error.what());
			}
			});

		lua.set_function("warn", [](const std::string& message, const sol::variadic_args& args, sol::this_state s) {
			try
			{
				sol::state_view L = s;
				sol::protected_function warn = L["ZZZ_Warn"];
				auto result = warn(message, args);
				if (!result.valid())
				{
					sol::error error = result;
					throw error;
				}
			}
			catch (const sol::error& error)
			{
				VORTEK_ERROR("Failed to get lua warning: {}", error.what());
			}
			});

		lua.set_function("error", [](const std::string& message, const sol::variadic_args& args, sol::this_state s) {
			try
			{
				sol::state_view L = s;
				sol::protected_function err = L["ZZZ_Error"];
				auto result = err(message, args);
				if (!result.valid())
				{
					sol::error error = result;
					throw error;
				}
			}
			catch (const sol::error& error)
			{
				VORTEK_ERROR("Failed to get lua errors: {}", error.what());
			}
			});

		auto assertResult = lua.safe_script(R"(
				S2D_assert = assert
				assert = function(arg1, message, ...)
					if not arg1 then 
						Logger.error(string.format(message, ...))
					end 
					S2D_assert(arg1)
				end
			)");
		};

	void ScriptingSystem::RegisterLuaBindings(sol::state& lua, VORTEK_CORE::ECS::Registry& registry)
	{
		VORTEK_CORE::Scripting::GLMBindings::CreateGLMBindings(lua);
		VORTEK_CORE::InputManager::CreateLuaInputBindings(lua, registry);
		VORTEK_RESOURCES::AssetManager::CreateLuaAssetManager(lua);
		VORTEK_CORE::Scripting::SoundBinder::CreateSoundBind(lua);
		VORTEK_CORE::Scripting::RendererBinder::CreateRenderingBind(lua, registry);
		VORTEK_CORE::Scripting::UserDataBinder::CreateLuaUserData(lua);
		VORTEK_CORE::Scripting::ContactListenerBinder::CreateLuaContactListener(lua, registry.GetRegistry());

		VORTEK_CORE::FollowCamera::CreateLuaFollowCamera(lua, registry);

		create_timer(lua);
		create_lua_logger(lua);

		VORTEK_CORE::State::CreateLuaStateBind(lua);
		VORTEK_CORE::StateStack::CreateLuaStateStackBind(lua);
		VORTEK_CORE::StateMachine::CreateLuaStateMachine(lua);

		Registry::CreateLuaRegistryBind(lua, registry);
		Entity::CreateLuaEntityBind(lua, registry);
		TransformComponent::CreateLuaTransformBind(lua);
		SpriteComponent::CreateSpriteLuaBind(lua);
		AnimationComponent::CreateAnimationLuaBind(lua);
		BoxColliderComponent::CreateLuaBoxColliderBind(lua);
		CircleColliderComponent::CreateLuaCircleColliderBind(lua);
		PhysicsComponent::CreatePhysicsLuaBind(lua, registry.GetRegistry());
		TextComponent::CreateLuaTextBindings(lua);
		RigidBodyComponent::CreateRigidBodyBind(lua);
	}

	void ScriptingSystem::RegisterLuaFunctions(sol::state& lua, VORTEK_CORE::ECS::Registry& registry)
	{
		auto& mainRegistry = MAIN_REGISTRY();

		lua.set_function("RunScript", [&](const std::string& path) {
			try
			{
				lua.safe_script_file(path);
			}
			catch (const sol::error& error)
			{
				VORTEK_ERROR("Error loading Lua Script: {}", error.what());
				return false;
			}

			return true;
			});

		lua.set_function("LoadScriptTable", [&](const sol::table& scriptList) {
			if (!scriptList.valid())
			{
				VORTEK_ERROR("Failed to load script list: Table is invalid.");
				return;
			}

			for (const auto& [index, script] : scriptList)
			{
				try
				{
					auto result = lua.safe_script_file(script.as<std::string>());
					if (!result.valid())
					{
						sol::error error = result;
						throw error;
					}
				}
				catch (const sol::error& error)
				{
					VORTEK_ERROR("Failed to load script: {}, Error: {}", script.as<std::string>(), error.what());
					return;
				}
			}
			});

		lua.set_function("GetTicks", [] { return SDL_GetTicks(); });

		auto& assetManager = mainRegistry.GetAssetManager();
		lua.set_function("MeasureText", [&](const std::string& text, const std::string& fontName) {
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
			});

		auto& engine = CoreEngineData::GetInstance();
		lua.set_function("DeltaTime", [&] { return engine.GetDeltaTime(); });
		lua.set_function("WindowWidth", [&] { return engine.WindowWidth(); });
		lua.set_function("WindowHeight", [&] { return engine.WindowHeight(); });

		// Physics Enable functions
		lua.set_function("DisablePhysics", [&] { engine.DisablePhysics(); });
		lua.set_function("EnablePhysics", [&] { engine.EnablePhysics(); });
		lua.set_function("IsPhysicsEnabled", [&] { return engine.IsPhysicsEnabled(); });

		// Render Colliders Enable functions
		lua.set_function("DisableCollisionRendering", [&] { engine.DisableColliderRender(); });
		lua.set_function("EnableCollisionRendering", [&] { engine.EnableColliderRender(); });
		lua.set_function("CollisionRenderingEnabled", [&] { return engine.RenderCollidersEnabled(); });

		lua.new_usertype<VORTEK_UTIL::RandomIntGenerator>(
			"RandomInt",
			sol::call_constructor,
			sol::constructors<VORTEK_UTIL::RandomIntGenerator(uint32_t, uint32_t), VORTEK_UTIL::RandomIntGenerator()>(),
			"get_value",
			&VORTEK_UTIL::RandomIntGenerator::GetValue);

		lua.new_usertype<VORTEK_UTIL::RandomFloatGenerator>(
			"RandomFloat",
			sol::call_constructor,
			sol::constructors<VORTEK_UTIL::RandomFloatGenerator(float, float), VORTEK_UTIL::RandomFloatGenerator()>(),
			"get_value",
			&VORTEK_UTIL::RandomFloatGenerator::GetValue);

		lua.set_function("EntityInView", [&](const TransformComponent& transform, float width, float height) {
			auto& camera = registry.GetContext<std::shared_ptr<VORTEK_RENDERING::Camera2D>>();
			return VORTEK_CORE::EntityInView(transform, width, height, *camera);
			});
	}
} // namespace VORTEK_CORE::Systems