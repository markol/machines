/*
 * A P P E N D . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <sstream>

#include "base/internal/append.hpp"
#include "base/internal/appbuff.hpp"
#include "base/internal/memchk.hpp"

BaseAppendOstream::BaseAppendOstream( const char* fileName )
: ostream( &dummyStreamBuf() ),
  appendBuffer_( fileName )
{
    //ostream& thisAsOStream = *this;
    // TODO: figure out this construction:
    //thisAsOStream = &appendBuffer_;
    this->rdbuf(&appendBuffer_);
}

BaseAppendOstream::BaseAppendOstream()
: ostream( &dummyStreamBuf() )
{
    //ostream& thisAsOStream = *this;

    //thisAsOStream = &appendBuffer_;
    this->rdbuf(&appendBuffer_);
}

BaseAppendOstream::~BaseAppendOstream()
{
}

void    BaseAppendOstream::name( const char* fileName )
{
    appendBuffer_.name( fileName );
}

// BaseAppendBuffer* BaseAppendOstream::pBuffer( const char* fileName )
// {
//     if( fileName )
//         pAppendBuffer_ = _NEW_WITHOUT_LOG( BaseAppendBuffer( fileName ) );
//     else
//         pAppendBuffer_ = _NEW_WITHOUT_LOG( BaseAppendBuffer() );
//
//     return pAppendBuffer_;
// }

// static
std::streambuf&  BaseAppendOstream::dummyStreamBuf()
{
    //  This is a dummy streambuf that is only used to keep
    //  the ostream constructor happy.

    static  BaseAppendBuffer   str_;

    return str_;
}

void BaseAppendOstream::clear()
{
    appendBuffer_.clear();
}

/* End APPEND.CPP *************************************************/
