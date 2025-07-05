#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include <Logger/Logger.h>
#include <Sounds/MusicPlayer/MusicPlayer.h>
#include <Sounds/SoundPlayer/SoundFxPlayer.h>
#include "Core/Systems/ScriptingSystem.h"
#include "Core/Systems/RenderSystem.h"
#include "Core/Systems/RenderUISystem.h"
#include "Core/Systems/RenderShapeSystem.h"
#include "Core/Systems/AnimationSystem.h"
#include "Core/Systems/PhysicsSystem.h"
#include "Core/Events/EventDispatcher.h"
#include <Rendering/Core/Renderer.h>
#include <VORTEKUtilities/HelperUtilities.h>

// Systems not needed outside of the editor
#ifdef IN_VORTEK_EDITOR
#include "Core/Systems/RenderPickingSystem.h"
#endif
//  End Editor only systems

namespace VORTEK_CORE::ECS
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

	auto pMusicPlayer = std::make_shared<VORTEK_SOUNDS::MusicPlayer>();
	m_pMainRegistry->AddToContext<std::shared_ptr<VORTEK_SOUNDS::MusicPlayer>>( std::move( pMusicPlayer ) );

	auto pSoundPlayer = std::make_shared<VORTEK_SOUNDS::SoundFxPlayer>();
	m_pMainRegistry->AddToContext<std::shared_ptr<VORTEK_SOUNDS::SoundFxPlayer>>( std::move( pSoundPlayer ) );

	auto renderer = std::make_shared<VORTEK_RENDERING::Renderer>();

	// Enable Alpha Blending
	renderer->SetCapability( VORTEK_RENDERING::Renderer::GLCapability::BLEND, true );
	renderer->SetCapability( VORTEK_RENDERING::Renderer::GLCapability::DEPTH_TEST, true );
	renderer->SetBlendCapability( VORTEK_RENDERING::Renderer::BlendingFactors::SRC_ALPHA,
								  VORTEK_RENDERING::Renderer::BlendingFactors::ONE_MINUS_SRC_ALPHA );

	renderer->SetLineWidth( 4.f );

	if ( !AddToContext<std::shared_ptr<VORTEK_RENDERING::Renderer>>( renderer ) )
	{
		VORTEK_ERROR( "Failed to add the renderer to the registry context!" );
		return false;
	}

	m_bInitialized = RegisterMainSystems();

	return m_bInitialized;
}

bool MainRegistry::RegisterMainSystems()
{
	auto renderSystem = std::make_shared<VORTEK_CORE::Systems::RenderSystem>();
	if ( !renderSystem )
	{
		VORTEK_ERROR( "Failed to create the render system!" );
		return false;
	}

	if ( !AddToContext<std::shared_ptr<VORTEK_CORE::Systems::RenderSystem>>( renderSystem ) )
	{
		VORTEK_ERROR( "Failed to add the render system to the registry context!" );
		return false;
	}

	auto renderUISystem = std::make_shared<VORTEK_CORE::Systems::RenderUISystem>();
	if ( !renderUISystem )
	{
		VORTEK_ERROR( "Failed to create the render UI system!" );
		return false;
	}

	if ( !AddToContext<std::shared_ptr<VORTEK_CORE::Systems::RenderUISystem>>( renderUISystem ) )
	{
		VORTEK_ERROR( "Failed to add the render UI system to the registry context!" );
		return false;
	}

	auto renderShapeSystem = std::make_shared<VORTEK_CORE::Systems::RenderShapeSystem>();
	if ( !renderShapeSystem )
	{
		VORTEK_ERROR( "Failed to create the render Shape system!" );
		return false;
	}

	if ( !AddToContext<std::shared_ptr<VORTEK_CORE::Systems::RenderShapeSystem>>( renderShapeSystem ) )
	{
		VORTEK_ERROR( "Failed to add the render Shape system to the registry context!" );
		return false;
	}

	AddToContext<std::shared_ptr<VORTEK_CORE::Systems::PhysicsSystem>>(
		std::make_shared<VORTEK_CORE::Systems::PhysicsSystem>() );

	AddToContext<std::shared_ptr<VORTEK_CORE::Systems::AnimationSystem>>(
		std::make_shared<VORTEK_CORE::Systems::AnimationSystem>() );

	AddToContext<std::shared_ptr<VORTEK_CORE::Events::EventDispatcher>>(
		std::make_shared<VORTEK_CORE::Events::EventDispatcher>() );

	// #ifdef IN_VORTEK_EDITOR
	AddToContext<std::shared_ptr<VORTEK_CORE::Systems::RenderPickingSystem>>(
		std::make_shared<VORTEK_CORE::Systems::RenderPickingSystem>() );
	VORTEK_LOG( "Added Render Picking System to Main Registry." );
	// #endif

	return true;
}

VORTEK_CORE::Events::EventDispatcher& MainRegistry::GetEventDispatcher()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_CORE::Events::EventDispatcher>>();
}

VORTEK_RESOURCES::AssetManager& MainRegistry::GetAssetManager()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_RESOURCES::AssetManager>>();
}

VORTEK_SOUNDS::MusicPlayer& MainRegistry::GetMusicPlayer()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_SOUNDS::MusicPlayer>>();
}

VORTEK_SOUNDS::SoundFxPlayer& MainRegistry::GetSoundPlayer()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_SOUNDS::SoundFxPlayer>>();
}

VORTEK_CORE::Systems::RenderSystem& MainRegistry::GetRenderSystem()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_CORE::Systems::RenderSystem>>();
}

VORTEK_CORE::Systems::RenderUISystem& MainRegistry::GetRenderUISystem()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_CORE::Systems::RenderUISystem>>();
}

VORTEK_CORE::Systems::RenderShapeSystem& MainRegistry::GetRenderShapeSystem()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_CORE::Systems::RenderShapeSystem>>();
}

VORTEK_CORE::Systems::AnimationSystem& MainRegistry::GetAnimationSystem()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_CORE::Systems::AnimationSystem>>();
}

VORTEK_CORE::Systems::PhysicsSystem& MainRegistry::GetPhysicsSystem()
{
	VORTEK_ASSERT( m_bInitialized && "Main Registry must be initialized before use." );
	return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_CORE::Systems::PhysicsSystem>>();
}

} // namespace VORTEK_CORE::ECS
