#pragma once
#include <boost\filesystem.hpp>

class CTagParent;
typedef std::vector<boost::filesystem::path> PathList;

class CRenderer
{
private:
	boost::filesystem::path m_fullPath, m_regionFolder;

	bool readDataFile( boost::filesystem::path datPath );
protected:
	PathList getRegionFiles();
public:
	CRenderer();
	virtual ~CRenderer();

	bool create( boost::filesystem::path fullPath );
	void destroy();

	virtual bool render() = 0;

	boost::filesystem::path getPath();
};
