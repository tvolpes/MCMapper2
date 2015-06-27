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

#pragma warning( disable:4996 )
#include <boost\gil\gil_all.hpp>
#include <boost\gil\extension\io\png_dynamic_io.hpp>
#pragma warning( default:4996 )
#include <iostream>
#include <boost\endian\conversion.hpp>
#include "def.h"
#include "renderer.h"
#include "rendererbasic.h"
#include "nbt.h"
#include "chunk.h"

CRendererBasic::CRendererBasic() {

}

bool CRendererBasic::render()
{
	PathList regionPaths;
	boost::filesystem::ifstream inStream;
	boost::filesystem::path regionImagePath;
	RegionHeader *pRegionHeader;
	CChunk *pCurrentChunk;
	boost::gil::rgb8_image_t *pRegionImage;

	// Find region files
	regionPaths = this->getRegionFiles();

	// Loop through each region file
	for( PathList::iterator it = regionPaths.begin(); it != regionPaths.end(); it++ )
	{
		std::cout << "\t > Rendering region " << (*it).filename().string() << std::endl;

		try
		{
			// Open the stream
			inStream.open( (*it), std::ios::binary );
			if( !inStream ) {
				std::cout << "\t > Failed: could not open region file" << std::endl;
				std::cout << "\t\t" << (*it).filename() << std::endl;
				return false;
			}

			// Read the region header
			pRegionHeader = this->readRegionHeader( inStream );
			if( !pRegionHeader )
				return false;

			// Create the image
			pRegionImage = new boost::gil::rgb8_image_t( 512, 512 );

			// Loop through all the chunks
			for( unsigned int i = 0; i < REGION_CHUNKCOUNT; i++ )
			{
				boost::gil::rgb8_image_t::view_t imageView;
				boost::int32_t xPos, zPos, xRelPos, zRelPos;

				// If theres no data, skip it
				if( pRegionHeader->size[i] == 0 )
					continue;
				// Read the chunk
				pCurrentChunk = this->readChunk( inStream, CHUNKLOADFLAGS_USE_POS | CHUNKLOADFLAGS_USE_HEIGHTMAP );
				// If there's not chunk here continue
				if( !pCurrentChunk )
					continue;
				
				// Get the data to work with
				imageView = boost::gil::view( *pRegionImage );

				// Get position and relative position
				xPos = pCurrentChunk->getXPos()->payload();
				zPos = pCurrentChunk->getZPos()->payload();
				xRelPos = abs( xPos % 32 );
				zRelPos = abs( zPos % 32 );

				// . . .
				for( int32_t x = xRelPos*16; x < xRelPos*16+16; x++ )
				{
					for( int32_t z = zRelPos*16; z < zRelPos*16+16; z++ )
					{
						imageView( x, z ) = boost::gil::rgb8_pixel_t( 0, 205, 205 );
					}
				}
				
				// Clean up
				if( pCurrentChunk ) {
					delete pCurrentChunk;
					pCurrentChunk = NULL;
				}
			}

			// Write the image file
			regionImagePath = m_outputPath / (*it).stem();
			regionImagePath += ".png";
			boost::gil::png_write_view( regionImagePath.string(), boost::gil::const_view( *pRegionImage ) );
		
			// Clean up
			if( pRegionImage ) {
				delete pRegionImage;
				pRegionImage = NULL;
			}
			if( pRegionHeader ) {
				delete pRegionHeader;
				pRegionHeader = NULL;
			}
			// Close the streams
			if( inStream.is_open() )
				inStream.close();
		}
		catch( const boost::filesystem::filesystem_error &e ) {
			std::cout << "\t > Failed: could not read region header" << std::endl;
			std::cout << "\t\t" << (*it).filename() << std::endl;
			std::cout << "\t\t\"" << e.what() << "\"" << std::endl;
			return false;
		}

	}

	std::cout << "\t > Rendering finished " << std::endl;
	
	return true;
}