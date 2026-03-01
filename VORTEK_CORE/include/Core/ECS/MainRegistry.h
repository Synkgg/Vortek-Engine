#pragma once
#include "Core/ECS/Registry.h"

#define MAIN_REGISTRY() Vortek::Core::ECS::MainRegistry::GetInstance()
#define ASSET_MANAGER() MAIN_REGISTRY().GetAssetManager()
#define EVENT_DISPATCHER() MAIN_REGISTRY().GetEventDispatcher()
#define ADD_EVENT_HANDLER( Event, Func, Handler ) EVENT_DISPATCHER().AddHandler<Event, Func>( Handler );

namespace VORTEK_RESOURCES
{
class AssetManager;
}
namespace Vortek::Sounds
{
class MusicPlayer;
class SoundFxPlayer;
} // namespace Vortek::Sounds

namespace Vortek::Core::Events
{
class EventDispatcher;
}

namespace Vortek::Rendering
{
class Renderer;
}

namespace Vortek::Core::Systems
{
class RenderSystem;
class RenderUISystem;
class RenderShapeSystem;
class AnimationSystem;
class PhysicsSystem;
} // namespace Vortek::Core::Systems

namespace Vortek::Core::ECS
{
class MainRegistry
{
  public:
	static MainRegistry& GetInstance();
	bool Initialize( bool bEnableFilewatcher = false );

	Vortek::Core::Events::EventDispatcher& GetEventDispatcher();
	VORTEK_RESOURCES::AssetManager& GetAssetManager();
	Vortek::Sounds::MusicPlayer& GetMusicPlayer();
	Vortek::Sounds::SoundFxPlayer& GetSoundPlayer();
	Vortek::Rendering::Renderer& GetRenderer();

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

	Vortek::Core::Systems::RenderSystem& GetRenderSystem();
	Vortek::Core::Systems::RenderUISystem& GetRenderUISystem();
	Vortek::Core::Systems::RenderShapeSystem& GetRenderShapeSystem();
	Vortek::Core::Systems::AnimationSystem& GetAnimationSystem();
	Vortek::Core::Systems::PhysicsSystem& GetPhysicsSystem();
	Registry* GetRegistry();

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
} // namespace Vortek::Core::ECS
