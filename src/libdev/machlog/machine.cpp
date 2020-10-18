/*
 * M A C H I N E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "stdlib/string.hpp"

#include "ctl/algorith.hpp"
#include "ctl/list.hpp"

#include "mathex/circle2d.hpp"
#include "mathex/poly2d.hpp"

//#include "render/colour.hpp"            //for fading

//#include "world4d/fdedapln.hpp"         //for fading
//#include "world4d/entyiter.hpp"         //for fading
#include "world4d/envirnmt.hpp"
#include "world4d/domain.hpp"
#include "world4d/manager.hpp"
//#include "world4d/scenemgr.hpp"
#include "world4d/soundman.hpp"
#include "world4d/entyplan.hpp"

#include "sim/manager.hpp"

#include "envirnmt/planet.hpp"

#include "machphys/machine.hpp"
#include "machphys/administ.hpp"
#include "machphys/epp.hpp"
#include "machphys/epps.hpp"
#include "machphys/genedata.hpp"
#include "machphys/healaura.hpp"
#include "machphys/machdata.hpp"
#include "machphys/marker.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/station.hpp"
#include "machphys/stations.hpp"
#include "machphys/treachry.hpp"
#include "machphys/ionweap.hpp"
#include "machphys/weputil.hpp"
#include "machphys/plansurf.hpp"

#include "machlog/machine.hpp"
#include "machlog/internal/machinei.hpp"

#include "machlog/apc.hpp"
#include "machlog/attack.hpp"
#include "machlog/administ.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/dyingent.hpp"
#include "machlog/cntrl.hpp"
#include "machlog/cntrl_ai.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/constron.hpp"
#include "machlog/construc.hpp"
#include "machlog/entrance.hpp"
#include "machlog/network.hpp"
#include "machlog/machvman.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/missilem.hpp"
#include "machlog/move.hpp"
#include "machlog/opmeatta.hpp"
#include "machlog/p1mchndl.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/spacial.hpp"
#include "machlog/squad.hpp"
#include "machlog/strategy.hpp"
#include "machlog/vmman.hpp"


/* //////////////////////////////////////////////////////////////// */

const double FPS = 15.0;
const double FRAME_TIME = 1.0 / FPS;

PER_DEFINE_PERSISTENT_ABSTRACT( MachLogMachine );

/* //////////////////////////////////////////////////////////////// */

MATHEX_SCALAR MachLogMachine::lowClearence() const
{
	return physMachine().machineData().lowClearance();
}

MATHEX_SCALAR MachLogMachine::highClearence() const
{
	return physMachine().machineData().highClearance();
}

/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachLogMachine::MachLogMachine( MachPhys::MachineType t,
								Level hwLevel, Level swLevel,
                                MachLogRace * pRace, MachPhysMachine * pPhysMachine, MachLog::ObjectType ot )
: 	MachLogMobile( pRace,
					pPhysMachine,
					ot,
					pPhysMachine->machineData().highClearance(),
					pPhysMachine->machineData().lowClearance() ),
	pImpl_( _NEW( MachLogMachineImpl( t, hwLevel, swLevel, pRace->race() ) ) )
{

	machineCreated();

    TEST_INVARIANT;
}

/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachLogMachine::MachLogMachine( MachPhys::MachineType t,
								Level hwLevel, Level swLevel,
                                MachLogRace * pRace, MachPhysMachine * pPhysMachine, MachLog::ObjectType ot, UtlId withId )
: MachLogMobile( pRace,
					pPhysMachine,
					ot,
					withId,
					pPhysMachine->machineData().highClearance(),
					pPhysMachine->machineData().lowClearance() ),
	pImpl_( _NEW( MachLogMachineImpl( t, hwLevel, swLevel, pRace->race() ) ) )
{
	machineCreated();

    TEST_INVARIANT;
}

/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */

