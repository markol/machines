/*
 * U I N T 6 4 . I P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#include <iomanip>

inline
UtlUint64::UtlUint64()
{
    data_[ LS ] = 0;
    data_[ MS ] = 0;
}

// inline
// UtlUint64::UtlUint64( const UtlUint64& copyMe )
// : data_[ LS ]( copyMe.data_[ LS ] ),
//   data_[ MS ]( copyMe.data_[ MS ] )
// {
// }

inline
UtlUint64::UtlUint64( uint32 copyMe )
{
    data_[ LS ] = copyMe;
    data_[ MS ] = 0;
}

inline
UtlUint64::~UtlUint64()
{
}

inline
UtlUint64& UtlUint64::operator =( uint32 rhs )
{
    data_[ LS ] = rhs;
    data_[ MS ] = 0;

    return *this;
}

inline
UtlUint64& UtlUint64::operator +=( const UtlUint64& rhs )
{
    size_t originalLs = data_[ LS ];

    data_[ LS ] += rhs.data_[ LS ];

    if( data_[ LS ] < originalLs or data_[ LS ] < rhs.data_[ LS ] )
        ++data_[ MS ];

    data_[ MS ] += rhs.data_[ MS ];

    return *this;
}

inline
UtlUint64& UtlUint64::operator -=( const UtlUint64& rhs )
{
    if( data_[ LS ] < rhs.data_[ LS ] )
    {
        //  Sort out borrow

        --data_[ MS ];
    }

    data_[ LS ] -= rhs.data_[ LS ];

    data_[ MS ] -= rhs.data_[ MS ];

    return *this;
}

inline
bool operator ==( const UtlUint64& a, const UtlUint64& b )
{
    return a.data_[ UtlUint64::MS ] == b.data_[ UtlUint64::MS ] and a.data_[ UtlUint64::LS ] == b.data_[ UtlUint64::LS ];
}

inline
ostream& operator <<( ostream& o, const UtlUint64& t )
{
    hex( o );

    if( t.data_[ UtlUint64::MS ] != 0 )
    {
        o << t.data_[ UtlUint64::MS ];
        o << std::setw( 8 );
        o << std::setfill( '0' );
    }

    o << t.data_[ UtlUint64::LS ];
    dec( o );

    return o;
}

inline
uint32* UtlUint64::data( void )
{
    return data_;
}

inline
double  UtlUint64::asDouble( void ) const
{
    double  dValue;

    dValue = data_[ UtlUint64::MS ];

    dValue *= 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2;
    dValue *= 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2;
    dValue *= 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2;
    dValue *= 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2;
    dValue += data_[ UtlUint64::LS ];

    return dValue;
}

/* End UtlUint64.IPP ***************************************************/
