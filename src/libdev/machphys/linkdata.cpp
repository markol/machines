/*
 * L I N K D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "ctl/list.hpp"
#include "world4d/composit.hpp"
#include "world4d/link.hpp"

#include "machphys/mexpdata.hpp"

MachPhysMachineExplosionData::LinkData::LinkData()
: initialised_( false )
{

    POST( not initialised_ );
    TEST_INVARIANT;
}

MachPhysMachineExplosionData::LinkData::LinkData(
    W4dLinkId linkId,
    MATHEX_SCALAR size,
    MATHEX_SCALAR shootOffProbability,
    const PhysRelativeTime& minExplosionTime,
    const PhysRelativeTime& maxExplosionTime  )
: initialised_( true ),
  linkId_( linkId ),
  size_( size ),
  shootOffProbability_( shootOffProbability ),
  minExplosionTime_( minExplosionTime ),
  maxExplosionTime_( maxExplosionTime )
{
    PRE( 0.0 <= shootOffProbability and shootOffProbability <= 1.0 );
    PRE( maxExplosionTime >= minExplosionTime );

    discreteSize_ = pow( size, 0.5 );

    discreteSize_ = std::max( 1U,  discreteSize_ );
    discreteSize_ = std::min( 10U, discreteSize_ );

    POST( initialised_ );
}

MachPhysMachineExplosionData::LinkData::LinkData( const LinkData& copyMe )
: initialised_( copyMe.initialised_ ),
  linkId_( copyMe.linkId_ ),
  size_( copyMe.size_ ),
  discreteSize_( copyMe.discreteSize_ ),
  shootOffProbability_( copyMe.shootOffProbability_ ),
  minExplosionTime_( copyMe.minExplosionTime_ ),
  maxExplosionTime_( copyMe.maxExplosionTime_ )
{
}

MachPhysMachineExplosionData::LinkData&
MachPhysMachineExplosionData::LinkData::operator =( const LinkData& copyMe )
{
    PRE( copyMe.initialised_ );

    initialised_ = copyMe.initialised_;
    linkId_ = copyMe.linkId_;
    size_ = copyMe.size_;
    discreteSize_ = copyMe.discreteSize_;
    shootOffProbability_ = copyMe.shootOffProbability_;
    minExplosionTime_ = copyMe.minExplosionTime_;
    maxExplosionTime_ = copyMe.maxExplosionTime_;

    return *this;
}

MachPhysMachineExplosionData::LinkData::~LinkData()
{
    TEST_INVARIANT;

}

W4dLinkId MachPhysMachineExplosionData::LinkData::linkId( void ) const
{
    PRE( initialised_ );

    return linkId_;
}

MATHEX_SCALAR MachPhysMachineExplosionData::LinkData::size( void ) const
{
    PRE( initialised_ );

    return size_;
}

MATHEX_SCALAR   MachPhysMachineExplosionData::LinkData::shootOffProbability( void ) const
{
    PRE( initialised_ );

    MATHEX_SCALAR result = shootOffProbability_;

    POST( 0.0 <= result and result <= 1.0 );

    return result;
}

const PhysRelativeTime& MachPhysMachineExplosionData::LinkData::minExplosionTime( void ) const
{
    PRE( initialised_ );

    return minExplosionTime_;
}

const PhysRelativeTime& MachPhysMachineExplosionData::LinkData::maxExplosionTime( void ) const
{
    PRE( initialised_ );

    return maxExplosionTime_;
}

void MachPhysMachineExplosionData::LinkData::size( MATHEX_SCALAR newSize )
{
    PRE( initialised_ );

    size_ = newSize;
}

void MachPhysMachineExplosionData::LinkData::shootOffProbability( MATHEX_SCALAR newProbability )
{
    PRE( initialised_ );
    PRE( 0.0 <= newProbability and newProbability <= 1.0 );

    shootOffProbability_ = newProbability;
}

void MachPhysMachineExplosionData::LinkData::minExplosionTime( const PhysRelativeTime& newTime )
{
    PRE( initialised_ );

    minExplosionTime_ = newTime;
}


void MachPhysMachineExplosionData::LinkData::maxExplosionTime( const PhysRelativeTime& newTime )
{
    PRE( initialised_ );

    maxExplosionTime_ = newTime;
}

//  Return the size as one of 10 discrete sizes. All parts
//  will be categorised in one of these sizes
uint    MachPhysMachineExplosionData::LinkData::discreteSize( void ) const
{
    PRE( initialised_ );

    uint    result = discreteSize_;

    POST( 1 <= result and result <= 10 );

    return result;
}

void MachPhysMachineExplosionData::LinkData::discreteSize( uint newDiscreteSize )
{
    PRE( initialised_ );
    PRE( 1 <= newDiscreteSize and newDiscreteSize <= 10 );

    discreteSize_ = newDiscreteSize;
}

void MachPhysMachineExplosionData::LinkData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

bool operator ==( const MachPhysMachineExplosionData::LinkData& a, const MachPhysMachineExplosionData::LinkData& b )
{
    PRE( a.initialised_ );
    PRE( b.initialised_ );

    return
        a.linkId_ == b.linkId_ and
        a.size_ == b.size_ and
        a.discreteSize_ == b.discreteSize_ and
        a.shootOffProbability_ == b.shootOffProbability_ and
        a.minExplosionTime_ == b.minExplosionTime_ and
        a.maxExplosionTime_ == b.maxExplosionTime_;
}

//  This is here purely for the ctl_list instantiation
bool operator <( const MachPhysMachineExplosionData::LinkData&, const MachPhysMachineExplosionData::LinkData& )
{
    ASSERT_FAIL( "" );

    return false;
}

ostream& operator <<( ostream& o, const MachPhysMachineExplosionData::LinkData& t )
{
    INSPECT_ON( o, t.linkId_ );
    INSPECT_ON( o, t.size_ );
    INSPECT_ON( o, t.discreteSize_ );
    INSPECT_ON( o, t.shootOffProbability_ );
    INSPECT_ON( o, t.minExplosionTime_ );
    INSPECT_ON( o, t.maxExplosionTime_ );

    return o;
}

/* End LINKDATA.CPP *************************************************/
