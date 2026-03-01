#pragma once
#include "IDisplay.h"

namespace Vortek::Editor::Events
{
enum class EFileAction;
enum class EContentCreateAction;
struct FileEvent;
struct ContentCreateEvent;
struct KeyPressedEvent;
} // namespace Vortek::Editor::Events

namespace Vortek::Core::Events
{
class EventDispatcher;
}

namespace Vortek::Editor
{
class ContentDisplay : public IDisplay
{
  public:
	ContentDisplay();
	virtual ~ContentDisplay();

	virtual void Update() override;
	virtual void Draw() override;

  private:
	virtual void DrawToolbar() override;

	void CopyDroppedFile( const std::string& sFileToCopy, const std::filesystem::path& droppedPath );
	void MoveFolderOrFile( const std::filesystem::path& movedPath, const std::filesystem::path& path );
	void HandleFileEvent( const Vortek::Editor::Events::FileEvent& fileEvent );
	void HandleCreateEvent( const Vortek::Editor::Events::ContentCreateEvent& createEvent );
	void HandlePopups();

	void OpenDeletePopup();
	void OpenCreateFolderPopup();

	void OpenCreateLuaClassPopup();
	void OpenCreateLuaStateClassPopup();
	void OpenCreateLuaTablePopup();
	void OpenCreateEmptyLuaFilePopup();

  private:
	std::unique_ptr<Vortek::Core::Events::EventDispatcher> m_pFileDispatcher;
	std::filesystem::path m_CurrentDir;
	std::string m_sFilepathToAction;
	int m_Selected;

	Vortek::Editor::Events::EFileAction m_eFileAction;
	Vortek::Editor::Events::EContentCreateAction m_eCreateAction;

	bool m_bItemCut;
	bool m_bWindowHovered;

};
} // namespace Vortek::Editor
