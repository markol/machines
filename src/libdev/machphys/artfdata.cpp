/*
 * A R T F D A T A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/artfdata.hpp"

#ifndef _INLINE
    #include "machphys/artfdata.ipp"
#endif

MachPhysArtefactData::MachPhysArtefactData(MATHEX_SCALAR height,
										   MexConvexPolygon2d* pPolygon,
										   const MachPhys::ArmourUnits& armour,
										   const MachPhys::HitPointUnits& hitPoints,
										   const MachPhys::BuildingMaterialUnits& cost,
                				  		   int subType,
				                 		   uint resourceStringId )
:height_(height),
pPolygon_(pPolygon),
subType_( subType ),
resourceStringId_( resourceStringId )
{

	this->armour( armour );
	this->hitPoints( hitPoints );
	this->cost( cost );

    TEST_INVARIANT;
}

MachPhysArtefactData::~MachPhysArtefactData()
{
    TEST_INVARIANT;

}

void MachPhysArtefactData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysArtefactData& t )
{

    o << "MachPhysArtefactData " << (void*)&t << " start" << std::endl;
    o << "MachPhysArtefactData " << (void*)&t << " end" << std::endl;

    return o;
}

/* End ARTFDATA.CPP *************************************************/
