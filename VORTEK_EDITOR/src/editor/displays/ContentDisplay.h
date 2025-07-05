#pragma once
#include "IDisplay.h"
#include <string>
#include <filesystem>

namespace VORTEK_EDITOR::Events
{
	enum class EFileAction;
	enum class EContentCreateAction;
	struct FileEvent;
	struct ContentCreateEvent;
	struct KeyPressedEvent;
} // namespace VORTEK_EDITOR::Events

namespace VORTEK_CORE::Events
{
	class EventDispatcher;
}

namespace VORTEK_EDITOR
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

		void CopyDroppedFile(const std::string& sFileToCopy, const std::filesystem::path& droppedPath);
		void MoveFolderOrFile(const std::filesystem::path& movedPath, const std::filesystem::path& path);
		void HandleFileEvent(const VORTEK_EDITOR::Events::FileEvent& fileEvent);
		void HandleCreateEvent(const VORTEK_EDITOR::Events::ContentCreateEvent& createEvent);
		void HandlePopups();

		void OpenDeletePopup();
		void OpenCreateFolderPopup();

		void OpenCreateLuaClassPopup();
		void OpenCreateLuaTablePopup();
		void OpenCreateEmptyLuaFilePopup();

	private:
		std::unique_ptr<VORTEK_CORE::Events::EventDispatcher> m_pFileDispatcher;
		std::filesystem::path m_CurrentDir;
		std::string m_sFilepathToAction;
		int m_Selected;

		VORTEK_EDITOR::Events::EFileAction m_eFileAction;
		VORTEK_EDITOR::Events::EContentCreateAction m_eCreateAction;

		bool m_bItemCut;
		bool m_bWindowHovered;

	};
} // namespace VORTEK_EDITOR
