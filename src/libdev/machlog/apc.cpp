/*
 * A P C . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */
#include "stdlib/string.hpp"
#include "machlog/apc.hpp"
#include "machlog/internal/apci.hpp"

#include "ctl/algorith.hpp"
#include "ctl/list.hpp"

#include "mathex/eulerang.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/radians.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/point2d.hpp"

#include "world4d/domain.hpp"

#include "phys/motchunk.hpp"

#include "machphys/apc.hpp"
#include "machphys/apcdata.hpp"

#include "machlog/animatio.hpp"
#include "machlog/cntrl.hpp"
#include "machlog/opdapc.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/spacial.hpp"
#include "machlog/strategy.hpp"
#include "machlog/vmman.hpp"
#include "machlog/vmdata.hpp"


#include "phys/cspace2.hpp"

PER_DEFINE_PERSISTENT( MachLogAPC );

//It is inefficient to dereference so many variables and we will get a hit under debug.
//under release all unreferenced variables will be optpomised out.
#define CB_MachLogAPC_DEPIMPL() \
		PRE( pImpl_ ); \
		CB_DEPIMPL( int	,nMachinesCarried_); \
		CB_DEPIMPL( int ,nMachinesIncoming_); \
		CB_DEPIMPL( MachPhys::BuildingMaterialUnits	,amountCarried_ ); \
		CB_DEPIMPL( MachLogAPC::Machines ,carriedMachines_ ); \
		CB_DEPIMPL( MachLogAPC::Machines ,incomingMachines_ ); \
		CB_DEPIMPL( bool ,turningTowardMachine_ ); \
		CB_DEPIMPL( int ,nDeployOps_ ); \
		CB_DEPIMPL( MexPoint3d ,dropzonePosition_ );

/* //////////////////////////////////////////////////////////////// */

MachLogAPC::MachLogAPC( MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel,
    								MachLogRace * pRace, const MexPoint3d& location )
: 	MachLogMachine( MachPhys::APC, hwLevel, swLevel, pRace,
                    pNewPhysAPC( hwLevel, swLevel, pRace, location ), MachLog::APC ),
	pImpl_( _NEW( MachLogAPCImpl ) )
{
	hp( data().hitPoints() );
	armour( data().armour() );
	ctl_append( &MachLogRaces::instance().apcs( pRace->race() ), this );
}

MachLogAPC::MachLogAPC( MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel,
    								MachLogRace * pRace, const MexPoint3d& location, UtlId withId )
: 	MachLogMachine( MachPhys::APC, hwLevel, swLevel, pRace,
                    pNewPhysAPC( hwLevel, swLevel, pRace, location ), MachLog::APC, withId ),
	pImpl_( _NEW( MachLogAPCImpl ) )
{
	hp( data().hitPoints() );
	armour( data().armour() );
	ctl_append( &MachLogRaces::instance().apcs( pRace->race() ), this );
}

///////////////////////////

MachLogAPC::~MachLogAPC()
{
	CB_MachLogAPC_DEPIMPL();
	HAL_STREAM("(" << id() << ") MachLogAPC::DTOR carriedMachines_.size() " << carriedMachines_.size() << std::endl );

	_DELETE( pImpl_ );
}

/* //////////////////////////////////////////////////////////////// */


void MachLogAPC::doOutputOperator( ostream& o ) const
{
	CB_MachLogAPC_DEPIMPL();
	o << " amountCarried " << amountCarried_ << std::endl;
	o << " capacity " << data().capacity() << std::endl;
	o << " carriedMachines_.size() " << carriedMachines_.size() << std::endl;
	o << " nMachinesCarried_ " << nMachinesCarried_ << std::endl;
	o << " nMachinesIncoming_ " << nMachinesIncoming_ << std::endl;
	MachLogMachine::doOutputOperator( o );
}

//////////////////////////////////////////////////////////////////////////////////

