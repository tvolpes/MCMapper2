// windows only
#include <shlobj.h>

#include <iostream>
#include <string>

#include <boost\filesystem.hpp>
#include "MCMapper.h"
#include "rendererbasic.h"

#pragma comment(lib, "libpng.lib")

CMCMapper::CMCMapper()
{
}

bool CMCMapper::initialize()
{
	return true;
}
void CMCMapper::shutdown()
{
}

bool CMCMapper::run( int argc, char* argv[] )
{
	char *pRenderMode, *pPathMod, *pPath;
	char appdata[260]; // 260 = MAX_PATH
	boost::filesystem::path fullPath;
	CRenderer *pRenderer;

	// We need atleast two arguments
	std::cout << std::endl;
	if( argc < 2 )
	{
		std::cout << "The following syntax is valid: <MODE> <PATHMODIFIER> <PATH>" << std::endl;
		std::cout << "\tThe following MODEs are valid: bsc" << std::endl;
		std::cout << "\tThe following PATHMODIFIERs are valid: mc, fs" << std::endl;
		std::cout << "\t\tmc: search for map in saves folder" << std::endl;
		std::cout << "\t\tfs: treat path as the full path" << std::endl;
		std::cout << "\tEnter a mode without a path for more information" << std::endl;
		return true;
	}
	else if( argc == 2 )
	{
		// Get the render mode
		pRenderMode = argv[1];
		// Make it lowercase
		*pRenderMode = tolower( *pRenderMode );

		// Basic render mode
		if( strcmp( pRenderMode, "bsc" ) == 0 ) {
			std::cout << "\tBasic render mode, renders map with no special modifiers" << std::endl;
			return true;
		}
	}
	else if( argc == 3 ) {
		std::cout << "Invalid arguments; run the program with no arguments to see help" << std::endl;
		return true;
	}
	else
	{
		// Get the render mode
		pRenderMode = argv[1];
		// Make it lowercase
		*pRenderMode = tolower( *pRenderMode );
		// Get the path modifier
		pPathMod = argv[2];
		// Make it lowercase
		*pPathMod = tolower( *pPathMod );
		// Get the path
		pPath = argv[3];

		// Construct the map path
		if( strcmp( pPathMod, "-mc" ) == 0 ) {
			SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appdata );
			fullPath = appdata;
			fullPath /= ".minecraft\\saves";
			fullPath /= pPath;
		}
		else if( strcmp( pPathMod, "-fs" ) == 0 ) {
			fullPath = pPath;
		}
		else {
			std::cout << "\tEntered invalid path modifier; run the program with no arguments to see help" << std::endl;
			return true;
		}

		std::cout << "\tMap path: " << fullPath.string().c_str() << std::endl;

		// Use the correct command
		if( strcmp( pRenderMode, "bsc" ) == 0 )
		{
			pRenderer = new CRendererBasic();
			if( !pRenderer->create( fullPath ) )
				return false;
			std::cout << "\t > Rendering map using basic renderer" << std::endl;
			if( !pRenderer->render() )
				return false;
			pRenderer->destroy();
			if( pRenderer ) {
				delete pRenderer;
				pRenderer = NULL;
			}
		}
		else {
			std::cout << "\tEntered invalid render mode; run the program with no arguments to see help" << std::endl;
			return true;
		}
	}

	return true;
}