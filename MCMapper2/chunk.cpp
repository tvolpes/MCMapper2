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
	CTag_List *pSections;

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
	// Sections
	if( loadFlags & CHUNKLOADFLAGS_USE_SECTIONS )
	{
		pSections = reinterpret_cast<CTag_List*>(pRoot->get( "Level.Sections", TAG_LIST ));
		if( !pSections ) {
			std::cout << "\t > Failed: could not get sections from chunk" << std::endl;
			return false;
		}
		m_sections.resize( 16 );
		// For each section
		auto sectionPayload = pSections->payload();
		for( auto it = sectionPayload.begin(); it != sectionPayload.end(); it++ )
		{
			CTag_Compound *pSection;
			ChunkSection sectionData;
			
			// Get the section
			if( (*it)->id() != TAG_COMPOUND )
				continue;
			pSection = reinterpret_cast<CTag_Compound*>(*it);
			// Get the Y value
			sectionData.pY = reinterpret_cast<CTag_Byte*>(pSection->get( "Y", TAG_BYTE ));
			if( !sectionData.pY )
				continue;
			// Get the blocks if needed
			if( loadFlags & CHUNKLOADFLAGS_USE_SECTIONS_BLOCKS )
			{
				// Blocks
				sectionData.pBlocks = reinterpret_cast<CTag_ByteArray*>(pSection->get( "Blocks", TAG_BYTE_ARRAY ));
				if( !sectionData.pBlocks )
					continue;
				// Add
				sectionData.pAdd = reinterpret_cast<CTag_ByteArray*>(pSection->get( "Add", TAG_BYTE_ARRAY ));
			}
			// Get the block data if needed
			if( loadFlags & CHUNKLOADFLAGS_USE_SECTIONS_BLOCKDATA ) {
				// Data
				sectionData.pData = reinterpret_cast<CTag_ByteArray*>(pSection->get( "Data", TAG_BYTE_ARRAY ));
				if( !sectionData.pData )
					continue;
			}
			// Get the light data if needed
			if( loadFlags & CHUNKLOADFLAGS_USE_SECTIONS_LIGHT )
			{
				// BlockLight
				sectionData.pBlockLight = reinterpret_cast<CTag_ByteArray*>(pSection->get( "BlockLight", TAG_BYTE_ARRAY ));
				if( !sectionData.pBlockLight )
					continue;
				// SkyLight
				sectionData.pSkyLight = reinterpret_cast<CTag_ByteArray*>(pSection->get( "SkyLight", TAG_BYTE_ARRAY ));
				if( !sectionData.pSkyLight )
					continue;
			}
			// Save
			m_sections[sectionData.pY->payload()] = sectionData;
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
std::vector<ChunkSection> CChunk::getSections() {
	return m_sections;
}