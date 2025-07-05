#pragma once
#include <functional>
#include <filesystem>
#include <memory>

namespace VORTEK_FILESYSTEM
{
class DirectoryWatcher
{
  public:
	using Callback = std::function<void( const std::filesystem::path&, bool )>;

	DirectoryWatcher( const std::filesystem::path& path, Callback callback );
	~DirectoryWatcher();

  private:
	struct Impl;
	std::unique_ptr<Impl> m_pImpl;
};
} // namespace VORTEK_FILESYSTEM
