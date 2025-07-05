#include "VORTEKFilesystem/Process/FileProcessor.h"
#include "VortekUtilities/VortekUtilities.h"

#include "Logger/Logger.h"

#include <unistd.h>
#include <errno.h>

using namespace VORTEK_UTIL;

namespace VORTEK_FILESYSTEM
{
bool FileProcessor::OpenApplicationFromFile( const std::string& sFilename, std::vector<const char*> params )
{
	// Separate any passed in parameters by a space and create a string
	std::string sParams = SeparateParams( params );

	// Create a new child process
	pid_t pid = fork();

	bool bSuccess{ true };

	// Check if the process id == 0, then try to open the file with it's default application
	if ( pid == 0 )
	{
		int error{ 0 };
		if ( !sParams.empty() )
		{
			error = execl( "/usr/bin/xdg-open", "xdg-open", sFilename.c_str(), sParams.c_str(), (char*)0 );
		}
		else
		{
			error = execl( "/usr/bin/xdg-open", "xdg-open", sFilename.c_str(), (char*)0 );
		}

		// Check for success
		exit( error );
	}

	// Log the error
	if ( pid == -1 )
	{
		// TODO: Get the correct reason/error
		VORTEK_ERROR( "Failed to open file [{}] in default application. Error: {}", sFilename, strerror( errno ) );
		bSuccess = false;
	}

	return bSuccess;
}

} // namespace VORTEK_FILESYSTEM
