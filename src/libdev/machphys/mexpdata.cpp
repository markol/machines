/*
 * M E X P D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/mexpdata.hpp"
#include "world4d/link.hpp"
#include "machphys/internal/mexidata.hpp"

#define CB_MacPhysMachineExplosionData_DEPIMPL() \
	CB_DEPIMPL( MachPhysMachineExplosionData::LinkDatas, links_ ); \
	CB_DEPIMPL( MATHEX_SCALAR, maxSize_ ); \
	CB_DEPIMPL( size_t, minToShootOff_ ); \
	CB_DEPIMPL( size_t, maxToShootOff_ ); \
	CB_DEPIMPL( MexPoint3d, explosionCenter_ );

PER_DEFINE_PERSISTENT( MachPhysMachineExplosionData );

MachPhysMachineExplosionData::MachPhysMachineExplosionData()
: pImpl_( _NEW( MachPhysIMachineExplosionData() ) )
{

    TEST_INVARIANT;
}

//  Copy the data "as is"
MachPhysMachineExplosionData::MachPhysMachineExplosionData(
    const MachPhysMachineExplosionData& copyMe )
: pImpl_( _NEW( MachPhysIMachineExplosionData( *copyMe.pImpl_ ) )	)
{

  TEST_INVARIANT;
}

MachPhysMachineExplosionData&
    MachPhysMachineExplosionData::operator =(
    const MachPhysMachineExplosionData& copyMe )
{
   CB_MacPhysMachineExplosionData_DEPIMPL()

	PRE(pImpl_ and copyMe.pImpl_);
    links_ = copyMe.pImpl_->links_;

    maxSize_ = copyMe.pImpl_->maxSize_;
    minToShootOff( copyMe.minToShootOff() );
    maxToShootOff( copyMe.maxToShootOff() );
    explosionCenter( copyMe.explosionCenter() );

    return *this;
}

MachPhysMachineExplosionData::~MachPhysMachineExplosionData()
{
    TEST_INVARIANT;
    _DELETE( pImpl_ );

}

void MachPhysMachineExplosionData::addData( const LinkData& data )
{
   CB_MacPhysMachineExplosionData_DEPIMPL()
    //  Put the links that have to shoot off at the front of the
    //  list so that they will be considered first. This means
    //  that if we hit our maximum links to shoot off, all of the
    //  ones that have to shoot off are most likely to have done so.

    if( data.shootOffProbability() == 1.0 )
        links_.push_front( data );
    else
        links_.push_back( data );

    maxSize_ = std::max( maxSize_, data.size() );
}

const MachPhysMachineExplosionData::LinkDatas&
  MachPhysMachineExplosionData::links() const
{
   CB_MacPhysMachineExplosionData_DEPIMPL()
    return links_;
}

MATHEX_SCALAR   MachPhysMachineExplosionData::maxSize() const
{
    return pImpl_->maxSize_;
}

size_t  MachPhysMachineExplosionData::maxToShootOff() const
{
	PRE(pImpl_);
    return pImpl_->maxToShootOff();
}

void    MachPhysMachineExplosionData::maxToShootOff( size_t newMax )
{
	PRE(pImpl_);
    pImpl_->maxToShootOff( newMax );
}

const MexPoint3d&  MachPhysMachineExplosionData::explosionCenter() const
{
	PRE(pImpl_);
    return pImpl_->explosionCenter();
}

void    MachPhysMachineExplosionData::explosionCenter( const MexPoint3d& newCenter )
{
	PRE(pImpl_);
    pImpl_->explosionCenter( newCenter );
}

size_t  MachPhysMachineExplosionData::minToShootOff() const
{
	PRE(pImpl_);
    return pImpl_->minToShootOff_;
}

void    MachPhysMachineExplosionData::minToShootOff( size_t newMin )
{
	PRE(pImpl_);
    pImpl_->minToShootOff_ = newMin;
}


void MachPhysMachineExplosionData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void perWrite( PerOstream& ostr, const MachPhysMachineExplosionData& data )
{
	PRE( data.pImpl_ );
    ostr << data.pImpl_;
}

void perRead( PerIstream& istr, MachPhysMachineExplosionData& data )
{
	_DELETE( data.pImpl_ );
    istr >> data.pImpl_;
}

/* End MEXPDATA.CPP *************************************************/
