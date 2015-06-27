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

#pragma once
#include "def.h"
#include "stream.h"

class CChunk;
class CTagParent;
typedef std::vector<boost::filesystem::path> PathList;
typedef boost::filesystem::ifstream InStream;

#pragma pack(push, 1)
struct RegionHeader {
	boost::int8_t offset2[REGION_CHUNKCOUNT], offset1[REGION_CHUNKCOUNT], offset0[REGION_CHUNKCOUNT];
	boost::int8_t size[REGION_CHUNKCOUNT];
	boost::int32_t timestamps[REGION_CHUNKCOUNT];
};
#pragma pack(pop)

class CRenderer
{
private:
	boost::filesystem::path m_fullPath, m_regionFolder;

	bool readDataFile( boost::filesystem::path datPath );
protected:
	boost::filesystem::path m_outputPath;

	PathList getRegionFiles();

	RegionHeader* readRegionHeader( InStream &inStream );
	CChunk* readChunk( InStream &inStream );
public:
	CRenderer();
	virtual ~CRenderer();

	bool create( boost::filesystem::path fullPath );
	void destroy();

	virtual bool render() = 0;

	boost::filesystem::path getPath();
};
