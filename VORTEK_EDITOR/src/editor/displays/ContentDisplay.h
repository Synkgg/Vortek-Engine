#pragma once
#include "IDisplay.h"
#include <string>
#include <filesystem>

namespace VORTEK_EDITOR::Events
{
	enum class EFileAction;
	struct FileEvent;
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
		void OpenDeletePopup();

	private:
		std::unique_ptr<VORTEK_CORE::Events::EventDispatcher> m_pFileDispatcher;
		std::filesystem::path m_CurrentDir;
		std::string m_sFilepathToAction;
		int m_Selected;

		VORTEK_EDITOR::Events::EFileAction m_eFileAction;
		bool m_bItemCut, m_bWindowHovered;
	};
} // namespace VORTEK_EDITOR