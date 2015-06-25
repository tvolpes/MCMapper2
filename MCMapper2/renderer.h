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
#include "stream.h"

class CTagParent;
typedef std::vector<boost::filesystem::path> PathList;

struct RegionHeader {
	boost::int8_t offset[1024][3];
	boost::int8_t size[1024];
	boost::int32_t timestamps[1024];
};

class CRenderer
{
private:
	boost::filesystem::path m_fullPath, m_regionFolder;

	bool readDataFile( boost::filesystem::path datPath );
protected:
	PathList getRegionFiles();

	RegionHeader* readRegionHeader( InputStream &decompStream );
public:
	CRenderer();
	virtual ~CRenderer();

	bool create( boost::filesystem::path fullPath );
	void destroy();

	virtual bool render() = 0;

	boost::filesystem::path getPath();
};
