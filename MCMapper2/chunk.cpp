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

#include <iostream>
#include "chunk.h"

CChunk::CChunk() {
	m_pHeightMap = NULL;
	m_pXPos = NULL;
	m_pZPos = NULL;
}

bool CChunk::loadChunk( CTagReader *pTagReader, ChunkLoadFlags loadFlags )
{
	CTag_Compound *pRoot;

	// Get the root
	pRoot = pTagReader->getRoot();
	if( !pRoot ) {
		std::cout << "\t > Failed: could not get chunk root tag" << std::endl;
		return false;
	}

	// XPos & ZPos
	if( loadFlags * CHUNKLOADFLAGS_USE_POS )
	{
		m_pXPos = reinterpret_cast<CTag_Int*>(pRoot->get( "Level.xPos", TAG_INT ));
		if( !m_pXPos ){
			std::cout << "\t > Failed: could not get x pos from chunk" << std::endl;
			return false;
		}
		m_pZPos = reinterpret_cast<CTag_Int*>(pRoot->get( "Level.zPos", TAG_INT ));
		if( !m_pZPos ){
			std::cout << "\t > Failed: could not get z pos from chunk" << std::endl;
			return false;
		}
	}
	// HeightMap
	if( loadFlags & CHUNKLOADFLAGS_USE_HEIGHTMAP )
	{
		m_pHeightMap = reinterpret_cast<CTag_IntArray*>(pRoot->get( "Level.HeightMap", TAG_INT_ARRAY ));
		if( !m_pHeightMap ) {
			std::cout << "\t > Failed: could not get height map from chunk" << std::endl;
			return false;
		}
	}

	return true;
}

CTag_Int* CChunk::getXPos() {
	return m_pXPos;
}
CTag_Int* CChunk::getZPos() {
	return m_pZPos;
}
CTag_IntArray* CChunk::getHeightMap() {
	return m_pHeightMap;
}