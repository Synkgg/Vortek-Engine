#pragma once
#include "IDisplay.h"
#include <imgui.h>

namespace Vortek::Core::ECS
{
class Entity;
}

namespace Vortek::Editor::Events
{
struct SwitchEntityEvent;
struct AddComponentEvent;
}

namespace Vortek::Core::Events
{
struct KeyEvent;
}

namespace Vortek::Editor
{
class SceneHierarchyDisplay : public IDisplay
{
  public:
	SceneHierarchyDisplay();
	~SceneHierarchyDisplay();

	virtual void Update() override;
	virtual void Draw() override;

  private:
	bool OpenTreeNode( Vortek::Core::ECS::Entity& entity );

	void AddComponent( Vortek::Core::ECS::Entity& entity, bool* bAddComponent );
	void DrawGameObjectDetails();
	void DrawUneditableTypes();
	void DrawPlayerStart();
	void DrawEntityComponents();
	bool DeleteSelectedEntity();
	bool DuplicateSelectedEntity();

	void OnEntityChanged( Vortek::Editor::Events::SwitchEntityEvent& swEntEvent );
	void OnKeyPressed( Vortek::Core::Events::KeyEvent& keyEvent );
	void OnAddComponent( Vortek::Editor::Events::AddComponentEvent& addCompEvent );

	void OpenContext( class SceneObject* pCurrentScene );
	
  private:
	std::shared_ptr<Vortek::Core::ECS::Entity> m_pSelectedEntity{ nullptr };
	ImGuiTextFilter m_TextFilter;
	bool m_bAddComponent{ false };
	bool m_bWindowActive{ false };
};
} // namespace Vortek::Editor
