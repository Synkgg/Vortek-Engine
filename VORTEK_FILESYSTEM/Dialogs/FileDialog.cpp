#include "FileDialog.h"
#include <tinyfiledialogs/tinyfiledialogs.h>

namespace VORTEK_FILESYSTEM
{

	std::string FileDialog::OpenFileDialog(const std::string& sTitle, const std::string& sDefaultPath,
		const std::vector<const char*>& filters, const std::string& sFilterDesc)
	{
		const char* file = tinyfd_openFileDialog(
			sTitle.c_str(), sDefaultPath.c_str(), filters.size(), filters.data(), sFilterDesc.c_str(), 1);

		if (!file)
			return std::string{};

		return std::string{ file };
	}

	std::string FileDialog::SaveFileDialog(const std::string& sTitle, const std::string& sDefaultPath,
		const std::vector<const char*>& filters, const std::string& sFilterDesc)
	{
		const char* file = tinyfd_saveFileDialog(
			sTitle.c_str(), sDefaultPath.c_str(), filters.size(), filters.data(), sFilterDesc.c_str());

		if (!file)
			return std::string{};

		return std::string{ file };
	}

	std::string FileDialog::SelectFolderDialog(const std::string& sTitle, const std::string& sDefaultPath)
	{
		const char* folder = tinyfd_selectFolderDialog(sTitle.c_str(), sDefaultPath.c_str());

		if (!folder)
			return std::string{};

		return std::string{ folder };
	}
} // namespace VORTEK_FILESYSTEM