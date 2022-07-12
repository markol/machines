/*
 * M I N E D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/minedata.hpp"

MachPhysMineData::MachPhysMineData()
{

    TEST_INVARIANT;
}

MachPhysMineData::MachPhysMineData( const MachPhysMineData& copyMe, const MexTransform3d& transform )
: MachPhysConstructionData( copyMe, transform ),
  capacity_( copyMe.capacity_ ),
  extractionRate_( copyMe.extractionRate_ ),
  mineralDistance_(copyMe.mineralDistance_)
{

}

MachPhysMineData::~MachPhysMineData()
{
    TEST_INVARIANT;

}

MachPhys::SizeUnits MachPhysMineData::capacity() const
{
	return capacity_;
}

void MachPhysMineData::capacity( const MachPhys::SizeUnits& newCapacity )
{
	capacity_ = newCapacity;
}

MachPhys::BuildingMaterialUnits MachPhysMineData::extractionRate() const
{
	return extractionRate_;
}

void MachPhysMineData::extractionRate( const MachPhys::BuildingMaterialUnits& newExtractionRate )
{
	extractionRate_ = newExtractionRate;
}

MATHEX_SCALAR MachPhysMineData::sqrMineralDistance() const
{
	return mineralDistance_;
}

void MachPhysMineData::sqrMineralDistance( const MATHEX_SCALAR& newMineralDistance)
{
	mineralDistance_ = newMineralDistance;
}

void MachPhysMineData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysMineData& t )
{

   // o << (MachPhysObjectData)t;
   // o << (MachPhysConstructionData)t;
	o << "Capacity " << t.capacity() << std::endl;
	o << "Extraction rate " << t.extractionRate() << std::endl;

    return o;
}

/* End MINEDATA.CPP *************************************************/
