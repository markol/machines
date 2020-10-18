/*
 * E X P P O I N T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/cexpdata.hpp"

MachPhysConstructionExplosionData::ExplosionPoint::ExplosionPoint( void )
{
    ASSERT_FAIL( "" );
}

MachPhysConstructionExplosionData::ExplosionPoint::ExplosionPoint(
    const MexPoint3d&      position,
    MATHEX_SCALAR   maxFireballOffset,
    size_t          minFireballs,
    size_t          maxFireballs,
    MATHEX_SCALAR   minTimeFactor,
    MATHEX_SCALAR   maxTimeFactor,
    MATHEX_SCALAR   size,
    MATHEX_SCALAR   depthOffset )
: position_( position ),
  maxFireballOffset_( maxFireballOffset ),
  minFireballs_( minFireballs ),
  maxFireballs_( maxFireballs ),
  minTimeFactor_( minTimeFactor ),
  maxTimeFactor_( maxTimeFactor ),
  size_( size ),
  depthOffset_( depthOffset )
{
    PRE_INFO( minTimeFactor );
    PRE_INFO( maxTimeFactor );
    PRE( 0.0 <= minTimeFactor and minTimeFactor <= 1.0 );
    PRE( 0.0 <= maxTimeFactor and maxTimeFactor <= 1.0 );

    TEST_INVARIANT;
}

MachPhysConstructionExplosionData::ExplosionPoint::~ExplosionPoint()
{
    TEST_INVARIANT;

}

const MexPoint3d& MachPhysConstructionExplosionData::ExplosionPoint::position( void ) const
{
    return position_;
}

MATHEX_SCALAR MachPhysConstructionExplosionData::ExplosionPoint::maxFireballOffset( void ) const
{
    return maxFireballOffset_;
}

size_t MachPhysConstructionExplosionData::ExplosionPoint::minFireballs( void ) const
{
    return minFireballs_;
}

size_t MachPhysConstructionExplosionData::ExplosionPoint::maxFireballs( void ) const
{
    return maxFireballs_;
}

MATHEX_SCALAR   MachPhysConstructionExplosionData::ExplosionPoint::minTimeFactor( void ) const
{
    MATHEX_SCALAR   result = minTimeFactor_;

    POST_INFO( minTimeFactor_ );
    POST( 0.0 <= result and result <= 1.0 );

    return result;
}

MATHEX_SCALAR   MachPhysConstructionExplosionData::ExplosionPoint::maxTimeFactor( void ) const
{
    MATHEX_SCALAR   result = maxTimeFactor_;

    POST_INFO( maxTimeFactor_ );
    POST( 0.0 <= result and result <= 1.0 );

    return result;
}

MATHEX_SCALAR   MachPhysConstructionExplosionData::ExplosionPoint::size( void ) const
{
    return size_;
}

MATHEX_SCALAR   MachPhysConstructionExplosionData::ExplosionPoint::depthOffset( void ) const
{
    return depthOffset_;
}

void MachPhysConstructionExplosionData::ExplosionPoint::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysConstructionExplosionData::ExplosionPoint& t )
{

    o << "ExplosionPoint " << (void*)&t << " start" << std::endl;
    o << "ExplosionPoint " << (void*)&t << " end" << std::endl;

    return o;
}

/* End EXPPOINT.CPP *************************************************/
