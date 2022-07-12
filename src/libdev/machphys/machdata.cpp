/*
 * M A C H D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/machdata.hpp"
#include "machphys/epps.hpp"

MachPhysMachineData::MachPhysMachineData()
:   locomotionType_( MachPhys::NO_LOCOMOTION ),
    locomotionParameter_( 1.0 ),
	minAccessSizeIndex_( 100 ),		// ensures machine cannot enter anything unless minAccessSizeIndex is actively set
	pEPP_( NULL ),				//!! Need to change this in HPP!!!!!
	hasNVG_( false ),
    highClearance_( 2.0 ),
    lowClearance_( 1.5 )
{

    TEST_INVARIANT;
}

MachPhysMachineData::~MachPhysMachineData()
{
    TEST_INVARIANT;

}

MachPhys::ResearchUnits MachPhysMachineData::hwResearchCost() const
{
	return hwResearchCost_;
}

void MachPhysMachineData::hwResearchCost( const MachPhys::ResearchUnits& newSwResearchCost )
{
	hwResearchCost_ = newSwResearchCost;
}

MachPhys::BuildingMaterialUnits MachPhysMachineData::swCost() const
{
	return swCost_;
}

MachPhys::BuildingMaterialUnits MachPhysMachineData::hwCost() const
{
	return hwCost_;
}

bool MachPhysMachineData::hasNVG() const
{
	return hasNVG_;
}

void MachPhysMachineData::hasNVG( bool status )
{
	hasNVG_ = status;
}

void MachPhysMachineData::swCost( const MachPhys::BuildingMaterialUnits& newSwCost )
{
	swCost_ = newSwCost;

    cost( swCost() + hwCost() );
}

void MachPhysMachineData::hwCost( const MachPhys::BuildingMaterialUnits& newHwCost )
{
    PRE( this != NULL );

	hwCost_ = newHwCost;

    cost( swCost() + hwCost() );
}

MachPhys::ResearchUnits MachPhysMachineData::swResearchCost() const
{
	return swResearchCost_;
}

void MachPhysMachineData::swResearchCost( const MachPhys::ResearchUnits& newSwResearchCost )
{
	swResearchCost_ = newSwResearchCost;
}

MachPhys::SizeUnits MachPhysMachineData::volume() const
{
	return volume_;
}

void MachPhysMachineData::volume( const MachPhys::SizeUnits& newVolume )
{
	volume_ = newVolume;
}

MATHEX_SCALAR MachPhysMachineData::speed() const
{
	return speed_;
}

void MachPhysMachineData::speed( const MATHEX_SCALAR& newSpeed )
{
	speed_ = newSpeed;
}

MATHEX_SCALAR MachPhysMachineData::taskSpeed() const
{
	return taskSpeed_;
}

void MachPhysMachineData::taskSpeed( const MATHEX_SCALAR& newSpeed )
{
	taskSpeed_ = newSpeed;
}

MATHEX_SCALAR MachPhysMachineData::acceleration() const
{
	return acceleration_;
}
void MachPhysMachineData::acceleration( const MATHEX_SCALAR& newAcceleration )
{
	acceleration_ = newAcceleration;
}

MexRadians MachPhysMachineData::rotationSpeed() const
{
	return rotationSpeed_;
}

void MachPhysMachineData::rotationSpeed( const MexRadians& newRotationSpeed )
{
	rotationSpeed_ = newRotationSpeed;
}

MexRadians MachPhysMachineData::rotationAcceleration() const
{
	return rotationAcceleration_;
}

void MachPhysMachineData::rotationAcceleration( const MexRadians& newRotationAcceleration )
{
	rotationAcceleration_ = newRotationAcceleration;
}

MachPhys::RepairRateUnits MachPhysMachineData::repairRate() const
{
	return repairRate_;
}
void MachPhysMachineData::repairRate( const MachPhys::RepairRateUnits& newRepairRate )
{
	repairRate_ = newRepairRate;
}

MATHEX_SCALAR MachPhysMachineData::scannerRange() const
{
	return scannerRange_;
}

void MachPhysMachineData::scannerRange( const MATHEX_SCALAR& newScannerRange )
{
	scannerRange_ = newScannerRange;
}

void MachPhysMachineData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysMachineData& t )
{

	o << "hwResearchCost " << t.hwResearchCost() << std::endl;
    o << "swCost " << t.swCost() << std::endl;
	o << "swResearchCost " << t.swResearchCost() << std::endl;
	o << "volume " << t.volume() << std::endl;
	o << "speed " << t.speed() << std::endl;
	o << "task speed " << t.taskSpeed() << std::endl;
	o << "Acceleration " << t.acceleration() << std::endl;
	o << "Repair Rate " << t.repairRate() << std::endl;
	o << "Scanner Range " << t.scannerRange() << std::endl;

    return o;
}
MachPhys::MachineType MachPhysMachineData::machineType() const
{
    return machineType_;
}

int MachPhysMachineData::subType() const
{
    return subType_;
}

uint MachPhysMachineData::hwLevel() const
{
    return hwLevel_;
}

uint MachPhysMachineData::swLevel() const
{
    return swLevel_;
}

int MachPhysMachineData::minAccessSizeIndex() const
{
    return minAccessSizeIndex_;
}

void MachPhysMachineData::minAccessSizeIndex( int size )
{
    minAccessSizeIndex_ = size;
}


void MachPhysMachineData::machineType( MachPhys::MachineType type )
{
    machineType_ = type;
}

void MachPhysMachineData::subType( int type )
{
    subType_ = type;
}

void MachPhysMachineData::hwLevel( uint level )
{
    hwLevel_ = level;
}

void MachPhysMachineData::swLevel( uint level )
{
    swLevel_ = level;
}

MachPhys::LocomotionType MachPhysMachineData::locomotionType() const
{
    return locomotionType_;
}

MATHEX_SCALAR MachPhysMachineData::locomotionParameter() const
{
    return locomotionParameter_;
}

void MachPhysMachineData::locomotionType( MachPhys::LocomotionType type )
{
    locomotionType_ = type;
}

void MachPhysMachineData::locomotionParameter( MATHEX_SCALAR v )
{
    locomotionParameter_ = v;
}

void MachPhysMachineData::setEPP( const string& EPPName )
{
	const MachPhysEvasionPriorityPlans& cEPPs = _CONST_CAST( const MachPhysEvasionPriorityPlans&, MachPhysEvasionPriorityPlans::instance() );

	const MachPhysEvasionPriorityPlan& EPP =  cEPPs.EPP( EPPName );
	pEPP_ = &( _CONST_CAST( MachPhysEvasionPriorityPlan&, EPP ) );
}

const MachPhysEvasionPriorityPlan& MachPhysMachineData::EPP() const
{
	PRE( pEPP_ != NULL );
	return *pEPP_;
}

const string& MachPhysMachineData::upperBodyTurnLinkName() const
{
    return upperBodyTurnLinkName_;
}

const MexRadians& MachPhysMachineData::upperBodyTurnRate() const
{
    return upperBodyTurnRate_;
}

void MachPhysMachineData::upperBodyTurnLinkName( const string& name )
{
    upperBodyTurnLinkName_ = name;
}

void MachPhysMachineData::upperBodyTurnRate(  const MexRadians& rate )
{
    upperBodyTurnRate_ = rate;
}

MATHEX_SCALAR MachPhysMachineData::highClearance() const
{
    return highClearance_;
}

MATHEX_SCALAR MachPhysMachineData::lowClearance() const
{
    return lowClearance_;
}

void MachPhysMachineData::highClearance( MATHEX_SCALAR clearance )
{
    highClearance_ = clearance;
}

void MachPhysMachineData::lowClearance( MATHEX_SCALAR clearance )
{
    lowClearance_ = clearance;
}

/* End MACHDATA.CPP *************************************************/
