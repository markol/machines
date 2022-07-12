/*
 * F I L T E R . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <sstream>

#include "base/private/filter.hpp"
#include "base/internal/filtbuff.hpp"

BaseFilterOstream::BaseFilterOstream( ostream& ostr )
: ostream( pFilterBuffer() ),
  pOstr_( &ostr )
{

}

BaseFilterOstream::BaseFilterOstream()
: ostream( pFilterBuffer() ),
  pOstr_( NULL )
{

}

BaseFilterOstream::~BaseFilterOstream()
{

    _DELETE( pBuffer_ );
}

void BaseFilterOstream::stream( ostream& ostr )
{
    pOstr_ = &ostr;
}

BaseFilterBuffer* BaseFilterOstream::pFilterBuffer()
{
    pBuffer_ = _NEW( BaseFilterBuffer( this ) );
    return pBuffer_;
}

void    BaseFilterOstream::filterCharacters( const char* pBuf, size_t nChars )
{
    doFilterCharacters( pBuf, nChars );
}

void    BaseFilterOstream::outputCharacters( const char* pBuf, size_t nChars )
{
    PRE( pOstr_ != NULL );

    pOstr_->write( pBuf, nChars );
}

/* End FILTER.CPP *************************************************/
