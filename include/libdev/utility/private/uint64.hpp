/*
 * U I N T 6 4 . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    UtlUint64

    A brief description of the class should go in here
*/

#ifndef _UTILITY_UINT64_HPP
#define _UTILITY_UINT64_HPP

#include "base/base.hpp"

class UtlUint64
{
public:
    UtlUint64( void );
    ~UtlUint64( void );

    //  Compiler supplied copy constructor is sufficient
//    UtlUint64( const UtlUint64& );
    UtlUint64( uint32 );

    //  Compiler supplied assignment op is sufficient
//    UtlUint64& operator =( const UtlUint64& );
    UtlUint64& operator =( uint32 );

    UtlUint64& operator +=( const UtlUint64& );
    UtlUint64& operator -=( const UtlUint64& );

    uint32* data( void );
    
    double  asDouble( void ) const;
    
    friend ostream& operator <<( ostream& o, const UtlUint64& t );
    friend bool operator ==( const UtlUint64&, const UtlUint64& );

private:

    enum {LS = 0, MS = 1 };
    uint32  data_[ 2 ];
};

#include "utility/private/uint64.ipp"

#endif

/* End UtlUint64.HPP ***************************************************/