//static
MachPhysAPC* MachLogAPC::pNewPhysAPC
(
    Level hwLevel, Level swLevel, MachLogRace * pRace, const MexPoint3d& location
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, 0, &localTransform );

    //Construct the physical machine
    return _NEW( MachPhysAPC( pDomain, localTransform, hwLevel, swLevel, pRace->race() ) );
}

//virtual
void MachLogAPC::beHit( const int& damage, MachPhys::WeaponType byType,
						MachActor* pByActor, MexLine3d* pByDirection, MachActor::EchoBeHit echo )
{
	MachLogMachine::beHit( damage, byType, pByActor, pByDirection, echo );;

	specialAPCHitStuff();

}

// virtual
void MachLogAPC::beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay, MachActor* pByActor, MachActor::EchoBeHit echo )
{
	MachLogMachine::beHitWithoutAnimation( damage, physicalTimeDelay, pByActor, echo );

	specialAPCHitStuff();
}

void MachLogAPC::specialAPCHitStuff()
{
	CB_MachLogAPC_DEPIMPL();

	if( isDead() )
	{
		// all machines inside me get blown to smithereens too.
		while( carriedMachines_.size() > 0 )
		{
	        //get the machine and remove it from the collection
			MachLogMachine* pmlm = carriedMachines_.back();
	        carriedMachines_.pop_back();

			// detach it
			pmlm->detach( this );

			//kill it off
			pmlm->beHitWithoutAnimation( 1000000 );
		}
	}
	else if( carriedMachines_.size() > 0
			 and ( ( virtualDefCon() == MachLog::DEFCON_LOW and hpRatio() < 0.20 )
				or ( virtualDefCon() == MachLog::DEFCON_NORMAL and hpRatio() < 0.10 ) ) )
	{
		// damage critical - evacuate craft! But only trigger new op if we're not already doing
		// a deploy in this area.
		if( not isDeploying() or not nearDropzone() )
			newOperation( _NEW( MachLogDeployAPCOperation( this, position() ) ) );
	}
}

bool MachLogAPC::isDeploying() const
{
	CB_MachLogAPC_DEPIMPL();
	return nDeployOps_ > 0;
}

void MachLogAPC::addedDeployOp()
{
	CB_MachLogAPC_DEPIMPL();

	++nDeployOps_;

	POST( nDeployOps_ > 0 );
}

void MachLogAPC::removedDeployOp()
{
	CB_MachLogAPC_DEPIMPL();

	--nDeployOps_;

	POST( nDeployOps_ >= 0 );
}

void MachLogAPC::dropzonePosition( const MexPoint3d& position )
{
	CB_MachLogAPC_DEPIMPL();

	dropzonePosition_ = position;
}

const MexPoint3d& MachLogAPC::dropzonePosition() const
{
	CB_MachLogAPC_DEPIMPL();

	return dropzonePosition_;
}

bool MachLogAPC::nearDropzone() const
{
	CB_MachLogAPC_DEPIMPL();

	PRE( isDeploying() );

	bool result = false;

	MATHEX_SCALAR sqrDistanceToDropzone = asAPC().position().sqrEuclidianDistance( dropzonePosition() );

	if( sqrDistanceToDropzone < 2500 )
		result = true;

	return result;
}

//////////////////////////////////////////////////////////////////////////////////

//virtual
const MachPhysMachineData& MachLogAPC::machineData() const
{
	return data();
}

//////////////////////////////////////////////////////////////////////////////////

const MachPhysAPCData& MachLogAPC::data() const
{
	//return  _STATIC_CAST( MachPhysAPCData&, physMachine().machineData() );
	return  _STATIC_CAST( const MachPhysAPCData&, physMachine().machineData() );
}

//////////////////////////////////////////////////////////////////////////////////

const MachPhys::BuildingMaterialUnits&
MachLogAPC::amountCarried() const
{
	CB_MachLogAPC_DEPIMPL();
	return amountCarried_;
}

//////////////////////////////////////////////////////////////////////////////////

MachPhys::BuildingMaterialUnits&
MachLogAPC::amountCarried()
{
	CB_MachLogAPC_DEPIMPL();
	return amountCarried_;
}

