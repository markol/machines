/*
 * S M E L D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/smeldata.hpp"

MachPhysSmelterData::MachPhysSmelterData()
{

    TEST_INVARIANT;
}

MachPhysSmelterData::MachPhysSmelterData( const MachPhysSmelterData& copyMe, const MexTransform3d& transform )
: MachPhysConstructionData( copyMe, transform ),
  capacity_( copyMe.capacity_ ),
  mineDistance_( copyMe.mineDistance_)
{
}

MachPhysSmelterData::~MachPhysSmelterData()
{
    TEST_INVARIANT;

}

MachPhys::SizeUnits MachPhysSmelterData::capacity( void ) const
{
	return capacity_;
}

void MachPhysSmelterData::capacity( const MachPhys::SizeUnits& newCapacity )
{
	capacity_ = newCapacity;
}

MATHEX_SCALAR MachPhysSmelterData::sqrMineDistance( void ) const
{
	return mineDistance_;
}

void MachPhysSmelterData::sqrMineDistance( MATHEX_SCALAR newMineDistance)
{
	mineDistance_ = newMineDistance;
}

void MachPhysSmelterData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysSmelterData& t )
{

    //o << (MachPhysObjectData)t;
    //o << (MachPhysConstructionData)t;

    return o;
}

/* End SMELDATA.CPP *************************************************/
