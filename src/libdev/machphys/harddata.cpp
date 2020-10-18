/*
 * H A R D D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/harddata.hpp"

MachPhysHardwareLabData::MachPhysHardwareLabData()
{

    TEST_INVARIANT;
}

MachPhysHardwareLabData::MachPhysHardwareLabData( const MachPhysHardwareLabData& copyMe, const MexTransform3d& transform )
: MachPhysConstructionData( copyMe, transform )
{
}

MachPhysHardwareLabData::~MachPhysHardwareLabData()
{
    TEST_INVARIANT;

}

void MachPhysHardwareLabData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysHardwareLabData& t )
{

    //o << (MachPhysObjectData)t;
    //o << (MachPhysConstructionData)t;

    return o;
}

/* End HARDDATA.CPP *************************************************/
