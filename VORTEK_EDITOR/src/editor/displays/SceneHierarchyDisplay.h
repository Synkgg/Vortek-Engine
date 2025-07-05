#pragma once
#include "IDisplay.h"
#include <imgui.h>

namespace VORTEK_CORE::ECS
{
class Entity;
}

namespace VORTEK_EDITOR::Events
{
struct SwitchEntityEvent;
struct AddComponentEvent;
} // namespace VORTEK_EDITOR::Events

namespace VORTEK_CORE::Events
{
struct KeyEvent;
}

namespace VORTEK_EDITOR
{
class SceneHierarchyDisplay : public IDisplay
{
  public:
	SceneHierarchyDisplay();
	~SceneHierarchyDisplay();

	virtual void Update() override;
	virtual void Draw() override;

  private:
	bool OpenTreeNode( VORTEK_CORE::ECS::Entity& entity );

	void AddComponent( VORTEK_CORE::ECS::Entity& entity, bool* bAddComponent );
	void DrawGameObjectDetails();
	void DrawUneditableTypes();
	void DrawPlayerStart();
	void DrawEntityComponents();
	bool DeleteSelectedEntity();
	bool DuplicateSelectedEntity();

	void OnEntityChanged( VORTEK_EDITOR::Events::SwitchEntityEvent& swEntEvent );
	void OnKeyPressed( VORTEK_CORE::Events::KeyEvent& keyEvent );
	void OnAddComponent( VORTEK_EDITOR::Events::AddComponentEvent& addCompEvent );

	void OpenContext( class SceneObject* pCurrentScene );

  private:
	std::shared_ptr<VORTEK_CORE::ECS::Entity> m_pSelectedEntity{ nullptr };
	ImGuiTextFilter m_TextFilter;
	bool m_bAddComponent{ false };
	bool m_bWindowActive{ false };
};
} // namespace VORTEK_EDITOR
