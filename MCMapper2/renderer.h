#pragma once
#include <boost\filesystem.hpp>

class CRenderer
{
private:
	boost::filesystem::path m_fullPath, m_regionFolder;

	bool readDataFile( boost::filesystem::path datPath );
public:
	CRenderer();
	virtual ~CRenderer();

	bool create( boost::filesystem::path fullPath );
	void destroy();

	virtual bool render() = 0;

	boost::filesystem::path getPath();
};
