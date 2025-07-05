#include "SceneHierarchyDisplay.h"
#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "editor/utilities/imgui/ImGuiUtils.h"

#include "editor/tools/ToolManager.h"

#include "Core/Scripting/InputManager.h"

#include "Core/ECS/MainRegistry.h"
#include "Core/ECS/Entity.h"
#include "Core/ECS/MetaUtilities.h"
#include "Core/ECS/Components/AllComponents.h"

#include "Core/Events/EventDispatcher.h"
#include "Core/Events/EngineEventTypes.h"
#include "editor/events/EditorEventTypes.h"

#include "Core/CoreUtilities/Prefab.h"
#include "Core/Resources/AssetManager.h"

#include <imgui.h>

using namespace VORTEK_CORE::Events;
using namespace VORTEK_CORE::ECS;
using namespace entt::literals;

namespace VORTEK_EDITOR
{

	bool SceneHierarchyDisplay::OpenTreeNode(VORTEK_CORE::ECS::Entity& entity)
	{
		const auto& name = entity.GetName();

		ImGui::PushID(static_cast<int32_t>(entity.GetEntity()));

		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		// Get a copy of the relationships of the current entity
		auto relations = entity.GetComponent<Relationship>();
		auto curr = relations.firstChild;

		// If the selected entity is the current node, highlight the selected node
		bool bSelected{ m_pSelectedEntity && m_pSelectedEntity->GetEntity() == entity.GetEntity() };
		if (bSelected)
			nodeFlags |= ImGuiTreeNodeFlags_Selected;

		bool bTreeNodeOpen{ false };

		bTreeNodeOpen = ImGui::TreeNodeEx(name.c_str(), nodeFlags);
		auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();

		if (ImGui::IsItemClicked())
		{
			m_pSelectedEntity = std::make_shared<Entity>(pCurrentScene->GetRegistry(), entity.GetEntity());
			TOOL_MANAGER().SetSelectedEntity(entity.GetEntity());
		}

		ImGui::PopID();

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("SceneHierarchy", &(entity.GetEntity()), sizeof(entity.GetEntity()));
			ImGui::Text(entity.GetName().c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SceneHierarchy");

			if (payload)
			{
				VORTEK_ASSERT(payload->DataSize == sizeof(entity.GetEntity()));
				entt::entity* ent = (entt::entity*)payload->Data;
				auto entID = static_cast<int32_t>(*ent);
				entity.AddChild(*ent);
			}

			ImGui::EndDragDropTarget();
		}

		while (curr != entt::null && bTreeNodeOpen)
		{
			Entity ent{ pCurrentScene->GetRegistry(), curr };
			if (OpenTreeNode(ent))
				ImGui::TreePop();

			curr = ent.GetComponent<Relationship>().nextSibling;
		}

		return bTreeNodeOpen;
	}

