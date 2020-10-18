/*
 * B E A C D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/beacdata.hpp"

MachPhysBeaconData::MachPhysBeaconData()
{

    TEST_INVARIANT;
}

MachPhysBeaconData::MachPhysBeaconData( const MachPhysBeaconData& copyMe, const MexTransform3d& transform )
: MachPhysConstructionData( copyMe, transform )
{
}

MachPhysBeaconData::~MachPhysBeaconData()
{
    TEST_INVARIANT;

}

void MachPhysBeaconData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysBeaconData& t )
{

    //o << (MachPhysObjectData)t;
    //o << (MachPhysConstructionData)t;

    return o;
}

/* End BEACDATA.CPP *************************************************/
