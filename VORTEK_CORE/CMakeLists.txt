add_library( VORTEK_CORE 
	# Core Utilities
	"src/Core/CoreUtilities/CoreEngineData.h"
	"src/Core/CoreUtilities/CoreEngineData.cpp"
	"src/Core/CoreUtilities/CoreUtilities.h"
	"src/Core/CoreUtilities/CoreUtilities.cpp"
	"src/Core/CoreUtilities/FollowCamera.h"
	"src/Core/CoreUtilities/FollowCamera.cpp"
	
	# ECS - Components
	"src/Core/ECS/Components/AllComponents.h"
	"src/Core/ECS/Components/AnimationComponent.h"
	"src/Core/ECS/Components/AnimationComponent.cpp"
	"src/Core/ECS/Components/BoxColliderComponent.h"
	"src/Core/ECS/Components/BoxColliderComponent.cpp"
	"src/Core/ECS/Components/CircleColliderComponent.h"
	"src/Core/ECS/Components/CircleColliderComponent.cpp"
	"src/Core/ECS/Components/ComponentSerializer.h"
	"src/Core/ECS/Components/ComponentSerializer.inl"
	"src/Core/ECS/Components/ComponentSerializer.cpp"
	"src/Core/ECS/Components/Identification.h"
	"src/Core/ECS/Components/PhysicsComponent.h"
	"src/Core/ECS/Components/PhysicsComponent.cpp"
	"src/Core/ECS/Components/RigidBodyComponent.h"
	"src/Core/ECS/Components/RigidBodyComponent.cpp"
	"src/Core/ECS/Components/ScriptComponent.h"
	"src/Core/ECS/Components/SpriteComponent.h"
	"src/Core/ECS/Components/SpriteComponent.cpp"
	"src/Core/ECS/Components/TextComponent.h"
	"src/Core/ECS/Components/TextComponent.cpp"
	"src/Core/ECS/Components/TileComponent.h"
	"src/Core/ECS/Components/TransformComponent.h"
	"src/Core/ECS/Components/TransformComponent.cpp"
	
	# ECS 
	"src/Core/ECS/Entity.h"
	"src/Core/ECS/Entity.inl"
	"src/Core/ECS/Entity.cpp"
	"src/Core/ECS/MainRegistry.h"
	"src/Core/ECS/MainRegistry.cpp"
	"src/Core/ECS/MetaUtilities.h"
	"src/Core/ECS/MetaUtilities.cpp"
	"src/Core/ECS/Registry.h"
	"src/Core/ECS/Registry.inl"
	"src/Core/ECS/Registry.cpp"
	
	# Loaders
	"src/Core/Loaders/TilemapLoader.h"
	"src/Core/Loaders/TilemapLoader.cpp"
	
	# Resources
	"src/Core/Resources/Fonts/default_fonts.h"
	"src/Core/Resources/AssetManager.h"
	"src/Core/Resources/AssetManager.cpp"
	
	# Scripting
	"src/Core/Scripting/ContactListenerBind.h"
	"src/Core/Scripting/ContactListenerBind.cpp"
	"src/Core/Scripting/GlmLuaBindings.h"
	"src/Core/Scripting/GlmLuaBindings.cpp"
	"src/Core/Scripting/InputManager.h"
	"src/Core/Scripting/InputManager.cpp"
	"src/Core/Scripting/RendererBindings.h"
	"src/Core/Scripting/RendererBindings.cpp"
	"src/Core/Scripting/SoundBindings.h"
	"src/Core/Scripting/SoundBindings.cpp"
	"src/Core/Scripting/UserDataBindings.h"
	"src/Core/Scripting/UserDataBindings.inl"
	"src/Core/Scripting/UserDataBindings.cpp"
	
	# States
	"src/Core/States/State.h"
	"src/Core/States/State.cpp"
	"src/Core/States/StateMachine.h"
	"src/Core/States/StateMachine.cpp"
	"src/Core/States/StateStack.h"
	"src/Core/States/StateStack.cpp"
	
	# Systems
	"src/Core/Systems/AnimationSystem.h"
	"src/Core/Systems/AnimationSystem.cpp"
	"src/Core/Systems/PhysicsSystem.h"
	"src/Core/Systems/PhysicsSystem.cpp"
	"src/Core/Systems/RenderShapeSystem.h"
	"src/Core/Systems/RenderShapeSystem.cpp"
	"src/Core/Systems/RenderSystem.h"
	"src/Core/Systems/RenderSystem.cpp"
	"src/Core/Systems/RenderUISystem.h"
	"src/Core/Systems/RenderUISystem.cpp"
	"src/Core/Systems/ScriptingSystem.h"
	"src/Core/Systems/ScriptingSystem.cpp"
)

target_include_directories(
    VORTEK_CORE
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
	PRIVATE ${SOL2_INCLUDE_DIRS}
)

target_link_libraries(
	VORTEK_CORE
	PUBLIC
    glm::glm EnTT::EnTT sol2::sol2 ${LUA_LIBRARIES}
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    VORTEK_LOGGER VORTEK_UTILITIES VORTEK_PHYSICS
	VORTEK_RENDERING VORTEK_FILESYSTEM VORTEK_SOUNDS VORTEK_WINDOW
)

target_compile_options(
    VORTEK_CORE PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILE_FLAGS}>
)
