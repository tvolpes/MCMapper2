#include "def.h"
#include "MCMapper.h"

int main( int argc, char* argv[] )
{
	CMCMapper *pProgram;

	pProgram = new CMCMapper();
	if( !pProgram->initialize() )
		return -1;
	if( !pProgram->run( argc, argv ) )
		return -2;

	if( pProgram ) {
		delete pProgram;
		pProgram = NULL;
	}

	return 0;
}