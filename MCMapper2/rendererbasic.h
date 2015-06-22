#pragma once
#include <boost\filesystem.hpp>
#include "renderer.h"

class CRendererBasic : public CRenderer
{
public:
	CRendererBasic();

	bool create();
	void destroy();

	bool render( boost::filesystem::path fullPath );
};