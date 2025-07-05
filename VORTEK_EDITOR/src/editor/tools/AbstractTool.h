#pragma once
#include <glm/glm.hpp>

namespace VORTEK_RENDERING
{
class Camera2D;
}

namespace VORTEK_CORE
{
struct Canvas;
namespace ECS
{
class Registry;
}
} // namespace VORTEK_CORE

namespace VORTEK_EDITOR
{

class SceneObject;

class AbstractTool
{
  private:
	glm::vec2 m_MouseScreenCoords, m_MouseWorldCoords;
	glm::vec2 m_GUICursorCoords, m_GUIRelativeCoords;
	glm::vec2 m_WindowPos, m_WindowSize;

	bool m_bActivated;
	bool m_bOutOfBounds;
	bool m_bOverTilemapWindow;

  private:
	void UpdateMouseWorldCoords();
	void CheckOutOfBounds( const VORTEK_CORE::Canvas& canvas );

  protected:
	enum class EMouseButton
	{
		UNKNOWN = 0,
		LEFT,
		MIDDLE,
		RIGHT,

		LAST
	};

	VORTEK_CORE::ECS::Registry* m_pRegistry{ nullptr };
	VORTEK_RENDERING::Camera2D* m_pCamera{ nullptr };
	SceneObject* m_pCurrentScene{ nullptr };
	glm::vec2 m_GridCoords;

  protected:
	bool MouseBtnJustPressed( EMouseButton eButton );
	bool MouseBtnJustReleased( EMouseButton eButton );
	bool MouseBtnPressed( EMouseButton eButton );
	bool MouseMoving();

	/*
	 * @brief Used for different tools to make adjustments to the mouse position if necessary.
	 */
	virtual void ExamineMousePosition() = 0;
	virtual void ResetSelectedEntity() { /* Override if tool uses selected entities */ }
	inline void SetMouseWorldCoords( const glm::vec2& newCoords ) { m_MouseWorldCoords = newCoords; }

  public:
	AbstractTool();
	virtual ~AbstractTool() = default;

	/*
	 * @brief Update the tool. If overrided, ensure to call the parent update. This will make sure the
	 * mouse world coords are updated accordingly.
	 */
	virtual void Update( VORTEK_CORE::Canvas& canvas );

	/*
	 * @brief Sets the current registry to the registry of the current scene and the camera.
	 */
	bool SetupTool( SceneObject* pSceneObject, VORTEK_RENDERING::Camera2D* pCamera );

	inline void SetRelativeCoords( const glm::vec2& relativeCoords ) { m_GUIRelativeCoords = relativeCoords; }
	inline void SetCursorCoords( const glm::vec2& cursorCoords ) { m_GUICursorCoords = cursorCoords; }
	inline void SetWindowPos( const glm::vec2& windowPos ) { m_WindowPos = windowPos; }
	inline void SetWindowSize( const glm::vec2& windowSize ) { m_WindowSize = windowSize; }

	inline const glm::vec2& GetMouseScreenCoords() const { return m_MouseScreenCoords; }
	inline const glm::vec2& GetMouseWorldCoords() const { return m_MouseWorldCoords; }
	inline const glm::vec2& GetGridCoords() const { return m_GridCoords; }
	inline const glm::vec2& GetWindowSize() const { return m_WindowSize; }

	inline void Activate() { m_bActivated = true; }
	inline void Deactivate() { m_bActivated = false; }
	inline void SetOverTilemapWindow( bool bOverWindow ) { m_bOverTilemapWindow = bOverWindow; }

	inline const bool IsActivated() const { return m_bActivated; }
	inline const bool OutOfBounds() const { return m_bOutOfBounds; }
	inline const bool IsOverTilemapWindow() const { return m_bOverTilemapWindow; }
};
} // namespace VORTEK_EDITOR
