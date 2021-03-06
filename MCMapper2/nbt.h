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
#include <boost\integer.hpp>
#include <vector>
#include "stream.h"

class CTag;
class CTagParent;
class CTagReader;
typedef std::vector<CTag*> TagList;
typedef std::vector<CTagParent*> TagParentList;

enum
{
	TAG_END			= 0,
	TAG_BYTE		= 1,
	TAG_SHORT		= 2,
	TAG_INT			= 3,		
	TAG_LONG		= 4,
	TAG_FLOAT		= 5,
	TAG_DOUBLE		= 6,
	TAG_BYTE_ARRAY	= 7,
	TAG_STRING		= 8,
	TAG_LIST		= 9,
	TAG_COMPOUND	= 10,
	TAG_INT_ARRAY	= 11
};

class CTag
{
protected:
	boost::int8_t m_tagId;
	std::string m_name;
public:
	CTag();
	virtual ~CTag();

	boost::int8_t id();
	std::string name();

	virtual bool read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly );
	virtual void destroy();
};

class CTagParent : public CTag
{
protected:
	TagList m_payload;
public:
	CTagParent();
	virtual ~CTagParent();

	void add( CTag* pTag );
	void destroy();
	TagList payload();

	CTag* get( std::string path, boost::int8_t tagId );
};

// TAG_END
class CTag_End : public CTag
{
public:
	CTag_End();

	bool read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly );
};
// TAG_BYTE
class CTag_Byte : public CTag
{
private:
	boost::int8_t m_payload;
public:
	CTag_Byte();

	bool read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly );
	boost::int8_t payload();
};
// TAG_SHORT
class CTag_Short : public CTag
{
private:
	boost::int16_t m_payload;
public:
	CTag_Short();

	bool read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly );
	boost::int16_t payload();
};
// TAG_INT
class CTag_Int : public CTag
{
private:
	boost::int32_t m_payload;
public:
	CTag_Int();

	bool read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly );
	boost::int32_t payload();
};
// TAG_LONG
class CTag_Long : public CTag
{
private:
	boost::int64_t m_payload;
public:
	CTag_Long();

	bool read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly );
	boost::int64_t payload();
};
// TAG_FLOAT
class CTag_Float : public CTag
{
private:
	float m_payload;
public:
	CTag_Float();

	bool read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly );
	float payload();
};
// TAG_DOUBLE
class CTag_Double : public CTag
{
private:
	double m_payload;
public:
	CTag_Double();

	bool read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly );
	double payload();
};
// TAG_BYTE_ARRAY
class CTag_ByteArray : public CTag
{
private:
	std::vector<boost::int8_t> m_payload;
public:
	CTag_ByteArray();

	bool read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly );
	std::vector<boost::int8_t> payload();
};
// TAG_STRING
class CTag_String : public CTag
{
private:
	std::string m_payload;
public:
	CTag_String();

	bool read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly );
	std::string payload();
};
// TAG_LIST
class CTag_List : public CTagParent
{
private:
	boost::int8_t m_payloadId;
	boost::int32_t m_payloadsLeft;
public:
	CTag_List();

	bool read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly );

	boost::int8_t payloadId();

	friend class CTagReader;
};
// TAG_COMPOUND
class CTag_Compound : public CTagParent
{
public:
	CTag_Compound();

	bool read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly );
};
// TAG_INT_ARRAY
class CTag_IntArray : public CTag
{
private:
	std::vector<boost::int32_t> m_payload;
public:
	CTag_IntArray();

	bool read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly );
	std::vector<boost::int32_t> payload();
};

class CTagReader
{
public:
	static CTag* allocateTag( boost::int8_t tagId );
private:
	TagParentList m_parentStack;
	TagList m_tagTree;

	std::string m_filename;

	CTag* readTag( InputStream &decompStream, size_t *pBytesRead );
public:
	CTagReader();

	bool open( boost::filesystem::path fullPath, size_t length );
	bool open( InputStream &decompStream, size_t length );
	void close();
	void destroy();

	CTag_Compound* getRoot();

	friend class CTag_List;
};