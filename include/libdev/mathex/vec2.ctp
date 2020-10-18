/* 
 * V E C 2 . T P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#include "base/base.hpp"

#include "mathex/mathex.hpp"
#include "mathex/vec2.hpp"

template< class SCALAR >
MexVec2< SCALAR >::MexVec2( SCALAR x, SCALAR y )
{
    vec_[ 0 ] = x;
    vec_[ 1 ] = y;
}

template< class SCALAR >
MexVec2< SCALAR >::MexVec2( const MexVec2< SCALAR >& v)
{
    vec_[ 0 ] = v.vec_[ 0 ];
    vec_[ 1 ] = v.vec_[ 1 ];
}

template< class SCALAR >
MexVec2< SCALAR >::~MexVec2()
{
    /* intentionally left blank */
}

template< class SCALAR >
const MexVec2< SCALAR >&
MexVec2< SCALAR >::operator =( const MexVec2< SCALAR >& v)
{
    PRE( isValid( v ) );
    
    if( this != &v )
    {   
        vec_[ 0 ] = v.vec_[ 0 ];
        vec_[ 1 ] = v.vec_[ 1 ];
    }

    POST( *this == v );

    return *this;
}

template< class SCALAR >
bool MexVec2< SCALAR >::operator ==( const MexVec2< SCALAR >& v ) const
{
    return vec_[ 0 ] == v.vec_[ 0 ] and vec_[ 1 ] == v.vec_[ 1 ];
}

template< class SCALAR >
bool MexVec2< SCALAR >::isZeroVector( ) const
{
    return vec_[ 0 ] == 0 and vec_[ 1 ] == 0;
}

template< class SCALAR >
void MexVec2< SCALAR >::operator +=( const MexVec2< SCALAR >& v )
{
    PRE( isValid( v ) );
    
    vec_[ 0 ] += v.vec_[ 0 ];
    vec_[ 1 ] += v.vec_[ 1 ];
}

template< class SCALAR >
const MexVec2< SCALAR >& MexVec2< SCALAR >::zeroVector()
{
    static MexVec2 zero_ = MexVec2( 0, 0 );

    return zero_;        
}

template< class SCALAR >
SCALAR MexVec2< SCALAR >::x( void ) const
{
    return vec_[ 0 ];
}

template< class SCALAR >
SCALAR MexVec2< SCALAR >::y( void ) const
{
    return vec_[ 1 ];
}

/////////////////////////////////////////////////////////////

template< class SCALAR >
SCALAR MexVec2< SCALAR >::operator []( size_t i ) const
{
    PRE( i < 2 );

    return( vec_[i] );    
}

/////////////////////////////////////////////////////////////

template< class SCALAR >
SCALAR& MexVec2< SCALAR >::operator []( size_t i )
{
    PRE( i < 2 );

    return( vec_[i] );
}

/////////////////////////////////////////////////////////////

template< class SCALAR >
void MexVec2< SCALAR >::setVector( SCALAR x, SCALAR y )
{
    vec_[0] = x;
    vec_[1] = y;
}

/* End VEC2.CTP *****************************************************/
