/*
 * G E N E D A T A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/genedata.hpp"

#include "mathex/radians.hpp"

#include "machphys/machphys.hpp"

class MachPhysGeneralDataImpl
{
public:
    MexRadians firstPersonLookUpDownRate_;
    MexRadians firstPersonLookUpDownMinAngle_;
    MexRadians firstPersonLookUpDownMaxAngle_;
    MexRadians firstPersonMaxFastTurnRate_;
    MexRadians firstPersonMaxSlowTurnRate_;
    MexRadians maxMoveGradient_;
    MATHEX_SCALAR startRotationDistance_;
    MATHEX_SCALAR maxTerrainUnevenness_;
	PhysRelativeTime virtualDefConInterval_;
    PhysPathFindingPriority pcPathFindingPriority_;
	MachPhys::BuildingMaterialUnits	nukeLaunchCost_;
	size_t	maxUnitsPerRace_;
	size_t	campaignExtraUnitsPerRace_;
};

MachPhysGeneralData::MachPhysGeneralData()
:   pImpl_( NULL )
{
    pImpl_ = _NEW( MachPhysGeneralDataImpl );
    pImpl_->firstPersonLookUpDownRate_ = MexDegrees( 0.1 );
    pImpl_->firstPersonLookUpDownMinAngle_ = MexDegrees( -85.0 );
    pImpl_->firstPersonLookUpDownMaxAngle_ = MexDegrees( 85.0 );
    pImpl_->firstPersonMaxFastTurnRate_ = MexDegrees( 60.0 );
    pImpl_->firstPersonMaxSlowTurnRate_ = MexDegrees( 1.0 );
    pImpl_->startRotationDistance_ = 2.0;
    pImpl_->maxMoveGradient_ = 65.0;
    pImpl_->maxTerrainUnevenness_ = 0.1;
	pImpl_->virtualDefConInterval_ = 5.0;
	pImpl_->pcPathFindingPriority_ = Phys::defaultPathFindingPriority();
	pImpl_->nukeLaunchCost_ = 500;
	pImpl_->maxUnitsPerRace_ = 400;
	#ifndef PRODUCTION
	if( getenv("CB_NO_POPULATION_LIMIT") )
		pImpl_->maxUnitsPerRace_ = 650;
	#endif
	pImpl_->campaignExtraUnitsPerRace_ = 40;

    TEST_INVARIANT;
}

MachPhysGeneralData::~MachPhysGeneralData()
{
    TEST_INVARIANT;

    _DELETE( pImpl_ );
}

void MachPhysGeneralData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysGeneralData& t )
{

    o << "MachPhysGeneralData " << (void*)&t << " start" << std::endl;
    o << "MachPhysGeneralData " << (void*)&t << " end" << std::endl;

    return o;
}

MexRadians MachPhysGeneralData::firstPersonLookUpDownRate() const
{
    return pImpl_->firstPersonLookUpDownRate_;
}

void MachPhysGeneralData::firstPersonLookUpDownRate( MexRadians rate )
{
    pImpl_->firstPersonLookUpDownRate_ = rate;
}

MexRadians MachPhysGeneralData::firstPersonLookUpDownMinAngle() const
{
    return pImpl_->firstPersonLookUpDownMinAngle_;
}

MexRadians MachPhysGeneralData::firstPersonLookUpDownMaxAngle() const
{
    return pImpl_->firstPersonLookUpDownMaxAngle_;
}

void MachPhysGeneralData::firstPersonLookUpDownMinAngle( MexRadians angle )
{
    pImpl_->firstPersonLookUpDownMinAngle_ = angle;
}

void MachPhysGeneralData::firstPersonLookUpDownMaxAngle( MexRadians angle )
{
    pImpl_->firstPersonLookUpDownMaxAngle_ = angle;
}

MexRadians MachPhysGeneralData::firstPersonMaxFastTurnRate() const
{
    return pImpl_->firstPersonMaxFastTurnRate_;
}

MexRadians MachPhysGeneralData::firstPersonMaxSlowTurnRate() const
{
    return pImpl_->firstPersonMaxSlowTurnRate_;
}

void MachPhysGeneralData::firstPersonMaxFastTurnRate( MexRadians angle )
{
    pImpl_->firstPersonMaxFastTurnRate_ = angle;
}

void MachPhysGeneralData::firstPersonMaxSlowTurnRate( MexRadians angle )
{
    pImpl_->firstPersonMaxSlowTurnRate_ = angle;
}

MATHEX_SCALAR MachPhysGeneralData::startRotationDistance() const
{
    return pImpl_->startRotationDistance_;
}

void MachPhysGeneralData::startRotationDistance( MATHEX_SCALAR distance )
{
    pImpl_->startRotationDistance_ = distance;
}

MexRadians MachPhysGeneralData::maxMoveGradient() const
{
    return pImpl_->maxMoveGradient_;
}

void MachPhysGeneralData::maxMoveGradient( MexRadians gradient )
{
    pImpl_->maxMoveGradient_ = gradient;
}

MATHEX_SCALAR    MachPhysGeneralData::maxTerrainUnevenness() const
{
    return pImpl_->maxTerrainUnevenness_;
}

void MachPhysGeneralData::maxTerrainUnevenness( MATHEX_SCALAR unevenness )
{
    pImpl_->maxTerrainUnevenness_ = unevenness;
}

PhysRelativeTime MachPhysGeneralData::virtualDefConInterval() const
{
	return pImpl_->virtualDefConInterval_;
}

void MachPhysGeneralData::virtualDefConInterval( PhysRelativeTime interval )
{
	pImpl_->virtualDefConInterval_ = interval;
}

PhysPathFindingPriority MachPhysGeneralData::pcPathFindingPriority() const
{
	return pImpl_->pcPathFindingPriority_;
}

void MachPhysGeneralData::pcPathFindingPriority( PhysPathFindingPriority priority )
{
	pImpl_->pcPathFindingPriority_ = priority;
}

void MachPhysGeneralData::nukeLaunchCost( MachPhys::BuildingMaterialUnits cost )
{
	pImpl_->nukeLaunchCost_ = cost;
}

MachPhys::BuildingMaterialUnits MachPhysGeneralData::nukeLaunchCost() const
{
	return pImpl_->nukeLaunchCost_;
}

void MachPhysGeneralData::maxUnitsPerRace( size_t maxUnits )
{
	pImpl_->maxUnitsPerRace_ = maxUnits;
}

size_t MachPhysGeneralData::maxUnitsPerRace() const
{
	return pImpl_->maxUnitsPerRace_;
}

void MachPhysGeneralData::campaignExtraUnitsPerRace( size_t campaignExtraUnits )
{
	pImpl_->campaignExtraUnitsPerRace_ = campaignExtraUnits;
}

size_t MachPhysGeneralData::campaignExtraUnitsPerRace() const
{
	return pImpl_->campaignExtraUnitsPerRace_;
}



/* End GENEDATA.CPP *************************************************/