//////////////////////////////////////////////////////////////////////////////////

const MachLogAPC::Machines&
MachLogAPC::machines() const
{
	CB_MachLogAPC_DEPIMPL();
	const Machines& result = carriedMachines_;
	return result;
}

//////////////////////////////////////////////////////////////////////////////////

bool MachLogAPC::add( MachLogMachine* pNewMachine )
{
	CB_MachLogAPC_DEPIMPL();
	PRE(pNewMachine != NULL);

	bool result=false;

   	if( roomForMachine( *pNewMachine ) )
	{
		amountCarried_ += pNewMachine->machineData().volume();
		carriedMachines_.push_back( pNewMachine );
		pNewMachine->attach( this );
		pNewMachine->insideAPC( true );
		pNewMachine->strategy().tryToRemoveAllOperations();
		pNewMachine->APCImInside( this );
		pNewMachine->teleportOutOfWorld();

		++nMachinesCarried_;

		result = true;

		// give voicemail
		MachLogVoiceMailManager::instance().postNewMail( VID_APC_UNIT_BOARDED, id(), race() );
	}

	POST( nMachinesCarried_ == carriedMachines_.size() );

	return result;
}

//////////////////////////////////////////////////////////////////////////////////

void MachLogAPC::remove( MachLogMachine* pMachineRemoved, const MexPoint3d& dest3d )
{
	CB_MachLogAPC_DEPIMPL();
	bool found = false;

	Machines::iterator i = find( carriedMachines_.begin(), carriedMachines_.end(), pMachineRemoved );

	ASSERT( i != carriedMachines_.end(), "Machine was not found!" );

	found = true;
	amountCarried_ -= pMachineRemoved->machineData().volume();
	carriedMachines_.erase( i );
	--nMachinesCarried_;

	pMachineRemoved->detach( this );

	pMachineRemoved->insideAPC( false );

	// for safety - ensures absolutely that there are no ops on the machine's strategy queue from the moment
	// it enters the world - it is *conceivable* that one could be added right as it teleports inside the APC.
	pMachineRemoved->strategy().tryToRemoveAllOperations();

	pMachineRemoved->snapTo( dest3d );//physObject().attachTo( MachLogPlanet::instance().pWorld() );
	pMachineRemoved->teleportIntoWorld();

	if( amountCarried_ == 0 )
	// give voicemail
		MachLogVoiceMailManager::instance().postNewMail( VID_APC_DEPLOYMENT_COMPLETE, id(), race() );

	POST( nMachinesCarried_ == carriedMachines_.size() );
}

//////////////////////////////////////////////////////////////////////////////////

bool MachLogAPC::nextDeployPoint(MexPoint3d* pDest3d, MATHEX_SCALAR clearanceOfDeployed)
{
   		PhysConfigSpace2d& cs = MachLogPlanet::instance().configSpace();
		bool spaceAvailable = true;

		MexPoint2d freePoint;

	   	//allow machines to deploy up to a cell away (~30m).


		MexTransform3d apcTransform	= physMachine().globalTransform();
		spaceAvailable = MachLogSpacialManipulation::getNearestFreeSpacePoint( apcTransform, highClearence() + 10, clearanceOfDeployed*1.5, &freePoint );

		if( spaceAvailable )
			*pDest3d = MachLogSpacialManipulation::heightAdjustedLocation( freePoint );

		return spaceAvailable;
}

//////////////////////////////////////////////////////////////////////////////////

bool MachLogAPC::alignedWithDeployPoint(const MexPoint3d& dest3d)
{
   	MexRadians alignmentAngle = MachLogSpacialManipulation::angleToTurnToFace( *this, dest3d );

	return ( alignmentAngle > MexRadians( 3 ) or alignmentAngle < MexRadians( -3 ) );
	//return true if REAR is (almost) facing deployment point

}


//////////////////////////////////////////////////////////////////////////////////

