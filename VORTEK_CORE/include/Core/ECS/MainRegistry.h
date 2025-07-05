#pragma once
#include <memory>
#include "Core/ECS/Registry.h"

#define MAIN_REGISTRY() VORTEK_CORE::ECS::MainRegistry::GetInstance()
#define ASSET_MANAGER() MAIN_REGISTRY().GetAssetManager()
#define EVENT_DISPATCHER() MAIN_REGISTRY().GetEventDispatcher()
#define ADD_EVENT_HANDLER( Event, Func, Handler ) EVENT_DISPATCHER().AddHandler<Event, Func>( Handler );

namespace VORTEK_RESOURCES
{
class AssetManager;
}
namespace VORTEK_SOUNDS
{
class MusicPlayer;
class SoundFxPlayer;
} // namespace VORTEK_SOUNDS

namespace VORTEK_CORE::Events
{
class EventDispatcher;
}

namespace VORTEK_CORE::Systems
{
class RenderSystem;
class RenderUISystem;
class RenderShapeSystem;
class AnimationSystem;
class PhysicsSystem;
} // namespace VORTEK_CORE::Systems

namespace VORTEK_CORE::ECS
{
class MainRegistry
{
  public:
	static MainRegistry& GetInstance();
	bool Initialize( bool bEnableFilewatcher = false );

	VORTEK_CORE::Events::EventDispatcher& GetEventDispatcher();
	VORTEK_RESOURCES::AssetManager& GetAssetManager();
	VORTEK_SOUNDS::MusicPlayer& GetMusicPlayer();
	VORTEK_SOUNDS::SoundFxPlayer& GetSoundPlayer();

	template <typename TContext>
	TContext AddToContext( TContext context )
	{
		return m_pMainRegistry->AddToContext<TContext>( context );
	}

	template <typename TContext>
	TContext& GetContext()
	{
		return m_pMainRegistry->GetContext<TContext>();
	}

	VORTEK_CORE::Systems::RenderSystem& GetRenderSystem();
	VORTEK_CORE::Systems::RenderUISystem& GetRenderUISystem();
	VORTEK_CORE::Systems::RenderShapeSystem& GetRenderShapeSystem();
	VORTEK_CORE::Systems::AnimationSystem& GetAnimationSystem();
	VORTEK_CORE::Systems::PhysicsSystem& GetPhysicsSystem();

  private:
	MainRegistry() = default;
	~MainRegistry() = default;
	MainRegistry( const MainRegistry& ) = delete;
	MainRegistry& operator=( const MainRegistry& ) = delete;

	bool RegisterMainSystems();

  private:
	std::unique_ptr<Registry> m_pMainRegistry{ nullptr };
	bool m_bInitialized{ false };
};
} // namespace VORTEK_CORE::ECS
