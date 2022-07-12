/*
 * E X P A N D B L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/expandbl.hpp"
#include "machlog/internal/expandbi.hpp"

#include "ctl/list.hpp"
#include "ctl/algorith.hpp"

#include "stdlib/string.hpp"

#include "mathex/abox2d.hpp"
#include "mathex/point2d.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/vec2.hpp"

#include "utility/id.hpp"
#include "utility/bndidgen.hpp"

#include "phys/rampacce.hpp"
#include "phys/cspace2.hpp"
#include "phys/motchunk.hpp"

#include "world4d/domain.hpp"
#include "world4d/composit.hpp"

#include "machphys/mphydata.hpp"
#include "machphys/constron.hpp"

#include "sim/manager.hpp"

#include "machlog/artefact.hpp"
#include "machlog/constron.hpp"
#include "machlog/lineproj.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/network.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/planet.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/spacial.hpp"
#include "machlog/stats.hpp"


MachLogExpandingBlast::MachLogExpandingBlast( MachLogRace* pRace,
											  W4dEntity* pPhysObject,
        								  	  const MexPoint3d& startPosition,
        									  MachActor* pOwnerActor,
											  MATHEX_SCALAR blastRadius,
											  CanHitAirUnits canHitAirUnits )
:	SimProjectile( pRace, pPhysObject ),
	pImpl_( _NEW( MachLogExpandingBlastImpl( pOwnerActor, pRace, startPosition, blastRadius, canHitAirUnits ) ) )
{
	CB_MachLogExpandingBlast_DEPIMPL();

	//Set the id
	if( MachLogNetwork::instance().isNetworkGame() )
	{
		id( MachLogRaces::idGenerator().nextId( pRace->race() * 250 + 1, pRace->race() * 250 + 251 ) );
	}
	else
	{
    	id( MachLogRaces::idGenerator().nextId() );
	}

	if( pOwner() )
	{
		// must observe owner in case he karks it during the explosion.
		pOwner_->attach( this );

		if( pOwner()->objectIsMachine() and pOwner()->asMachine().insideBuilding() )
		{
			pConstruction_ = &( pOwner()->asMachine().insideWhichBuilding() );
			pConstruction_->attach( this );
		}
	}
}

MachLogExpandingBlast::~MachLogExpandingBlast()
{

	CB_MachLogExpandingBlast_DEPIMPL();

	TEST_INVARIANT;

    MachLogRaces::idGenerator().freeId( id() );

	if( startedFirstWave_ )
		detachFromFirstWave();

	if( pOwner() )
	{
		pOwner_->detach( this );
	}

	if( pConstruction_ )
	{
		pConstruction_->detach( this );
	}

	_DELETE( pImpl_ );
}

void MachLogExpandingBlast::detachFromFirstWave()
{
	CB_MachLogExpandingBlast_DEPIMPL();

	//detach from all victims still on the victim list
	for( Victims::iterator i = victims_.begin(); i != victims_.end(); ++i )
	{
		MachActor* pVictim = (*i);
		if( pVictim )
			pVictim->detach( this );
	}

	TEST_INVARIANT;
}

PhysAbsoluteTime MachLogExpandingBlast::creationTime() const
{
	CB_MachLogExpandingBlast_DEPIMPL();

	return creationTime_;
}

//virtual
PhysRelativeTime MachLogExpandingBlast::update( const PhysRelativeTime& /*maxCPUTime*/,
                                     MATHEX_SCALAR /*clearanceFromDisplayedVolume*/ )
{

	ASSERT( firstWaveStartTime() <= secondWaveStartTime(), "Second wave cannot begin before first wave." );
	ASSERT( secondWaveStartTime() <= thirdWaveStartTime(), "Third wave cannot begin before second wave." );
	ASSERT( firstWaveStartTime() <= firstWaveFinishTime(), "A wave's end time cannot be before its start time." );
	ASSERT( secondWaveStartTime() <= secondWaveFinishTime(), "A wave's end time cannot be before its start time." );
	ASSERT( thirdWaveStartTime() <= thirdWaveFinishTime(), "A wave's end time cannot be before its start time." );

	ASSERT( firstWaveFinishTime() <= destructionTime(), "A wave's end time must be on or before the total destruction time." );
	ASSERT( secondWaveFinishTime() <= destructionTime(), "A wave's end time must be on or before the total destruction time." );
	ASSERT( thirdWaveFinishTime() <= destructionTime(), "A wave's end time must be on or before the total destruction time." );

	CB_MachLogExpandingBlast_DEPIMPL();

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	PhysRelativeTime interval = 0.0;

	// some blasts can be aborted before the first wave begins
	if( finished() and timeNow < firstWaveStartTime() )
	{
		// self-terminate
		isDead( true );
	}
	// check if object has reached the end of its allotted time and should be destructed
	else if( timeNow >= destructionTime() )
	{
		isDead( true );
	}
	else
	{
		// we should actually do some logical hit checking
		if( timeNow >= firstWaveStartTime() and timeNow < firstWaveFinishTime()  )
		{
			if( not startedFirstWave_ )
				buildVictimList();

			if( nVictims() > 0 )
	 			damageCheckFirstWave();
		}
		if( timeNow >= secondWaveStartTime() and timeNow < secondWaveFinishTime()  )
		{
			// this may change later
			if( not startedFirstWave_ )
				buildVictimList();

			if( nVictims() > 0 )
	 			damageCheckSecondWave();
		}
		if( timeNow >= thirdWaveStartTime() and timeNow < thirdWaveFinishTime()  )
		{
			// this may change later
			if( not startedFirstWave_ )
				buildVictimList();

			if( nVictims() > 0 )
	 			damageCheckThirdWave();
		}
	}

	return interval;
}

