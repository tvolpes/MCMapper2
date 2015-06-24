#include <iostream>
#pragma warning( disable:4244 )
#include <boost\iostreams\filter\gzip.hpp>
#pragma warning( default:4244 )
#include <boost\endian\conversion.hpp>
#include "nbt.h"

/////////////
// CTag(s) //
/////////////

CTag::CTag() {
	m_name = "";
}
boost::int8_t CTag::id() {
	return m_tagId;
}
std::string CTag::name() {
	return m_name;
}
bool CTag::read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly )
{
	char *pName;
	boost::int16_t nameLength;

	// Read the name length
	decompStream.read( reinterpret_cast<char*>(&nameLength), 2 );
	boost::endian::reverse( nameLength );
	// Read the name
	if( nameLength > 0 ) {
		pName = new char[nameLength+1];
		decompStream.read( pName, nameLength );
		pName[nameLength] = '\0'; // add terminator
		this->m_name = std::string( pName );
		if( pName ) {
			delete[] pName;
			pName = NULL;
		}
	}

	return true;
}
void CTag::destroy() {

}

// TAG_END
CTag_End::CTag_End() {
	m_tagId = TAG_END;
}
bool CTag_End::read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly ) {
	return true;
}
// TAG_BYTE
CTag_Byte::CTag_Byte() {
	m_tagId = TAG_BYTE;
	m_payload = 0;
}
bool CTag_Byte::read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly )
{
	if( !payloadOnly ) {
		if( !CTag::read( decompStream ) )
			return false;
	}
	// Read the payload
	decompStream.read( reinterpret_cast<char*>(&m_payload), 1 );
	return true;
}
// TAG_SHORT
CTag_Short::CTag_Short() {
	m_tagId = TAG_SHORT;
	m_payload = 0;
}
bool CTag_Short::read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly )
{
	if( !payloadOnly ) {
		if( !CTag::read( decompStream ) )
			return false;
	}
	// Read the payload
	decompStream.read( reinterpret_cast<char*>(&m_payload), 2 );
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( m_payload );
#endif
	return true;
}
// TAG_INT
CTag_Int::CTag_Int() {
	m_tagId = TAG_INT;
	m_payload = 0;
}
bool CTag_Int::read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly )
{
	if( !payloadOnly ) {
		if( !CTag::read( decompStream ) )
			return false;
	}
	// Read the payload
	decompStream.read( reinterpret_cast<char*>(&m_payload), 4 );
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( m_payload );
#endif
	return true;
}
// TAG_LONG
CTag_Long::CTag_Long() {
	m_tagId = TAG_LONG;
	m_payload = 0;
}
bool CTag_Long::read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly )
{
	if( !payloadOnly ) {
		if( !CTag::read( decompStream ) )
			return false;
	}
	// Read the payload
	decompStream.read( reinterpret_cast<char*>(&m_payload), 8 );
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( m_payload );
#endif
	return true;
}
// TAG_FLOAT
CTag_Float::CTag_Float() {
	m_tagId = TAG_FLOAT;
	m_payload = 0;
}
bool CTag_Float::read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly )
{
	if( !payloadOnly ) {
		if( !CTag::read( decompStream ) )
			return false;
	}
	// Read the payload
	decompStream.read( reinterpret_cast<char*>(&m_payload), 4 );
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( m_payload );
#endif
	return true;
}
// TAG_DOUBLE
CTag_Double::CTag_Double() {
	m_tagId = TAG_DOUBLE;
	m_payload = 0;
}
bool CTag_Double::read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly )
{
	if( !payloadOnly ) {
		if( !CTag::read( decompStream ) )
			return false;
	}
	// Read the payload
	decompStream.read( reinterpret_cast<char*>(&m_payload), 8 );
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( m_payload );
#endif
	return true;
}
// TAG_BYTE_ARRAY
CTag_ByteArray::CTag_ByteArray() {
	m_tagId = TAG_BYTE_ARRAY;
}
bool CTag_ByteArray::read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly )
{
	boost::int8_t payloadItem;
	boost::int32_t payloadSize;

	if( !payloadOnly ) {
		if( !CTag::read( decompStream ) )
			return false;
	}
	// Read the payload size
	decompStream.read( reinterpret_cast<char*>(&payloadSize), 4 );
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( payloadSize );
#endif
	// Read the payload
	for( boost::int32_t i = 0; i < payloadSize; i++ ) {
		decompStream.read( reinterpret_cast<char*>(&payloadItem), 1 );
		m_payload.push_back( payloadItem );
	}
	return true;
}
// TAG_STRING
CTag_String::CTag_String() {
	m_tagId = TAG_STRING;
	m_payload = "";
}
bool CTag_String::read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly )
{
	char *pString;
	boost::int16_t payloadSize;

	if( !payloadOnly ) {
		if( !CTag::read( decompStream ) )
			return false;
	}
	// Read the payload size
	decompStream.read( reinterpret_cast<char*>(&payloadSize), 2 );
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( payloadSize );
#endif
	// Read the string
	if( payloadSize > 0 ) {
		pString = new char[payloadSize+1];
		decompStream.read( pString, payloadSize );
		pString[payloadSize] = '\0'; // add terminator
		m_payload = std::string( pString );
		if( pString ) {
			delete[] pString;
			pString = NULL;
		}
	}
	return true;
}
// TAG_LIST
CTag_List::CTag_List() {
	m_tagId = TAG_LIST;
	m_payloadId = TAG_END;
}
bool CTag_List::read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly ) {
	boost::int32_t payloadSize;

	if( !payloadOnly ) {
		if( !CTag::read( decompStream ) )
			return false;
	}
	// Read the payload id
	decompStream.read( reinterpret_cast<char*>(&m_payloadId), 1 );
	// Read the payload size
	decompStream.read( reinterpret_cast<char*>(&payloadSize), 4 );
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( payloadSize );
#endif
	// Read the payload

	return true;

}
void CTag_List::destroy()
{
	// Delete payload
	for( TagList::iterator it = m_payload.begin(); it != m_payload.end(); it++ ) {
		if( (*it) ) {
			(*it)->destroy();
			delete (*it);
			(*it) = NULL;
		}
	}
	m_payload.clear();
}
// TAG_COMPOUND
CTag_Compound::CTag_Compound() {
	m_tagId = TAG_COMPOUND;
}
void CTag_Compound::add( CTag* pTag ) {
	m_children.push_back( pTag );
}
bool CTag_Compound::read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly ) {
	if( !payloadOnly ) {
		if( !CTag::read( decompStream ) )
			return false;
	}
	return true;
}
void CTag_Compound::destroy()
{
	// Delete children
	for( TagList::iterator it = m_children.begin(); it != m_children.end(); it++ ) {
		if( (*it) ) {
			(*it)->destroy();
			delete (*it);
			(*it) = NULL;
		}
	}
	m_children.clear();
}
// TAG_INT_ARRAY
CTag_IntArray::CTag_IntArray() {
	m_tagId = TAG_INT_ARRAY;
}
bool CTag_IntArray::read( boost::iostreams::filtering_istream &decompStream, bool payloadOnly )
{
	boost::int32_t payloadItem;
	boost::int32_t payloadSize;

	if( !payloadOnly ) {
		if( !CTag::read( decompStream ) )
			return false;
	}
	// Read the payload size
	decompStream.read( reinterpret_cast<char*>(&payloadSize), 4 );
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( payloadSize );
#endif
	// Read the payload
	for( boost::int32_t i = 0; i < payloadSize; i++ ) {
		decompStream.read( reinterpret_cast<char*>(&payloadItem), 4 );
		m_payload.push_back( payloadItem );
#ifdef BOOST_LITTLE_ENDIAN
		boost::endian::reverse( payloadItem );
#endif
	}
	return true;
}

