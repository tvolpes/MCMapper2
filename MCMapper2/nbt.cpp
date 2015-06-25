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
#include <algorithm>
#include <sstream>
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
CTag::~CTag() {
}
boost::int8_t CTag::id() {
	return m_tagId;
}
std::string CTag::name() {
	return m_name;
}
bool CTag::read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly )
{
	char *pName;
	boost::int16_t nameLength;

	if( payloadOnly )
		return true;

	// Read the name length
	decompStream.read( reinterpret_cast<char*>(&nameLength), 2 );
	(*pBytesRead)+=2;
	boost::endian::reverse( nameLength );
	// Read the name
	if( nameLength > 0 ) {
		pName = new char[nameLength+1];
		decompStream.read( pName, nameLength );
		(*pBytesRead)+=nameLength;
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

CTagParent::CTagParent() {
}
CTagParent::~CTagParent() {
}

void CTagParent::add( CTag* pTag ) {
	m_payload.push_back( pTag );
}
void CTagParent::destroy()
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
TagList CTagParent::payload() {
	return m_payload;
}
CTag* CTagParent::get( std::string path, boost::int8_t tagId )
{
	std::string token;
	std::istringstream tokenStream;
	CTagParent *pCurrentParent;
	TagList parentTagList;

	// Split the string into its tokens
	tokenStream = std::istringstream( path );
	pCurrentParent = this;
	while( std::getline( tokenStream, token, '.' ) ) {
		// Search parent for name equals token
		parentTagList = pCurrentParent->payload();
		for( TagList::iterator it = parentTagList.begin(); it != parentTagList.end(); it++ ) {
			if( strcmp( (*it)->name().c_str(), token.c_str() ) == 0 ) {
				// If its a parent
				if( (*it)->id() == TAG_LIST || (*it)->id() == TAG_COMPOUND )
					pCurrentParent = reinterpret_cast<CTag_Compound*>(*it);
				else { // if its not, we return it because we cant go any deeper 
					if( (*it)->id() != tagId )
						return NULL;
					return (*it);
				}
			}
		}
	}
	
	if( pCurrentParent->id() != tagId )
		return NULL;
	return pCurrentParent;
}


// TAG_END
CTag_End::CTag_End() {
	m_tagId = TAG_END;
}
bool CTag_End::read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly ) {
	return true;
}
// TAG_BYTE
CTag_Byte::CTag_Byte() {
	m_tagId = TAG_BYTE;
	m_payload = 0;
}
bool CTag_Byte::read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly )
{
	if( !CTag::read( decompStream, pBytesRead, payloadOnly ) )
		return false;
	// Read the payload
	decompStream.read( reinterpret_cast<char*>(&m_payload), 1 );
	(*pBytesRead)+=1;
	return true;
}
boost::int8_t CTag_Byte::payload() {
	return m_payload;
}
// TAG_SHORT
CTag_Short::CTag_Short() {
	m_tagId = TAG_SHORT;
	m_payload = 0;
}
bool CTag_Short::read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly )
{
	if( !CTag::read( decompStream, pBytesRead, payloadOnly ) )
		return false;
	// Read the payload
	decompStream.read( reinterpret_cast<char*>(&m_payload), 2 );
	(*pBytesRead)+=2;
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( m_payload );
#endif
	return true;
}
boost::int16_t CTag_Short::payload() {
	return m_payload;
}
// TAG_INT
CTag_Int::CTag_Int() {
	m_tagId = TAG_INT;
	m_payload = 0;
}
bool CTag_Int::read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly )
{
	if( !CTag::read( decompStream, pBytesRead, payloadOnly ) )
		return false;
	// Read the payload
	decompStream.read( reinterpret_cast<char*>(&m_payload), 4 );
	(*pBytesRead)+=4;
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( m_payload );
#endif
	return true;
}
boost::int32_t CTag_Int::payload() {
	return m_payload;
}
// TAG_LONG
CTag_Long::CTag_Long() {
	m_tagId = TAG_LONG;
	m_payload = 0;
}
bool CTag_Long::read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly )
{
	if( !CTag::read( decompStream, pBytesRead, payloadOnly ) )
		return false;
	// Read the payload
	decompStream.read( reinterpret_cast<char*>(&m_payload), 8 );
	(*pBytesRead)+=8;
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( m_payload );
#endif
	return true;
}
boost::int64_t CTag_Long::payload() {
	return m_payload;
}
// TAG_FLOAT
CTag_Float::CTag_Float() {
	m_tagId = TAG_FLOAT;
	m_payload = 0;
}
bool CTag_Float::read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly )
{
	if( !CTag::read( decompStream, pBytesRead, payloadOnly ) )
		return false;
	// Read the payload
	decompStream.read( reinterpret_cast<char*>(&m_payload), 4 );
	(*pBytesRead)+=4;
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( m_payload );
#endif
	return true;
}
float CTag_Float::payload() {
	return m_payload;
}
// TAG_DOUBLE
CTag_Double::CTag_Double() {
	m_tagId = TAG_DOUBLE;
	m_payload = 0;
}
bool CTag_Double::read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly )
{
	if( !CTag::read( decompStream, pBytesRead, payloadOnly ) )
		return false;
	// Read the payload
	decompStream.read( reinterpret_cast<char*>(&m_payload), 8 );
	(*pBytesRead)+=8;
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( m_payload );
#endif
	return true;
}
double CTag_Double::payload() {
	return m_payload;
}
// TAG_BYTE_ARRAY
CTag_ByteArray::CTag_ByteArray() {
	m_tagId = TAG_BYTE_ARRAY;
}
bool CTag_ByteArray::read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly )
{
	boost::int8_t payloadItem;
	boost::int32_t payloadSize;

	if( !CTag::read( decompStream, pBytesRead, payloadOnly ) )
		return false;
	// Read the payload size
	decompStream.read( reinterpret_cast<char*>(&payloadSize), 4 );
	(*pBytesRead)+=4;
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( payloadSize );
#endif
	m_payload.reserve( payloadSize );
	// Read the payload
	for( boost::int32_t i = 0; i < payloadSize; i++ ) {
		decompStream.read( reinterpret_cast<char*>(&payloadItem), 1 );
		(*pBytesRead)+=1;
		m_payload.push_back( payloadItem );
	}
	return true;
}
std::vector<boost::int8_t> CTag_ByteArray::payload() {
	return m_payload;
}
// TAG_STRING
CTag_String::CTag_String() {
	m_tagId = TAG_STRING;
	m_payload = "";
}
bool CTag_String::read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly )
{
	char *pString;
	boost::int16_t payloadSize;

	if( !CTag::read( decompStream, pBytesRead, payloadOnly ) )
		return false;
	// Read the payload size
	decompStream.read( reinterpret_cast<char*>(&payloadSize), 2 );
	(*pBytesRead)+=2;
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( payloadSize );
#endif
	// Read the string
	if( payloadSize > 0 ) {
		pString = new char[payloadSize+1];
		decompStream.read( pString, payloadSize );
		(*pBytesRead)+=payloadSize;
		pString[payloadSize] = '\0'; // add terminator
		m_payload = std::string( pString );
		if( pString ) {
			delete[] pString;
			pString = NULL;
		}
	}
	return true;
}
std::string CTag_String::payload() {
	return m_payload;
}
// TAG_LIST
CTag_List::CTag_List() {
	m_tagId = TAG_LIST;
	m_payloadId = -1;
	m_payloadsLeft = 0;
}
bool CTag_List::read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly )
{
	boost::int32_t payloadSize;

	if( !CTag::read( decompStream, pBytesRead, payloadOnly ) )
		return false;
	// Read the payload id
	decompStream.read( reinterpret_cast<char*>(&m_payloadId), 1 );
	(*pBytesRead)+=1;
	// Read the payload size
	decompStream.read( reinterpret_cast<char*>(&payloadSize), 4 );
	(*pBytesRead)+=4;
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( payloadSize );
#endif
	m_payload.reserve( payloadSize );
	m_payloadsLeft = payloadSize;

	return true;
}
boost::int8_t CTag_List::payloadId() {
	return m_payloadId;
}
// TAG_COMPOUND
CTag_Compound::CTag_Compound() {
	m_tagId = TAG_COMPOUND;
}
bool CTag_Compound::read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly )
{
	if( !CTag::read( decompStream, pBytesRead, payloadOnly ) )
		return false;
	return true;
}
// TAG_INT_ARRAY
CTag_IntArray::CTag_IntArray() {
	m_tagId = TAG_INT_ARRAY;
}
bool CTag_IntArray::read( InputStream &decompStream, size_t *pBytesRead, bool payloadOnly )
{
	boost::int32_t payloadItem;
	boost::int32_t payloadSize;

	if( !CTag::read( decompStream, pBytesRead, payloadOnly ) )
		return false;
	// Read the payload size
	decompStream.read( reinterpret_cast<char*>(&payloadSize), 4 );
	(*pBytesRead)+=4;
#ifdef BOOST_LITTLE_ENDIAN
	boost::endian::reverse( payloadSize );
#endif
	m_payload.reserve( payloadSize );
	// Read the payload
	for( boost::int32_t i = 0; i < payloadSize; i++ ) {
		decompStream.read( reinterpret_cast<char*>(&payloadItem), 4 );
		(*pBytesRead)+=4;
		m_payload.push_back( payloadItem );
#ifdef BOOST_LITTLE_ENDIAN
		boost::endian::reverse( payloadItem );
#endif
	}
	return true;
}
std::vector<boost::int32_t> CTag_IntArray::payload() {
	return m_payload;
}

