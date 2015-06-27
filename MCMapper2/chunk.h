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

#pragma once
#include "def.h"
#include "nbt.h"
#include "renderer.h"

enum
{
	CHUNK_COMPRESSION_GZIP = 1,
	CHUNK_COMPRESSION_ZLIB = 2
};

enum
{
	CHUNKLOADFLAGS_NONE						= 1 << 0,
	CHUNKLOADFLAGS_USE_POS					= 1 << 1,
	CHUNKLOADFLAGS_USE_BIOMES				= 1 << 2,
	CHUNKLOADFLAGS_USE_HEIGHTMAP			= 1 << 3,
	CHUNKLOADFLAGS_USE_SECTIONS				= 1 << 4,
	CHUNKLOADFLAGS_USE_SECTIONS_DATA		= 1 << 5,
	CHUNKLOADFLAGS_USE_SECTIONS_BLOCKDATA	= 1 << 6,
	CHUNKLOADFLAGS_USE_SECITONS_SKYLIGHT	= 1 << 7,
	CHUNKLOADFLAGS_USE_ENTITIES				= 1 << 8,
	CHUNKLOADFLAGS_USE_TILEENTITIES			= 1 << 9
};

class CChunk
{
private:
	CTag_IntArray *m_pHeightMap;
public:
	CChunk();

	bool loadChunk( CTagReader *pTagReader, ChunkLoadFlags loadFlags );

	void setHeightMap( CTag_IntArray *pHeightMap );
	CTag_IntArray* getHeightMap();
};