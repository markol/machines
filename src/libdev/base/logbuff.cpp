/*
 * L O G S T R E A . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <sstream>

#include "base/logbuff.hpp"
#include "base/internal/cstrbuff.hpp"
#include "base/internal/infsbuff.hpp"

BaseLogBuffer::BaseLogBuffer()
//: ostream( pInfiniteStreamBuffer() )
{

}

BaseLogBuffer::BaseLogBuffer( size_t bufferSize )
//: ostream( pCircularStreamBuffer( bufferSize ) )
{
}

BaseLogBuffer::~BaseLogBuffer()
{

//    _DELETE( pStreamBuffer_ );
}

void BaseLogBuffer::clear()
{

//    pStreamBuffer_->clear();
    this->str("");
}

BaseLogStreamBuffer* BaseLogBuffer::pInfiniteStreamBuffer()
{
    pStreamBuffer_ = _NEW( BaseInfiniteStreamBuffer() );
    return pStreamBuffer_;
}

BaseLogStreamBuffer* BaseLogBuffer::pCircularStreamBuffer( size_t bufferSize )
{
    pStreamBuffer_ = _NEW( BaseCircularStreamBuffer( bufferSize ) );

    return pStreamBuffer_;
}

std::string BaseLogBuffer::dataAsString() const
{
    /*size_t  length = pStreamBuffer_->nCharactersInBuffer() + 1;
    char* str = _NEW_ARRAY( char, length );

    //  This shouldn't be necessary but I have had problems if I fail to include it.
    str[ 0 ] = '\0';

    //TODO check
    //std::ostrstream strstream( str, length - 1, std::ios::out );
    //std::stringstream strstream( str, std::ios::out );

    //strstream << *this;

    str[ length - 1 ] = '\0';

    std::string  result( str );;
    _DELETE_ARRAY( str );

    return result;*/

    return this->str();
}

ostream& operator <<( ostream& o, const BaseLogBuffer& t )
{
    o << t.rdbuf();

    return o;
}

/* End LOGSTREA.CPP *************************************************/
