#include "renderer.h"
#include "rendererbasic.h"

CRendererBasic::CRendererBasic() {

}

bool CRendererBasic::create() {
	return true;
}
void CRendererBasic::destroy() {
}

bool CRendererBasic::render( boost::filesystem::path fullPath ) {
	return true;
}