////////////////
// CTagReader //
////////////////

CTag* CTagReader::allocateTag( boost::int8_t tagId )
{
	CTag *pTag;

	switch( tagId )
	{
	case TAG_END:
		pTag = new CTag_End();
		break;
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
	case TAG_COMPOUND:
		pTag = new CTag_Compound();
		break;
	case TAG_INT_ARRAY:
		pTag = new CTag_IntArray();
		break;
	default:
		std::cout << "\t > Failed: tag reader found unknown tag type" << std::endl;
		return NULL;
	}

	return pTag;
}

CTagReader::CTagReader()
{
}

CTag* CTagReader::readTag( InputStream &decompStream, size_t *pBytesRead )
{
	boost::int8_t tagId;
	CTag *pTag;
	CTag_List *pParentList;
	bool inList;

	pTag = NULL;
	pParentList = NULL;
	inList = false;

	// Check if we're reading into a list
	if( !m_parentStack.empty() )
	{
		if( m_parentStack.back()->id() == TAG_LIST )
		{
			inList = true;
			pParentList = reinterpret_cast<CTag_List*>(m_parentStack.back());
			// If theres none left, we're not in a list and remove from stack
			if( pParentList->m_payloadsLeft <= 0 ) {
				m_parentStack.pop_back();
				inList = false;
				pParentList = NULL;
			}
		}
	}

	// First read the tag id, or use the list we're in's payload id
	if( !inList ) {
		decompStream.read( reinterpret_cast<char*>(&tagId), 1 );
		(*pBytesRead)+=1;
	}
	else
		tagId = pParentList->payloadId();
	
	// If we've reached the end of the file, we're done
	if( decompStream.eof() )
		return NULL;

	if( tagId == TAG_END || tagId == TAG_COMPOUND || tagId == TAG_LIST )
	{
		switch( tagId )
		{
		case TAG_END:
		{
			pTag = new CTag_End();
			// Read
			pTag->read( decompStream, pBytesRead, inList );
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
			break;
		}
		case TAG_LIST:
		case TAG_COMPOUND:
		{
			CTagParent *pTagParent = reinterpret_cast<CTagParent*>(CTagReader::allocateTag( tagId ));
			if( !pTagParent )
				return NULL;
			// Read
			pTagParent->read( decompStream, pBytesRead, inList );
			// Parent
			if( !m_parentStack.empty() )
				m_parentStack.back()->add( pTagParent );
			else
				m_tagTree.push_back( pTagParent );
			// Add to parent stack
			m_parentStack.push_back( pTagParent );
			pTag = pTagParent;
			break;
		}
		default:
			std::cout << "\t > Failed: tag reader found unknown tag type" << std::endl;
			return NULL;
		}

	}
	else
	{
		pTag = CTagReader::allocateTag( tagId );
		if( !pTag )
			return NULL;
		// Read
		pTag->read( decompStream, pBytesRead, inList );
		// Parent
		if( !m_parentStack.empty() )
			m_parentStack.back()->add( pTag );
		else
			m_tagTree.push_back( pTag );
	}

	// If we're in a list, decrease the amount of payloads left to read
	if( inList )
		pParentList->m_payloadsLeft--;

	/*std::string prefix = "";
	if( !m_parentStack.empty() ) {
		if( m_parentStack.back() == pTag ) {
			for( unsigned int i = 0; i < m_parentStack.size()-1; i++ )
				prefix += "   ";
		}
		else {
			for( unsigned int i = 0; i < m_parentStack.size(); i++ )
				prefix += "   ";
		}
	}
	else {
		for( unsigned int i = 0; i < m_parentStack.size(); i++ )
			prefix += "   ";
	}
	std::cout << prefix << " > \"" << pTag->name() << "\" (" << (int)pTag->id() << ")" << std::endl;*/

	return pTag;
}