////////////////
// CTagReader //
////////////////

CTagReader::CTagReader()
{
	m_streamPos = 0;
}

CTag* CTagReader::readTag()
{
	boost::int8_t tagId;
	CTag *pTag;

	pTag = NULL;

	// First read the tag id
	m_decompStream.read( reinterpret_cast<char*>(&tagId), 1 );

	if( tagId == TAG_END || tagId == TAG_COMPOUND )
	{
		switch( tagId )
		{
		case TAG_END:
		{
			pTag = new CTag_End();
			// Read
			pTag->read( m_decompStream );
			// Pop a parent off the stack
			if( !m_parentStack.empty() )
				m_parentStack.pop_back();
			else
				std::cout << "\t > Warning: tag reader unmatched end tag" << std::endl;
			// Parent
			if( m_parentStack.empty() )
				m_tagTree.push_back( pTag );
			else
				m_parentStack.back()->add( pTag );
			return pTag;
		}
		case TAG_COMPOUND:
		{
			CTag_Compound *pTagParent = new CTag_Compound();
			// Read
			pTagParent->read( m_decompStream );
			// Parent
			if( !m_parentStack.empty() )
				m_parentStack.back()->add( pTagParent );
			else
				m_tagTree.push_back( pTagParent );
			// Add to parent stack
			m_parentStack.push_back( pTagParent );
			return pTagParent;
		}
		}

	}
	else
	{
		switch( tagId )
		{
		case TAG_BYTE:
			pTag = new CTag_Byte();
			break;
		case TAG_SHORT:
			pTag = new CTag_Short();
			break;
		case TAG_INT:
			pTag = new CTag_Int();
			break;
		case TAG_LONG:
			pTag = new CTag_Long();
			break;
		case TAG_FLOAT:
			pTag = new CTag_Float();
			break;
		case TAG_DOUBLE:
			pTag = new CTag_Double();
			break;
		case TAG_BYTE_ARRAY:
			pTag = new CTag_ByteArray();
			break;
		case TAG_STRING:
			pTag = new CTag_String();
			break;
		case TAG_LIST:
			pTag = new CTag_List();
			break;
		case TAG_INT_ARRAY:
			pTag = new CTag_IntArray();
			break;
		default:
			std::cout << "\t > Failed: tag reader found unknown tag type" << std::endl;
			return NULL;
		}
		// Read
		pTag->read( m_decompStream );
		// Parent
		if( !m_parentStack.empty() )
			m_parentStack.back()->add( pTag );
		else
			m_tagTree.push_back( pTag );
	}

	return pTag;
}

