/*
 * S O F T D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/softdata.hpp"

MachPhysSoftwareLabData::MachPhysSoftwareLabData()
{

    TEST_INVARIANT;
}

MachPhysSoftwareLabData::MachPhysSoftwareLabData( const MachPhysSoftwareLabData& copyMe, const MexTransform3d& transform )
: MachPhysConstructionData( copyMe, transform )
{
}

MachPhysSoftwareLabData::~MachPhysSoftwareLabData()
{
    TEST_INVARIANT;

}

void MachPhysSoftwareLabData::CLASS_INVARIANT
{
}

ostream& operator <<( ostream& o, const MachPhysSoftwareLabData& t )
{

    //o << (MachPhysObjectData)t;
    //o << (MachPhysConstructionData)t;

    return o;
}

/* End SOFTDATA.CPP *************************************************/
