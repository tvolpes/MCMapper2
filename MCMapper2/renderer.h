#pragma once
#include <boost\filesystem.hpp>

class CRenderer
{
public:
	CRenderer();
	virtual ~CRenderer();

	virtual bool create() = 0;
	virtual void destroy() = 0;

	virtual bool render( boost::filesystem::path fullPath ) = 0;
};
