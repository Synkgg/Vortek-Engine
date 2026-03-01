#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include <Logger/Logger.h>
#include <Sounds/MusicPlayer/MusicPlayer.h>
#include <Sounds/SoundPlayer/SoundFxPlayer.h>
#include <Core/Systems/ScriptingSystem.h>
#include <Core/Systems/RenderSystem.h>
#include <Core/Systems/RenderUISystem.h>
#include <Core/Systems/RenderShapeSystem.h>
#include <Core/Systems/AnimationSystem.h>
#include <Core/Systems/PhysicsSystem.h>
#include <Core/Events/EventDispatcher.h>
#include <Rendering/Core/Renderer.h>
#include <VortekUtilities/HelperUtilities.h>

// Systems not needed outside of the editor
#ifdef IN_VORTEK_EDITOR
#include "Core/Systems/RenderPickingSystem.h"
#endif
// End Editor only systems

namespace Vortek::Core::ECS
{

MainRegistry& MainRegistry::GetInstance()
{
	static MainRegistry instance{};
	return instance;
}

bool MainRegistry::Initialize( bool bEnableFilewatcher )
{
	m_pMainRegistry = std::make_unique<Registry>();
	VORTEK_ASSERT( m_pMainRegistry && "Failed to initialize main registry." );

	auto pAssetManager = std::make_shared<VORTEK_RESOURCES::AssetManager>( bEnableFilewatcher );
	m_pMainRegistry->AddToContext<std::shared_ptr<VORTEK_RESOURCES::AssetManager>>( std::move( pAssetManager ) );

	auto pMusicPlayer = std::make_shared<Vortek::Sounds::MusicPlayer>();
	m_pMainRegistry->AddToContext<std::shared_ptr<Vortek::Sounds::MusicPlayer>>( std::move( pMusicPlayer ) );

	auto pSoundPlayer = std::make_shared<Vortek::Sounds::SoundFxPlayer>();
	m_pMainRegistry->AddToContext<std::shared_ptr<Vortek::Sounds::SoundFxPlayer>>( std::move( pSoundPlayer ) );

	auto renderer = std::make_shared<Vortek::Rendering::Renderer>();

	// Enable Alpha Blending
	renderer->SetCapability( Vortek::Rendering::Renderer::GLCapability::BLEND, true );
	renderer->SetBlendCapability( Vortek::Rendering::Renderer::BlendingFactors::SRC_ALPHA,
								  Vortek::Rendering::Renderer::BlendingFactors::ONE_MINUS_SRC_ALPHA );

	// Currently we only need the depth test in the editor.
#ifdef IN_VORTEK_EDITOR
	renderer->SetCapability( Vortek::Rendering::Renderer::GLCapability::DEPTH_TEST, true );
#endif

	if ( !AddToContext<std::shared_ptr<Vortek::Rendering::Renderer>>( renderer ) )
	{
		VORTEK_ERROR( "Failed to add the renderer to the registry context!" );
		return false;
	}

	m_bInitialized = RegisterMainSystems();

	return m_bInitialized;
}

bool MainRegistry::RegisterMainSystems()
{
	auto renderSystem = std::make_shared<Vortek::Core::Systems::RenderSystem>();
	if ( !renderSystem )
	{
		VORTEK_ERROR( "Failed to create the render system!" );
		return false;
	}

	if ( !AddToContext<std::shared_ptr<Vortek::Core::Systems::RenderSystem>>( renderSystem ) )
	{
		VORTEK_ERROR( "Failed to add the render system to the registry context!" );
		return false;
	}

	auto renderUISystem = std::make_shared<Vortek::Core::Systems::RenderUISystem>();
	if ( !renderUISystem )
	{
		VORTEK_ERROR( "Failed to create the render UI system!" );
		return false;
	}

	if ( !AddToContext<std::shared_ptr<Vortek::Core::Systems::RenderUISystem>>( renderUISystem ) )
	{
		VORTEK_ERROR( "Failed to add the render UI system to the registry context!" );
		return false;
	}

	auto renderShapeSystem = std::make_shared<Vortek::Core::Systems::RenderShapeSystem>();
	if ( !renderShapeSystem )
	{
		VORTEK_ERROR( "Failed to create the render Shape system!" );
		return false;
	}

	if ( !AddToContext<std::shared_ptr<Vortek::Core::Systems::RenderShapeSystem>>( renderShapeSystem ) )
	{
		VORTEK_ERROR( "Failed to add the render Shape system to the registry context!" );
		return false;
	}

	AddToContext<std::shared_ptr<Vortek::Core::Systems::PhysicsSystem>>(
		std::make_shared<Vortek::Core::Systems::PhysicsSystem>() );

	AddToContext<std::shared_ptr<Vortek::Core::Systems::AnimationSystem>>(
		std::make_shared<Vortek::Core::Systems::AnimationSystem>() );

	AddToContext<std::shared_ptr<Vortek::Core::Events::EventDispatcher>>(
		std::make_shared<Vortek::Core::Events::EventDispatcher>() );

#ifdef IN_VORTEK_EDITOR
	AddToContext<std::shared_ptr<Vortek::Core::Systems::RenderPickingSystem>>(
		std::make_shared<Vortek::Core::Systems::RenderPickingSystem>() );
	VORTEK_LOG( "Added Render Picking System to Main Registry." );
#endif

	return true;
}

Vortek::Core::Events::EventDispatcher& MainRegistry::GetEventDispatcher()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Vortek::Core::Events::EventDispatcher>>();
}

VORTEK_RESOURCES::AssetManager& MainRegistry::GetAssetManager()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_RESOURCES::AssetManager>>();
}

Vortek::Sounds::MusicPlayer& MainRegistry::GetMusicPlayer()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Vortek::Sounds::MusicPlayer>>();
}

Vortek::Sounds::SoundFxPlayer& MainRegistry::GetSoundPlayer()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Vortek::Sounds::SoundFxPlayer>>();
}

Vortek::Rendering::Renderer& MainRegistry::GetRenderer()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Vortek::Rendering::Renderer>>();
}

Vortek::Core::Systems::RenderSystem& MainRegistry::GetRenderSystem()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Vortek::Core::Systems::RenderSystem>>();
}

Vortek::Core::Systems::RenderUISystem& MainRegistry::GetRenderUISystem()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Vortek::Core::Systems::RenderUISystem>>();
}

Vortek::Core::Systems::RenderShapeSystem& MainRegistry::GetRenderShapeSystem()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Vortek::Core::Systems::RenderShapeSystem>>();
}

Vortek::Core::Systems::AnimationSystem& MainRegistry::GetAnimationSystem()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Vortek::Core::Systems::AnimationSystem>>();
}

Vortek::Core::Systems::PhysicsSystem& MainRegistry::GetPhysicsSystem()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<Vortek::Core::Systems::PhysicsSystem>>();
}

Registry* MainRegistry::GetRegistry()
{
	if ( !m_pMainRegistry )
		m_pMainRegistry = std::make_unique<Registry>();

	return m_pMainRegistry.get();
}


} // namespace Vortek::Core::ECS
