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