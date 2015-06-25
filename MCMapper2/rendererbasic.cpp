#include <iostream>
#include <boost\endian\conversion.hpp>
#include "renderer.h"
#include "rendererbasic.h"
#include "nbt.h"

CRendererBasic::CRendererBasic() {

}

bool CRendererBasic::render()
{
	PathList regionPaths;
	boost::filesystem::ifstream inStream;
	InputStream decompStream;
	RegionHeader *pRegionHeader;

	// Find region files
	regionPaths = this->getRegionFiles();

	// Loop through each region file
	for( PathList::iterator it = regionPaths.begin(); it != regionPaths.end(); it++ )
	{
		try
		{
			// Open the stream
			inStream.open( (*it) );
			if( !inStream ) {
				std::cout << "\t > Failed: could not open region file" << std::endl;
				std::cout << "\t\t" << (*it).filename() << std::endl;
				return false;
			}
			// Open the decompression stream without decompression
			decompStream.push( inStream );

			// Read the region header
			pRegionHeader = this->readRegionHeader( decompStream );
			if( !pRegionHeader )
				return false;

			// Loop through all the data . . .

			// Add decompression
			decompStream.push( boost::iostreams::gzip_decompressor() );
		
			// Clean up
			if( pRegionHeader ) {
				delete pRegionHeader;
				pRegionHeader = NULL;
			}
			// Close the streams
			if( inStream.is_open() )
				inStream.close();
			if( decompStream )
				boost::iostreams::close( decompStream );
		}
		catch( const boost::filesystem::filesystem_error &e ) {
			std::cout << "\t > Failed: could not read region header" << std::endl;
			std::cout << "\t\t" << (*it).filename() << std::endl;
			std::cout << "\t\t\"" << e.what() << "\"" << std::endl;
			return false;
		}

	}
	
	return true;
}