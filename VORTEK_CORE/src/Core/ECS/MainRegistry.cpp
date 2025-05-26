#include "../ECS/MainRegistry.h"
#include "../Resources/AssetManager.h"
#include <Logger/Logger.h>
#include <Sounds/MusicPlayer/MusicPlayer.h>
#include <Sounds/SoundPlayer/SoundFXPlayer.h>
#include "../Systems/ScriptingSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/RenderUISystem.h"
#include "../Systems/RenderShapeSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/PhysicsSystem.h"
#include <Rendering/Core/Renderer.h>
#include <VORTEKUtilities/HelperUtilities.h>

namespace VORTEK_CORE::ECS
{

	MainRegistry& MainRegistry::GetInstance()
	{
		static MainRegistry instance{};
		return instance;
	}

	bool MainRegistry::Initialize()
	{
		m_pMainRegistry = std::make_unique<Registry>();
		VORTEK_ASSERT(m_pMainRegistry && "Failed to initialize main registry.");

		auto pAssetManager = std::make_shared<VORTEK_RESOURCES::AssetManager>();
		m_pMainRegistry->AddToContext<std::shared_ptr<VORTEK_RESOURCES::AssetManager>>(std::move(pAssetManager));

		auto pMusicPlayer = std::make_shared<VORTEK_SOUNDS::MusicPlayer>();
		m_pMainRegistry->AddToContext<std::shared_ptr<VORTEK_SOUNDS::MusicPlayer>>(std::move(pMusicPlayer));

		auto pSoundPlayer = std::make_shared<VORTEK_SOUNDS::SoundFxPlayer>();
		m_pMainRegistry->AddToContext<std::shared_ptr<VORTEK_SOUNDS::SoundFxPlayer>>(std::move(pSoundPlayer));

		auto renderer = std::make_shared<VORTEK_RENDERING::Renderer>();

		// Enable Alpha Blending
		renderer->SetCapability(VORTEK_RENDERING::Renderer::GLCapability::BLEND, true);
		renderer->SetBlendCapability(VORTEK_RENDERING::Renderer::BlendingFactors::SRC_ALPHA,
			VORTEK_RENDERING::Renderer::BlendingFactors::ONE_MINUS_SRC_ALPHA);

		renderer->SetLineWidth(4.f);

		if (!AddToContext<std::shared_ptr<VORTEK_RENDERING::Renderer>>(renderer))
		{
			VORTEK_ERROR("Failed to add the renderer to the registry context!");
			return false;
		}

		m_bInitialized = RegisterMainSystems();

		return m_bInitialized;
	}

	bool MainRegistry::RegisterMainSystems()
	{
		auto renderSystem = std::make_shared<VORTEK_CORE::Systems::RenderSystem>();
		if (!renderSystem)
		{
			VORTEK_ERROR("Failed to create the render system!");
			return false;
		}

		if (!AddToContext<std::shared_ptr<VORTEK_CORE::Systems::RenderSystem>>(renderSystem))
		{
			VORTEK_ERROR("Failed to add the render system to the registry context!");
			return false;
		}

		auto renderUISystem = std::make_shared<VORTEK_CORE::Systems::RenderUISystem>();
		if (!renderUISystem)
		{
			VORTEK_ERROR("Failed to create the render UI system!");
			return false;
		}

		if (!AddToContext<std::shared_ptr<VORTEK_CORE::Systems::RenderUISystem>>(renderUISystem))
		{
			VORTEK_ERROR("Failed to add the render UI system to the registry context!");
			return false;
		}

		auto renderShapeSystem = std::make_shared<VORTEK_CORE::Systems::RenderShapeSystem>();
		if (!renderShapeSystem)
		{
			VORTEK_ERROR("Failed to create the render Shape system!");
			return false;
		}

		if (!AddToContext<std::shared_ptr<VORTEK_CORE::Systems::RenderShapeSystem>>(renderShapeSystem))
		{
			VORTEK_ERROR("Failed to add the render Shape system to the registry context!");
			return false;
		}

		AddToContext<std::shared_ptr<VORTEK_CORE::Systems::PhysicsSystem>>(
			std::make_shared<VORTEK_CORE::Systems::PhysicsSystem>());

		AddToContext<std::shared_ptr<VORTEK_CORE::Systems::AnimationSystem>>(
			std::make_shared<VORTEK_CORE::Systems::AnimationSystem>());

		return true;
	}

	VORTEK_RESOURCES::AssetManager& MainRegistry::GetAssetManager()
	{
		VORTEK_ASSERT(m_bInitialized && "Main Registry must be initialized before use.");
		return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_RESOURCES::AssetManager>>();
	}

	VORTEK_SOUNDS::MusicPlayer& MainRegistry::GetMusicPlayer()
	{
		VORTEK_ASSERT(m_bInitialized && "Main Registry must be initialized before use.");
		return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_SOUNDS::MusicPlayer>>();
	}

	VORTEK_SOUNDS::SoundFxPlayer& MainRegistry::GetSoundPlayer()
	{
		VORTEK_ASSERT(m_bInitialized && "Main Registry must be initialized before use.");
		return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_SOUNDS::SoundFxPlayer>>();
	}

	VORTEK_CORE::Systems::RenderSystem& MainRegistry::GetRenderSystem()
	{
		VORTEK_ASSERT(m_bInitialized && "Main Registry must be initialized before use.");
		return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_CORE::Systems::RenderSystem>>();
	}

	VORTEK_CORE::Systems::RenderUISystem& MainRegistry::GetRenderUISystem()
	{
		VORTEK_ASSERT(m_bInitialized && "Main Registry must be initialized before use.");
		return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_CORE::Systems::RenderUISystem>>();
	}

	VORTEK_CORE::Systems::RenderShapeSystem& MainRegistry::GetRenderShapeSystem()
	{
		VORTEK_ASSERT(m_bInitialized && "Main Registry must be initialized before use.");
		return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_CORE::Systems::RenderShapeSystem>>();
	}

	VORTEK_CORE::Systems::AnimationSystem& MainRegistry::GetAnimationSystem()
	{
		VORTEK_ASSERT(m_bInitialized && "Main Registry must be initialized before use.");
		return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_CORE::Systems::AnimationSystem>>();
	}

	VORTEK_CORE::Systems::PhysicsSystem& MainRegistry::GetPhysicsSystem()
	{
		VORTEK_ASSERT(m_bInitialized && "Main Registry must be initialized before use.");
		return *m_pMainRegistry->GetContext<std::shared_ptr<VORTEK_CORE::Systems::PhysicsSystem>>();
	}

} // namespace VORTEK_CORE::ECS