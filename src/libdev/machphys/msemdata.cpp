/*
 * M S E M D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/msemdata.hpp"

MachPhysMissileEmplacementData::MachPhysMissileEmplacementData()
{

    TEST_INVARIANT;
}

MachPhysMissileEmplacementData::MachPhysMissileEmplacementData( const MachPhysMissileEmplacementData& copyMe, const MexTransform3d& transform )
: MachPhysConstructionData( copyMe, transform ),
  nWeapons_( copyMe.nWeapons_ ),
  accuracy_( copyMe.accuracy_ ),
  maxRotation_( copyMe.maxRotation_ )
{
}

MachPhysMissileEmplacementData::~MachPhysMissileEmplacementData()
{
    TEST_INVARIANT;

}

MATHEX_SCALAR MachPhysMissileEmplacementData::nWeapons( void ) const
{
	return nWeapons_;
}

void MachPhysMissileEmplacementData::nWeapons( const MATHEX_SCALAR& newNumberOfWeapons )
{
	nWeapons_ = newNumberOfWeapons;
}

MATHEX_SCALAR MachPhysMissileEmplacementData::accuracy( void ) const
{
	return accuracy_;
}

void MachPhysMissileEmplacementData::accuracy( const MATHEX_SCALAR& newAccuracy )
{
	accuracy_ = newAccuracy;
}

MexRadians MachPhysMissileEmplacementData::maxRotation( void ) const
{
	return maxRotation_;
}

void MachPhysMissileEmplacementData::maxRotation( const MexRadians& newMaxRotation )
{
	maxRotation_ = newMaxRotation;
}

void MachPhysMissileEmplacementData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysMissileEmplacementData& t )
{

    //o << (MachPhysObjectData)t;
    //o << (MachPhysConstructionData)t;
	o << "Number Weap " << t.nWeapons() << std::endl;
	o << "Accuracy " << t.accuracy() << std::endl;
	o << "MaxRotation " << t.maxRotation() << std::endl;

    return o;
}

/* End msemDATA.CPP *************************************************/
