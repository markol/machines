/*
 * A P P E N D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    BaseAppendOstream

    Define an ostream class that, on every write, will open a file,
    seek to the end, then close the file. This is extremely
    inefficient but if the program is crashing the machine may be
    the only way of getting debugging information out.
*/

#ifndef _BASE_APPEND_HPP
#define _BASE_APPEND_HPP

#include <iostream>
#include <string>

#include "base/base.hpp"
#include "base/internal/appbuff.hpp"

class BaseAppendOstream : public std::ostream
{
public:
    //  Initialise an append stream to write data out to the given file.
    //  Note that the file will not be cleared automatically
    BaseAppendOstream( const char* fileName );
    //  PRE( fileName != NULL );

    BaseAppendOstream( void );

    //  Clear the file
    void    clear( void );

    void    name( const char* fileName );

    virtual ~BaseAppendOstream( void );

private:
    // Operation deliberately revoked
    BaseAppendOstream( const BaseAppendOstream& );

    // Operation deliberately revoked
    BaseAppendOstream& operator =( const BaseAppendOstream& );

    // Operation deliberately revoked
    bool operator ==( const BaseAppendOstream& );

//    BaseAppendBuffer* pBuffer( const char* fileName );

    BaseAppendBuffer  appendBuffer_;

    static  std::streambuf&  dummyStreamBuf();
};


#endif

/* End APPEND.HPP *************************************************/
