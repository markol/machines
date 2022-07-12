/*
 * P O D D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/poddata.hpp"

MachPhysPodData::MachPhysPodData()
{

    TEST_INVARIANT;
}

MachPhysPodData::MachPhysPodData( const MachPhysPodData& copyMe, const MexTransform3d& transform )
: 	MachPhysConstructionData( copyMe, transform ),
	capacity_ (copyMe.capacity_),
	extractionRate_ (copyMe.extractionRate_)
{

}

MachPhysPodData::~MachPhysPodData()
{
    TEST_INVARIANT;

}


void MachPhysPodData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

MachPhys::SizeUnits MachPhysPodData::capacity() const
{
	return capacity_;
}

void MachPhysPodData::capacity( const MachPhys::SizeUnits& newCapacity )
{
	capacity_ = newCapacity;
}

MachPhys::BuildingMaterialUnits MachPhysPodData::extractionRate() const
{
	return extractionRate_;
}

void MachPhysPodData::extractionRate( const MachPhys::BuildingMaterialUnits& newExtractionRate )
{
	extractionRate_ = newExtractionRate;
}



ostream& operator <<( ostream& o, const MachPhysPodData& t )
{

   // o << (MachPhysObjectData)t;
   // o << (MachPhysConstructionData)t;

    return o;
}

/* End PODDATA.CPP *************************************************/
