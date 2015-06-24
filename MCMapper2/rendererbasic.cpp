#include "renderer.h"
#include "rendererbasic.h"
#include "nbt.h"

CRendererBasic::CRendererBasic() {

}

bool CRendererBasic::render()
{
	PathList regionPaths;

	// Find region files
	regionPaths = this->getRegionFiles();

	// Loop through each region file
	for( PathList::iterator it = regionPaths.begin(); it != regionPaths.end(); it++ )
	{

	}
	
	return true;
}