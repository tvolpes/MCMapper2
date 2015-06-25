/*
MCMapper2 Minecraft Mapping Utility
Copyright( C ) 2015 Timothy Volpe

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
( at your option ) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.

*/

#include <iostream>
#include <algorithm>
#include "def.h"
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
	m_fullPath = "";
	m_regionFolder = "";
}

bool CRenderer::readDataFile( boost::filesystem::path datPath )
{
	CTagReader *pTagReader;
	CTag_Byte *pInitialized;
	CTag_Int *pVersion;

	// Read the level data with the tag reader
	pTagReader = new CTagReader();
	if( !pTagReader->open( datPath ) ) {
		delete pTagReader;
		return false;
	}
	pTagReader->close();
	
	// Check the level and initialization
	pVersion = reinterpret_cast<CTag_Int*>(pTagReader->getRoot()->get( "Data.version", TAG_INT ));
	if( !pVersion ) {
		std::cout << "\t > Failed: version tag was invalid or missing";
		return NULL;
	}
	pInitialized = reinterpret_cast<CTag_Byte*>(pTagReader->getRoot()->get( "Data.initialized", TAG_BYTE ));
	if( !pInitialized ) {
		std::cout << "\t > Failed: version tag was invalid or missing";
		return NULL;
	}
	// Make sure its initialized
	if( !pInitialized->payload() ) {
		std::cout << "\t > Failed: the map hasn't been initialized" << std::endl;
		return false;
	}
	// Make sure version is 19133
	if( pVersion->payload() != NBT_VERSION ) {
		std::cout << "\t > Failed: NBT version (" << pVersion->payload() << ") is incorrect; must be " << NBT_VERSION << std::endl;
		return false;
	}

	// Clean up
	if( pTagReader ) {
		delete pTagReader;
		pTagReader = NULL;
	}

	return true;
}

RegionHeader* CRenderer::readRegionHeader( InputStream &decompStream )
{
	boost::filesystem::ifstream m_stream;
	InputStream m_decompStream;
	RegionHeader *pRegionHeader;

	try
	{
		// Allocate space
		pRegionHeader = new RegionHeader();
		// Read the stream
		decompStream.read( reinterpret_cast<char*>(pRegionHeader), sizeof( RegionHeader ) );
	}
	catch( const boost::filesystem::filesystem_error &e ) {
		std::cout << "\t > Failed: could not read region header" << std::endl;
		std::cout << "\t\t\"" << e.what() << "\"" << std::endl;
		return NULL;
	}

	return pRegionHeader;
}


std::vector<boost::filesystem::path> CRenderer::getRegionFiles()
{
	PathList regionPaths;
	std::string extension;
	boost::filesystem::directory_iterator directoryEndItr;

	// Get all files in region folder
	for( boost::filesystem::directory_iterator it( m_regionFolder ); it != directoryEndItr; it++ ) {
		if( boost::filesystem::is_regular_file( it->path() ) )
		{
			// Get the extension
			extension = it->path().extension().string();
			// Convert to lowercase 
			std::transform( extension.begin(), extension.end(), extension.begin(), ::tolower );
			// Make sure it is 'mca'
			if( extension.compare( ".mca" ) == 0 )
				regionPaths.push_back( it->path() );
		}
	}

	return regionPaths;
}

boost::filesystem::path CRenderer::getPath() {
	return m_fullPath;
}