MATHEX_SCALAR MachLogExpandingBlast::blastRadius() const
{
	CB_MachLogExpandingBlast_DEPIMPL();

	return blastRadius_;
}

const MachLogRace* MachLogExpandingBlast::pRace() const
{
	CB_MachLogExpandingBlast_DEPIMPL();

	return pRace_;
}

MachLogRace* MachLogExpandingBlast::pRace()
{
	CB_MachLogExpandingBlast_DEPIMPL();

	return pRace_;
}

const MachActor* MachLogExpandingBlast::pOwner() const
{
	CB_MachLogExpandingBlast_DEPIMPL();

	return pOwner_;
}

MachActor* MachLogExpandingBlast::pOwner()
{
	CB_MachLogExpandingBlast_DEPIMPL();

	return pOwner_;
}

const MexPoint3d& MachLogExpandingBlast::startPosition() const
{
	CB_MachLogExpandingBlast_DEPIMPL();

	return startPosition_;
}

void MachLogExpandingBlast::CLASS_INVARIANT
{
	CB_MachLogExpandingBlast_DEPIMPL();

    INVARIANT( this != NULL );
	INVARIANT( nVictims_ >= 0 );
}

ostream& operator <<( ostream& o, const MachLogExpandingBlast& t )
{

    o << "MachLogExpandingBlast " << (void*)&t << " start" << std::endl;
    o << "MachLogExpandingBlast " << (void*)&t << " end" << std::endl;

    return o;
}

int MachLogExpandingBlast::nVictims() const
{
	CB_MachLogExpandingBlast_DEPIMPL();

	return nVictims_;
}

void MachLogExpandingBlast::finished( bool status )
{
	CB_MachLogExpandingBlast_DEPIMPL();

	finished_ = status;
}

bool MachLogExpandingBlast::finished() const
{
	CB_MachLogExpandingBlast_DEPIMPL();

	return finished_;
}

MachLogExpandingBlast::Victims& MachLogExpandingBlast::victims()
{
	CB_MachLogExpandingBlast_DEPIMPL();

	return victims_;
}

void MachLogExpandingBlast::finishedWithVictim( MachActor* pVictim, SuppressDetach suppressDetach )
{
	CB_MachLogExpandingBlast_DEPIMPL();

	PRE( pVictim != NULL );
	PRE( nVictims_ >= 1 );

	Victims::iterator i = find( victims().begin(), victims().end(), pVictim );

	if( i != victims().end() )
	{
		// do NOT detach if the victim is the owner - only the owner's death
		// (signalled via the observer pattern) can warrant a detach from him.
		// Same applies for the pConstruction_ pointer if we happen to be inside
		// a building domain
		if( suppressDetach == DONT_SUPPRESS_DETACH and pVictim != pOwner()
												   and pVictim != pConstruction_ )
			pVictim->detach( this );

		(*i) = NULL;

		--nVictims_;
	}
}

void MachLogExpandingBlast::victimMayHaveBeenKilled( MachActor* pVictim, int damageTaken )
{
	if( ( pVictim->hp() + pVictim->armour() ) <= damageTaken )
		beNotified( pVictim, W4dSubject::DELETED );
}

bool MachLogExpandingBlast::addVictim( MachActor* pVictim )
{
	CB_MachLogExpandingBlast_DEPIMPL();

	PRE( pVictim != NULL );

	bool added = false;

	// check for duplicates
	Victims::iterator i = find( victims_.begin(), victims_.end(), pVictim );
	if( i == victims_.end() )
	{
		victims().push_back( pVictim );
		pVictim->attach( this );

		++nVictims_;

		added = true;

		ASSERT( nVictims_ > 0, "nVictims_ counter has an unexpected <=0 value." );
	}

	return added;
}

//virtual
void MachLogExpandingBlast::ownerDestroyed()
{
	CB_MachLogExpandingBlast_DEPIMPL();

	pOwner_ = NULL;
}

