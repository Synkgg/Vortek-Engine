#include "VortekFilesystem/Process/FileProcessor.h"
#include "VortekUtilities/VortekUtilities.h"
#include "Logger/Logger.h"

#include <Windows.h>
#include <shellapi.h>
#include <fmt/format.h>

using namespace VORTEK_UTIL;

namespace VORTEK_FILESYSTEM
{
bool FileProcessor::OpenApplicationFromFile( const std::string& sFilename, std::vector<const char*> params )
{
	std::string sParams = SeparateParams( params );

	INT_PTR shell =
		reinterpret_cast<INT_PTR>( ::ShellExecuteW( NULL,
													L"open",
													ConvertAnsiToWide( sFilename ).c_str(),
													!sParams.empty() ? ConvertAnsiToWide( sParams ).c_str() : L"",
													L"",
													SW_SHOWNORMAL ) );

	// If there is no default program set for the file type, prompt the user to choose an application.
	if ( shell == SE_ERR_NOASSOC || shell == SE_ERR_ASSOCINCOMPLETE )
	{
		shell = reinterpret_cast<INT_PTR>(
			::ShellExecuteW( NULL,
							 L"open",
							 L"RUNDLL32.EXE",
							 ConvertAnsiToWide( fmt::format( "shell32.dll, OpenAs_RunDLL {}", sFilename ) ).c_str(),
							 L"",
							 SW_SHOWNORMAL ) );
	}

	// If the shell code us greater that 32 == SUCCESS
	if ( shell >= 32 )
	{
		return true;
	}

	DWORD error{ GetLastError() };
	VORTEK_ERROR( "Failed to open file [{}]. Error: {}", sFilename, error );
	return true;
}

} // namespace VORTEK_FILESYSTEM
