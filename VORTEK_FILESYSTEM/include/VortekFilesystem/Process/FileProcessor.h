#pragma once
#include <string>
#include <vector>

namespace VORTEK_FILESYSTEM
{
class FileProcessor
{
  public:
	bool OpenApplicationFromFile( const std::string& sFilename, std::vector<const char*> params );

  private:
	std::string SeparateParams( std::vector<const char*>& params )
	{
		std::string sParams{};
		for ( const auto& param : params )
			sParams += param + ' ';

		return sParams;
	}
};
} // namespace VORTEK_FILESYSTEM
