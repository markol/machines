/*
 * F A C T D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/factdata.hpp"

MachPhysFactoryData::MachPhysFactoryData()
{

    TEST_INVARIANT;
}

MachPhysFactoryData::MachPhysFactoryData( const MachPhysFactoryData& copyMe, const MexTransform3d& transform )
: MachPhysConstructionData( copyMe, transform ),
  buildRate_( copyMe.buildRate_ )
{
}

MachPhysFactoryData::~MachPhysFactoryData()
{
    TEST_INVARIANT;

}

void MachPhysFactoryData::buildRate( const MachPhys::BuildingMaterialUnits& newBuildRate )
{
	buildRate_ = newBuildRate;
}

MachPhys::BuildingMaterialUnits MachPhysFactoryData::buildRate( void ) const
{
	return buildRate_;
}


void MachPhysFactoryData::CLASS_INVARIANT
{
    INVARIANT(this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysFactoryData& t )
{

    //o << (MachPhysObjectData)t;
    //o << (MachPhysConstructionData)t;
	o << "Build Rate " << t.buildRate() << std::endl;

    return o;
}

/* End FACTDATA.CPP *************************************************/
