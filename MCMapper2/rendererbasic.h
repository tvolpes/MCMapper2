#pragma once
#include "stream.h"
#include "renderer.h"

class CRendererBasic : public CRenderer
{
public:
	CRendererBasic();

	bool render();
};