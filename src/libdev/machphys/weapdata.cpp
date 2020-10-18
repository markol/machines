/*
 * W E A P D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/weapdata.hpp"

MachPhysWeaponsLabData::MachPhysWeaponsLabData()
{

    TEST_INVARIANT;
}

MachPhysWeaponsLabData::MachPhysWeaponsLabData( const MachPhysWeaponsLabData& copyMe, const MexTransform3d& transform )
: MachPhysConstructionData( copyMe, transform )
{
}

MachPhysWeaponsLabData::~MachPhysWeaponsLabData()
{
    TEST_INVARIANT;

}

void MachPhysWeaponsLabData::CLASS_INVARIANT
{
}

ostream& operator <<( ostream& o, const MachPhysWeaponsLabData& t )
{

    //o << (MachPhysObjectData)t;
    //o << (MachPhysConstructionData)t;

    return o;
}

/* End WEAPDATA.CPP *************************************************/
