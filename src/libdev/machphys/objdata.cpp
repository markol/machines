/*
 * O B J D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/objdata.hpp"

MachPhysObjectData::MachPhysObjectData()
{

    TEST_INVARIANT;
}

MachPhysObjectData::MachPhysObjectData( const MachPhysObjectData& rhs )
: armour_( rhs.armour_ ),
  hitPoints_( rhs.hitPoints_ ),
  cost_( rhs.cost_ )
{
}

MachPhysObjectData::~MachPhysObjectData()
{
    TEST_INVARIANT;

}

MachPhys::ArmourUnits MachPhysObjectData::armour( void ) const
{
	return armour_;
}

void MachPhysObjectData::armour( const MachPhys::ArmourUnits& newArmour )
{
	armour_ = newArmour;
}

MachPhys::HitPointUnits MachPhysObjectData::hitPoints( void ) const
{
	return hitPoints_;
}

void MachPhysObjectData::hitPoints( const MachPhys::HitPointUnits& newHp )
{
	hitPoints_ = newHp;
}

MachPhys::BuildingMaterialUnits MachPhysObjectData::cost( void ) const
{
	return cost_;
}

void MachPhysObjectData::cost( const MachPhys::BuildingMaterialUnits& newCost )
{
	cost_ = newCost;
}


void MachPhysObjectData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysObjectData& t )
{

	o << "armour " << t.armour() << std::endl;
	o << "hit points " << t.hitPoints() << std::endl;
	o << "cost " << t.cost() << std::endl;

    return o;
}

/* End OBJDATA.CPP **************************************************/
