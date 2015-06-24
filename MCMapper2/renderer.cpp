#include <iostream>
#include "renderer.h"
#include "nbt.h"

CRenderer::CRenderer() {
}
CRenderer::~CRenderer() {
}

bool CRenderer::create( boost::filesystem::path fullPath )
{
	boost::filesystem::path datPath;

	// Confirm that the map exists and is valid
	datPath = fullPath / "level.dat";
	m_regionFolder = fullPath / "region";
	try
	{
		if( !boost::filesystem::is_regular_file( datPath ) ) {
			std::cout << "\t > Failed: could not find data file" << std::endl;
			return false;
		}
		else
			std::cout << "\t > Found map level data file" << std::endl;
		if( !boost::filesystem::is_directory( m_regionFolder ) ) {
			std::cout << "\t > Failed: could not find region folder" << std::endl;
			return false;
		}
		else
			std::cout << "\t > Found map region folder" << std::endl;
	}
	catch( const boost::filesystem::filesystem_error &e ) {
		std::cout << "\t > Failed: could not find or use 1 or more files" << std::endl;
		std::cout << "\t\t\"" << e.what() << "\"" << std::endl;
		return false;
	}

	// Read the maps data file
	if( !this->readDataFile( datPath ) )
		return false;

	return true;
}
void CRenderer::destroy()
{
}

bool CRenderer::readDataFile( boost::filesystem::path datPath )
{
	CTagReader *pTagReader;

	// Read the level data with the tag reader
	pTagReader = new CTagReader();
	if( !pTagReader->open( datPath ) ) {
		delete pTagReader;
		return false;
	}
	pTagReader->close();
	
	// Do stuff...

	// Clean up
	if( pTagReader ) {
		delete pTagReader;
		pTagReader = NULL;
	}

	return true;
}

boost::filesystem::path CRenderer::getPath() {
	return m_fullPath;
}