void MachLogAPC::addIncoming( MachLogMachine* pNewIncomingMachine )
{
	CB_MachLogAPC_DEPIMPL();

	PRE( find( incomingMachines_.begin(), incomingMachines_.end(), pNewIncomingMachine ) == incomingMachines_.end() );

   	incomingMachines_.push_back( pNewIncomingMachine );

	++nMachinesIncoming_;

	//	Ensures rapid response to new 'incoming' data.
	if (nextUpdateTime()>0.5)
		nextUpdateTime(0.5);
}

//////////////////////////////////////////////////////////////////////////////////

void MachLogAPC::removeIncoming( MachLogMachine* pOldIncomingMachine )
{
	CB_MachLogAPC_DEPIMPL();

	PRE( find( incomingMachines_.begin(), incomingMachines_.end(), pOldIncomingMachine ) != incomingMachines_.end() );

	Machines::iterator i = find( incomingMachines_.begin(), incomingMachines_.end(), pOldIncomingMachine );

	incomingMachines_.erase( i );
	--nMachinesIncoming_;
}

//////////////////////////////////////////////////////////////////////////////////

//static
void MachLogAPC::assignAPCTask( MachLogAPC* obj )
{
	bool failedToAssign = true;

	if( failedToAssign )
		MachLogRaces::instance().controller( obj->race() ).moveOutOfTheWay( obj );
}

//////////////////////////////////////////////////////////////////////////////////

//virtual
PhysRelativeTime MachLogAPC::update( const PhysRelativeTime& maxCPUTime,
                                     MATHEX_SCALAR clearance )
{
	CB_MachLogAPC_DEPIMPL();
	MachLogMachine::update( maxCPUTime, clearance );

	if( isIdle() )
		turningTowardMachine_ = false;

	if ( not isIn1stPersonView()
		 and nMachinesIncoming_ > 0
		 and ( ( virtualDefCon() != MachLog::DEFCON_HIGH and not evading() )
			  or isIdle() ) )
	{
		if( not turningTowardMachine_ )
			turnRearToFaceNearestIncoming();

		// ensure APC will not be jostled around by other machines while it prepares to load incoming machines
		motionSeq().allowMoveOutOfWay( false );
	}
	else
		// ensure actor's motion sequencer is not locked
		motionSeq().allowMoveOutOfWay( true );


    return 1.0;
}


//////////////////////////////////////////////////////////////////////////////////

void MachLogAPC::turnRearToFaceNearestIncoming()
{
	CB_MachLogAPC_DEPIMPL();

	PRE( nMachinesIncoming_ > 0 );

	turningTowardMachine_ = true;
	motionSeq().stop();

	MexRadians newAngle = ( MachLogSpacialManipulation::angleToTurnToFace( *this, currentNearestMachine().position() ) + MexRadians( Mathex::PI ) );
	//	(Add 180 degrees as you want to point your arse towards it).

	newOperation( _NEW(MachLogTurnAnimation( this, newAngle ) ) );
}

//////////////////////////////////////////////////////////////////////////////////

const MachLogMachine& MachLogAPC::currentNearestMachine() const
{
	CB_MachLogAPC_DEPIMPL();

	PRE( nMachinesIncoming_ > 0 );

	MATHEX_SCALAR nearestDistance = 9999;

	MachLogMachine* pCurrentNearest = NULL;

  	Machines::iterator i;

	// Find best incoming machine to turn towards (nominally 'nearest' although in truth not necessarily so)
	// using one of two different iterate and compare loops.


	// firstly, evaluate using distance as priority

	for (i = incomingMachines_.begin();i != incomingMachines_.end(); ++i)
	{
   	 	MATHEX_SCALAR testDistance = position().euclidianDistance( (*i)->position() );

		if( testDistance + graceDistance() < nearestDistance )
		{
			pCurrentNearest = (*i);
			nearestDistance = testDistance;
		}
	 }

	if( sqr( nearestDistance ) <= sqrCloseDistance() )
	{
		// distance of nearest found is sufficiently close for relative angle of APC to machines to be priority
		// when deciding which to attend to first - no point spinning around 180 degrees just because another
		// machine is 1m closer.

		MexRadians absRelativeAngleOfNearest = 0;

		for (i = incomingMachines_.begin();i != incomingMachines_.end(); ++i)
		{
	   		MexRadians absTempRelativeAngle = fabs( MachLogSpacialManipulation::angleToTurnToFace( *this, (*i)->position() ).asScalar() );

			if( ( position().sqrEuclidianDistance( (*i)->position() ) < sqrCloseDistance() )
				and ( absTempRelativeAngle > absRelativeAngleOfNearest ) )
			{
				pCurrentNearest = (*i);
				absRelativeAngleOfNearest = absTempRelativeAngle;
 			}
		}
	}

	ASSERT( pCurrentNearest, "No nearest machine was found!" );

	return *pCurrentNearest;
}

