/*
 * M A D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "planeted/madata.hpp"

MachPhysActorData::MachPhysActorData()
{

    TEST_INVARIANT;
}

MachPhysActorData::~MachPhysActorData()
{
    TEST_INVARIANT;

}

MachPhys::buildingMaterialUnits MachPhysActorData::cost() const
{
	return cost_;
}

void MachPhysActorData::cost( const MachPhys::buildingMaterialUnits& newCost )
{
	cost_ = newCost;
}

MachPhys::researchUnits MachPhysActorData::hwResearchCost() const
{
	return hwResearchCost_;
}

void MachPhysActorData::hwResearchCost( const MachPhys::researchUnits& newResearchCost )
{
	hwResearchCost_ = newResearchCost;
}


void MachPhysActorData::CLASS_INVARIANT
{
}

ostream& operator <<( ostream& o, const MachPhysActorData& t )
{

    o << "Cost " << t.cost() << endl;
	o << "hwResearchCost " << t.hwResearchCost() << endl;

    return o;
}

/* End MaDATA.CPP **************************************************/
