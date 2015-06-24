#pragma once
#include <boost\filesystem.hpp>
#include <boost\filesystem\fstream.hpp>
#include <boost\iostreams\filtering_stream.hpp>
#include <boost\integer.hpp>
#include <vector>

class CTag;
typedef std::vector<CTag*> TagList;

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

	boost::int8_t id();
	std::string name();

	virtual bool read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly = false );
	virtual void destroy();
};
// TAG_END
class CTag_End : public CTag
{
public:
	CTag_End();

	bool read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly = false );
};
// TAG_BYTE
class CTag_Byte : public CTag
{
private:
	boost::int8_t m_payload;
public:
	CTag_Byte();

	bool read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly = false );
};
// TAG_SHORT
class CTag_Short : public CTag
{
private:
	boost::int16_t m_payload;
public:
	CTag_Short();

	bool read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly = false );
};
// TAG_INT
class CTag_Int : public CTag
{
private:
	boost::int32_t m_payload;
public:
	CTag_Int();

	bool read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly = false );
};
// TAG_LONG
class CTag_Long : public CTag
{
private:
	boost::int64_t m_payload;
public:
	CTag_Long();

	bool read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly = false );
};
// TAG_FLOAT
class CTag_Float : public CTag
{
private:
	float m_payload;
public:
	CTag_Float();

	bool read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly = false );
};
// TAG_DOUBLE
class CTag_Double : public CTag
{
private:
	double m_payload;
public:
	CTag_Double();

	bool read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly = false );
};
// TAG_BYTE_ARRAY
class CTag_ByteArray : public CTag
{
private:
	std::vector<boost::int8_t> m_payload;
public:
	CTag_ByteArray();

	bool read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly = false );
};
// TAG_STRING
class CTag_String : public CTag
{
private:
	std::string m_payload;
public:
	CTag_String();

	bool read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly = false );
};
// TAG_LIST
class CTag_List : public CTag
{
private:
	boost::int8_t m_payloadId;
	std::vector<CTag*> m_payload;
public:
	CTag_List();

	bool read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly = false );
	void destroy();
};
// TAG_COMPOUND
class CTag_Compound : public CTag
{
private:
	TagList m_children;
public:
	CTag_Compound();

	bool read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly = false );
	void add( CTag* pTag );
	void destroy();
};
// TAG_INT_ARRAY
class CTag_IntArray : public CTag
{
private:
	std::vector<boost::int32_t> m_payload;
public:
	CTag_IntArray();

	bool read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly = false );
};

class CTagReader
{
private:
	boost::filesystem::ifstream m_stream;
	boost::iostreams::filtering_istream m_decompStream;

	std::vector<CTag_Compound*> m_parentStack;
	TagList m_tagTree;

	std::string m_filename;
	size_t m_streamPos;

	CTag* readTag();
public:
	CTagReader();

	bool open( boost::filesystem::path fullPath );
	void close();
	void destroy();
};