void MachLogExpandingBlast::doBeDestroyed()
{

}

void MachLogExpandingBlast::buildVictimList()
{
	CB_MachLogExpandingBlast_DEPIMPL();

	buildVictimMachinesList();
	buildVictimNonMachinesList();

	startedFirstWave_ = true;
}

void MachLogExpandingBlast::buildVictimMachinesList()
{
	CB_MachLogExpandingBlast_DEPIMPL();

    const MATHEX_SCALAR dummyAcceleration = 1.0;
    const MATHEX_SCALAR accelerationTime = 0.0;
	MATHEX_SCALAR victimRadius = blastRadius_ * potentialKillRadiusMultiplier();
	// Multiplies actual maximum explosion radius by stats-based value to give estimate of range from which machines
	// could potentially walk into the explosion

	const MATHEX_SCALAR dummyDistance = 0.5;
    PhysRampAcceleration    acc(
        1.0,
        dummyAcceleration, //Signed: -ve indicates deceleration
        dummyAcceleration, //Signed: -ve indicates deceleration
        2.0,
        accelerationTime,
        accelerationTime );

    const   PhysRelativeTime    timeOffset = 0.0;
	MexPoint3d endPosition( startPosition_ );
	endPosition.x( endPosition.x() + dummyDistance );

    //  Define the height limits for the blast
    const MexDouble minHeight = MexDouble::minusInfinity();
    const MexDouble maxHeight = MexDouble::plusInfinity();

    PhysMotionChunk motionChunk(
        startPosition_,
        endPosition,
        victimRadius,
        acc,
        SimManager::instance().currentTime(),
        timeOffset,
        minHeight,
        maxHeight );
    PhysConfigSpace2d::Intersections    intersections;

	PhysConfigSpace2d* pConfigSpace = NULL;

	if( pConstruction_ )
	{
		ASSERT( pConstruction_->physConstruction().hasInterior(), "Something clearly going wrong here - expected construction to have interior." );
		pConfigSpace = &( pConstruction_->interiorConfigSpace() );
	}
	else
	{
		//the blast is to be created in the main planet config space.
		pConfigSpace = &( MachLogPlanet::instance().configSpace() );
	}

    pConfigSpace->findIntersections( motionChunk, id(), &intersections );

	for( PhysConfigSpace2d::Intersections::iterator i = intersections.begin(); i != intersections.end(); ++i )
	{
		if( MachLogRaces::instance().actorExists( (*i).collisionObjectId_.asScalar() ) )
		{
			MachActor* pActor = &( MachLogRaces::instance().actor( (*i).collisionObjectId_.asScalar() ) );

			if( canHitAirUnits_ == CAN_HIT_AIR_UNITS or not( pActor->objectIsMachine() and pActor->asMachine().machineIsGlider() ) )
			{
				addVictim( pActor );
			}

		}
	}

	TEST_INVARIANT;
}

void MachLogExpandingBlast::buildVictimNonMachinesList()
{
	CB_MachLogExpandingBlast_DEPIMPL();

	if( pConstruction_ )
	{
		// only building to be affected is this one itself
		addVictim( pConstruction_ );
	}
	else
	{
		MATHEX_SCALAR victimRadius = blastRadius_;

	    //Store various useful values
	    PhysConfigSpace2d& planetConfigSpace = MachLogPlanet::instance().configSpace();
	    MexAlignedBox2d tempBoundary;

		//check for constructions.
		MexAlignedBox2d boundary( startPosition(), victimRadius );
		for( MachLogRaces::Objects::iterator i = MachLogRaces::instance().objects().begin();
			i != MachLogRaces::instance().objects().end(); ++i )
	    {
	        MachActor& candidateActor = *(*i);
	        bool hit = false;

			if( candidateActor.objectIsConstruction() )
			{
	            //Check for intersection of vortex boundary with construction boundary
	            hit = candidateActor.asConstruction().globalBoundary().intersects( boundary );
			}
	        else if( candidateActor.objectIsArtefact() )
	        {
	            //get the boundary of the obstacle polygon for this artefact via the config space
	            planetConfigSpace.polygon( candidateActor.asArtefact().obstaclePolygonId() ).boundary( &tempBoundary );
	            hit = tempBoundary.intersects( boundary );
	        }
			else if( candidateActor.objectIsLandMine() )
			{
				hit = candidateActor.position().sqrEuclidianDistance( startPosition_ ) <= sqr( victimRadius );
			}

	        if( hit )
	        {
				addVictim( &candidateActor );
			}
		}
	}

		TEST_INVARIANT;
}


void MachLogExpandingBlast::damageCheckFirstWave()
{
	for( Victims::iterator i = victims().begin(); i != victims().end(); ++i )
	{
		MachActor* pVictim = (*i);
		// first part of conditional ignores entries now set to NULL
		if( pVictim and hitVictimFirstWave( *pVictim ) )
			inflictDamageFirstWave( pVictim );
	}
}

