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

#include <boost\endian\conversion.hpp>
#include <iostream>
#include <algorithm>
#include "def.h"
#include "renderer.h"
#include "nbt.h"
#include "chunk.h"
#include "stream.h"

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

	// Create the output folder if it doesnt exist
	m_outputPath = boost::filesystem::current_path() / "maps";
	m_outputPath /= fullPath.filename();
	try
	{
		// Create the directory if it doesnt exist
		if( !boost::filesystem::exists( m_outputPath ) ) {
			if( !boost::filesystem::create_directories( m_outputPath ) ) {
				std::cout << "\t > Failed: could not create output path" << std::endl;
				return false;
			}
		}
	}
	catch( const boost::filesystem::filesystem_error &e ) {
		std::cout << "\t > Failed: could not create output path" << std::endl;
		std::cout << "\t\t\"" << e.what() << "\"" << std::endl;
		return false;
	}

	return true;
}
void CRenderer::destroy()
{
	m_fullPath = "";
	m_regionFolder = "";
	m_outputPath = "";
}

bool CRenderer::readDataFile( boost::filesystem::path datPath )
{
	CTagReader *pTagReader;
	CTag_Byte *pInitialized;
	CTag_Int *pVersion;

	// Read the level data with the tag reader
	pTagReader = new CTagReader();
	if( !pTagReader->open( datPath, 0 ) ) {
		delete pTagReader;
		return false;
	}
	pTagReader->close();

	// make sure root tag exists
	if( !pTagReader->getRoot() )
		return false;
	
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

RegionHeader* CRenderer::readRegionHeader( InStream &inStream )
{
	boost::filesystem::ifstream m_stream;
	InputStream m_decompStream;
	RegionHeader *pRegionHeader;

	try
	{
		// Allocate space
		pRegionHeader = new RegionHeader();
		// Read the stream
		inStream.read( reinterpret_cast<char*>(pRegionHeader), sizeof( RegionHeader ) );
	}
	catch( const boost::filesystem::filesystem_error &e ) {
		std::cout << "\t > Failed: could not read region header" << std::endl;
		std::cout << "\t\t\"" << e.what() << "\"" << std::endl;
		return NULL;
	}

	return pRegionHeader;
}
CChunk* CRenderer::readChunk( InStream &inStream, ChunkLoadFlags loadFlags )
{
	CChunk *pChunk;
	boost::int32_t chunkLength, paddedLength;
	boost::int8_t compressionType;
	char *pBufferData;
	InputStream decompStream;
	CTagReader *pTagReader;

	// Read the length
	inStream.read( reinterpret_cast<char*>(&chunkLength), sizeof( boost::int32_t ) );
	boost::endian::reverse( chunkLength );
	// Read the compression type
	inStream.read( reinterpret_cast<char*>(&compressionType), sizeof( boost::int8_t ) );
	// Compute the padded length
	paddedLength = chunkLength + (4096 - (chunkLength % 4096));
	// Read the compressed data into a buffer
	pBufferData = new char[chunkLength];
	inStream.read( pBufferData, chunkLength );
	// Setup decompression stream
	if( compressionType == CHUNK_COMPRESSION_GZIP )
		decompStream.push( boost::iostreams::gzip_decompressor() );
	else if( compressionType == CHUNK_COMPRESSION_ZLIB )
		decompStream.push( boost::iostreams::zlib_decompressor() );
	else {
		std::cout << "\t > Warning: invalid compression type for chunk" << std::endl;
		return NULL;
	}
	decompStream.push( boost::iostreams::array_source( pBufferData, chunkLength ) );
	// Skip the dead space
	inStream.ignore( paddedLength - chunkLength - 5 ); // 5 is for header size

	// Read the NBT data
	pTagReader = new CTagReader();
	pTagReader->open( decompStream, 0 );
	// Destory compressed data
	if( pBufferData ) {
		delete[] pBufferData;
		pBufferData = NULL;
	}

	// Load the data into a chunk obj
	pChunk = new CChunk();
	pChunk->loadChunk( pTagReader, loadFlags );

	// Clean up
	if( decompStream )
		boost::iostreams::close( decompStream );
	if( pTagReader ) {
		delete pTagReader;
		pTagReader = NULL;
	}

	return pChunk;
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