MachLogMachine::~MachLogMachine()
{
    TEST_INVARIANT;

	CB_DEPIMPL( MachPhysHealAura*, pHealAura_ );
	CB_DEPIMPL( MachPhysStation*, pLockedStation_ );
	CB_DEPIMPL( MachLogEntrance*, pLockedEntrance_ );
	CB_DEPIMPL( MachLogSquadron*, pSquadron_ );

	_DELETE( pHealAura_ );
	pHealAura_ = NULL;

	if( pLockedStation_ != NULL )
		pLockedStation_->lock( false );

	//need to check entrance is still locked. The entrances can come out in a different
	//order upon destruction.
	//HAL_STREAM("MLMachine::DTOR pLockedEntrance_ is valid\n" );
	if( pLockedEntrance_ != NULL )
		if( pLockedEntrance_->locked() )
		{
			//HAL_STREAM("...Entrance unlocked\n" );
			pLockedEntrance_->unlock( );
		}
    // This is bad, method cast this obj to asCanAttack which was already deleted before MachLogMachine
	// Call to squadron(NULL) was moved to CanAttack destructor to prevent this problem, pSquadron_ is set to NULL then
	//HAL_STREAM("(" << id() << ") MLMachine::DTOR checking against squadron\n" );
	if( pSquadron_ )
	{
		//HAL_STREAM(" removing from squadron\n" );
		pSquadron_->removeFromControl( this );
	}

	// to be tested========================

	// revoked 5/1/99 CPS. Hopefully the domain counted reference problems are now sorted, and we can
	// once again have machines blowing up inside buildings with impunity........
	//insideWhichBuilding( NULL );

	if( insideBuilding() )
	{
		insideWhichBuilding().machineExited( this );

		MachLogNetwork& network = MachLogNetwork::instance();

		if( network.isNetworkGame() and network.remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
		{
		 	network.messageBroker().sendEnteredBuildingMessage( id(), 0 );
		}
	}

	// to be tested========================


	//Don't do this. The entity will be deleted because it is a child
   	//_DELETE( pMarker_ );


	CB_DEPIMPL( MachLogMissileEmplacement*, pSafestMissileEmplacement_ );

	// Cancel observer relations
	if( pSafestMissileEmplacement_ )
		pSafestMissileEmplacement_->detach( this );

	CB_DEPIMPL( MachLogMachine*, pSafestMachine_ );
	if( pSafestMachine_ )
		pSafestMachine_->detach( this );

	CB_DEPIMPL( MachLogConstruction*, pInOrOnPadConstruction_ );
    if( pInOrOnPadConstruction_ )
        pInOrOnPadConstruction_->detach( this );

	//MachLogRaces::instance().nMachines( race() )--;

    _DELETE( pImpl_ );
}

//* ///////////////////////////////////////////////////////////////////////////////////////////////////////////// */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  TBD: Put accurate values in the clearance functions

// static
MATHEX_SCALAR MachLogMachine::minLowClearance()
{
    return 1.0;
}

// static
MATHEX_SCALAR MachLogMachine::maxLowClearance()
{
    return 9.0;
}

// static
MATHEX_SCALAR MachLogMachine::minHighClearance()
{
    return 1.5;
}

// static
MATHEX_SCALAR MachLogMachine::maxHighClearance()
{
    return 9.0;
}

// static
PhysRelativeTime MachLogMachine::virtualDefConInterval()
{
	static PhysRelativeTime virtualDefConInterval_ = MachPhysData::instance().generalData().virtualDefConInterval();
	return virtualDefConInterval_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::machineCreated()
{
	HAL_STREAM("(" << id() << ") MachLogMachineCreated type " << objectType() << std::endl );
	MachLogRaces& races = MachLogRaces::instance();

	races.addToMachinesCollection( this );

	// This is an old routine, new stuff controlled by squad code.
	//seeIfNeedRegister();

    //No motion sequencer indicates this instance is being created via load game persistence.
	if( not machineMotionSequencerDefined() )
	{
        //reallocate the id etc
    	actorCreated();

        //Give it a motion sequencer
		createNewMachineMotionSequencer( &physMachine() );

        //Hide if in an APC
    	if( insideAPC() )
    		hidePhysicalMachine();

        //If was in a building, switch it
        if( pImpl_->restoreInsideBuilding_ )
        {
            ASSERT( pImpl_->pInOrOnPadConstruction_, "Should be inside but no building" );
            insideWhichBuilding( pImpl_->pInOrOnPadConstruction_ );

			//Attach to the building's interior domain
            physMachine().attachTo( &pImpl_->pInOrOnPadConstruction_->interiorDomain() );
        }

        //Ignore polygons if required
        if( pImpl_->needToIgnoreEntrancePolygon_ )
        {
            ignoreEntrancePolygon( true );
            pImpl_->needToIgnoreEntrancePolygon_ = false;
        }

        if( pImpl_->needToIgnoreConstructionPolygon_ )
        {
            ignoreConstructionPolygon( true );
            pImpl_->needToIgnoreEntrancePolygon_ = false;
        }

		//If displayed as a different race, convert it
        if( displayMapAndIconRace() != race() )
            displayAsRace( displayMapAndIconRace() );
	}

	HAL_STREAM("(" << id() << ") MachLogMachineCreated done" << std::endl );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MachPhysMachine& MachLogMachine::physMachine()
{
	return _STATIC_CAST( MachPhysMachine&, physObject() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const MachPhysMachine& MachLogMachine::physMachine() const
{
	return _STATIC_CAST( const MachPhysMachine&, physObject() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::sendMessage( MachLogCommsId, const MachLogMessage& msg ) const
{
	CB_DEPIMPL( MachLogSquadron*, pSquadron_ );
	//If machine is in a squadron and the squadron has a commander (by def must be an administrator )
	//Then send the message there unless the commander is this machine
	if( pSquadron_ and pSquadron_->hasCommander() and pSquadron_->commander().id() != id() )
		pSquadron_->commander().receiveMessage( msg );
	else
		MachLogRaces::instance().controller( race() ).receiveMessage( msg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::receiveMessage( const MachLogMessage& )
{
	ASSERT( false, logic_error( "shouldn't be here!" ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime
MachLogMachine::update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR )
{
	// Note! This method has multiple exit points - will exit prematurely if machine is mid-teleport.

	CB_DEPIMPL( State, state_ );
	CB_DEPIMPL( bool, isTeleporting_ );
	CB_DEPIMPL( PhysAbsoluteTime, teleportFinishTime_ );
	PhysRelativeTime alteredMaxCPUTime = maxCPUTime;
	if( isDead() and not state_ == EXPLODING )
		return 1.0;

	// if we're a non-first person-controlled machine that's idle inside a building that has
	// no attack-capable enemy machines in, try to ensure we're clear of the entrance.
	// This is good for the game. Trust me.
	if( insideBuilding()
		and not motionSeq().is1stPersonControlled()
		and isIdle() )
	{
		MachLogConstruction& building = insideWhichBuilding();

		if( not building.isEnemyCanAttackInside( race() ) )
			checkAndDoClearEntrance( building );
	}


	if( isIdle() )
		sendMessage( NULL, MachLogMessage( this, MachLogMessage::IDLE ) );
	// classify();

//	if( motionSeq().hasDestination() )
//	{
//		PhysAbsoluteTime timeBefore = SimManager::instance().currentTime();
//		motionSeq().update( alteredMaxCPUTime - 0.005 );
//		alteredMaxCPUTime -= ( SimManager::instance().currentTime() - timeBefore );
//	}

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	// right, we may be involved in a teleport, in which case, the usual update stuff is bypassed.
	if( isTeleporting() )
	{
		if( timeNow > teleportFinishTime_ )
		{
			isTeleporting_ = false;
			if( state_ == TELEPORTING_OUT )
			{
				ASSERT( insideAPC(), "Should be inside an APC here!" );
				hidePhysicalMachine();
                notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::TELEPORTED_OUT_OF_WORLD );
			}
		}

		else
			// reasonably high priority for teleporting machines to get their teleport registered as having been
			// completed pretty swiftly
			return 0.5;
	}


	CB_DEPIMPL( PhysAbsoluteTime, lastUpdateTime_ );


	if( armour() < machineData().armour() )
	{
		const MachPhysMachineData& machData = machineData();
		MachPhys::ArmourUnits maxArmour = machData.armour();

		bool changed = false;

		PhysRelativeTime lastUpdateTimeGap = timeNow - lastUpdateTime_;

		MachPhys::RepairRateUnits repairRate = machData.repairRate();
		if( (int) ( lastUpdateTimeGap * repairRate ) > 0 )
		{
			armour( armour() + ( lastUpdateTimeGap * repairRate ) );

			lastUpdateTime_ = timeNow;
			changed = true;
		}
		if( armour() > maxArmour )
			armour( maxArmour );
		if( changed )
		{
			notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::HEALTH_STATUS_CHANGED );
			//there is no real need to send update for hp/armour more regularly than once every two hundred milliseconds
			if( MachLogNetwork::instance().isNetworkGame() and ( lastUpdateTimeGap > 0.2 or armour() == machData.armour() ) )
				MachLogNetwork::instance().messageBroker().sendAdjustHPAndArmourMessage( id(), hp(), armour() );
		}
	}
	else
	{
		// no need to boost armour, but must ensure we don't "save up" repair time by not updating the lastUpdateTime_
		lastUpdateTime_ = timeNow;
	}

	CB_DEPIMPL( MachLogSquadron*, pOriginalSquadron_ );

	// functionality to revert to the original race revoked 11/1/99
	/*
	if( changeRaceStartTime() > 0 )
	{
		HAL_STREAM("(" << id() << ") MLMachine::update changeRaceStartTime " << changeRaceStartTime() << std::endl );
		if( timeNow - changeRaceStartTime() > 60 )
		{
			HAL_STREAM(" its time to change the race back again...race " << race() << " original race " << logOriginalRace().race() << std::endl );
			if( race() != logOriginalRace().race() )
			{
				HAL_STREAM(" races are different - playing animation and calling into the base class version\n" );
				MachPhysTreacheryWeapon::traitorAnimation( &physMachine(), timeNow, race(), logOriginalRace().race() );
				MachActor::assignToDifferentRace( logOriginalRace() );
				physMachine().changeRace( logOriginalRace().race() );
				if( pOriginalSquadron_ )
				{
					squadron( pOriginalSquadron_ );
					pOriginalSquadron_ = NULL;
				}
			}
		}
	}
	*/

	//Update the motion sequencer (unless in 1st person or inside an APC) and the actor level.
	//Next update should occur at earliest of these times
	PhysRelativeTime intervalMotionSeq = ( ( isIn1stPersonView() or insideAPC() ) ? 1.0 : motionSeq().update( alteredMaxCPUTime ) );

	MATHEX_SCALAR junk = 0;
    PhysRelativeTime intervalActor = MachActor::update( alteredMaxCPUTime, junk );

	return std::min( intervalMotionSeq, intervalActor );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 void MachLogMachine::doOutputOperator( ostream& o ) const
 {
     o << "MachLogMachine* " << (void*)this << " HW " << hwLevel() << " SW " << swLevel() << " RR " << machineData().repairRate() << std::endl;
 	o << "Squadron ";
 	CB_DEPIMPL( MachLogSquadron*, pSquadron_ );
 	if( pSquadron_ )
 	{
 		o << "(" << pSquadron_->id() << ") " << *pSquadron_ << std::endl;/* ->squadronId() << " " << pSquadron_->strategy().currentOperationType() << std::endl;
 		if( pSquadron_->hasCommander() )
 			o << "administrated by " << pSquadron_->commander().id() << std::endl;
 			*/
 	}
 	else
 		o << "NULL " << std::endl;

 	if( insideBuilding() )
	{
 		o << "inside " << insideWhichBuilding().id() << std::endl;
		o << " interior config space is " << (void*)&insideWhichBuilding().interiorConfigSpace() << " motion seq c cspace " << (void*)&motionSeq().currentConfigSpace() << std::endl;
		o << " interior domain " << (void*)insideWhichBuilding().interiorDomain().getAsDomain() << std::endl;
	}
 	else
 		o << "not inside a building\n";

	o << " the parent W4dEntity of physObject is " << (void*)physObject().pParent() << std::endl;

 	if( insideAPC() )
 		o << "Inside APC\n";

 	o << "DefCon :";
 	switch( defCon() )
 	{
 		case MachLog::DEFCON_HIGH: o << "HIGH" ; break;
 		case MachLog::DEFCON_NORMAL: o << "NORMAL" ; break;
 		case MachLog::DEFCON_LOW: o << "LOW" ; break;
 		default:
 			o << "Unknown Defcon " << (int)defCon() << std::endl;
 	}
 	o << std::endl;

 	if( evading() )
 		o << "Evading" << std::endl;

 	if( doingTaskAttack() )
 		o << "Doing task attack" << std::endl;

	o << "Perceived darkness:" << perceivedDarkness();
	if( hasNVG() )
		o << " (With NVG capability)";

	o << ", scanner range = " << effectiveScannerRange() << std::endl;

 	MachActor::doOutputOperator( o );

 	o << motionSeq();
 }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::inScannerRange( const MachActor& target, MATHEX_SCALAR multiplier ) const
{
	MATHEX_SCALAR sqrScannerRange = effectiveScannerRange() * multiplier;
	sqrScannerRange *= sqrScannerRange;

	return position().sqrEuclidianDistance( target.position() ) <= sqrScannerRange;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::inScannerRange( const MexPoint3d& targetPosition, MATHEX_SCALAR multiplier ) const
{
	MATHEX_SCALAR sqrScannerRange = effectiveScannerRange() * multiplier;
	sqrScannerRange *= sqrScannerRange;

	return position().sqrEuclidianDistance( targetPosition ) <= sqrScannerRange;
}

MATHEX_SCALAR MachLogMachine::effectiveScannerRange() const
{
	return scannerRange() * lightLevelEfficiency();
}

MATHEX_SCALAR MachLogMachine::scannerRange() const
{
	return machineData().scannerRange();
}

MATHEX_SCALAR MachLogMachine::lightLevelEfficiency() const
{
	// midnight means scanner range is halved.
	return ( ( 2.0 - perceivedDarkness() ) / 2.0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< class CONTAINER, class T >
void
ctl_erase( CONTAINER *cPtr, const T& t )
{
	cPtr->erase( remove( cPtr->begin(), cPtr->end(), t ), cPtr->end() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachPhys::MachineType& t )
{
	string s;
	switch( t )
	{
		case MachPhys::ADMINISTRATOR	:
			s = "Administrator";
		break;
		case MachPhys::AGGRESSOR		:
			s = "Aggressor";
		break;
		case MachPhys::CONSTRUCTOR	:
			s = "Constructor";
		break;
		case MachPhys::GEO_LOCATOR		:
			s = "GeoLocator";
		break;
		case MachPhys::SPY_LOCATOR		:
			s = "SpyLocator";
		break;
		case MachPhys::RESOURCE_CARRIER	:
			s = "Resource Carrier";
		break;
		case MachPhys::TECHNICIAN		:
			s = "Technician";
		break;
		case MachPhys::APC:
			s = "APC";
		break;
		default								:
			ASSERT_BAD_CASE;
		break;
	}

	return o << s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachLogMachine::State& s )
{
	string state;
	switch( s )
	{
		case MachLogMachine::NOT_EXPLODING:
			state = "Not exploding";
		break;
		case MachLogMachine::EXPLODING:
			state = "Exploding";
		break;
		case MachLogMachine::TELEPORTING_IN:
			state = "Teleporting into world";
		break;
		case MachLogMachine::TELEPORTING_OUT:
			state = "Teleporting out of world";
		break;
		default:
			ASSERT_BAD_CASE;
		break;
	}

	return o << state;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::doStartExplodingAnimation()
{

	CB_DEPIMPL( State, state_ );
	if( MachLogRaces::instance().controller( race() ).type() == MachLogController::AI_CONTROLLER )
	{
		MachLogRaces::instance().AIController( race() ).machineDestroyed( *this );
	}
    state_ = EXPLODING;
}

void MachLogMachine::doEndExplodingAnimation()
{
	CB_DEPIMPL( State, state_ );
	state_ = NOT_EXPLODING;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
const MachPhysObjectData& MachLogMachine::objectData() const
{
	return machineData();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::isDying() const
{
	CB_DEPIMPL( State, state_ );
    return state_ == EXPLODING;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::isFreeToAttack() const
{
    return isIdle() or isStandingGround() or doingTaskAttack();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::isEntering( W4dDomain& )
{
	//HAL_STREAM("(" << id() << ") isEntering " << w4dd << std::endl );
	//HAL_STREAM(" intersects returns BEFORE " << physMachine().intersects( w4dd ) << std::endl );
	//HAL_STREAM(" domain.intersects BEFORE " << w4dd.intersects( physMachine() ) << std::endl );
//	physMachine().intersects( &w4dd, true );
	//HAL_STREAM(" intersects returns AFTER " << physMachine().intersects( w4dd ) << std::endl );
	//HAL_STREAM(" domain.intersects AFTER " << w4dd.intersects( physMachine() ) << std::endl );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::hasEntered( W4dDomain& w4dd )
{
	//HAL_STREAM("(" << id() << ") hasEntered " << w4dd << std::endl );
	//HAL_STREAM(" intersects returns BEFORE " << physMachine().intersects( w4dd ) << std::endl );
	//should this be perhaps w4dd.intersects( physMachine(),false) ??
	//HAL_STREAM(" domain.intersects BEFORE " << w4dd.intersects( physMachine() ) << std::endl );
//	physMachine().intersects( &w4dd, false );
	//HAL_STREAM(" intersects returns AFTER " << physMachine().intersects( w4dd ) << std::endl );
	physMachine().attachTo( &w4dd );
	//HAL_STREAM(" attach to the domain\n" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::entranceLocked( MachLogEntrance* pEntrance )
{
	//NULL is an allowable value. NULL indicates that the entrance is no longer locked.
	//HAL_STREAM("(" << id() << ") MLMachine::entranceLocked " << (void*)pEntrance << std::endl );
	CB_DEPIMPL( MachLogEntrance*, pLockedEntrance_ );
	pLockedEntrance_ = pEntrance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::stationLocked( MachPhysStation* pStation )
{
	//NULL is an allowable value. NULL indicates that the entrance is no longer locked.
	CB_DEPIMPL( MachPhysStation*, pLockedStation_ );
	pLockedStation_ = pStation;
}

MachPhysStation& MachLogMachine::stationLocked( void ) const
{
	CB_DEPIMPL( MachPhysStation*, pLockedStation_ );
	return *pLockedStation_;
}

bool MachLogMachine::hasStationLocked() const
{
	CB_DEPIMPL( MachPhysStation*, pLockedStation_ );
	return pLockedStation_ != NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//return machine level information
MachLogMachine::Level MachLogMachine::hwLevel() const
{
	CB_DEPIMPL( Level, hwLevel_ );
	return hwLevel_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MachLogMachine::Level MachLogMachine::swLevel() const
{
	CB_DEPIMPL( Level, swLevel_ );
	return swLevel_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::squadron( const MachLogSquadron* pNewSquadron )
{
	CB_DEPIMPL( MachLogSquadron*, pSquadron_ );
	if( pSquadron_ != NULL )
//		if( MachLogRaces::instance().objectExists( pAdministrator_ ) )
			pSquadron_->removeFromControl( this );

	if( pNewSquadron )
	{
		MachLogSquadron* pNonConst = _CONST_CAST( MachLogSquadron*, pNewSquadron );
		if( pNonConst->addToControl( this ) )
		{
			pSquadron_ = pNonConst;
			return true;
		}
	}
	else
	{
		pSquadron_ = NULL;
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const MachLogSquadron* MachLogMachine::squadron( void ) const
{
	CB_DEPIMPL( MachLogSquadron*, pSquadron_ );

	return pSquadron_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MachLogSquadron* MachLogMachine::squadron( void )
{
	CB_DEPIMPL( MachLogSquadron*, pSquadron_ );
	if( pSquadron_ == NULL )
		return NULL;

	return pSquadron_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::insideBuilding() const
{
    //Deemed to be inside a building if using its interior config space
    return pImpl_->pInOrOnPadConstruction_ and
           &motionSeq().currentConfigSpace() == &pImpl_->pInOrOnPadConstruction_->interiorConfigSpace();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MachLogConstruction& MachLogMachine::insideWhichBuilding()
{
	PRE( insideBuilding() );
	return *pImpl_->pInOrOnPadConstruction_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const MachLogConstruction& MachLogMachine::insideWhichBuilding() const
{
	PRE( insideBuilding() );
	return *pImpl_->pInOrOnPadConstruction_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::insideWhichBuilding( MachLogConstruction* pEnteredBuilding )
{
    PRE_INFO( (void*)pEnteredBuilding );
    PRE_INFO( (void*)pImpl_->pInOrOnPadConstruction_ );

    PRE( pEnteredBuilding == NULL  or  pImpl_->pInOrOnPadConstruction_ == NULL or
          pImpl_->pInOrOnPadConstruction_ == pEnteredBuilding);


     //Don't do anything if neither in nor entering
     if( pEnteredBuilding or pImpl_->pInOrOnPadConstruction_ )
     {
         //Get the config spaces currently in use and of the new construction
         PhysConfigSpace2d* pOldSpace = &motionSeq().currentConfigSpace();
         PhysConfigSpace2d* pNewSpace = &(pEnteredBuilding ? pEnteredBuilding->interiorConfigSpace() :
                                                            MachLogPlanet::instance().configSpace());

         if( pOldSpace != pNewSpace )
         {
             //Maintain the list of machines inside the buildings
             if( pEnteredBuilding )
             {
                 pEnteredBuilding->machineEntered( this );

                 //Ensure this machine observes the construction
                 inOrOnPadBuilding( pEnteredBuilding );
             }
             else
                 pImpl_->pInOrOnPadConstruction_->machineExited( this );

             //Switch the config spaces
			NETWORK_STREAM(" switchConfigSpaces\n" );
			MachLogNetwork& network = MachLogNetwork::instance();

            motionSeq().switchConfigSpace( pNewSpace );
			//this bit is for Enter/Leave building operations
			//So that things are logically attach correctly at the correct time.
			if( network.isNetworkGame() and network.remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
			{
				if( pEnteredBuilding )
					network.messageBroker().sendEnteredBuildingMessage( id(), pEnteredBuilding->id() );
				else
					network.messageBroker().sendEnteredBuildingMessage( id(), 0 );
			}
         }
     }
 }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::hidePhysicalMachine()
{
	while( motionSeq().hasDestination() )
	{
		motionSeq().stop();
		motionSeq().update( 0.1 );
	}
//	HAL_STREAM(" motionSeq.HasDestination " << motionSeq().hasDestination() << std::endl );
//	HAL_STREAM(" telling motionSeq() to switch config spaces to hidden config space etc\n" );
	PhysConfigSpace2d& cs = MachLogPlanet::instance().hiddenConfigSpace();
	bool foundSpace = false;
	MexPoint2d dest;
	MATHEX_SCALAR radius = 0;
	MexPoint2d target( position() );
	target.x( target.x() + 0.01 );
//	HAL_STREAM("doing a findspace loop\n" );
	while( not foundSpace )
	{
		if( not cs.findSpace( position(), target, highClearence() * 2, radius, obstacleFlags(), &dest ) )
			radius += 5;
		else
			foundSpace = true;
	}

	MexTransform3d localTransform;
	localTransform.position( dest );
	//move the machine to the relevant corrdinates before switching configspaces
	//this ensures that the chunks will get dropped at the correct place.
	physObject().globalTransform( localTransform );
    motionSeq().clearIntersectingDomains();
	motionSeq().switchConfigSpace( &cs );
	localTransform.position( MexPoint3d( 10000000, 1000000, 1000000 ) );
	physObject().attachTo( MachLogPlanet::instance().hiddenRoot().getAsDomain(), localTransform );
//	position( dest );
//    MexTransform3d localTransform;
//	physObject().attachTo( &MachLogPlanet::instance().hiddenRoot(), localTransform );
//	HAL_STREAM("exit.\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::snapTo( const MexPoint3d& target )
{
	HAL_STREAM("(" << id() << ") MLMachine::snapTo " << target << std::endl );

    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( target, 0, &localTransform );
	PhysConfigSpace2d& cs = MachLogPlanet::instance().configSpace();
	//Move the object to the correct coordinates first
	physObject().attachTo( pDomain, localTransform );

	//Ensure we aren't moving etc
	motionSeq().stop();

	//and switch configuration spaces
	motionSeq().switchConfigSpace( &cs );
	notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::ENTERED_WORLD );
	HAL_STREAM(" exiting\n" );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachLogMachine::doVisualiseSelectionState()
{
	CB_DEPIMPL( W4dEntity*, pMarker_ );
	//Delete any existing marker
	if( pMarker_ )
	{
		_DELETE( pMarker_ );
		pMarker_ = NULL;
	}
	//If highlighted or selected add a marker entity
	MachLog::SelectionState state = selectionState();
	if( state != MachLog::NOT_SELECTED )
	{
		MachPhysMachine& physObject = physMachine();
		bool permanent = state == MachLog::SELECTED;
		pMarker_ = _NEW( MachPhysMarker( &physObject, MexTransform3d(),
						physObject.compositeBoundingVolume(), permanent,( 100 * hp() ) / machineData().hitPoints() ) );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//get the machine's true defcon
MachLog::DefCon MachLogMachine::defCon() const
{
	CB_DEPIMPL( MachLog::DefCon, defCon_ );

	return defCon_;
}

//get the machine's defcon, but override to HIGH if direct order was issued in last X seconds and we're not yet idle
MachLog::DefCon MachLogMachine::virtualDefCon() const
{
	CB_DEPIMPL( MachLog::DefCon, defCon_ );
	CB_DEPIMPL( PhysAbsoluteTime, nextTrueDefConTime_ );

	if( nextTrueDefConTime_ > SimManager::instance().currentTime() and not isIdle() )
		return MachLog::DEFCON_HIGH;
	else
		return defCon_;
}

void MachLogMachine::manualCommandIssued()
{
	CB_DEPIMPL( PhysAbsoluteTime, nextTrueDefConTime_ );

	nextTrueDefConTime_ = SimManager::instance().currentTime() + virtualDefConInterval();
	if( objectType() == MachLog::AGGRESSOR
		and asAggressor().isEradicator() )
	{
		setBusyPeriod( 0.0 );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::defCon( MachLog::DefCon newDefCon )
{
        ASSERT( newDefCon == MachLog::DEFCON_HIGH or
                        newDefCon == MachLog::DEFCON_NORMAL or
                        newDefCon == MachLog::DEFCON_LOW,
                        "Unknown defcon in MLMAchine(defcon)" );
	CB_DEPIMPL( MachLog::DefCon, defCon_ );
	defCon_ = newDefCon;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::CLASS_INVARIANT
{
	CB_DEPIMPL( MachLogAPC*, pAPCImInside_ );
	CB_DEPIMPL( bool, insideAPC_ );

    INVARIANT( this != NULL );
	INVARIANT( insideAPC_ == ( pImpl_->pAPCImInside_ != NULL ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	virtual
void MachLogMachine::assignToDifferentRace( MachLogRace& newRace )
{
	//if we have come through here for a restore to original race (which can happen in network game)
	//then reset squadron as before.
	CB_DEPIMPL( MachLogSquadron*, pOriginalSquadron_ );
	CB_DEPIMPL( MachLogSquadron*, pSquadron_ );

	if( displayMapAndIconRace() != newRace.race() )
		MachLogMachineVoiceMailManager::instance().postNewMail( objectType(), subType(), MachLogMachineVoiceMailManager::MEV_CHANGED_RACE, id(),  newRace.race() );

	/*
	// decrement losing race's machine numbers
	MachLogRaces::instance().nMachines( race() )--;
	*/

	if( newRace.race() == logOriginalRace().race() )
	{
		if( pOriginalSquadron_ )
		{
			squadron( pOriginalSquadron_ );
			pOriginalSquadron_ = NULL;
		}
	}
	else
	{
		pOriginalSquadron_ = pSquadron_;
		squadron( NULL );
	}

	//base class version MUST be called
	MachActor::assignToDifferentRace( newRace );
	physMachine().changeRace( newRace.race() );

	/*
	// increment gaining race's machine numbers
	MachLogRaces::instance().nMachines( race() )++;
	*/

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachLogMachine::beHit( const int& damage, MachPhys::WeaponType byType,
						MachActor* pByActor, MexLine3d* pByDirection, MachActor::EchoBeHit echo )
{
	if( not isDead() )
	{
		if( byType != MachPhys::N_WEAPON_TYPES and pByDirection != NULL )
			MachPhysWeaponUtility::victimAnimation( byType, SimManager::instance().currentTime(), *pByDirection, &physMachine() );
		MachActor::beHit( damage, byType, pByActor, pByDirection, echo );
		sendMessage( NULL, MachLogMessage( this, MachLogMessage::ENEMY_SIGHTED ) );

		if( not isDead() )
		{
			notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::HEALTH_STATUS_CHANGED );

			if( pByActor and pByActor->race() != race() )
			{
				// give voicemail if appropriate
				checkAndDoHitVoiceMail();
			}

		}
		else
		{
			// I've just kharked it - give death voicemail
			MachLogVoiceMailManager::instance().postDeathMail( id(),  race() );

		}
	}

}

// virtual
void MachLogMachine::beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay, MachActor* pByActor, MachActor::EchoBeHit echo )
{
	if( not isDead() )
	{
		MachPhys::ArmourUnits armourLeft = armour();
		MachPhys::ArmourUnits absorb = 0;
		MachPhys::HitPointUnits hpLeft = hp();
		if( damage > armourLeft )
			absorb = armourLeft;
		else
			absorb = damage - 1;
		armour( armourLeft - absorb );
		hp( hpLeft - damage + absorb );
		if( armour() < 0 )
			armour( 0 );
		MachLogMessageBroker::ActorNowDead actorNowDead = MachLogMessageBroker::ACTOR_NOT_DEAD;
		if( hp() <= 0 )
			actorNowDead = MachLogMessageBroker::ACTOR_DEAD;
		if( echo == ECHO and MachLogNetwork::instance().isNetworkGame() )
			MachLogNetwork::instance().messageBroker().sendBeHitMessage( id(), damage, MachPhys::N_WEAPON_TYPES, pByActor, NULL, physicalTimeDelay, MachLogMessageBroker::NO_ANIMATION_HIT, actorNowDead );
		//Oops I'm dead!
		if( hp() <= 0 )
		{
			MATHEX_SCALAR junk = 0.0;
			PhysAbsoluteTime nowTime = SimManager::instance().currentTime();
			if( physicalTimeDelay > 0 )
			{
				MachLogMachine& mlm = asMachine();
                mlm.preservePhysicalModel( physicalTimeDelay );
                mlm.dropDebris( nowTime + physicalTimeDelay );
			}

			if( pByActor )
				MachLogRaces::instance().actorDestroyedScoreAdjust( race(), objectData().cost(), *this, pByActor->race() );
			else
				MachLogRaces::instance().actorDestroyedScoreAdjust( race(), objectData().cost(), *this );

			//We must stop its strategy now, because the strategy cannot be destroyed as part of the actor,
            //since some operations call eg motionsequencer which will already have disappeared in the destructors
            //of MachLogMobile.
			strategy().beInterrupted();

			isDead( true );

            //Ensure marker gets removed
            selectionState( MachLog::NOT_SELECTED );
		}
		if( not isDead() )
		{
			notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::HEALTH_STATUS_CHANGED );

			if( pByActor and pByActor->race() != race() )
			{
				// give voicemail if appropriate
				checkAndDoHitVoiceMail();
			}

			doVisualiseSelectionState();
		}
		else
		{
			// I've just kharked it - give death voicemail
			MachLogVoiceMailManager::instance().postDeathMail( id(),  race() );

		}

	}
	lastBeHitFrame( W4dManager::instance().frameNumber() );
}

bool MachLogMachine::sufficientTimePassedSinceLastHit()
{
	bool result = false;
	CB_DEPIMPL( PhysAbsoluteTime, lastHitTime_ );

	PhysRelativeTime acceptableInterval;
	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	// require less time if our health is now very low
	if( hpRatio() < 0.35 )
	{
		// much more likely to give a voicemail if you're badly-damaged
		acceptableInterval = 4.0;

		if( timeNow - lastHitTime_ > acceptableInterval )
		{
			result = true;
			lastHitTime_ = timeNow;
		}
	}
	else
	{
		// not badly-damaged.....much more stringent tests for these sorts of things.
		// first of all, have to do a check against the global hit voice mail cull.
		MexPoint2d myPosition = position();

		// allow this mail if actor is over 200m away from the last actor to report having been hit
		// or sufficient time has elapsed since last actor reported having been hit
		if( myPosition.sqrEuclidianDistance( lastGlobalHitVoiceMailPosition() ) > 40000.0
			or timeNow >= nextGlobalHitVoiceMailTime() )
		{
			// yes, we've passed the requirements for the global-level cull....now still have to satisfy
			// our own personal time period requirements.
			acceptableInterval = hitVoiceMailInterval();
			if( timeNow - lastHitTime_ > acceptableInterval )
			{
				result = true;
				lastHitTime_ = timeNow;
				lastGlobalHitVoiceMailPosition() = myPosition;
				resetNextGlobalHitVoiceMailTime();
			}
		}
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::addHealingAuraReference()
{
	CB_DEPIMPL( MachPhysHealAura*, pHealAura_ );
	CB_DEPIMPL( int, healAuraReferences_ );
	if( pHealAura_ == NULL )
		pHealAura_ = _NEW( MachPhysHealAura( &physMachine(), SimManager::instance().currentTime() ) );
	++healAuraReferences_;
	HAL_STREAM("(" << id() << ") MLMachine::addHealingAuraReference at exit " << healAuraReferences_ << "references\n" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::releaseHealingAuraReference()
{
	CB_DEPIMPL( MachPhysHealAura*, pHealAura_ );
	CB_DEPIMPL( int, healAuraReferences_ );
	HAL_STREAM("(" << id() << ") MLMachine::releaseHealingAuraReference at entry " << healAuraReferences_ << "references\n" );
	--healAuraReferences_;
	if( healAuraReferences_ <= 0 and pHealAura_ != NULL )
	{
		HAL_STREAM(" got no references left and heal aura is not null so delete\n" );
		_DELETE( pHealAura_ );
		pHealAura_ = NULL;
	}
	HAL_STREAM("(" << id() << ") MLMachine::releaseHealingAuraReference at exit " << healAuraReferences_ << "references\n" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void MachLogMachine::smeltMe()
{
	PhysRelativeTime result = 0.25 + MachPhysVortexWeapon::destroy( &physObject() ,
				SimManager::instance().currentTime() );

	SimManager::instance().add( _NEW( MachLogDyingEntityEvent( physObjectPtr(), NULL, result,
				MachLogDyingEntityEvent::NOT_INSIDE_BUILDING, NULL ) ) );

	isDead( true );
	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() and network.remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
	{
		network.messageBroker().sendSmeltMachineMessage( id() );
	}
}
*/
void MachLogMachine::smeltMe()
{
	PhysRelativeTime result = 0.25 + MachPhysIonWeapon::destroy( &physObject() ,
				SimManager::instance().currentTime() );

    W4dEntity& physObj = physObject();
    W4dSoundManager::instance().play(&physObj, SID_RECYCLE, PhysAbsoluteTime(0), 1);

	SimManager::instance().add( _NEW( MachLogDyingEntityEvent( physObjectPtr(), NULL, result,
				MachLogDyingEntityEvent::NOT_INSIDE_BUILDING, NULL ) ) );

	isDead( true );
	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() and network.remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
	{
		network.messageBroker().sendSmeltMachineMessage( id() );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::seeIfNeedRegister()
{
	if( MachLogRaces::instance().controller( race() ).type() == MachLogController::AI_CONTROLLER )
	{
		MachLogRaces::instance().AIController( race() ).machineCreated( *this );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MachPhys::BuildingMaterialUnits MachLogMachine::machineOriginalCost() const
{
	return objectData().cost();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::canEnterConstruction( const MachLogConstruction& construction ) const
{
 	return construction.constructionData().accessSizeIndex() >= machineData().minAccessSizeIndex();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::hasNVG() const
{
	return machineData().hasNVG();
}

bool MachLogMachine::usingNVG() const
{
	CB_DEPIMPL( bool, usingNVG_ );

	ASSERT( implies( usingNVG_, hasNVG() ), "Machine without NVG capability is using NVG!" );

	return usingNVG_;
}

void MachLogMachine::usingNVG( bool status )
{
	CB_DEPIMPL( bool, usingNVG_ );

	PRE( hasNVG() );

	usingNVG_ = status;
}



///////////////////////////////////

void MachLogMachine::insideAPC( bool newStatus )
{
	CB_DEPIMPL( bool, insideAPC_ );
	CB_DEPIMPL( MachLogAPC*, pAPCImInside_ );
	insideAPC_ = newStatus;

	if( insideAPC_ )
	{
		W4dEntity& physObj = physObject();
		W4dSoundManager::instance().play(&physObj, SID_ENTERED_APC, PhysAbsoluteTime(0), 1);
	}
	else
		pImpl_->pAPCImInside_ = NULL;
}

const MachLogAPC& MachLogMachine::APCImInside() const
{
	CB_DEPIMPL( MachLogAPC*, pAPCImInside_ );

	PRE( pAPCImInside_ );
	return *pAPCImInside_;
}

void MachLogMachine::APCImInside( MachLogAPC* pAPC )
{
	CB_DEPIMPL( MachLogAPC*, pAPCImInside_ );

	PRE( pAPCImInside_ == NULL );

	pAPCImInside_ = pAPC;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::insideAPC( void ) const
{
	CB_DEPIMPL( bool, insideAPC_ );
	return insideAPC_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::isStandingGround( bool newStandGroundStatus )
{
	CB_DEPIMPL( bool, standGround_ );
	standGround_ = newStandGroundStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::isStandingGround( void ) const
{
	CB_DEPIMPL( bool, standGround_ );
	return standGround_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::evading( bool status )
{
	CB_DEPIMPL( bool, evading_ );
	evading_ = status;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::evading() const
{
	CB_DEPIMPL( bool, evading_ );
	return evading_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachine::camouflaged( bool status )
{
	CB_DEPIMPL( bool, camouflaged_ );
	if( status == true and camouflaged_ == false )
	{
		MachLogNetwork& network = MachLogNetwork::instance();
		if( network.isNetworkGame() )
		{
			if( network.remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
			   network.messageBroker().sendCamouflageMachineMessage( id(), MachLogMessageBroker::BEGIN_CAMOUFLAGE );
		}

		if( race() != MachLogRaces::instance().pcController().race() )
			physMachine().changeColour(  MachLogRaces::instance().pcController().race() );
			//physMachine().convertMaterials( physMachine().machineData(), MachLogRaces::instance().pcController().race() );

		camouflaged_ = status;
		notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::CHANGED_CAMOUFLAGE_STATUS );
	}
	else if( status == false and camouflaged_ == true )
	{
		MachLogNetwork& network = MachLogNetwork::instance();
		if( network.isNetworkGame() )
		{
			if( network.remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
				network.messageBroker().sendCamouflageMachineMessage( id(), MachLogMessageBroker::STOP_CAMOUFLAGE );
		}

		if( race() != MachLogRaces::instance().pcController().race() )
			physMachine().changeColour(  MachLogRaces::instance().pcController().race() );
			//physMachine().convertMaterials( physMachine().machineData(), MachLogRaces::instance().pcController().race() );

		camouflaged_ = status;
		notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::CHANGED_CAMOUFLAGE_STATUS );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::camouflaged() const
{
	CB_DEPIMPL( bool, camouflaged_ );
	return camouflaged_;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::followingMachines( Machines* pMachines )
{
	return motionSeq().followingMachines( pMachines );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachLogMachine::checkAndDoOnDefCon( const Actors& strongThreats )
{
	// !! This method has multiple exit points.

	// if this machine is not capable of evading at all, no need to check anything else.
	if( machineData().EPP().garrisonPriority() == -2 )
		return;

	// if already evading, don't bother doing anything new
	if ( evading() )
		return;

	// if standing ground, do NOT evade.
	if ( isStandingGround() )
		return;

	// if part of a convoy, do not initiate evade behaviour.
	if( motionSeq().isFollowing() )
		return;

	// if defcon is HIGH, we are not allowed to evade at all.

	MachLog::DefCon defCon = virtualDefCon();

	if( defCon == MachLog::DEFCON_HIGH )
		return;

	// if we're inside a fristd::endly garrison and our hitpoints are low, it's probably best to stay here.
	if( insideBuilding()
		and insideWhichBuilding().objectType() == MachLog::GARRISON
		and insideWhichBuilding().race() == race()
		and hpRatio() < 0.2 )
		return;


	// if we're an attack-capable machine and one of the strong threats is our target, do not evade

	if( objectIsCanAttack() )
	{
		if( asCanAttack().hasCurrentTarget() and asCanAttack().currentTargetIsOneOfThese( strongThreats ) )
			return;
	}
	else
	{
		// specific "don't evade" clauses for certain non-attack-capable machines

		if( objectType() == MachLog::APC and asAPC().isDeploying() /*and asAPC().nearDropzone()*/ )
			return;

		// better to try and complete the missile emplacement than run
		if( objectType() == MachLog::CONSTRUCTOR and asConstructor().constructingMissileEmplacement() )
			return;
	}

	// APCs who are deploying will not initiate an evade


	// if defcon is normal, we may not need to do anything if our target destination is out of the range of
	//	the strong threat machines.

	if( defCon == MachLog::DEFCON_NORMAL )
	{
		if ( destinationTakesUsClearOfThreats( strongThreats ) )
			return;
	}

	if( notAfraidOfStrongThreats( strongThreats ) )
		return;

	// do NOT evade if we are already in the strongest perceived place on the map
	// in the case of finding fristd::endly aggressives and turrets.
	if( wontFindAnywhereSafer() )
		return;

	// okay, our hand is forced.....we must attempt an evasion. Initiate an evade operation, I say....

	if( not strategy().isUninterruptable() )
		strategy().changeToEvadeMode();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachLogMachine::populateStrongThreats( Actors* pActors )
{
	// builds up a vector of threats within this machine's scannerrange that also satisfy certain other
	// criteria depending on whether or not this machine is inside a building

	MATHEX_SCALAR scannerMultiplier = 1.0;
	if( evading() )
		scannerMultiplier = 1.7;

	PRE( pActors->size() == 0)

	Actors& strongThreats = *pActors;

	//Junk any new redundant threatening actors
	validateActorsThreateningMe();

	MachLogRaces& races = MachLogRaces::instance();

	if ( insideBuilding() )
	{
		for( Actors::iterator i = actorsThreateningMe().begin(); i != actorsThreateningMe().end(); ++i )
		{
			// we're inside - only count as strong threats those threats which are also inside this building.
			// note that we can get away with not checking scannerrange here.
			MachActor& threatActor = races.actor( *i );

			MachLogRaces::DispositionToRace disposition = races.dispositionToRace( race(), threatActor.race() );

			// only add this as a strong threat if we don't think it's a friend (same race or ally),
			if( disposition == MachLogRaces::NEUTRAL or disposition == MachLogRaces::ENEMY )
			{
				// can only check building location status of machines, not other actor types
				if ( threatActor.objectIsMachine() )
				{
					const MachLogMachine& threatMachine = threatActor.asMachine();

					if ( evading() )
					{
						// we're already inside a building and we're in the middle of an evade operation
						// this being the case, we DO want to include any and all machines in scanner range.
						// If we didn't, we'd blindly waltz back outside into their unwelcome arms.
						if ( inScannerRange( threatMachine, scannerMultiplier ) )
							strongThreats.push_back( *i );
					}
					else
					{
						// we're not yet evading, and don't wish to do so if we're inside and no baddies
						// are here with us, otherwise we might stupidly leave this place to get to a nearby
						// garrison or something, which'd be dumb. So only add hostiles inside with us.
						if ( threatMachine.insideBuilding() and threatMachine.insideWhichBuilding().id() == insideWhichBuilding().id() )
							strongThreats.push_back( *i );
					}
				}
			}
		}
	}
	else
	{
		// we're outside - only count as strong threats those threats which are not inside a building.
		for( Actors::iterator i = actorsThreateningMe().begin(); i != actorsThreateningMe().end(); ++i )
		{
			MachActor& threatActor = races.actor( *i );

			MachLogRaces::DispositionToRace disposition = races.dispositionToRace( race(), threatActor.race() );

			// only add this as a strong threat if we don't think it's a friend (same race or ally),
			if( disposition == MachLogRaces::NEUTRAL or disposition == MachLogRaces::ENEMY )
			{
				// can only check building location status of machines, not other actor types
				if ( threatActor.objectIsMachine() )
				{
					const MachLogMachine& threatMachine = threatActor.asMachine();

					if ( ( not threatMachine.insideBuilding() ) and inScannerRange( threatMachine, scannerMultiplier ) )
						strongThreats.push_back( *i );
				}
				// for the moment, machines will not treat missile emplacements as threats. We'll see how that behaves.
				/*
				else
				{
					// in the case of missile emplacements, only count them as a strong threat if they're capable
					// of firing at us right now

					ASSERT_INFO( (*i)->objectType() );
					ASSERT( (*i)->objectType() == MachLog::MISSILE_EMPLACEMENT, "Non-machine threat actor should only be a missile emplacement!" );

					const MachLogMissileEmplacement& missileEmp = (*i)->asMissileEmplacement();

					if ( missileEmp.canDetect( *this ) )
						strongThreats.push_back( *i );
				}
				*/
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::notAfraidOfStrongThreats( const Actors& strongThreats ) const
{
	bool notAfraid=true;
	MachLogRaces& races = MachLogRaces::instance();

	 for( Actors::const_iterator i = strongThreats.begin(); notAfraid and i != strongThreats.end(); ++i )
	{
		MachActor& threatActor = races.actor( *i );
		if( fearsThisActor( threatActor ) )
			notAfraid = false;
	}

	return notAfraid;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::destinationTakesUsClearOfThreats( const Actors& threats )
{
	if( not motionSeq().hasDestination() )
		return false;

	MachLogRaces& races = MachLogRaces::instance();

	bool safe = true;
	for( Actors::const_iterator i = threats.begin(); safe and i != threats.end(); ++i )
	{
		MachActor& threatActor = races.actor( *i );

		if( threatActor.objectIsMachine() )
		{
			const MachLogMachine& threatMachine = threatActor.asMachine();
			if( threatMachine.inScannerRange( motionSeq().destination() ) )
				safe = false;
		}
		else if( threatActor.objectType() == MachLog::MISSILE_EMPLACEMENT and threatActor.subType() != MachPhys::ICBM )
		{
			const MachLogMissileEmplacement& threatME = threatActor.asMissileEmplacement();
			if( threatME.inWeaponRange( motionSeq().destination() ) )
				safe = false;
		}

	}

	return safe;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
bool MachLogMachine::fearsThisActor( const MachActor& otherActor ) const
{
	ASSERT( otherActor.objectIsCanAttack(), "Shouldn't be testing for fear on an actor that isn't attack-capable." );

	// default is to papper myself if the other actor is over 70% as tough as all me and all my local
	// military buddies put together
	// and/or
	// the other actor is not yet outgunned

	return otherActor.militaryStrength() > ( localStrength() * 0.7)
			or
		   ( not otherActor.isOutGunned() );

}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
int MachLogMachine::localStrength() const
{
	CB_DEPIMPL( PhysAbsoluteTime, lastStrengthEstimateTime_ );
	if( SimManager::instance().currentTime() - lastStrengthEstimateTime_ > 6.0 )
	{
		// const-cast naughtiness, but this is okay as it doesn't violate the spirit of const
		// from the client's point of view - this is for cached lazy evaluation.
		MachLogMachine* pThis = _CONST_CAST( MachLogMachine*, this ) ;
		pThis->updateLocalStrengthEstimate();
		lastStrengthEstimateTime_ = SimManager::instance().currentTime();
		lastStrengthEstimateTime_ = SimManager::instance().currentTime();
	}

	return localStrengthValue();

}



/////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::wontFindAnywhereSafer() const
{
	bool inSafestPosition = false;

	MachActor* pSafestActor = NULL;
	bool firstEPPencountered = false;
	const MachPhysEvasionPriorityPlan& EPP = machineData().EPP();
	for( int priority = 0; firstEPPencountered == false and priority < MachPhysEvasionPriorityPlans::N_PRIORITY_LEVELS; ++priority )
	{
		if( EPP.aggressivesPriority() == priority and EPP.turretsPriority() == priority )
		{
			// special case if aggressive machines and missile emplacements	share the same EPP priority level.
			// we must check both, and apply the "nowhere safer" check on the safest of the two candidates returned
			firstEPPencountered = true;
			MachLogMachine* pMachine=NULL;
			MachLogMissileEmplacement* pMissileEmplacement=NULL;
			int bestMilitaryStrength = 0;
			if( findSafestNearbyAggressiveMachine( &pMachine ) )
			{
				pSafestActor = pMachine;
				bestMilitaryStrength = pMachine->localStrengthFromPosition( position() );
				if( findSafestNearbyMissileEmplacement( &pMissileEmplacement ) )
				{
					int rivalMilitaryStrength = pMissileEmplacement->localStrengthFromPosition( position() );
					if( rivalMilitaryStrength > bestMilitaryStrength )
						pSafestActor = pMissileEmplacement;
				}
			}
			else if( findSafestNearbyMissileEmplacement( &pMissileEmplacement ) )
				pSafestActor = pMissileEmplacement;
		}
		else if( EPP.aggressivesPriority() == priority  )
		{
			MachLogMachine* pMachine=NULL;
			// note that if no machines are found, an attempt is still allowed to check for missile emplacements as
			// these may become the preferred destination actors
			if( findSafestNearbyAggressiveMachine( &pMachine ) )
			{
				pSafestActor = pMachine;
				firstEPPencountered = true;
			}

		}
		else if( EPP.turretsPriority() == priority  )
		{
			MachLogMissileEmplacement* pMissileEmplacement=NULL;
			// note that if no missile emplacements are found, an attempt is still allowed to check for machines as
			// these may become the preferred destination actors
			if( findSafestNearbyMissileEmplacement( &pMissileEmplacement ) )
			{
				pSafestActor = pMissileEmplacement;
				firstEPPencountered = true;
			}
		}
		else
		{
			// priority scheme will not be concerned with aggressive machines or missile emplacements.
			// stop the search.
			firstEPPencountered = true;
		}

	}

	if( pSafestActor
		and position().sqrEuclidianDistance( pSafestActor->position() ) <= localStrengthMaximumRadius()/2 )
	{
		inSafestPosition = true;
	}
	else if( not pSafestActor and objectIsCanAttack() )
	{
		// If there are no safe actors, and we're attack-capable, it's time to fight.
		// Hey, ya can't run forever....
		inSafestPosition = true;
	}


	return inSafestPosition;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachLogMachine::dispatchSOS( Actors& strongThreats )
{
	// Attempt to recruit fristd::endly thugs to help me tonk the rascals threatening me.
	// Said thugs must be idle and have a non-HIGH DefCon.

	Actors::iterator iThreats = strongThreats.begin();

	// first try and recruit following machines to the cause;

	Machines followers;
	followingMachines( &followers );
	Machines::iterator iFollowers = followers.begin();

	MachLogRaces& races = MachLogRaces::instance();

	while( iThreats != strongThreats.end() and iFollowers != followers.end() )
	{
		MachActor& threatActor = races.actor( *iThreats );
		if( threatActor.objectIsMissileEmplacement() )
			// don't bother dispatching SOSs for turret attacks.
			++iThreats;
		else if( threatActor.isOutGunned()
				 or not MachLogCanAttack::IHaveClearWeaponPathToHim( this, &threatActor ) )
		{
			++iThreats;
		}
		else
		{
			MachLogMachine* pFollowingMachine = (*iFollowers);

			// don't need a DefCon check for following aggressives - attacking your enemies is their JOB.
			// We don't want to interrupt them if they're evading, though.
		   	if( 	pFollowingMachine->objectIsCanAttack()
		   		and not pFollowingMachine->evading()
		   		and ( pFollowingMachine->asCanAttack().canFireAt( threatActor ) ) )
		   	{
				// even if this actor doesn't respond to the SOS call, bump its alertness up to full
				pFollowingMachine->asCanAttack().setMinimumAlertness( 125 );

				MachLogOperation *pOp = _NEW( MachLogAttackOperation( pFollowingMachine, &threatActor, MachLogAttackOperation::TERMINATE_ON_CHANGE ) );
				if( not( pFollowingMachine->strategy().addOperationAsSubOperationToFollowOperation( pOp ) ) )
					_DELETE( pOp );
			}
			++iFollowers;
		}
	}

	// then try and recruit nearby idle aggressives to the cause;
	MachActor::dispatchSOS( strongThreats );
}



/////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::findSafestNearbyAggressiveMachine( MachLogMachine** ppSafestMachine ) const
{
	CB_DEPIMPL( MachLogMachine*, pSafestMachine_ );
	CB_DEPIMPL( PhysAbsoluteTime, lastSafestMachineEstimateTime_ );
	bool found=false;

	if( ( not pSafestMachine_ )
	    or SimManager::instance().currentTime() - lastSafestMachineEstimateTime_ > 6.0 )
	{
		MachActor* pNewSafestMachine = NULL;

		MachLogRaces::instance().findSafestFriendly( *this, &pNewSafestMachine, MachLogRaces::AGGRESSIVE_MACHINES );


		// const-cast naughtiness, but this is okay as it doesn't violate the spirit of const
		// from the client's point of view - this is for cached lazy evaluation.
		MachLogMachine* pThis = _CONST_CAST( MachLogMachine*, this ) ;

		if( pSafestMachine_ )
		{
			pSafestMachine_->detach( pThis );
			pSafestMachine_ =  NULL;
		}

		if( pNewSafestMachine )
		{

			ASSERT( pNewSafestMachine->objectIsMachine(), "Pointer returned is not pointer to machine." );
			ASSERT( pNewSafestMachine->objectIsCanAttack(), "Pointer returned is not pointer to canattack." );

			pSafestMachine_ = &( pNewSafestMachine->asMachine() );
			pSafestMachine_->attach( pThis );
			lastSafestMachineEstimateTime_ = SimManager::instance().currentTime();
			found = true;
		}
	}
	else
		found = true;

	*ppSafestMachine=pSafestMachine_;

	POST(  found == ( *ppSafestMachine != NULL ) );

	return found;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::findSafestNearbyMissileEmplacement( MachLogMissileEmplacement** ppSafestMissileEmplacement) const
{
	CB_DEPIMPL( MachLogMissileEmplacement*, pSafestMissileEmplacement_ );
	CB_DEPIMPL( PhysAbsoluteTime, lastSafestMissileEmplacementEstimateTime_ );
	bool found=false;

	if( ( not pSafestMissileEmplacement_ )
	    or SimManager::instance().currentTime() - lastSafestMissileEmplacementEstimateTime_ > 6.0 )
	{
		MachActor* pNewSafestMissileEmplacement = NULL;

		MachLogRaces::instance().findSafestFriendly( *this, &pNewSafestMissileEmplacement, MachLogRaces::MISSILE_EMPLACEMENTS );

		// const-cast naughtiness, but this is okay as it doesn't violate the spirit of const
		// from the client's point of view - this is for cached lazy evaluation.
		MachLogMachine* pThis = _CONST_CAST( MachLogMachine*, this ) ;

		if( pSafestMissileEmplacement_ )
		{
			pSafestMissileEmplacement_->detach( pThis );
			pSafestMissileEmplacement_ = NULL;
		}

		if( pNewSafestMissileEmplacement )
		{
			ASSERT( pNewSafestMissileEmplacement->objectType() == MachLog::MISSILE_EMPLACEMENT, "Pointer returned is not pointer to missile emplacement." );

			pSafestMissileEmplacement_	= &( pNewSafestMissileEmplacement->asMissileEmplacement() );
			pSafestMissileEmplacement_->attach( pThis );
			lastSafestMissileEmplacementEstimateTime_ = SimManager::instance().currentTime();
			found = true;
		}
	}
	else
	{
		found=true;
	}

	*ppSafestMissileEmplacement=pSafestMissileEmplacement_;

	POST( found == ( *ppSafestMissileEmplacement != NULL ) );

	return found;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
bool MachLogMachine::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData )
{
	CB_DEPIMPL( MachLogConstruction*, pInOrOnPadConstruction_ );
	CB_DEPIMPL( MachLogMissileEmplacement*, pSafestMissileEmplacement_ );
	CB_DEPIMPL( MachLogMachine*, pSafestMachine_ );

	bool remainAttached = true;

	switch( event )
	{
	case W4dSubject::DELETED:
		if( pSubject == (W4dSubject*)pSafestMachine_ )
		{
			pSafestMachine_ = NULL;
			remainAttached = false;
		}
		else if( pSubject == (W4dSubject*)pSafestMissileEmplacement_ )
		{
			pSafestMissileEmplacement_ = NULL;
			remainAttached = false;
		}
		else if( pSubject == (W4dSubject*)pInOrOnPadConstruction_ )
		{
            //Stop moving
            motionSeq().stop();

            //Remove any locks
			CB_DEPIMPL( MachLogEntrance*, pLockedEntrance_ );
			CB_DEPIMPL( MachPhysStation*, pLockedStation_ );
            pLockedEntrance_ = NULL;
            pLockedStation_ = NULL;

            //Attach to the planet exterior domain at current position
			snapTo( position() );
			pImpl_->pInOrOnPadConstruction_ = NULL;
            physMachine().notInOrOnPadConstruction();

            //And blow up, unless shutting game down
            if( not MachLogRaces::instance().isUnloadingGame() )
			    beHit( 1000000 );

			remainAttached = false;
		}
        else if( motionSeq().isUsingSpaceDomain() )
        {
            //The subject might be a construction we have observed by entering as a remote machine.
            //See if it is a construction first.
            MachLogRaces& races = MachLogRaces::instance();
            UtlId subjectId = pSubject->id();
            if( races.actorExists( subjectId ) )
            {
                MachActor& subjectActor = races.actor( subjectId );
                if( subjectActor.objectIsConstruction() )
                {
                    //Get the interior domain
                    W4dDomain* pInteriorDomain = &subjectActor.asConstruction().interiorDomain();
                    if( pInteriorDomain == &motionSeq().spaceDomain() )
                    {
                        //Clear any intersect relation with the domain
                        if( physMachine().intersects( *pInteriorDomain ) )
                            physMachine().intersects( pInteriorDomain, false );

                        motionSeq().useSpaceDomain( NULL );
                    }
                }
            }
			remainAttached = false;
        }
		break;
	case W4dSubject::CLIENT_SPECIFIC:
	{
		switch( clientData )
		{
			case MachLog::RACE_CHANGED:
				if( pSubject == (W4dSubject*)pSafestMachine_ )
				{
					pSafestMachine_ = NULL;
					remainAttached = false;
				}
				if( pSubject == (W4dSubject*)pSafestMissileEmplacement_ )
				{
					pSafestMissileEmplacement_ = NULL;
					remainAttached = false;
				}
				break;
			default:
				;
		}
	}

	default:
		;
	}
	return remainAttached;
}

void MachLogMachine::teleportIntoWorld()
{
	CB_DEPIMPL( bool, isTeleporting_ );
	CB_DEPIMPL( PhysAbsoluteTime, teleportFinishTime_ );
	CB_DEPIMPL( MachLogMachine::State, state_ );

	PRE( not isTeleporting() );
	doFadeInAnimation();
	isTeleporting_ = true;
	teleportFinishTime_ = SimManager::instance().currentTime() + 1.5;
	state_ = TELEPORTING_IN;

	if( objectIsCanAttack() )
	{
		// Come into the world pumped and alert
		asCanAttack().setMinimumAlertness( 125 );
	}
}

void MachLogMachine::teleportOutOfWorld()
{
	CB_DEPIMPL( bool, isTeleporting_ );
	CB_DEPIMPL( PhysAbsoluteTime, teleportFinishTime_ );
	CB_DEPIMPL( MachLogMachine::State, state_ );

	PRE( not isTeleporting() );
	doFadeOutAnimation();
	isTeleporting_ = true;
	teleportFinishTime_ = SimManager::instance().currentTime() + 1.5;
	state_ = TELEPORTING_OUT;
	motionSeq().stop();
}

bool MachLogMachine::isTeleporting() const
{
	CB_DEPIMPL( bool, isTeleporting_ );

	return isTeleporting_;
}

void MachLogMachine::doFadeInAnimation()
{
	physMachine().doFadeInAnimation();
/*
	const PhysAbsoluteTime& now = SimManager::instance().currentTime();
	RenMaterial solidGreen(RenColour::green());
	solidGreen.diffuse(RenColour::black());
	solidGreen.emissive(RenColour::green());
	uint nMaterialsInVector = 100; // a reasonablely large number. Why does this matter? not sure.
	uint nSolidFrames=0, nFadedFrames=15;
	MATHEX_SCALAR minAlpha=0.0, maxAlpha=1.0;
	W4dSolidFadedAlphaPlan* pPlan = _NEW( W4dSolidFadedAlphaPlan(nFadedFrames, nSolidFrames,
													nMaterialsInVector,
													solidGreen,
													10,
													2,
													minAlpha,
													maxAlpha) );


    W4dMaterialPlanPtr materialPlanPtr( pPlan );

    for( W4dEntityIter it( &physObject() ); not it.isFinished(); ++it )
	{
	    //(*it).entityPlanForEdit().clearAnimation(MachPhys::CONSTRUCTION_CONSTRUCTING);
        (*it).entityPlanForEdit().materialPlan( materialPlanPtr, now, 0, MachPhys::CONSTRUCTION_CONSTRUCTING );
	}
*/
}



void MachLogMachine::doFadeOutAnimation()
{
	physMachine().doFadeOutAnimation();
/*
	const PhysAbsoluteTime& now = SimManager::instance().currentTime();
	RenMaterial solidGreen(RenColour::green());
	solidGreen.diffuse(RenColour::black());
	solidGreen.emissive(RenColour::green());
	uint nMaterialsInVector = 100; // a reasonablely large number. Why does this matter? not sure.
	uint nSolidFrames=0, nFadedFrames=35;
	MATHEX_SCALAR minAlpha=0.0, maxAlpha=1.0;
	W4dSolidFadedAlphaPlan* pPlan = _NEW( W4dSolidFadedAlphaPlan(nFadedFrames, nSolidFrames,
													nMaterialsInVector,
													solidGreen,
													10,
													2,
													minAlpha,
													maxAlpha) );


    W4dMaterialPlanPtr materialPlanPtr( pPlan );

    for( W4dEntityIter it( &physObject() ); not it.isFinished(); ++it )
	{
	    //(*it).entityPlanForEdit().clearAnimation(MachPhys::CONSTRUCTION_CONSTRUCTING);
        (*it).entityPlanForEdit().materialPlan( materialPlanPtr, now, 0, MachPhys::CONSTRUCTION_CONSTRUCTING );
	}
*/

	/*
	 W4dSimpleAlphaPlan( const RenMaterial& mat, uint nMaterialsInVector,
                        const PhysScalarPlanPtr& alphaPlanPtr, W4dLOD maxLOD );

	W4dSolidFadedAlphaPlan( uint nFadedFrames,
                            uint nSolidFrames,
                            uint nMaterialsInVector,
							const RenMaterial& mat,
                            W4dLOD maxLOD,
							PhysRelativeTime duration,
                            MATHEX_SCALAR minAlpha,
                            MATHEX_SCALAR maxAlpha );

	*/

}





/////////////////////////////////////////////////// persistence /////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogMachine& actor )
{
	const MachLogMobile& base1 = actor;
	const MachLogCanCommunicate& base2 = actor;

    //This is needed to enable us to persist locked entrances and stations.
    actor.pImpl_->pTempConstruction( actor.pImpl_->pInOrOnPadConstruction() );
    if( actor.pImpl_->pTempConstruction() == NULL )
    {
        if( actor.pImpl_->pLockedEntrance() != NULL )
            actor.pImpl_->pTempConstruction( &actor.pImpl_->pLockedEntrance()->construction() );
    }

    //Determine whether we are ignoring entrance and construction obstacle polygons and set the flags
    actor.pImpl_->needToIgnoreEntrancePolygon( actor.ignoringEntrancePolygon() );
    actor.pImpl_->needToIgnoreConstructionPolygon( actor.ignoringConstructionPolygon() );
    actor.pImpl_->restoreInsideBuilding( actor.insideBuilding() );

	ostr << base1;
	ostr << base2;
	ostr << actor.pImpl_;

}

void perRead( PerIstream& istr, MachLogMachine& actor )
{
	MachLogMobile& base1 = actor;
	MachLogCanCommunicate& base2 = actor;

	istr >> base1;
	istr >> base2;
	istr >> actor.pImpl_;
    //Re-establish observer relations
	if( actor.pImpl_->pSafestMissileEmplacement() )
		actor.pImpl_->pSafestMissileEmplacement()->attach( &actor );

	if( actor.pImpl_->pSafestMachine() )
		actor.pImpl_->pSafestMachine()->attach( &actor );

    if( actor.pImpl_->pInOrOnPadConstruction() )
        actor.pImpl_->pInOrOnPadConstruction()->attach( &actor );
}

MachLogMachine::MachLogMachine( PerConstructor con )
:	MachLogMobile( con )
{
}

bool MachLogMachine::inBuildingOrOnPad() const
{
    return pImpl_->pInOrOnPadConstruction_ != NULL;
}

MachLogConstruction& MachLogMachine::inOrOnPadWhichBuilding()
{
    PRE( pImpl_->pInOrOnPadConstruction_ != NULL );
    return *pImpl_->pInOrOnPadConstruction_;
}

const MachLogConstruction& MachLogMachine::inOrOnPadWhichBuilding() const
{
    PRE( pImpl_->pInOrOnPadConstruction_ != NULL );
    return *pImpl_->pInOrOnPadConstruction_;
}

void MachLogMachine::inOrOnPadBuilding( MachLogConstruction* pConstruction )
{
    //Check for a change
    if( pConstruction != pImpl_->pInOrOnPadConstruction_ )
    {
        //cancel any old observer relation
        if( pImpl_->pInOrOnPadConstruction_ )
            pImpl_->pInOrOnPadConstruction_->detach( this );

        //Store the pointer
        pImpl_->pInOrOnPadConstruction_ = pConstruction;

        //Set up any new observer relation
        if( pConstruction )
        {
            pConstruction->attach( this );
            physMachine().inOrOnPadConstruction( &pConstruction->physConstruction() );
        }
        else
        {
            physMachine().notInOrOnPadConstruction();
        }
    }
}

bool MachLogMachine::hasLockedEntrance() const
{
	CB_DEPIMPL( MachLogEntrance*, pLockedEntrance_ );
    return pLockedEntrance_ != NULL;
}

void MachLogMachine::ignoreEntrancePolygon( bool setIgnore )
{
    PRE( hasLockedEntrance() );
	CB_DEPIMPL( MachLogEntrance*, pLockedEntrance_ );

    //Lock the interior or exterior polygon depending on which config space in use
    MachLogMachineMotionSequencer& mSeq = motionSeq();
    PhysConfigSpace2d::PolygonId id =
        ( &mSeq.currentConfigSpace() == &MachLogPlanet::instance().configSpace() ?
            pLockedEntrance_->exteriorPolygonId() : pLockedEntrance_->interiorPolygonId() );

    //See if already ignored
    bool alreadyIgnored = mSeq.isPolygonIgnored( id );

    //Make change if required
    if( (setIgnore and not alreadyIgnored) or (not setIgnore and alreadyIgnored) )
        mSeq.isPolygonIgnored( id, setIgnore );
}

void MachLogMachine::ignoreConstructionPolygon( bool setIgnore )
{
    PRE( inBuildingOrOnPad() );
    PRE( not insideBuilding() );

    //Get the polygon id for the construction
    PhysConfigSpace2d::PolygonId id =  pImpl_->pInOrOnPadConstruction_->obstaclePolygonId();

    //See if already ignored
    MachLogMachineMotionSequencer& mSeq = motionSeq();
    bool alreadyIgnored = mSeq.isPolygonIgnored( id );

    //Make change if required
    if( (setIgnore and not alreadyIgnored) or (not setIgnore and alreadyIgnored) )
        mSeq.isPolygonIgnored( id, setIgnore );
}

bool MachLogMachine::ignoringEntrancePolygon() const
{
	CB_DEPIMPL( MachLogEntrance*, pLockedEntrance_ );
    bool result = false;

    if( hasLockedEntrance() )
    {
        const MachLogMachineMotionSequencer& mSeq = motionSeq();
        PhysConfigSpace2d::PolygonId id =
            ( &mSeq.currentConfigSpace() == &MachLogPlanet::instance().configSpace() ?
              pLockedEntrance_->exteriorPolygonId() : pLockedEntrance_->interiorPolygonId() );

        result = mSeq.isPolygonIgnored( id );
    }

    return result;
}

bool MachLogMachine::ignoringConstructionPolygon() const
{
    bool result = false;

    if( inBuildingOrOnPad()  and  not insideBuilding() )
    {
        PhysConfigSpace2d::PolygonId id =  pImpl_->pInOrOnPadConstruction_->obstaclePolygonId();
        const MachLogMachineMotionSequencer& mSeq = motionSeq();
        result = mSeq.isPolygonIgnored( id );
    }

    return result;
}

MachLogEntrance& MachLogMachine::lockedEntrance() const
{
    PRE( hasLockedEntrance() );
	CB_DEPIMPL( MachLogEntrance*, pLockedEntrance_ );
    return *pLockedEntrance_;
}

// static
const PhysRelativeTime& MachLogMachine::hitVoiceMailInterval()
{
	static PhysRelativeTime result = 10.0;
	return result;
}

// static
const PhysRelativeTime& MachLogMachine::militaryStrengthEstimateInterval()
{
	static PhysRelativeTime result = 7.0;
	return result;
}

MATHEX_SCALAR MachLogMachine::perceivedDarkness() const
{
	CB_DEPIMPL( MachLogSquadron*, pSquadron_ );

	ASSERT(MachLogPlanet::instance().surface()->hasEnvironment(), "Planet has no environment.");
	MATHEX_SCALAR darkness = MachLogPlanet::instance().surface()->environment().darkness();

	if( hasNVG() )
		return MachLogMachine::thresholdNVGDarkness();
	else if( darkness > 0.0
			 and pSquadron_ != NULL
			 and pSquadron_->hasCommander() )
	{
		// if within 200m of the administrator, get incrementally better information on the environment
		// as he can use his NVG to increase your awareness of, err, stuff
	 	return std::min( darkness, position().euclidianDistance( pSquadron_->commander().position() ) / 200.0 );
	}
	else
		return darkness;
}


// static
MATHEX_SCALAR MachLogMachine::thresholdNVGDarkness()
{
	static MATHEX_SCALAR result	= 0.0;
	return result;
}

//virtual
MachPhys::MachineType MachLogMachine::type() const
{
	CB_DEPIMPL( MachPhys::MachineType, type_ );
	return type_;
}

MATHEX_SCALAR MachLogMachine::scalarSpeedNow() const
{
	MATHEX_SCALAR speedNow = 0.0;

	if( isIn1stPersonView() )
	{
		speedNow = pFirstPersonMachineHandler()->currentSpeed();
	}
	else
	{
		// under normal computer motion sequencer control.
		speedNow = motionSeq().scalarSpeedNow();
	}

	return speedNow;
}

void MachLogMachine::pFirstPersonMachineHandler( MachLog1stPersonMachineHandler* pHandler )
{
	pImpl_->pFirstPersonMachineHandler_	= pHandler;
}

MachLog1stPersonMachineHandler* MachLogMachine::pFirstPersonMachineHandler() const
{
	PRE( isIn1stPersonView() );

	return pImpl_->pFirstPersonMachineHandler_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachine::doingTaskAttack() const
{
	CB_DEPIMPL( MachLogSquadron*, pSquadron_ );
	return ( pSquadron_ and pSquadron_->strategy().currentOperationType() == MachLogOperation::TASK_ATTACK_OPERATION );
}

bool MachLogMachine::machineIsGlider() const
{
	return mobileIsGlider();
}

void MachLogMachine::teleportingInterrupted()
{
	CB_DEPIMPL( bool, isTeleporting_ );
	CB_DEPIMPL( bool, insideAPC_ );

	isTeleporting_ = false;
	insideAPC_ = false;
}

bool MachLogMachine::canEnterConstructionNow( const MachLogConstruction& construction ) const
{
	return( construction.nLogicalEntrances() > 0
			and not( construction.entrance( 0 ).locked() )
			and canFitInsideConstructionNow( construction ) );
}

bool MachLogMachine::canFitInsideConstructionNow( const MachLogConstruction& construction ) const
{
    bool result = true;

	// machine cannot enter if it's too fat, or if the building has a locked entrance
	if( not canEnterConstruction( construction ) )
		result = false;
	else
	{
		// okay, we can potentially fit - but is there room for us at the moment?

		// Need to examine bays if this is a building of our race OR if this machine is an aggressor;
		// Don't care otherwise.
		if( construction.race() == race() and not objectType() == MachLog::AGGRESSOR )
		{
			//See if there is a free station.
			//The internal station will be a PARKING BAY unless this is a research lab.
		    MachLog::ObjectType constructionType = construction.objectType();
		    MachPhysStation::Type stationType =
		        ((constructionType == MachLog::HARDWARE_LAB or
		          constructionType == MachLog::SOFTWARE_LAB) ?
		         MachPhysStation::RESEARCH_BAY :
		         MachPhysStation::PARKING_BAY );

			MachPhysStation* pDummyStation = NULL;

		    MachPhysConstructionData& constructionData =
		        _CONST_CAST( MachPhysConstructionData&, construction.constructionData() );

		    MachPhysStations& stations = constructionData.stations();
			result = stations.freeStation( stationType, &pDummyStation );
		}
		else
			result = construction.hasInterior();
	}
	return result;
}

bool MachLogMachine::isLeavingBuilding() const
{
	return strategy().isDoingLeaveOperation();
}

bool MachLogMachine::isEnteringBuilding( const MachLogConstruction& constron ) const
{
	return strategy().isEnteringBuilding( constron );
}

void MachLogMachine::checkAndDoClearEntrance( MachLogConstruction& constron )
{
	//Construct a circle at actor's current position
    MexCircle2d testCircle( MexPoint2d( position() ), motionSeq().lowClearance() + 0.3 );

	PhysConfigSpace2d& interiorConfigSpace = constron.interiorConfigSpace();

    //Get the interior entrance polygon
    const MexPolygon2d& interiorPolygon =
        interiorConfigSpace.polygon( constron.entrance(0).interiorPolygonId() );

    //See if intersects the interior entrance polygon
    if( testCircle.intersects( interiorPolygon ) )
    {

    	// yes, we need to move clear, if possible.

		MexVec2 pushVector ( constron.entranceExternalPoint( 0 ), constron.entranceInternalPoint( 0 ) );
		pushVector.makeUnitVector();

		bool succeeded = false;

		MATHEX_SCALAR myClearance = motionSeq().lowClearance()*1.2;

		for( MATHEX_SCALAR proximity = 10.0; not succeeded and proximity > 1.0; proximity -= 0.1 )
		{
			MexVec2 moveVector( pushVector );
			moveVector *= proximity;

			MexPoint2d clearPoint( constron.entranceInternalPoint( 0 ) );
			clearPoint += moveVector;

			if( MachLogSpacialManipulation::pointInsideConfigSpaceIsFree( clearPoint, myClearance, interiorConfigSpace ) )
	        {
				succeeded = true;
				strategy().newOperation( _NEW( MachLogMoveToOperation( this, clearPoint, false ) ) );
	        }
		}
    }
}
void MachLogMachine::displayAsRace( MachPhys::Race displayAsRace )
{
	//physMachine().convertMaterials( physMachine().machineData(), displayAsRace );
	physMachine().changeColour( displayAsRace );
	displayMapAndIconRace( displayAsRace );
}

//static
void MachLogMachine::resetNextGlobalHitVoiceMailTime()
{
	// note small 0.5 seconds margin of safety
	nextGlobalHitVoiceMailTime() = SimManager::instance().currentTime() + hitVoiceMailInterval() - 0.5;
}

void MachLogMachine::checkAndDoHitVoiceMail()
{
	if( race() == MachLogRaces::instance().pcController().race()
		and sufficientTimePassedSinceLastHit() )
	{
		if( hpRatio() <= 0.35 )
			// give voicemail to warn that damage is dangerously high
			MachLogMachineVoiceMailManager::instance().postNewMail( objectType(), subType(), MachLogMachineVoiceMailManager::MEV_NEAR_DEATH, id(),  race() );
		else
			// just report on having been hit
			MachLogMachineVoiceMailManager::instance().postNewMail( objectType(), subType(), MachLogMachineVoiceMailManager::MEV_DAMAGED, id(),  race() );
	}
}

// static
PhysAbsoluteTime& MachLogMachine::nextGlobalHitVoiceMailTime()
{
	static PhysAbsoluteTime nextGlobalHitVoiceMailTime_ = 0.0;
	return nextGlobalHitVoiceMailTime_;
}

// static
MexPoint2d& MachLogMachine::lastGlobalHitVoiceMailPosition()
{
	static MexPoint2d lastGlobalHitVoiceMailPosition_( 0, 0 );
	return lastGlobalHitVoiceMailPosition_;
}

void MachLogMachine::rampaging( bool status )
{
	CB_DEPIMPL( bool, rampaging_ );

	rampaging_ = status;
}

bool MachLogMachine::rampaging() const
{
	CB_DEPIMPL( bool, rampaging_ );

	return rampaging_;
}

// Forced recompile 16/2/99 CPS
/////////////////////////////////////////////////////////////////////////////////////////////////////

/* End MACHINE.CPP **************************************************/