	void SceneHierarchyDisplay::AddComponent(VORTEK_CORE::ECS::Entity& entity, bool* bAddComponent)
	{
		if (!bAddComponent)
			return;

		if (*bAddComponent)
			ImGui::OpenPopup("Add Component");

		if (ImGui::BeginPopupModal("Add Component"))
		{
			auto& registry = entity.GetEnttRegistry();

			// Get all of the reflected types from entt::meta
			std::map<entt::id_type, std::string> componentMap;

			for (auto&& [id, type] : entt::resolve())
			{
				const auto& info = type.info();
				auto pos = info.name().find_last_of(':') + 1;
				auto name = info.name().substr(pos);
				componentMap[id] = std::string{ name };
			}

			static std::string componentStr{};
			static std::string componentStrPrev{};
			static entt::id_type id_type{ 0 };
			static bool bError{ false };

			if (ImGui::BeginCombo("Choose Component", componentStr.c_str()))
			{
				for (const auto& [id, name] : componentMap)
				{
					if (ImGui::Selectable(name.c_str(), name == componentStr))
					{
						componentStr = name;
						id_type = id;
					}
				}
				ImGui::EndCombo();
			}

			if (componentStr != componentStrPrev)
			{
				componentStrPrev = componentStr;
				bError = false;
			}

			if (bError)
			{
				ImGui::TextColored(
					ImVec4{ 1.f, 0.f, 0.f, 1.f },
					std::format("Game Object already has [{}] - Please make another selection.", componentStr).c_str());
			}

			if (ImGui::Button("Ok") && !bError)
			{
				if (!componentStr.empty())
				{
					for (auto&& [id, storage] : registry.storage())
					{
						if (id != id_type)
							continue;

						if (storage.contains(entity.GetEntity()))
						{
							bError = true;
							VORTEK_ERROR("Entity already has component [{}]", componentStr);
							break;
						}

						// The entity does not have that component yet, proceed.
						break;
					}
				}

				if (!bError && id_type != 0)
				{
					auto&& storage = registry.storage(id_type);
					if (!storage)
					{
						const auto addComponent =
							VORTEK_CORE::Utils::InvokeMetaFunction(id_type, "add_component_default"_hs, entity);

						if (addComponent)
						{
							EVENT_DISPATCHER().EmitEvent(Events::AddComponentEvent{
								.pEntity = &entity, .eType = Events::GetComponentTypeFromStr(componentStr) });

							*bAddComponent = false;
							ImGui::CloseCurrentPopup();
						}
						else
						{
							// This should probably fail/throw
							VORTEK_ASSERT(addComponent && "Failed to add component.");
							*bAddComponent = false;
							bError = true;
							ImGui::CloseCurrentPopup();
						}
					}
					else
					{
						storage->push(entity.GetEntity());

						EVENT_DISPATCHER().EmitEvent(Events::AddComponentEvent{
							.pEntity = &entity, .eType = Events::GetComponentTypeFromStr(componentStr) });

						*bAddComponent = false;
						ImGui::CloseCurrentPopup();
					}
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				bError = false;
				*bAddComponent = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void SceneHierarchyDisplay::DrawGameObjectDetails()
	{
		if (!ImGui::Begin("Object Details"))
		{
			ImGui::End();
			return;
		}

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Add Component"))
				m_bAddComponent = true;

			ImGui::EndPopup();
		}

		if (m_pSelectedEntity && m_bAddComponent)
		{
			AddComponent(*m_pSelectedEntity, &m_bAddComponent);
		}

		if (m_pSelectedEntity)
		{
			if (!m_pSelectedEntity->HasComponent<UneditableComponent>())
			{
				DrawEntityComponents();
			}
			else
			{
				DrawUneditableTypes();
			}
		}

		ImGui::End();
	}

	void SceneHierarchyDisplay::DrawUneditableTypes()
	{
		if (!m_pSelectedEntity)
			return;

		if (auto* pUnedit = m_pSelectedEntity->TryGetComponent<UneditableComponent>())
		{
			if (pUnedit->eType == EUneditableType::PlayerStart)
			{
				DrawPlayerStart();
			}
		}
	}

	void SceneHierarchyDisplay::DrawPlayerStart()
	{
		ImGui::SeparatorText("Player Start");
		ImGui::InlineLabel(ICON_FA_FLAG ICON_FA_GAMEPAD " Player Start Character: ");
		if (auto pCurrentScene = SCENE_MANAGER().GetCurrentScene())
		{
			std::string sPlayerStartCharacter{ pCurrentScene->GetPlayerStart().GetCharacterName() };
			ImGui::SetCursorPosX(225.f);
			ImGui::TextColored(ImVec4{ 0.7f, 0.7f, 0.7f, 1.f }, sPlayerStartCharacter.c_str());
		}
	}

	void SceneHierarchyDisplay::DrawEntityComponents()
	{
		auto& registry = m_pSelectedEntity->GetEnttRegistry();

		for (const auto&& [id, storage] : registry.storage())
		{
			if (!storage.contains(m_pSelectedEntity->GetEntity()))
				continue;

			// Do not draw tile or relationship components
			if (id == entt::type_hash<TileComponent>::value() || id == entt::type_hash<Relationship>::value())
				continue;

			const auto drawInfo =
				VORTEK_CORE::Utils::InvokeMetaFunction(id, "DrawEntityComponentInfo"_hs, *m_pSelectedEntity);

			if (drawInfo)
			{
				ImGui::Spacing();
				ImGui::PushID(id);
				if (ImGui::Button("remove"))
				{
					storage.remove(m_pSelectedEntity->GetEntity());
				}
				ImGui::PopID();
			}

			ImGui::Spacing();
			ImGui::Separator();
		}
	}

	bool SceneHierarchyDisplay::DeleteSelectedEntity()
	{
		VORTEK_ASSERT(m_pSelectedEntity && "Selected Entity must be valid if trying to delete!");

		if (auto pCurrentScene = SCENE_MANAGER().GetCurrentSceneObject())
		{
			if (m_pSelectedEntity->HasComponent<UneditableComponent>())
			{
				VORTEK_ERROR("Failed to delete selected entity. - Selected entity is uneditable.");
				return false;
			}

			if (!pCurrentScene->DeleteGameObjectById(m_pSelectedEntity->GetEntity()))
			{
				VORTEK_ERROR("Failed to delete selected entity.");
				return false;
			}

			m_pSelectedEntity = nullptr;
			SCENE_MANAGER().GetToolManager().SetSelectedEntity(entt::null);
		}
		else
		{
			VORTEK_ERROR("Trying to delete an entity with no active scene.");
			return false;
		}

		return true;
	}

	bool SceneHierarchyDisplay::DuplicateSelectedEntity()
	{
		VORTEK_ASSERT(m_pSelectedEntity && "Selected Entity must be valid if trying to duplicate!");

		if (auto pCurrentScene = SCENE_MANAGER().GetCurrentSceneObject())
		{
			if (!pCurrentScene->DuplicateGameObject(m_pSelectedEntity->GetEntity()))
			{
				VORTEK_ERROR("Failed to duplicate selected entity.");
				return false;
			}
		}
		else
		{
			VORTEK_ERROR("Trying to duplicate an entity with no active scene.");
			return false;
		}

		return true;
	}

	void SceneHierarchyDisplay::OnEntityChanged(VORTEK_EDITOR::Events::SwitchEntityEvent& swEntEvent)
	{
		if (!swEntEvent.pEntity)
		{
			VORTEK_ERROR("Failed to change entity. Entity was invalid.");
			return;
		}
		auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();
		if (!pCurrentScene)
		{
			VORTEK_ERROR("Failed to change entity. Current scene was invalid.");
			return;
		}

		// Ensure that the entity is valid
		if (!pCurrentScene->GetRegistry().IsValid(swEntEvent.pEntity->GetEntity()))
		{
			VORTEK_ERROR("Failed to change entity. Entity was invaild.");
			return;
		}

		m_pSelectedEntity =
			std::make_shared<VORTEK_CORE::ECS::Entity>(pCurrentScene->GetRegistry(), swEntEvent.pEntity->GetEntity());

		VORTEK_ASSERT(m_pSelectedEntity && "Entity must be valid here!");
	}

	void SceneHierarchyDisplay::OnKeyPressed(VORTEK_CORE::Events::KeyEvent& keyEvent)
	{
		if (!m_bWindowActive || keyEvent.eType == EKeyEventType::Released)
			return;

		if (m_pSelectedEntity)
		{
			auto& keyboard = INPUT_MANAGER().GetKeyboard();
			if (keyboard.IsKeyPressed(VORTEK_KEY_RCTRL) || keyboard.IsKeyPressed(VORTEK_KEY_LCTRL))
			{
				if (keyEvent.key == VORTEK_KEY_D)
				{
					DuplicateSelectedEntity();
				}
			}
			else
			{
				if (keyEvent.key == VORTEK_KEY_DELETE)
				{
					DeleteSelectedEntity();
				}
			}
		}
	}

	void SceneHierarchyDisplay::OnAddComponent( VORTEK_EDITOR::Events::AddComponentEvent& addCompEvent )
	{
		if ( addCompEvent.pEntity && addCompEvent.eType == Events::EComponentType::Text )
		{
			if ( !addCompEvent.pEntity->HasComponent<UIComponent>() )
			{
				addCompEvent.pEntity->AddComponent<UIComponent>();
			}
		}
	}

	void SceneHierarchyDisplay::OpenContext(SceneObject* pCurrentScene)
	{
		if (!pCurrentScene)
			return;

		if (m_pSelectedEntity)
		{
			if (ImGui::BeginPopupContextWindow())
			{
				// Uneditable entities cannot be made into prefabs
				if (!m_pSelectedEntity->HasComponent<UneditableComponent>())
				{
					if (ImGui::Selectable("Create Prefab"))
					{
						auto newPrefab = VORTEK_CORE::PrefabCreator::CreatePrefab(VORTEK_CORE::EPrefabType::Character,
							*m_pSelectedEntity);
						ASSET_MANAGER().AddPrefab(m_pSelectedEntity->GetName() + "_pfab", std::move(newPrefab));
					}
				}

				if (ImGui::Selectable("Duplicate"))
				{
					if (!DuplicateSelectedEntity())
					{
						VORTEK_ERROR("Failed to duplicated the selected entity.");
					}
				}

				if (ImGui::Selectable("Delete"))
				{
					if (!DeleteSelectedEntity())
					{
						VORTEK_ERROR("Failed to delete the selected entity.");
					}
				}

				ImGui::EndPopup();
			}
		}
		else
		{
			if (ImGui::BeginPopupContextWindow("##SceneHierarchyContext"))
			{
				if (ImGui::Selectable("Add New Game Object"))
				{
					if (!pCurrentScene->AddGameObject())
					{
						VORTEK_ERROR("Failed to add new game object to scene [{}]", pCurrentScene->GetName());
					}
				}

				ImGui::EndPopup();
			}
		}

		if ( m_bWindowActive && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() &&
			(ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right)))
		{
			m_pSelectedEntity = nullptr;
		}
	}

	SceneHierarchyDisplay::SceneHierarchyDisplay()
	{
		ADD_SWE_HANDLER(Events::SwitchEntityEvent, &SceneHierarchyDisplay::OnEntityChanged, *this);
		ADD_EVENT_HANDLER(KeyEvent, &SceneHierarchyDisplay::OnKeyPressed, *this);
		ADD_EVENT_HANDLER( Events::AddComponentEvent, &SceneHierarchyDisplay::OnAddComponent, *this )
	}

	SceneHierarchyDisplay::~SceneHierarchyDisplay() = default;

	void SceneHierarchyDisplay::Update()
	{
	}

	void SceneHierarchyDisplay::Draw()
	{
		auto pCurrentScene = SCENE_MANAGER().GetCurrentSceneObject();
		if (!ImGui::Begin(ICON_FA_TREE " Scene Hierarchy") || !pCurrentScene)
		{
			ImGui::End();
			return;
		}

		m_bWindowActive = ImGui::IsWindowFocused();

		OpenContext(pCurrentScene);

		ImGui::Separator();
		ImGui::AddSpaces(1);
		ImGui::InlineLabel(ICON_FA_SEARCH, 32.f);
		m_TextFilter.Draw("##search_filter");
		ImGui::AddSpaces(1);
		ImGui::Separator();

		auto& registry = pCurrentScene->GetRegistry();
		auto sceneEntities = registry.GetRegistry().view<entt::entity>(entt::exclude<TileComponent, ScriptComponent>);

		for (auto entity : sceneEntities)
		{
			Entity ent{ registry, entity };
			if (!m_TextFilter.PassFilter(ent.GetName().c_str()))
				continue;

			// Do not redraw the children
			const auto& relations = ent.GetComponent<Relationship>();
			if (relations.parent == entt::null)
			{
				if (OpenTreeNode(ent))
				{
					ImGui::TreePop();
				}
			}
		}

		ImGui::End();

		DrawGameObjectDetails();
	}
} // namespace VORTEK_EDITOR
