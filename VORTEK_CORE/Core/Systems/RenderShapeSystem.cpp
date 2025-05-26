#include "RenderShapeSystem.h"
#include "../ECS/Components/BoxColliderComponent.h"
#include "../ECS/Components/CircleColliderComponent.h"
#include "../ECS/Components/TransformComponent.h"
#include "../ECS/Components/PhysicsComponent.h"
#include "../ECS/MainRegistry.h"
#include "../Resources/AssetManager.h"
#include "../CoreUtilities/CoreEngineData.h"
#include "../CoreUtilities/CoreUtilities.h"

#include <Rendering/Core/Camera2D.h>
#include <Rendering/Essentials/Primitives.h>
#include <Rendering/Core/RectBatchRenderer.h>
#include <Rendering/Core/CircleBatchRenderer.h>

using namespace VORTEK_CORE::ECS;
using namespace VORTEK_RENDERING;
using namespace VORTEK_RESOURCES;

namespace VORTEK_CORE::Systems
{

	RenderShapeSystem::RenderShapeSystem()
		: m_pRectRenderer{ std::make_unique<RectBatchRenderer>() }
		, m_pCircleRenderer{ std::make_unique<CircleBatchRenderer>() }
	{
	}

	void RenderShapeSystem::Update(VORTEK_CORE::ECS::Registry& registry, VORTEK_RENDERING::Camera2D& camera)
	{

		auto& assetManager = MAIN_REGISTRY().GetAssetManager();

		auto colorShader = assetManager.GetShader("color");
		auto cam_mat = camera.GetCameraMatrix();

		colorShader->Enable();
		colorShader->SetUniformMat4("uProjection", cam_mat);
		m_pRectRenderer->Begin();

		auto boxView = registry.GetRegistry().view<TransformComponent, BoxColliderComponent>();
		for (auto entity : boxView)
		{
			const auto& transform = boxView.get<TransformComponent>(entity);
			const auto& boxCollider = boxView.get<BoxColliderComponent>(entity);

			if (!VORTEK_CORE::EntityInView(transform,
				static_cast<float>(boxCollider.width),
				static_cast<float>(boxCollider.height),
				camera))
				continue;

			glm::mat4 model = VORTEK_CORE::RSTModel(transform, boxCollider.width, boxCollider.height);

			auto color = Color{ 255, 0, 0, 135 };

			if (registry.GetRegistry().all_of<PhysicsComponent>(entity))
			{
				auto& physics = registry.GetRegistry().get<PhysicsComponent>(entity);
				if (physics.IsSensor())
					color = Color{ 0, 255, 0, 135 };
			}

			Rect rect{ .position = glm::vec2{ transform.position.x + boxCollider.offset.x,
											  transform.position.y + boxCollider.offset.y },
					   .width = static_cast<float>(boxCollider.width),
					   .height = static_cast<float>(boxCollider.height),
					   .color = color };

			m_pRectRenderer->AddRect(rect, model);
		}

		m_pRectRenderer->End();
		m_pRectRenderer->Render();
		colorShader->Disable();

		auto circleShader = assetManager.GetShader("circle");

		circleShader->Enable();
		circleShader->SetUniformMat4("uProjection", cam_mat);
		m_pCircleRenderer->Begin();

		auto circleView = registry.GetRegistry().view<TransformComponent, CircleColliderComponent>();
		for (auto entity : circleView)
		{
			const auto& transform = circleView.get<TransformComponent>(entity);
			const auto& circleCollider = circleView.get<CircleColliderComponent>(entity);

			glm::vec4 circle{ transform.position.x + circleCollider.offset.x,
							  transform.position.y + circleCollider.offset.y,
							  circleCollider.radius * transform.scale.x * 2,
							  circleCollider.radius * transform.scale.y * 2 };

			m_pCircleRenderer->AddCircle(circle, Color{ 0, 255, 0, 135 }, 1.f);
		}

		m_pCircleRenderer->End();
		m_pCircleRenderer->Render();
		circleShader->Disable();
	}
} // namespace VORTEK_CORE::Systems