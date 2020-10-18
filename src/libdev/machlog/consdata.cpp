/*
 * C O N S D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

//#include "machlog/consdata.hpp"

#ifndef _INLINE
    #include "machlog/consdata.ipp"
#endif

//////////////////////////////////////////////////////////////////////////////////////////

MachLogConstructionData::MachLogConstructionData
(
    const MexAlignedBox2d& localBoundary, const MachPhys::BuildingMaterialUnits& cost
)
:   localBoundary_( localBoundary ),
    cost_( cost )
{
    entrances_.reserve( 2 );
    pickUpPoints_.reserve( 2 );
    putDownPoints_.reserve( 2 );

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

MachLogConstructionData::~MachLogConstructionData()
{
    TEST_INVARIANT;

}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogConstructionData::CLASS_INVARIANT
{
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogConstructionData::addEntrance
(
    const MexPoint2d& externalPoint, const MexPoint2d& internalPoint
)
{
    EntranceData entrance;
    entrance.externalPoint = externalPoint;
    entrance.internalPoint = internalPoint;
    entrances_.push_back( entrance );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogConstructionData::addPickUpPoint( const MexPoint2d& point )
{
    pickUpPoints_.push_back( point );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogConstructionData::addPutDownPoint( const MexPoint2d& point )
{
    putDownPoints_.push_back( point );
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End CONSDATA.CPP *************************************************/