void MachLogExpandingBlast::damageCheckSecondWave()
{
	for( Victims::iterator i = victims().begin(); i != victims().end(); ++i )
	{
		MachActor* pVictim = (*i);

		// First part of conditional ignores entries now set to NULL
		if( pVictim and hitVictimSecondWave( *pVictim ) )
			inflictDamageSecondWave( pVictim );
	}
}

void MachLogExpandingBlast::damageCheckThirdWave()
{
	for( Victims::iterator i = victims().begin(); i != victims().end(); ++i )
	{
		MachActor* pVictim = (*i);
		// First part of conditional ignores entries now set to NULL
		if( pVictim and hitVictimThirdWave( *pVictim ) )
			inflictDamageThirdWave( pVictim );
	}
}


bool MachLogExpandingBlast::actorWithinRadius( const MachActor& victim, MATHEX_SCALAR distanceFromEpicentre ) const
{
	bool result;

	if( victim.objectIsConstruction() )
		result = constructionWithinRadius( victim.asConstruction(), distanceFromEpicentre );
	else
		result = centreOfActorWithinRadius( victim, distanceFromEpicentre );

	return result;
}

bool MachLogExpandingBlast::centreOfActorWithinRadius( const MachActor& victim, MATHEX_SCALAR distanceFromEpicentre ) const
{
	bool result = false;

	MATHEX_SCALAR sqrEucDistanceToTarget = position().sqrEuclidianDistance( victim.position() );

 	//if( sqrEucDistanceToTarget  <= sqr( distanceFromEpicentre ) )
 	if( sqrEucDistanceToTarget  <= ( distanceFromEpicentre*distanceFromEpicentre ) )
   		result = true;

	return result;
}

bool MachLogExpandingBlast::constructionWithinRadius( const MachLogConstruction& victim, MATHEX_SCALAR distanceFromEpicentre ) const
{
	return MachLogSpacialManipulation::withinRangeOfMexAlignedBox2d( victim.globalBoundary(), startPosition(), distanceFromEpicentre );
}

//virtual
bool MachLogExpandingBlast::hitVictimSecondWave( const MachActor& /*pVictim*/ ) const
{
	// required
	return false;
}

//virtual
bool MachLogExpandingBlast::hitVictimThirdWave( const MachActor& /*pVictim*/ ) const
{
	// required
	return false;
}

//virtual
void MachLogExpandingBlast::inflictDamageSecondWave( MachActor* /*pVictim*/ )
{
	// do nothing
}

//virtual
void MachLogExpandingBlast::inflictDamageThirdWave( MachActor* /*pVictim*/ )
{
	// do nothing
}

//virtual
bool MachLogExpandingBlast::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData )
{
	CB_MachLogExpandingBlast_DEPIMPL();

	ASSERT ( nVictims() or( pOwner() ) or pConstruction_, "This expanding blast believes it has no observation subjects, yet it is being notified." );

	//remove subject from list of victims if event = I'm dead, by setting pointer in ctl_pvector to NULL

	bool remainAsObserver = true;


	switch( event )
	{
	case W4dSubject::DELETED:
	{
		remainAsObserver = false;
		if( nVictims_ > 0 )
			// this ATTEMPTS to remove the actor from the victim list; however, if it is the owner it may
			// not actually BE one of the victims.
			finishedWithVictim( _STATIC_CAST( MachActor*, pSubject), MachLogExpandingBlast::SUPPRESS_DETACH );

		if( pSubject == (W4dSubject*)pOwner() )
		{
			ownerDestroyed();
		}
		if( pConstruction_ and pSubject == (W4dSubject*)pConstruction_ )
		{
			// this blast was inside a building which has itself now been destroyed. So the blast should terminate.
			finished( true );

			// need to switch the physical explosion to the hidden root
			physObject().attachTo( &MachLogPlanet::instance().hiddenRoot() );

			pConstruction_ = NULL;	// NULLify dangling pointer

		}
	}
	break;
	case W4dSubject::CLIENT_SPECIFIC:
		switch( clientData )
		{
			case MachLog::TELEPORTED_OUT_OF_WORLD:
			{
			 	remainAsObserver = false;
				finishedWithVictim( _STATIC_CAST( MachActor*, pSubject), MachLogExpandingBlast::SUPPRESS_DETACH );
			}
			break;
		}
	break;
	}


	TEST_INVARIANT;

	return remainAsObserver;
}

MachLogConstruction* MachLogExpandingBlast::pConstruction()
{
	CB_MachLogExpandingBlast_DEPIMPL();

	return pConstruction_;
}



/* End EXPANDBL.CPP *************************************************/
