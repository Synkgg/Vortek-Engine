#pragma once
#include "IDisplay.h"
#include <imgui.h>

namespace VORTEK_CORE::ECS
{
	class Entity;
}

namespace VORTEK_EDITOR
{
	class SceneHierarchyDisplay : public IDisplay
	{
	private:
		std::shared_ptr<VORTEK_CORE::ECS::Entity> m_pSelectedEntity{ nullptr };
		ImGuiTextFilter m_TextFilter;
		bool m_bAddComponent{ false };

	private:
		bool OpenTreeNode(VORTEK_CORE::ECS::Entity& entity);

		void AddComponent(VORTEK_CORE::ECS::Entity& entity, bool* bAddComponent);
		void DrawGameObjectDetails();
		void DrawEntityComponents();

	public:
		SceneHierarchyDisplay();
		~SceneHierarchyDisplay();

		virtual void Update() override;
		virtual void Draw() override;
	};
} // namespace VORTEK_EDITOR