bool CTagReader::open( boost::filesystem::path fullPath )
{
	m_filename = fullPath.filename().string();

	// Open the file
	try
	{
		// Make sure it exists
		if( !boost::filesystem::is_regular_file( fullPath ) ) {
			std::cout << "\t > Failed: tag reader failed because file is not valid" << std::endl;
			std::cout << "\t\t" << m_filename << std::endl;
			return false;
		}
		// Open a handle
		m_stream.open( fullPath );
		m_streamPos = 0;
		if( !m_stream ) {
			std::cout << "\t > Failed: tag reader failed to open file" << std::endl;
			std::cout << "\t\t" << m_filename << std::endl;
			return false;
		}
		// First step is to set up decompression filter
		m_decompStream.push( boost::iostreams::gzip_decompressor() );
		m_decompStream.push( m_stream );

		// Read all tags in the file
		m_parentStack.clear();
		while( true ) {
			if( !this->readTag() )
				break;
			std::cout << "Stream Pos: " << m_stream.tellg() << std::endl;
		}

	}
	catch( const boost::filesystem::filesystem_error &e ) {
		std::cout << "\t > Failed: tag reader filesystem operation failed" << std::endl;
		std::cout << "\t\t" << m_filename << std::endl;
		std::cout << "\t\t\"" << e.what() << "\"" << std::endl;
		return false;
	}

	return true;
}
void CTagReader::close()
{
	// Close the stream
	try {
		if( m_stream.is_open() )
			m_stream.close();
		if( m_decompStream )
			boost::iostreams::close( m_decompStream );
	}
	catch( const boost::filesystem::filesystem_error &e ) {
		std::cout << "\t > Warning: failed to close tag reader stream" << std::endl;
		std::cout << "\t\t" << m_filename << std::endl;
		std::cout << "\t\t\"" << e.what() << "\"" << std::endl;
	}
	m_filename = "";
	m_streamPos = 0;
}

void CTagReader::destroy()
{
	// Delete all the tags
	for( TagList::iterator it = m_tagTree.begin(); it != m_tagTree.end(); it++ ) {
		if( (*it) ) {
			(*it)->destroy();
			delete (*it);
			(*it) = NULL;
		}
	}
	m_tagTree.clear();
}