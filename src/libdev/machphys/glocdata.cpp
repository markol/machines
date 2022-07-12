/*
 * L O C R D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/glocdata.hpp"

MachPhysGeoLocatorData::MachPhysGeoLocatorData()
{

    TEST_INVARIANT;
}

MachPhysGeoLocatorData::~MachPhysGeoLocatorData()
{
    TEST_INVARIANT;

}

MachPhys::MineralGrade MachPhysGeoLocatorData::mineralGrade() const
{
	return mineralGrade_;
}

void MachPhysGeoLocatorData::mineralGrade( const MachPhys::MineralGrade& newMineralGrade )
{
	mineralGrade_ = newMineralGrade;
}

void MachPhysGeoLocatorData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysGeoLocatorData& t )
{

    //o << (MachPhysObjectData)t;
	//o << (MachPhysMachineData)t;

    return o;
}

/* End LOCRDATA.CPP *************************************************/
