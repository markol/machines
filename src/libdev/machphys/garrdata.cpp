/*
 * G A R R D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/garrdata.hpp"

MachPhysGarrisonData::MachPhysGarrisonData()
{

    TEST_INVARIANT;
}

MachPhysGarrisonData::MachPhysGarrisonData( const MachPhysGarrisonData& copyMe, const MexTransform3d& transform )
: 	MachPhysConstructionData( copyMe, transform ),
	repairRate_( copyMe.repairRate_ )
{
}

MachPhysGarrisonData::~MachPhysGarrisonData()
{
    TEST_INVARIANT;

}

MachPhys::HitPointUnits MachPhysGarrisonData::repairRate( void ) const
{
	return repairRate_;
}

void MachPhysGarrisonData::repairRate( MachPhys::HitPointUnits newRepairRate )
{
	repairRate_ = newRepairRate;
}

void MachPhysGarrisonData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysGarrisonData& t )
{

    //o << (MachPhysObjectData)t;
    //o << (MachPhysConstructionData)t;

    return o;
}

/* End GARRDATA.CPP *************************************************/
