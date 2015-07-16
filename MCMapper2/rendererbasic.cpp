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
				boost::int32_t xPos, zPos, xRelPos, zRelPos, blockIndex;
				boost::int8_t blockId;
				unsigned int offset, currentColumn;
				ChunkSection sectionData;

				// If theres no data, skip it
				if( pRegionHeader->location[i].size == 0 )
					continue;
				// Calculate the offset
				offset = pRegionHeader->location[i].offset[2] + (pRegionHeader->location[i].offset[1] << 8) + (pRegionHeader->location[i].offset[0] << 16);
				//std::cout << "offset: " << offset << ", size: " << (int)pRegionHeader->location[i].size << std::endl;
				// Read the chunk
				pCurrentChunk = this->readChunk( inStream, offset, CHUNKLOADFLAGS_USE_POS | CHUNKLOADFLAGS_USE_HEIGHTMAP | CHUNKLOADFLAGS_USE_SECTIONS | CHUNKLOADFLAGS_USE_SECTIONS_BLOCKS );
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

				// Render the chunk
				currentColumn = 0;
				for( int32_t x = xRelPos*16; x < xRelPos*16+16; x++ )
				{
					for( int32_t z = zRelPos*16; z < zRelPos*16+16; z++ )
					{
						unsigned int columnCol, columnRow, transformedColumn;
						unsigned char height, relHeight;

						// Get the column
						columnCol = (int)(currentColumn / 16);
						columnRow = currentColumn - (columnCol * 16);
						transformedColumn = ((256-columnCol)-(columnRow*16))-1;
						transformedColumn = 255 - transformedColumn;
						// Height
						height = (unsigned char)pCurrentChunk->getHeightMap()->payload()[transformedColumn];
						// Only air
						if( height == 0 ) {
							imageView( x, z ) = boost::gil::rgb8_pixel_t( 205, 205, 205 );
							continue;
						}

						// Get the corresponding section
						sectionData = pCurrentChunk->getSections()[(int)floor( (height-1) / 16 )];
						if( !sectionData.pY ) {
							std::cout << "\t > Warning: missing section" << std::endl;
							continue;
						}
						relHeight = (height-1) - sectionData.pY->payload() * 16;
						// Get the corresponding block
						blockIndex = (relHeight * 16 * 16) + (columnRow * 16 + columnCol);
						blockId = sectionData.pBlocks->payload()[blockIndex];

						if( blockId ==  9 )
							imageView( x, z ) = boost::gil::rgb8c_pixel_t( 43, 95, 255 );
						else
							imageView( x, z ) = boost::gil::rgb8_pixel_t( 0, 0, (unsigned char)(255.0*((double)height/255.0)) );

						currentColumn++;
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