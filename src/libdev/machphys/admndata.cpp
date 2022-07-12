/*
 * A D M N D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/admndata.hpp"

MachPhysAdministratorData::MachPhysAdministratorData()
{

    TEST_INVARIANT;
}

MachPhysAdministratorData::~MachPhysAdministratorData()
{
    TEST_INVARIANT;

}

MATHEX_SCALAR MachPhysAdministratorData::accuracy() const
{
	return accuracy_;
}

void MachPhysAdministratorData::accuracy( const MATHEX_SCALAR& newAccuracy)
{
	accuracy_ = newAccuracy;
}

MATHEX_SCALAR MachPhysAdministratorData::nWeapons() const
{
	return nWeapons_;
}

void MachPhysAdministratorData::nWeapons( const MATHEX_SCALAR& newNumberOfWeapons)
{
	nWeapons_ = newNumberOfWeapons;
}

MATHEX_SCALAR MachPhysAdministratorData::maxSubordinates() const
{
	return maxSubordinates_;
}

void MachPhysAdministratorData::maxSubordinates( const MATHEX_SCALAR& newMaxSubordinates )
{
	maxSubordinates_ = newMaxSubordinates;
}

void MachPhysAdministratorData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysAdministratorData& t )
{

    //o << (MachPhysObjectData)t;
	//o << (MachPhysMachineData)t;
	o << "Accuracy " << t.accuracy() << std::endl;
	o << "Number Weaps " << t.nWeapons() << std::endl;
	o << "Max Subordinates " << t.maxSubordinates() << std::endl;

    return o;
}

/* End ADMNDATA.CPP *************************************************/
