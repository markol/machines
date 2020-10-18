/*
 * A G G R D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/aggrdata.hpp"

MachPhysAggressorData::MachPhysAggressorData()
{

    TEST_INVARIANT;
}

MachPhysAggressorData::~MachPhysAggressorData()
{
    TEST_INVARIANT;

}

MATHEX_SCALAR MachPhysAggressorData::accuracy( void ) const
{
	return accuracy_;
}

void MachPhysAggressorData::accuracy( const MATHEX_SCALAR& newAccuracy)
{
	accuracy_ = newAccuracy;
}

MATHEX_SCALAR MachPhysAggressorData::nWeapons( void ) const
{
	return nWeapons_;
}

void MachPhysAggressorData::nWeapons( const MATHEX_SCALAR& newNumberOfWeapons)
{
	nWeapons_ = newNumberOfWeapons;
}

void MachPhysAggressorData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysAggressorData& t )
{

    //o << (MachPhysObjectData)t;
	//o << (MachPhysMachineData)t;
	o << "Accuracy " << t.accuracy() << std::endl;
	o << "Number Weaps " << t.nWeapons() << std::endl;

    return o;
}

/* End AGGRDATA.CPP *************************************************/
