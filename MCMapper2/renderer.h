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