// Pass zero to read until end of file
bool CTagReader::open( boost::filesystem::path fullPath, size_t length )
{
	boost::filesystem::ifstream inStream;
	InputStream decompStream;

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
		inStream.open( fullPath, std::ios::binary );
		if( !inStream ) {
			std::cout << "\t > Failed: tag reader failed to open file" << std::endl;
			std::cout << "\t\t" << m_filename << std::endl;
			return false;
		}
		// First step is to set up decompression filter
		decompStream.push( boost::iostreams::gzip_decompressor() );
		decompStream.push( inStream );
	}
	catch( const boost::filesystem::filesystem_error &e ) {
		std::cout << "\t > Failed: tag reader filesystem operation failed" << std::endl;
		std::cout << "\t\t" << m_filename << std::endl;
		std::cout << "\t\t\"" << e.what() << "\"" << std::endl;
		return false;
	}

	if( !this->open( decompStream, length ) )
		return false;

	// close
	if( inStream.is_open() )
		inStream.close();
	if( decompStream )
		boost::iostreams::close( decompStream );

	return true;
}
bool CTagReader::open( InputStream &decompStream, size_t length )
{
	size_t bytesRead;

	// Open the file
	try
	{
		// Read all tags in the file
		m_parentStack.clear();
		bytesRead = 0;
		while( length == 0 || bytesRead < length ) {
			if( !this->readTag( decompStream, &bytesRead ) )
				break;
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
void CTagReader::close() {
	m_filename = "";
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

CTag_Compound* CTagReader::getRoot()
{
	if( m_tagTree.size() > 0 ) {
		if( m_tagTree[0]->id() == TAG_COMPOUND )
			return reinterpret_cast<CTag_Compound*>(m_tagTree[0]);
		else {
			std::cout << "\t > Failed: first tag in tree was not a compound (root)" << std::endl;
			return NULL;
		}
	}
	else {
		std::cout << "\t > Failed: no tags in tag tree" << std::endl;
		return NULL;
	}
}