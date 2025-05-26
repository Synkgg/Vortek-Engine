#include "RenderUISystem.h"
#include "../ECS/Components/TextComponent.h"
#include "../ECS/Components/TransformComponent.h"
#include "../ECS/MainRegistry.h"
#include "../Resources/AssetManager.h"
#include "../CoreUtilities/CoreEngineData.h"
#include "../CoreUtilities/CoreUtilities.h"

#include <Rendering/Essentials/Font.h>
#include <Rendering/Core/BatchRenderer.h>
#include <Rendering/Core/TextBatchRenderer.h>
#include <Rendering/Core/Camera2D.h>

#include <Logger/Logger.h>

using namespace VORTEK_CORE::ECS;
using namespace VORTEK_RESOURCES;

namespace VORTEK_CORE::Systems
{

	RenderUISystem::RenderUISystem()
		: m_pSpriteRenderer{ std::make_unique<VORTEK_RENDERING::SpriteBatchRenderer>() }
		, m_pTextRenderer{ std::make_unique<VORTEK_RENDERING::TextBatchRenderer>() }
		, m_pCamera2D{ nullptr }
	{
		auto& coreEngine = CoreEngineData::GetInstance();

		m_pCamera2D = std::make_unique<VORTEK_RENDERING::Camera2D>(coreEngine.WindowWidth(), coreEngine.WindowHeight());

		m_pCamera2D->Update();
	}

	RenderUISystem::~RenderUISystem()
	{
	}

	void RenderUISystem::Update(VORTEK_CORE::ECS::Registry& registry)
	{
		auto& mainRegistry = MAIN_REGISTRY();

		// If there are no entities in the view, leave
		auto textView = registry.GetRegistry().view<TextComponent, TransformComponent>();
		if (textView.size_hint() < 1)
			return;

		auto& assetManager = mainRegistry.GetAssetManager();
		auto pFontShader = assetManager.GetShader("font");

		if (!pFontShader)
		{
			VORTEK_ERROR("Failed to get the font shader from the asset manager!");
			return;
		}

		auto cam_mat = m_pCamera2D->GetCameraMatrix();

		pFontShader->Enable();
		pFontShader->SetUniformMat4("uProjection", cam_mat);

		m_pTextRenderer->Begin();

		for (auto entity : textView)
		{
			const auto& text = textView.get<TextComponent>(entity);

			if (text.sFontName.empty() || text.bHidden)
				continue;

			const auto& pFont = assetManager.GetFont(text.sFontName);
			if (!pFont)
			{
				VORTEK_ERROR("Font [{}] does not exist in the asset manager!", text.sFontName);
				continue;
			}

			const auto& transform = textView.get<TransformComponent>(entity);
			const auto fontSize = pFont->GetFontSize();

			glm::mat4 model = VORTEK_CORE::RSTModel(transform, fontSize, fontSize);
			m_pTextRenderer->AddText(
				text.sTextStr, pFont, transform.position, text.padding, text.wrap, text.color, model);
		}

		m_pTextRenderer->End();
		m_pTextRenderer->Render();

		pFontShader->Disable();
	}
} // namespace VORTEK_CORE::Systems