//////////////////////////////////////////////////////////////////////////////////


void perWrite( PerOstream& ostr, const MachLogAPC& actor )
{
	const MachLogMachine& base1 = actor;

	ostr << base1;
	ostr << actor.pImpl_;

	MexTransform3d trans = actor.globalTransform();

    //If we don't do this, the large coords of the transform cause are used when read back in to find
    //a planet domain, which of course falls over big time.
    if( actor.insideAPC() )
        trans.position( MexPoint3d(10,10,0) );

	PER_WRITE_RAW_OBJECT( ostr, trans );
	const W4dId id = actor.id();
	PER_WRITE_RAW_OBJECT( ostr, id );
	actor.persistenceWriteStrategy( ostr );
}

void perRead( PerIstream& istr, MachLogAPC& actor )
{
	MachLogMachine& base1 = actor;

	istr >> base1;
	istr >> actor.pImpl_;

	MexTransform3d trans;
	W4dId id;
	PER_READ_RAW_OBJECT( istr, trans );
	PER_READ_RAW_OBJECT( istr, id );

	MachPhysAPC* pPhysAPC = MachLogAPC::pNewPhysAPC( base1.hwLevel(), base1.swLevel(), &actor.logRace(), trans.position() );
	actor.setObjectPtr( pPhysAPC, trans );
	actor.id( id );
	actor.machineCreated();
	actor.persistenceReadStrategy( istr );
}

MachLogAPC::MachLogAPC( PerConstructor con )
:	MachLogMachine( con )
{
}

const MATHEX_SCALAR& MachLogAPC::graceDistance()
{
	static MATHEX_SCALAR result = 10;
	return result;
}

const MATHEX_SCALAR& MachLogAPC::sqrCloseDistance()
{
	static MATHEX_SCALAR result = 400;
	return result;
}

int	MachLogAPC::nMachinesCarried() const
{
	CB_MachLogAPC_DEPIMPL();
	return nMachinesCarried_;
}

bool MachLogAPC::roomForMachine( const MachLogMachine& machine ) const
{
	CB_MachLogAPC_DEPIMPL();
	return amountCarried_ + machine.machineData().volume() <= data().capacity();
}

//virtual
bool MachLogAPC::beNotified( W4dSubject* pSubject,
                 W4dSubject::NotificationEvent event, int /*clientData*/ )
{
	CB_MachLogAPC_DEPIMPL();

	bool stayAttached = true;

	switch( event )
	{
		case W4dSubject::DELETED:
		{
			Machines::iterator i = find( carriedMachines_.begin(), carriedMachines_.end(), pSubject );

			if( i != carriedMachines_.end() )
			{
				MachLogMachine& deletedMachine = (**i);

				deletedMachine.insideAPC( false );

				amountCarried_ -= deletedMachine.machineData().volume();
				carriedMachines_.erase( i );
				--nMachinesCarried_;

				stayAttached = false;
			}
		}
		break;

	default:
		;
	}

	return stayAttached;
}


//////////////////////////////////////////////////////////////////////////////////

/* End APC.CPP *************************************************/
