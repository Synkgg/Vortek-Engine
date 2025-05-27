#pragma once
#include <map>
#include <memory>
#include <string>
#include <entt.hpp>


namespace VORTEK_RENDERING
{
	class Camera2D;
}

namespace VORTEK_EDITOR
{

	struct Canvas;
	class AbstractTool;
	class TileTool;
	class Gizmo;
	class SceneObject;
	enum class EToolType;
	enum class EGizmoType;

	class ToolManager
	{
	private:
		std::map<EToolType, std::unique_ptr<TileTool>> m_mapTools;
		std::map<EGizmoType, std::unique_ptr<Gizmo>> m_mapGizmos;

		EToolType m_eActiveToolType;
		EGizmoType m_eActiveGizmoType;

	public:
		ToolManager();
		~ToolManager() = default;

		void Update(Canvas& canvas);

		void SetToolActive(EToolType eToolType);
		void SetGizmoActive(EGizmoType eGizmoType);

		TileTool* GetActiveTool();

		Gizmo* GetActiveGizmo();

		AbstractTool* GetActiveToolFromAbstract();

		bool SetupTools(SceneObject* pSceneObject, VORTEK_RENDERING::Camera2D* pCamera);

		void SetToolsCurrentTileset(const std::string& sTileset);

		void SetTileToolStartCoords(int x, int y);

		inline EToolType GetActiveToolType() const { return m_eActiveToolType; }
		inline EGizmoType GetActiveGizmoType() const { return m_eActiveGizmoType; }

		void SetSelectedEntity(entt::entity entity);

		void EnableGridSnap(bool bEnable);
	};
} // namespace VORTEK_EDITOR