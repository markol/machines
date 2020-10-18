/*
 * C O N S T R O N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/constron.hpp"
#include "machlog/internal/constroi.hpp"

#include "machlog/buildpnt.hpp"
#include "ctl/pvector.hpp"

#include "stdlib/string.hpp"

#include "mathex/circle2d.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/cvexpgon.hpp"

//#include "mathex/line3d.hpp"
#include "phys/cspace2.hpp"
#include "phys/motchunk.hpp"

#include "world4d/domain.hpp"
#include "world4d/manager.hpp"
#include "sim/manager.hpp"
#include "machphys/constron.hpp"
#include "machphys/marker.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/weputil.hpp"

#include "machlog/race.hpp"
#include "machlog/races.hpp"
////#include "machlog/consdata.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/presspad.hpp"
#include "machlog/camera.hpp"
#include "machlog/entrance.hpp"
#include "machlog/dyingent.hpp"
#include "machlog/garrison.hpp"
#include "machlog/machine.hpp"
#include "machlog/missilem.hpp"
#include "machlog/spacial.hpp"
#include "machlog/strategy.hpp"
#include "machlog/produnit.hpp"
#include "machlog/cntrl_ai.hpp"
#include "machlog/network.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/actmaker.hpp"
#include "machlog/construc.hpp"	//needed by persistence mechanism
#include "machlog/vmman.hpp"
#include "machlog/oplabour.hpp"

#include <algorithm>

#ifndef _INLINE
    #include "machlog/constron.ipp"
#endif

//Forward declarations
class W4dDomain;

//////////////////////////////////////////////////////////////////////////////////////////

PER_DEFINE_PERSISTENT_ABSTRACT( MachLogConstruction );
#define CB_MachLogConstruction_DEPIMPL()	\
	PRE( pImpl_ );	\
    CB_DEPIMPL( MachLogConstruction::PolygonId , obstaclePolygonId_ ); \
    CB_DEPIMPL( MachPhys::BuildingMaterialUnits, constructedUnits_ ); \
    CB_DEPIMPL( PhysAbsoluteTime, timeConstructedUnitsUpdated_ ); \
    CB_DEPIMPL( MachPhys::BuildingMaterialUnits, visualisedUnitsOnUpdate_ ); \
    CB_DEPIMPL( MachPhys::BuildingMaterialUnits, visualisedUnits_ ); \
    CB_DEPIMPL( PhysConfigSpace2d*, pConfigSpace_ ); \
    CB_DEPIMPL( MachLogConstruction::Entrances,	entrances_ ); \
	CB_DEPIMPL( MachPhysMarker*, pMarker_ ); \
	CB_DEPIMPL( MachLogConstruction::BuildPoints, buildPoints_ ); \
	CB_DEPIMPL( MachLogConstruction::Constructors, constructors_ ); \
	CB_DEPIMPL( MachLogConstruction::Machines, machines_ ); \
	CB_DEPIMPL( bool, needRebuild_ ); \
	CB_DEPIMPL( int, nReservations_ ); \
	CB_DEPIMPL( int, constructionId_ ); \
	CB_DEPIMPL( int, originalRaceConstructionId_ ); \
	CB_DEPIMPL( bool, isComplete_ ); \
	CB_DEPIMPL( MachLogConstruction::Times, notRecentlyUsedPadTimes_ ); \
	CB_DEPIMPL( bool, removedDoorPoint_ ); \
   	CB_DEPIMPL( PhysAbsoluteTime, lastHitTime_ );


MachLogConstruction::MachLogConstruction
(
    MachLogRace* pRace, MachPhysConstruction* pConstruction, MachLog::ObjectType ot,
    const MachPhysConstructionData& /* data */
)
:   MachActor( pRace, pConstruction, ot ),
	pImpl_( _NEW( MachLogConstructionImpl ) )
{
	CB_MachLogConstruction_DEPIMPL();
    PRE( pRace != NULL );
    PRE( pConstruction != NULL );

	//Set the model to not been built yet...like so:
    pConstruction->percentageComplete( 0.1 );
	MachLogRaces::instance().nConstructions( pRace->race() )++;
	initialiseFromConstructionData( pConstruction );
	hp( 1 );

    TEST_INVARIANT;
}


MachLogConstruction::MachLogConstruction
(
    MachLogRace* pRace, MachPhysConstruction* pConstruction, MachLog::ObjectType ot, UtlId withId,
    const MachPhysConstructionData& /* data */
)
:   MachActor( pRace, pConstruction, ot, withId ),
	pImpl_( _NEW( MachLogConstructionImpl ) )
{
	CB_MachLogConstruction_DEPIMPL();
    PRE( pRace != NULL );
    PRE( pConstruction != NULL );

	//Set the model to not been built yet...like so:
    pConstruction->percentageComplete( 0.1 );

	MachLogRaces::instance().nConstructions( pRace->race() )++;

	initialiseFromConstructionData( pConstruction );
	//Set the model to not been built yet...like so:
    pConstruction->percentageComplete( 0.1 );
	hp( 1 );

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////
MachLogConstruction::~MachLogConstruction()
{
	CB_MachLogConstruction_DEPIMPL();
    //remove any pressure pad entries for this construction's entrances
    if( entrances_.size() != 0 )
        MachLogPlanet::instance().pressurePads().remove( this );

	// delete buildpoints around construction

	while( buildPoints_.size() > 0 )
	{
		_DELETE( buildPoints_.back() );
		buildPoints_.pop_back();
	}

	//if we have an interior then remove the interior config space.
	//The phy library will remove domains, polygons, etc as necessary.
	//HAL_STREAM(" removing entrances and pconfigspace\n" );
	while( entrances_.size() > 0 )
	{
		_DELETE( entrances_.front() );
		entrances_.erase( entrances_.begin() );
	}
	if( physConstruction().hasInterior() )
		_DELETE( pConfigSpace_ );

	MachLogRaces::instance().nConstructions( race() )--;
	_DELETE( pImpl_ );
    TEST_INVARIANT;

}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogConstruction::initialiseFromConstructionData( MachPhysConstruction* pConstruction )
{
	CB_MachLogConstruction_DEPIMPL();
    //get the building global boundary
    MexAlignedBox2d boundary = globalBoundary();

    //Get the list of domains which the footprint of the construction overlaps
    MachLogPlanetDomains::Domains domains;
    domains.reserve( 4 );
    MachLogPlanetDomains::intersecting( boundary, &domains );

    //Set up intersect relations with each domain except the nominal containing domain
    W4dDomain* pMyDomain = pConstruction->containingDomain();

    MexPoint3d loc3d;
    pConstruction->globalTransform().position( &loc3d );

    for( MachLogPlanetDomains::Domains::iterator it = domains.begin();
         it != domains.end(); ++it )
    {
        W4dDomain* pDomain = *it;
        if( pDomain != pMyDomain )
            pConstruction->intersects( pDomain, true );
    }

    //Add the obstacle to the config space
    addObstaclePolygon( boundary );

    if( pConstruction->hasInterior() )
        setupInterior( pConstruction->constructionData() );

    //Don't do this. The entity will be deleted because it is a child
    //_DELETE( pMarker_ );

	constructors_.erase ( constructors_.begin(), constructors_.end() );


	// lastly, build up the vector of notRecentlyUsedPadTimes_, with an entry
	// for every pad. Set the value of each to be the current time.

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	// farcical separation of pickuppoints and putdownpoints (??which way to the funny farm??)
	// necessitates this hardwired value. It's a kludge, no doubt about it.
	for( size_t point = 0; point < 6; ++point )
	{
		notRecentlyUsedPadTimes_.push_back(	timeNow );
	}


	// create the array of buildpoints around the building

	const unsigned CLEARENCE = 0.0;
	MexAlignedBox2d clearBoundary( globalBoundary().minCorner(), globalBoundary().maxCorner(), CLEARENCE );
	MexPoint2d gMinC( clearBoundary.minCorner() );
	MexPoint2d gMaxC( clearBoundary.maxCorner() );

	MATHEX_SCALAR xLength = gMaxC.x() - gMinC.x();
	MATHEX_SCALAR yLength = gMaxC.y() - gMinC.y();

	int xIntervals = ( xLength / 14 ) + 1;
	int yIntervals = ( yLength / 14 ) + 1;

	MATHEX_SCALAR xIntervalLength = xLength / (float)xIntervals;
	MATHEX_SCALAR yIntervalLength = yLength / (float)yIntervals;

	for( int xIndex = 0; xIndex <= xIntervals; ++xIndex )
		for( int yIndex = 0; yIndex <= yIntervals; ++yIndex )
			if( not( xIndex > 0 and xIndex < xIntervals and yIndex > 0 and yIndex < yIntervals ) )
			{
				MexPoint2d candidatePoint = MexPoint2d( gMinC.x() + ( xIndex * xIntervalLength ), gMinC.y() + ( yIndex * yIntervalLength ) );

				// this is a valid construction point position. Let's build it.
				MachLogBuildPoint* buildPoint = _NEW( MachLogBuildPoint( candidatePoint ) );
				buildPoints_.push_back( buildPoint );
			}
}


void MachLogConstruction::removeDoorPoint()
{
	// gets rid of the buildpoint nearest the entrance in the case of constructions
	// that HAVE entrances........this prevents constructors from hanging around blocking the doors when they've
	// finished building.

	// Note that this cannot be done cleanly in the constructor or initialiseFromConstructionData as it uses
	// virtual methods which would be relying on objects not fully constructed

	CB_MachLogConstruction_DEPIMPL();

	ASSERT( not removedDoorPoint_, "Trying to remove door point when method has already been called previously." );

	if( nEntrances() > 0 )
	{
		MATHEX_SCALAR sqrClosestDistanceFound = 99999999;

		MachLogBuildPoint* pClosestPoint = NULL;

		bool found = false;

		for( BuildPoints::iterator i = buildPoints_.begin(); i != buildPoints_.end(); ++i )
		{
			MachLogBuildPoint* pCandidateBuildPoint = (*i);
			MATHEX_SCALAR sqrDistanceToEntrancePoint = pCandidateBuildPoint->position().sqrEuclidianDistance( entranceExternalPoint( 0 ) );

			if( sqrDistanceToEntrancePoint <= sqrClosestDistanceFound )
			{
				if( sqrDistanceToEntrancePoint < sqrClosestDistanceFound )
				{
					sqrClosestDistanceFound = sqrDistanceToEntrancePoint;
					pClosestPoint = pCandidateBuildPoint;
					found = true;
				}
				else
				{
					// the distance must be EQUAL to the old closest distance. What does this mean?
					// I'll tell you. It means we have 2 "closest (so far) points" at identical distance
					// from the external entrance point. This being the case, they must be flanking either side
					// of the doorway, NOT in front of it. So don't treat either as being a door-blocking point.
					// Discount the previously-found point as a candidate for elimination.
					found = false;
				}

			}
		}

		if( found )
		{
			// chop out the point closest to the external entrance point.

			ASSERT( pClosestPoint, "Unexpected NULL for pClosestPoint." );

			BuildPoints::iterator iClosest = find( buildPoints_.begin(), buildPoints_.end(), pClosestPoint);
			ASSERT( iClosest != buildPoints_.end(), "Couldn't find pClosestPoint in buildPoints_ collection!" );
			buildPoints_.erase( iClosest );

			_DELETE( pClosestPoint );
		}
	}

	removedDoorPoint_ = true;
}

bool MachLogConstruction::bestAvailableBuildPoint( const MachLogConstructor& enquirer, MachLogBuildPoint** ppBestBuildPoint )
{
	CB_MachLogConstruction_DEPIMPL();

	if( not removedDoorPoint_ )
		removeDoorPoint();

	MexPoint2d enquirersPosition = enquirer.position();

	MATHEX_SCALAR sqrClosestFound = 99999999;
	MATHEX_SCALAR sqrCheckDistance;
	MachLogBuildPoint* pBestSoFar = NULL;

	bool found = false;

	for( BuildPoints::iterator i = buildPoints_.begin(); i != buildPoints_.end(); ++i )
	{
		MachLogBuildPoint& candidateBuildPoint = (**i);

		bool reservationsPreventMeFromTaking = false;
		if( candidateBuildPoint.isReserved() )
			reservationsPreventMeFromTaking = true;
		else if( enquirer.subType() == MachPhys::BEHEMOTH
				 and buildPoints_.size() > 4
				 and behemothsRestrict( candidateBuildPoint ) )
		{
			// not reserved, but if we're a BEHEMOTH, we cannot take a buildpoint which has another
			// behemoth reserving either or both of the two nearest buildpoints to this one, except in cases
			// of having 4 buildpoints, where we're always nicely-spaced anyway
			reservationsPreventMeFromTaking = true;
		}


		if( not reservationsPreventMeFromTaking )
		{
			MexPoint2d buildPointLocation =  candidateBuildPoint.position();
			PhysConfigSpace2d& cs = MachLogPlanet::instance().configSpace();
			MexPoint2d dummy;

			MexTransform3d constructionPointAsTransform = MachLogSpacialManipulation::sourceToDestination3dTransform( position(), buildPointLocation, MachLogSpacialManipulation::DESTINATION_END_IS_CENTRE );

			MATHEX_SCALAR radius = enquirer.highClearence() * 1.8;
			bool actorCanFitThere = MachLogSpacialManipulation::getNearestFreeSpacePoint( constructionPointAsTransform, radius , enquirer.highClearence(), &dummy, enquirer.highClearence() * MachLogLabourOperation::proximityLeewayMultiplier() );

			if( actorCanFitThere )
			{
				sqrCheckDistance = enquirersPosition.sqrEuclidianDistance( buildPointLocation );
				if( sqrCheckDistance < sqrClosestFound )
				{
					sqrClosestFound = sqrCheckDistance;
					pBestSoFar = &candidateBuildPoint;
					found = true;
				}
			}
		}
	}

	*ppBestBuildPoint = pBestSoFar;

	return found;
}

bool MachLogConstruction::behemothsRestrict( const MachLogBuildPoint& candidatePoint )
{
	CB_MachLogConstruction_DEPIMPL();

	bool foundRestriction = false;

	MATHEX_SCALAR sqrDistanceToClosestPoint = 9999999;
	MATHEX_SCALAR sqrDistanceToSecondClosestPoint = 9999999;
	MachLogBuildPoint* pBest = NULL;
	MachLogBuildPoint* pSecondBest = NULL;

	for( BuildPoints::iterator i = buildPoints_.begin(); i != buildPoints_.end(); ++i )
	{
		MachLogBuildPoint* pTestPoint = (*i);
		if( pTestPoint != &candidatePoint )
		{
			MATHEX_SCALAR sqrDistanceToTestPoint = candidatePoint.position().sqrEuclidianDistance( pTestPoint->position() );

			if( pBest == NULL )
			{
				// no problem - simply make this the new best
				pBest = pTestPoint;
				sqrDistanceToClosestPoint = sqrDistanceToTestPoint;
			}
			else if( sqrDistanceToTestPoint < sqrDistanceToClosestPoint )
			{
				// it's better than our "best so far"......make the old best the new second best, and replace it with
				// this new point
				pSecondBest = pBest;
				sqrDistanceToSecondClosestPoint = sqrDistanceToClosestPoint;
				pBest = pTestPoint;
				sqrDistanceToClosestPoint = sqrDistanceToTestPoint;
			}
			else if( pSecondBest == NULL )
			{
				// no problem - simply make this the new second best
				pSecondBest = pTestPoint;
				sqrDistanceToSecondClosestPoint = sqrDistanceToTestPoint;
			}
			else if( sqrDistanceToTestPoint < sqrDistanceToSecondClosestPoint )
			{
				// it's better than our "second best so far"......replace that with this new point
				pSecondBest = pTestPoint;
				sqrDistanceToSecondClosestPoint = sqrDistanceToTestPoint;
			}
		}
	}

	ASSERT( pBest, "Unexpected NULL for pBest!" );
	ASSERT( pSecondBest, "Unexpected NULL for pSecondBest!" );

	if(    ( pBest->isReserved() and pBest->subTypeOfReserver() == MachPhys::BEHEMOTH )
		or ( pSecondBest->isReserved() and pSecondBest->subTypeOfReserver() == MachPhys::BEHEMOTH ) )
		foundRestriction = true;

	return foundRestriction;
}


void MachLogConstruction::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

void MachLogConstruction::usedPickupPoint( size_t whichPoint )
{
	CB_MachLogConstruction_DEPIMPL();

	PRE( whichPoint < std::max( nPickUpPoints(), nPutDownPoints() ) );
	ASSERT( whichPoint < 6, "If you have a building that genuinely has >6 pickup or putdown points, you'll need to change the hard-wired hack in MachLogConstruction::initialiseFromConstructionData. Sorry!" );

	notRecentlyUsedPadTimes_[ whichPoint ] = SimManager::instance().currentTime() + activePadInterval();
}

bool MachLogConstruction::pickupPointUsedRecently( size_t whichPoint )
{
	CB_MachLogConstruction_DEPIMPL();

	PRE( whichPoint < std::max( nPickUpPoints(), nPutDownPoints() ) );
	ASSERT( whichPoint < 6, "If you have a building that genuinely has >6 pickup or putdown points, you'll need to change the hard-wired hack in MachLogConstruction::initialiseFromConstructionData. Sorry!" );

	return notRecentlyUsedPadTimes_[ whichPoint ] > SimManager::instance().currentTime();
}


//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachLogConstruction& t )
{

    o << "MachLogConstruction " << (void*)&t << " start" << std::endl;
    o << "MachLogConstruction " << (void*)&t << " end" << std::endl;

    return o;
}

//////////////////////////////////////////////////////////////////////////////////////////

// static
PhysRelativeTime MachLogConstruction::activePadInterval()
{
	static PhysRelativeTime interval = 3.5;
	return interval;
}


//virtual
void MachLogConstruction::doOutputOperator( ostream& o ) const
{
	CB_MachLogConstruction_DEPIMPL();
	o << *this;
	o << "Machines contained within:" << machines().size() << std::endl;
	o << " isComplete " << isComplete() << " constr units " << constructedUnits_ << " constructionData().cost() " << constructionData().cost() << std::endl;
//	for( Machines::const_iterator i = machines().begin(); i != machines().end(); ++i )
//		o << (**i) << std::endl;
	o << "Construction data (virtual call):\n" << constructionData() << std::endl;
	if( needRebuild() )
		o << "Scheduled for rebuild." << std::endl;
	else
		o << "Not scheduled for rebuild." << std::endl;

	if( nEntrances() > 0 and entrance( 0 ).locked() )
		o << "Entrance LOCKED" << std::endl;

	MachActor::doOutputOperator( o );
}

//////////////////////////////////////////////////////////////////////////////////////////

//virtual
PhysRelativeTime MachLogConstruction::beDestroyed()
{
	CB_MachLogConstruction_DEPIMPL();
	MachPhysConstruction * pConstruction = &physConstruction();
	PhysRelativeTime result = pConstruction->destroy();

    //Set up dying entity event to ensure physical model exists long enough to do its animation
    preservePhysicalModel( result );

    //Add the debris model
    dropDebris( SimManager::instance().currentTime() );

	if( needRebuild() )
	{
		MachLogProductionUnit* prod = _NEW( MachLogProductionUnit( objectType(), subType(), level(), 0, 0 ) );
		prod->globalTransform( globalTransform() );
		prod->constructionId( constructionId() );
		// ensure that the construction would be rebuilt once again if destroyed after this rebuild
		prod->needRebuild( true );
		MachLogRaces::instance().AIController( logOriginalRace().race() ).addConstructionProductionUnit( prod );
	}
	return result;
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachLogConstruction::doStartExplodingAnimation()
{
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachLogConstruction::doEndExplodingAnimation()
{
}
//////////////////////////////////////////////////////////////////////////////////////////

MexPoint2d MachLogConstruction::globalPoint2d( const MexPoint2d& localPoint ) const
{
    //Get local coordinates and convert into global
    MexPoint3d globalLoc( localPoint.x(), localPoint.y(), 0 );
    physObject().globalTransform().transform( &globalLoc );

    return MexPoint2d( globalLoc.x(), globalLoc.y() );
}
//////////////////////////////////////////////////////////////////////////////////////////

MexAlignedBox2d MachLogConstruction::globalBoundary() const
{
    //Get the local boundary and construct each corner point in global coords
    const MexAlignedBox2d& localBox = physConstruction().constructionData().localBoundary();
//    const MexAlignedBox2d& localBox = constructionData().localBoundary();
    MexPoint2d aCorners[4];
    aCorners[0] = globalPoint2d( MexPoint2d( localBox.minCorner().x(), localBox.minCorner().y() ) );
    aCorners[1] = globalPoint2d( MexPoint2d( localBox.minCorner().x(), localBox.maxCorner().y() ) );
    aCorners[2] = globalPoint2d( MexPoint2d( localBox.maxCorner().x(), localBox.minCorner().y() ) );
    aCorners[3] = globalPoint2d( MexPoint2d( localBox.maxCorner().x(), localBox.maxCorner().y() ) );

    //Compute the bounding box
    MATHEX_SCALAR xMin = aCorners[0].x();
    MATHEX_SCALAR xMax = xMin;
    MATHEX_SCALAR yMin = aCorners[0].y();
    MATHEX_SCALAR yMax = yMin;

    for( size_t i = 1; i != 4 ; ++i )
    {
        MATHEX_SCALAR x = aCorners[i].x();
        if( x < xMin ) xMin = x;
        else if( x > xMax ) xMax = x;

        MATHEX_SCALAR y = aCorners[i].y();
        if( y < yMin ) yMin = y;
        else if( y > yMax ) yMax = y;
    }

    return MexAlignedBox2d( xMin, yMin, xMax, yMax );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogConstruction::addObstaclePolygon( const MexAlignedBox2d& boundary )
{
	CB_MachLogConstruction_DEPIMPL();
    //Construct a polygon from the boundary
    MexConvexPolygon2d* pPolygon =
        _NEW( MexConvexPolygon2d( boundary ) );
    std::auto_ptr< MexPolygon2d > polygonAPtr( pPolygon );

	//Get the boundary of the construction, so we know how high to make the obstacle polygon
    const MexAlignedBox3d& constructionBoundingVolume = physConstruction().compositeBoundingVolume();
    MATHEX_SCALAR height = physConstruction().globalTransform().position().z()
                           + constructionBoundingVolume.maxCorner().z();



    //Add this to the config space
    PhysConfigSpace2d& configSpace = MachLogPlanet::instance().configSpace();
	obstaclePolygonId_ = configSpace.add( polygonAPtr, height, MachLog::OBSTACLE_NORMAL, PhysConfigSpace2d::PERMANENT );
}
//////////////////////////////////////////////////////////////////////////////////////////

MachPhys::HitPointUnits MachLogConstruction::hitPointValueOfBMUs( MachPhys::BuildingMaterialUnits BMUs )
{
	MATHEX_SCALAR proportionOfTotalConstruction = (double)BMUs / (double)constructionData().cost();

	return proportionOfTotalConstruction * constructionData().hitPoints();
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachLogConstruction::advanceConstructionState( MachPhys::BuildingMaterialUnits addUnits )
{
	CB_MachLogConstruction_DEPIMPL();
	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() and network.remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
	{
		network.messageBroker().sendAdvanceConstructionStateMessage( id(), addUnits );
	}
 // commented out for D6
 //    PRE( addUnits + constructedUnits() <= constructionData().cost() );
 	//HAL_STREAM("(" << id() << ") MLConstruction::advanceConState " << addUnits << std::endl );
 	if( addUnits + constructedUnits() > constructionData().cost() )
		addUnits = constructionData().cost() - constructedUnits();

	// now add the construction units and the proportional hitpoints.
	// we actually have to be quite clever here, to avoid clipping due to double->int conversions
	//
	// what we do is calculate the amount of damage that must have been taken so far as a discrepancy between
	// expected HP for the proportion of construction completed, and actual HP. Then we work out what WOULD be
	// the new hp given the new construction units added to the construction had no damage ever been taken, and
	// set the new hp to this figure minus the damage taken so far.

	MATHEX_SCALAR proportionOfTotalConstruction = (double)constructedUnits_ / (double)constructionData().cost();

	MachPhys::HitPointUnits expectedHP = proportionOfTotalConstruction * constructionData().hitPoints();
	MachPhys::HitPointUnits damageTaken = ( expectedHP - hp() );

	if( damageTaken == -1)
		damageTaken = 0;		// for initial case when nothing is built but 1 free hp is given.

	// now add the new construction units, recalculate expectedHP with the new proportion, and remove the damage taken
	// so far.

	constructedUnits_ += addUnits;

	proportionOfTotalConstruction = (double)constructedUnits_ / (double)constructionData().cost();

	expectedHP = proportionOfTotalConstruction * constructionData().hitPoints();

	setHPAndArmour( expectedHP - damageTaken, armour() );




    //Get current time
    PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
    timeConstructedUnitsUpdated_ = timeNow;

    //Store the current visualisation state
    visualisedUnitsOnUpdate_ = visualisedUnits_;

    //Ensure the construction is updated soon
    nextUpdateTime( timeNow );

	//Primarily (exclusively?) used for GUI elements giving visual rep. of build percentage
	notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::PERCENTAGE_COMPLETE_CHANGED );

	//We might now be complete.
	if(	constructedUnits_ >= physConstruction().constructionData().cost() and not isComplete_ )
	{
		isComplete_ = true;

		// add me to the race statistics
		MachLogRaces::instance().constructionsBuiltScoreAdjust( race(), *this );

		// force visualisation update NOW.
		updateCompletionVisualisation();

		// bit of a non-OO hack here, but it'll have to do. Special logic is needed for when
		// an enemy builds an ICBM launcher.

		if ( objectIsMissileEmplacement() and subType() == MachPhys::ICBM )
		{
			MachLogRaces::instance().builtNuclearWeapon( race() );

			// ........and whizz the warning round the network if it's a network game.
			if( MachLogNetwork::instance().isNetworkGame() )
					MachLogNetwork::instance().messageBroker().sendWeaponInformationMessage( MachLogMessageBroker::NUKE_ONLINE, race() );
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////

MachPhysConstruction& MachLogConstruction::physConstruction()
{
    //  This looks dodgy but is really OK since we know we have a MachPhysConstruction
	return _STATIC_CAST( MachPhysConstruction&, physObject() );
}
//////////////////////////////////////////////////////////////////////////////////////////

const MachPhysConstruction& MachLogConstruction::physConstruction() const
{
    //  This looks dodgy but is really OK since we know we have a MachPhysConstruction
	return _STATIC_CAST( const MachPhysConstruction&, physObject() );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogConstruction::updateCompletionVisualisation()
{
	CB_MachLogConstruction_DEPIMPL();

	bool result = false;

	if(  visualisedUnits_ != constructedUnits_ )
	{
		physConstruction().percentageComplete( percentageComplete() );

		MachLogNetwork& network = MachLogNetwork::instance();
		if( network.isNetworkGame() )
			if( network.remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
				network.messageBroker().sendUpdateCompletionVisualisationMessage( id(), percentageComplete() );

		visualisedUnits_ = constructedUnits_;
	}
	else
		result = true;

    //Now work out the visualized state of the building overall
    return result;
}

bool MachLogConstruction::updateCompletionVisualisation( double toPercentage )
{
	CB_MachLogConstruction_DEPIMPL();
	if( fabs( toPercentage - 100.0 ) < MexEpsilon::instance() )
	{
		makeComplete();
		return true;
	}
	constructedUnits_ = (double)constructionData().cost() * toPercentage / 100.0;
	return updateCompletionVisualisation();
}


//////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime MachLogConstruction::isEntranceOpen( size_t i, bool doOpen )
{
    PRE_INFO( i );
    PRE_INFO( nLogicalEntrances() );
    PRE( i < nLogicalEntrances() );

	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() )
	{
		network.messageBroker().sendIsEntranceOpenMessage( id(), i, doOpen );
	}

    return physConstruction().isEntranceOpen( i, doOpen );
}
//////////////////////////////////////////////////////////////////////////////////////////

/* //////////////////////////////////////////////////////////////// */

bool MachLogConstruction::isEntranceOpen( size_t i ) const
{
    PRE( i < nLogicalEntrances() );

    return physConstruction().isEntranceOpen( i );
}

/* //////////////////////////////////////////////////////////////// */


int MachLogConstruction::percentageComplete() const
{
	CB_MachLogConstruction_DEPIMPL();
    return (int)( ( (float)constructedUnits_ / (float)physConstruction().constructionData().cost() ) * 100.0 );
}

//////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime MachLogConstruction::update( const PhysRelativeTime& maxCPUTime,
                                              MATHEX_SCALAR clearence )
{
	return MachActor::update( maxCPUTime, clearence );
}

//virtual
const MachPhysObjectData& MachLogConstruction::objectData() const
{
	return constructionData();
}

void MachLogConstruction::setupInterior( const MachPhysConstructionData& data )
{
	CB_MachLogConstruction_DEPIMPL();
    PRE( physConstruction().hasInterior() );

    setupInteriorConfigSpace();

    ASSERT( pConfigSpace_ != NULL, "" );

    //  Set up appropriate MachLogEntrance structures
    const MachPhysConstructionData::Entrances& entrances = data.entrances();

    size_t  entranceNum = 0;

    entrances_.reserve( entrances.size() );

    for( MachPhysConstructionData::Entrances::const_iterator i = entrances.begin();
      i != entrances.end(); ++i, ++entranceNum )
    {
        entrances_.push_back( _NEW( MachLogEntrance( *i, entranceNum, this, pConfigSpace_ ) ) );
    }
}

void MachLogConstruction::enterConstruction( MachLogCamera* pCamera )
{
	CB_MachLogConstruction_DEPIMPL();
    PRE( physConstruction().hasInterior() );

    ASSERT( pConfigSpace_ != NULL, "" );

    pCamera->pushParent( physConstruction().pInteriorDomain(), pConfigSpace_ );
}

void MachLogConstruction::leaveConstruction( MachLogCamera* pCamera )
{
    PRE( physConstruction().hasInterior() );

    pCamera->popParent();
}

void    MachLogConstruction::setupInteriorConfigSpace()
{
        CB_MachLogConstruction_DEPIMPL();

    PRE( physConstruction().hasInterior() );

    MexPoint2d  minBound = physConstruction().constructionData().interiorBoundary().minCorner();
    MexPoint2d  maxBound = physConstruction().constructionData().interiorBoundary().maxCorner();

    const MexTransform3d&  transform = physConstruction().globalTransform();

    transform.transform( &minBound );
    transform.transform( &maxBound );

    MATHEX_SCALAR  domainClearance = 1.5;

    MexPoint2d  actualMinBound( std::min( minBound.x(), maxBound.x() ), std::min( minBound.y(), maxBound.y() ) );
    MexPoint2d  actualMaxBound( std::max( minBound.x(), maxBound.x() ), std::max( minBound.y(), maxBound.y() ) );

    pConfigSpace_ = _NEW( PhysConfigSpace2d( actualMinBound, actualMaxBound,
                                             PhysConfigSpace2d::SUBTRACTIVE,
                                             domainClearance, MachLogMachine::minLowClearance(),
                                             MachLogMachine::maxLowClearance(),
                                             MachLog::OBSTACLE_ALL ) );

        pConfigSpace_->addDomain( MexAlignedBox2d( actualMinBound, actualMaxBound ) );

    const MachPhysConstructionData::Polygons&  interiorObstacles = physConstruction().constructionData().interiorObstacles();

    for( MachPhysConstructionData::Polygons::const_iterator i = interiorObstacles.begin(); i != interiorObstacles.end(); ++i )
    {

        MexConvexPolygon2d* pPolygon = _NEW( MexConvexPolygon2d( *(*i) ) );
        std::auto_ptr< MexPolygon2d > polygonAPtr( pPolygon );

        const   MATHEX_SCALAR   height = 50.0;
                pConfigSpace_->add( polygonAPtr, height, MachLog::OBSTACLE_NORMAL, PhysConfigSpace2d::PERMANENT );
        }
}

MachLogEntrance&    MachLogConstruction::entrance( size_t i ) const
{
	CB_MachLogConstruction_DEPIMPL();
    PRE( i < nLogicalEntrances() );

    return *entrances_[ i ];
}

W4dDomain& MachLogConstruction::interiorDomain() const
{
	return *physConstruction().pInteriorDomain();
}

PhysConfigSpace2d& MachLogConstruction::interiorConfigSpace() const
{
	CB_MachLogConstruction_DEPIMPL();
	return *pConfigSpace_;
}

size_t MachLogConstruction::level() const
{
	return physConstruction().level();
}

size_t MachLogConstruction::level()
{
	return physConstruction().level();
}

//virtual
void MachLogConstruction::doVisualiseSelectionState()
{
	CB_MachLogConstruction_DEPIMPL();
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
		MachPhysConstruction& physObject = physConstruction();
		bool permanent = state == MachLog::SELECTED;
		pMarker_ = _NEW( MachPhysMarker( &physObject, MexTransform3d(),
						physObject.compositeBoundingVolume(), permanent, ( 100 * hp() ) / constructionData().hitPoints() ) );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogConstruction::remove( MachLogMachine* oldMachine )
{
	CB_MachLogConstruction_DEPIMPL();
	bool result = false;
	//HAL_STREAM("(" << id() << "MLAPC::remove MLMachine:: (void*)" << (void*)oldMachine << "\n" );
	//HAL_STREAM(" found a match i->id() " << (*i)->id() << " oldMachine->id() " << oldMachine->id() << std::endl );
	PhysConfigSpace2d& cs = MachLogPlanet::instance().configSpace();
	MexPoint2d dest2d;
	bool noSpace = false;

	//allow machines to deploy up to a cell away (~30m).

	if(  not MachLogSpacialManipulation::getNearestFreeSpacePoint( globalTransform(), 10, 5, &dest2d ) )
	{
		MexPoint3d dest3d( dest2d );
		oldMachine->snapTo( dest3d );//physObject().attachTo( MachLogPlanet::instance().pWorld() );
	 	result = true;
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogConstruction::addConstructor( MachLogConstructor* pConstructor )
{
	CB_MachLogConstruction_DEPIMPL();
	bool duplicateFound = false;
	Constructors::iterator i = find( constructors_.begin(), constructors_.end(), pConstructor );
	if( i != constructors_.end() )
	{
		duplicateFound = true;
	}

	if ( not duplicateFound )
		constructors_.push_back( pConstructor );
}

void MachLogConstruction::removeConstructor( MachLogConstructor* pConstructor )
{
	CB_MachLogConstruction_DEPIMPL();
	HAL_STREAM("(" << id() << ") MLConstruction::removeConstructor currently have " << constructors_.size() << std::endl );
	constructors_.remove( pConstructor );
	if( constructors_.size() == 0 and constructedUnits() == 0 and nReservations_ == 0  and not isDead())
	{
		HAL_STREAM(" no constructors, no reservations and constructedUnits == 0 so killing\n" );
		isDead(true);
		SimManager::instance().add( _NEW( MachLogDyingEntityEvent( physObjectPtr(), obstaclePolygonId(), 0.1, MachLogDyingEntityEvent::NOT_INSIDE_BUILDING, NULL ) ) );
		if( MachLogNetwork::instance().isNetworkGame() )
			MachLogNetwork::instance().messageBroker().sendRemoveConstructionMessage( id() );
	}
	HAL_STREAM("(" << id() << ") MLConstruction::removeConstructor Done currently have " << constructors_.size() << std::endl );
}

void MachLogConstruction::networkRemove()
{
    if( not isDead() )
    {
    	isDead(true);
    	SimManager::instance().add( _NEW( MachLogDyingEntityEvent( physObjectPtr(), obstaclePolygonId(), 0.1, MachLogDyingEntityEvent::NOT_INSIDE_BUILDING, NULL ) ) );
    }
}

void MachLogConstruction::machineEntered( MachLogMachine* pMachine )
{
	CB_MachLogConstruction_DEPIMPL();
	machines_.push_back( pMachine );
	if( subType() == MachPhys::GARRISON )
	{
		// update the healing list pronto.
		asGarrison().healAll();
	}
	notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::CHANGED_N_MACHINES_INSIDE_CONSTRUCTION );
}

void MachLogConstruction::machineExited( MachLogMachine* pMachine )
{
	CB_MachLogConstruction_DEPIMPL();
	machines_.remove( pMachine );
	if( subType() == MachPhys::GARRISON )
	{
		// update the healing list pronto.
		asGarrison().healAll();
	}
	notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::CHANGED_N_MACHINES_INSIDE_CONSTRUCTION );
}

MachLogConstruction::Machines& MachLogConstruction::machines()
{
	CB_MachLogConstruction_DEPIMPL();
	return machines_;
}

const MachLogConstruction::Machines& MachLogConstruction::machines() const
{
	CB_MachLogConstruction_DEPIMPL();
	return machines_;
}

void MachLogConstruction::needRebuild( bool newNeedRebuild )
{
	CB_MachLogConstruction_DEPIMPL();
	needRebuild_ = newNeedRebuild;
}

bool MachLogConstruction::needRebuild() const
{
	CB_MachLogConstruction_DEPIMPL();
	return needRebuild_;
}

void MachLogConstruction::addReservation()
{
	CB_MachLogConstruction_DEPIMPL();
	HAL_STREAM("(" << id() << ") MLConstruction::addReservation now " << nReservations_ << std::endl );
	++nReservations_;

	POST ( nReservations_ > 0 );
	HAL_STREAM("(" << id() << ") MLConstruction::addReservation Done now " << nReservations_ << std::endl );
}

void MachLogConstruction::cancelReservation()
{
	CB_MachLogConstruction_DEPIMPL();
	HAL_STREAM("(" << id() << ") MLConstruction::cancelReservation now " << nReservations_ << std::endl );
	--nReservations_;

    //Somebody had commented out the  "and not isDead()" bit below. This could cause multiple dying entity
    //events to be added for one dying construction, which in turn causes the polygon id to be removed
    //twice, leading to a crash.
	if( constructors_.size() == 0 and constructedUnits() == 0 and nReservations_ == 0 and not isDead() )
	{
		isDead(true);
		SimManager::instance().add( _NEW( MachLogDyingEntityEvent( physObjectPtr(), obstaclePolygonId(), 0.1, MachLogDyingEntityEvent::NOT_INSIDE_BUILDING, NULL ) ) );
		if( MachLogNetwork::instance().isNetworkGame() )
			MachLogNetwork::instance().messageBroker().sendRemoveConstructionMessage( id() );
	}

	POST ( nReservations_ >= 0 );
	HAL_STREAM("(" << id() << ") MLConstruction::cancelReservation Done now " << nReservations_ << std::endl );
}

int MachLogConstruction::constructionId() const
{
	CB_MachLogConstruction_DEPIMPL();
	TEST_INVARIANT;
	return constructionId_;
}

void MachLogConstruction::constructionId( int ID )
{
	CB_MachLogConstruction_DEPIMPL();
	TEST_INVARIANT;
	constructionId_ = ID;
}

int MachLogConstruction::originalRaceConstructionId() const
{
	CB_MachLogConstruction_DEPIMPL();
	TEST_INVARIANT;
	return originalRaceConstructionId_;
}

void MachLogConstruction::originalRaceConstructionId( int ID )
{
	CB_MachLogConstruction_DEPIMPL();
	TEST_INVARIANT;
	originalRaceConstructionId_ = ID;
}


//	virtual
void MachLogConstruction::assignToDifferentRace( MachLogRace& newRace )
{
	CB_MachLogConstruction_DEPIMPL();

	MachLogRaces& races = MachLogRaces::instance();

	// if the race captured from is the original race, store old construction id as original race id
	if( logOriginalRace().race() == race() )
		originalRaceConstructionId( constructionId() );

	// decrement losing race's construction numbers
	races.nConstructions( race() )--;

	// if this is now being REcaptured by the original race, restore the original construction id to
	// prevent any alternative sites being built so long as this building stays in existence and in
	// the race's control
	if( newRace.race() == logOriginalRace().race() )
		constructionId( originalRaceConstructionId() );
	else
		// The construction ID was specific to the race just captured from. It is no longer valid.
		// Set it to a safe -1 value, which cannot clash with anything.
		constructionId( -1 );

	MachActor::assignToDifferentRace( newRace );
	physConstruction().changeRace( newRace.race() );

	// increment gaining race's construction numbers
	races.nConstructions( race() )++;
}

//virtual
void MachLogConstruction::beHit( const int& damage, MachPhys::WeaponType byType,
						MachActor* pByActor, MexLine3d* pByDirection, MachActor::EchoBeHit echo )
{
	if( not isDead() )

	{
		if( byType != MachPhys::N_WEAPON_TYPES and pByDirection != NULL )
			MachPhysWeaponUtility::victimAnimation( byType, SimManager::instance().currentTime(), *pByDirection, &physConstruction() );

		//Set the damage level
	    double hpMax = maximumhp();
	    double hpNow = hp();
	    if( hpNow < hpMax )
	    {
	        double damagePercent = ((hpMax - hpNow) / hpMax) * 100.0;
	        physConstruction().damageLevel( damagePercent );
	    }

		MachActor::beHit( damage, byType, pByActor, pByDirection, echo );

		// give voicemail if appropriate
		if( pByActor and pByActor->race() != race() )
			dealWithHitVoiceMail();
	}
}

// virtual
void MachLogConstruction::beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay, MachActor* pByActor, MachActor::EchoBeHit echo )
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

		//Set the damage level
    	double hpMax = maximumhp();
    	double hpNow = hp();
	    if( hpNow < hpMax )
	    {
	        double damagePercent = ((hpMax - hpNow) / hpMax) * 100.0;
	        physConstruction().damageLevel( damagePercent );
	    }

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
	        	MachLogConstruction& mlc = asConstruction();
                mlc.preservePhysicalModel( physicalTimeDelay );
                mlc.dropDebris( nowTime + physicalTimeDelay );
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
			doVisualiseSelectionState();
			notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::HEALTH_STATUS_CHANGED );
		}

		if( pByActor and pByActor->race() != race() )
			// give voicemail if appropriate
			dealWithHitVoiceMail();

	}
	lastBeHitFrame( W4dManager::instance().frameNumber() );
}

void MachLogConstruction::dealWithHitVoiceMail()
{
	if( not isDead() )
	{
		if( sufficientTimePassedSinceLastHit() )
		{
			switch( objectType() )
			{
				case MachLog::POD:
					if( hpRatio() < 0.10 )
					{
						// give voicemail to warn that damage is dangerously high, and, err, we're evacuating the building (??)
						MachLogVoiceMailManager::instance().postNewMail( VID_POD_POD_CRITICAL_DAMAGE, /*id(),*/ race() );												   							}
					else
					{
						// just report on having been hit
						MachLogVoiceMailManager::instance().postNewMail( VID_POD_POD_ATTACKED, /*id(),*/ race() );
					}
					break;

				case MachLog::MISSILE_EMPLACEMENT:
					if( asMissileEmplacement().subType() != MachPhys::ICBM )
					{
						MachLogVoiceMailManager::instance().postNewMail( VID_POD_TURRET_ATTACKED, race() );
						break;
					}
					// note drop through if we're an ICBM silo.


				default:
					if( hpRatio() < 0.10 and not isEmpty() )
					{
						// give voicemail to warn that damage is dangerously high, and, err, we're evacuating the building (??)
						MachLogVoiceMailManager::instance().postNewMail( VID_POD_BUILDING_CRITICAL, /*id(),*/ race() );
					}
					else
					{
						// just report on having been hit
						MachLogVoiceMailManager::instance().postNewMail( VID_POD_CONSTRUCTION_ATTACKED, /*id(),*/ race() );
					}
			}

		}
	}
	else
		// report destruction of building
		switch( objectType() )
		{
			case MachLog::BEACON:
				switch( level() )
				{
					case 1:
						MachLogVoiceMailManager::instance().postNewMail( VID_POD_BEACON_DESTROYED, /*id(),*/ race() );
						break;
					case 3:
						MachLogVoiceMailManager::instance().postNewMail( VID_POD_UPLINK_DESTROYED, /*id(),*/ race() );
						break;
					DEFAULT_ASSERT_BAD_CASE( level() );
				}
				break;
			case MachLog::POD:
				MachLogVoiceMailManager::instance().postNewMail( VID_POD_POD_DESTROYED, /*id(),*/ race() );
				break;

			case MachLog::MISSILE_EMPLACEMENT:
				if( asMissileEmplacement().subType() != MachPhys::ICBM )
					// only drop through if we're an ICBM silo.
					break;
			default:
				MachLogVoiceMailManager::instance().postNewMail( VID_POD_BUILDING_DESTROYED, /*id(),*/ race() );
		}
}

bool MachLogConstruction::sufficientTimePassedSinceLastHit()
{
	bool result = false;
	CB_DEPIMPL( PhysAbsoluteTime, lastHitTime_ );

	PhysRelativeTime acceptableInterval;
	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	// require less time if our health is now very low
	if( hpRatio() < 0.10 )
	{
		// much more likely to give a voicemail if you're badly-damaged
		acceptableInterval = 5.0;

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
			acceptableInterval = VOICE_MAIL_HIT_INTERVAL;
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


bool MachLogConstruction::hasInterior() const
{
	return physConstruction().hasInterior();
}

void MachLogConstruction::makeComplete( CompleteWithFullHPs setHPsFullStrength )
{
	CB_MachLogConstruction_DEPIMPL();
	// in what circumstances might we NOT want to set the hitpoints to full here? Simple,
	// me old china - when a construction has been completed but may have taken damage along the
	// way whilst being built - it doesn't get these back as a gift just for finally being completed.

	if( setHPsFullStrength == FULL_HP_STRENGTH )
		hp( constructionData().hitPoints() );
    //Get current time
    PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
    timeConstructedUnitsUpdated_ = timeNow;

    //Store the current visualisation state
    visualisedUnitsOnUpdate_ = visualisedUnits_ = constructedUnits_ = constructionData().cost();

    //Ensure physical building is complete
    physConstruction().percentageComplete( 100.0 );
	isComplete_ = true;

	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() and network.remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
		network.messageBroker().sendMakeCompleteConstructionMessage( id() );

    //Ensure the construction is updated soon
    nextUpdateTime( timeNow );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void MachLogConstruction::preservePhysicalModel( const PhysRelativeTime& forTime )
{
	SimManager::instance().add( _NEW( MachLogDyingEntityEvent( physObjectPtr(), obstaclePolygonId(), forTime,
	                                                           MachLogDyingEntityEvent::NOT_INSIDE_BUILDING, NULL ) ) );
}

void MachLogConstruction::dropDebris( const PhysAbsoluteTime& )
{
	if( not MachLogNetwork::instance().isNetworkGame()  or
		MachLogNetwork::instance().remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
    {
		MATHEX_SCALAR completionMultiplier = percentageComplete() / 100.0;
		MATHEX_SCALAR recoveredValue = _STATIC_CAST( MATHEX_SCALAR, constructionData().cost() ) * completionMultiplier / 5.0;
		int quantity = _STATIC_CAST( int, recoveredValue );
		if( quantity > 0 )
	 		MachLogActorMaker::newLogDebris( race(), quantity, position(), globalBoundary() );
	}
}

bool MachLogConstruction::isEnemyCanAttackInside( MachPhys::Race r ) const
{
	CB_MachLogConstruction_DEPIMPL();
	bool found = false;

	for( Machines::const_iterator i = machines_.begin(); not found and i != machines_.end(); ++i )
	{
		MachLogRaces::DispositionToRace disposition = MachLogRaces::instance().dispositionToRace( r, (*i)->race() );
		if ( disposition != MachLogRaces::OUR_RACE and disposition != MachLogRaces::ALLY and (*i)->objectIsCanAttack() )
			found = true;
	}

	return found;
}

bool MachLogConstruction::isEmpty() const
{
	CB_MachLogConstruction_DEPIMPL();
	return machines_.size() == 0;
}



bool MachLogConstruction::containsMachine( MachLogMachine* pMachine ) const
{
	CB_MachLogConstruction_DEPIMPL();
	Machines::const_iterator i = find( machines_.begin(), machines_.end(), pMachine );

	if( i != machines_.end() )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogConstruction::constructionCreated( MachPhysConstruction* pConstruction )
{
	initialiseFromConstructionData( pConstruction );
	advanceConstructionState( 0 );
    updateCompletionVisualisation();
	actorCreated();

	// lay pads on terrain
	pConstruction->layPadsOnTerrain( *MachLogPlanet::instance().surface() );

	//If displayed as a different race, convert the colours etc
	if( displayMapAndIconRace() != race() )
	{
		displayAsRace( displayMapAndIconRace() );
	}
}

void perWrite( PerOstream& ostr, const MachLogConstruction& actor )
{
	const MachActor& base1 = actor;

	ostr << base1;
    ostr << actor.pImpl_;
}

void perRead( PerIstream& istr, MachLogConstruction& actor )
{
	MachActor& base1 = actor;

	istr >> base1;
    istr >> actor.pImpl_;
}

MachLogConstruction::MachLogConstruction( PerConstructor con )
:	MachActor( con )
{
}

bool MachLogConstruction::onOutsideSideOfThreshold( size_t entranceIndex, const MexPoint2d& location ) const
{
    PRE( entranceIndex < nEntrances() );

	const MachPhysConstructionData::EntranceData& entranceData =
      constructionData().entrances()[ entranceIndex ];

    return MexLine2d::side( entranceData.doorLine.end1(), entranceData.doorLine.end2(), location ) == Mathex::CHOCOLATE;
}

const MachPhys::BuildingMaterialUnits& MachLogConstruction::constructedUnits() const
{
	CB_MachLogConstruction_DEPIMPL();
    return constructedUnits_;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogConstruction::isComplete() const
{
	CB_MachLogConstruction_DEPIMPL();

    return isComplete_;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysConfigSpace2d::PolygonId MachLogConstruction::obstaclePolygonId() const
{
	CB_MachLogConstruction_DEPIMPL();
    return obstaclePolygonId_;
}


size_t MachLogConstruction::nEntrances() const
{
    return constructionData().entrances().size();
}
//////////////////////////////////////////////////////////////////////////////////////////

MexPoint2d MachLogConstruction::entranceExternalPoint( size_t i ) const
{
    PRE( i < nEntrances() );

//    return globalPoint2d( constructionData().entrances()[ i ].externalPoint );
    return constructionData().entrances()[ i ].externalPoint;
}
//////////////////////////////////////////////////////////////////////////////////////////

MexPoint2d MachLogConstruction::entranceInternalPoint( size_t i ) const
{
    PRE( i < nEntrances() );

//    return globalPoint2d( constructionData().entrances()[ i ].internalPoint );
    return constructionData().entrances()[ i ].internalPoint;
}
//////////////////////////////////////////////////////////////////////////////////////////

size_t MachLogConstruction::nPickUpPoints() const
{
    return constructionData().pickUpPoints().size();
}
//////////////////////////////////////////////////////////////////////////////////////////

size_t MachLogConstruction::nPutDownPoints() const
{
    return constructionData().putDownPoints().size();
}
//////////////////////////////////////////////////////////////////////////////////////////

MexPoint2d MachLogConstruction::pickUpPoint( size_t i ) const
{
    PRE( i < nPickUpPoints() );

//    return globalPoint2d( constructionData().pickUpPoints()[ i ] );
    return constructionData().pickUpPoints()[ i ];
}
//////////////////////////////////////////////////////////////////////////////////////////

MexPoint2d MachLogConstruction::putDownPoint( size_t i ) const
{
    PRE( i < nPutDownPoints() );

//    return globalPoint2d( constructionData().putDownPoints()[ i ] );
    return constructionData().putDownPoints()[ i ];
}

void MachLogConstruction::addRepairPoints( MachPhys::HitPointUnits hpsAdded )
{
	addHitPoints( hpsAdded );
	double hpMax = maximumhp();
    double hpNow = hp();
	double damagePercent = ( ( hpMax - hpNow ) / hpMax ) * 100.0;

	physConstruction().damageLevel( damagePercent );

	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() and network.remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
	{
		network.messageBroker().sendAddRepairPointsMessage( id(), hpsAdded );
	}
}

size_t MachLogConstruction::nConstructors() const
{
	CB_MachLogConstruction_DEPIMPL();
	return constructors_.size();
}

bool MachLogConstruction::isEnemyMachineInside( MachPhys::Race r ) const
{
	CB_MachLogConstruction_DEPIMPL();
	bool found = false;

	for( Machines::const_iterator i = machines_.begin(); not found and i != machines_.end(); ++i )
	{
		MachLogRaces::DispositionToRace disposition = MachLogRaces::instance().dispositionToRace( r, (*i)->race() );
		if ( disposition != MachLogRaces::OUR_RACE and disposition != MachLogRaces::ALLY )
			found = true;
	}

	return found;
}

size_t MachLogConstruction::nLogicalEntrances() const
{
	CB_MachLogConstruction_DEPIMPL();

    return entrances_.size();
}

void MachLogConstruction::displayAsRace( MachPhys::Race displayAsRace )
{
	//physConstruction().convertMaterials( physConstruction().constructionData(), displayAsRace );
	physConstruction().changeColour( displayAsRace );
	displayMapAndIconRace( displayAsRace );
}

// static
PhysAbsoluteTime& MachLogConstruction::nextGlobalHitVoiceMailTime()
{
	static PhysAbsoluteTime nextGlobalHitVoiceMailTime_ = 0.0;
	return nextGlobalHitVoiceMailTime_;
}

// static
MexPoint2d& MachLogConstruction::lastGlobalHitVoiceMailPosition()
{
	static MexPoint2d lastGlobalHitVoiceMailPosition_( 0, 0 );
	return lastGlobalHitVoiceMailPosition_;
}

//static
void MachLogConstruction::resetNextGlobalHitVoiceMailTime()
{
	// note small 0.5 seconds margin of safety
	nextGlobalHitVoiceMailTime() = SimManager::instance().currentTime() + VOICE_MAIL_HIT_INTERVAL - 0.5;
}


// Forced recompile 16/2/99 CPS
/* //////////////////////////////////////////////////////////////// */

PhysRelativeTime MachLogConstruction::isEntranceOpenNoEcho( size_t i, bool doOpen )
{
    PRE_INFO( i );
    PRE_INFO( nLogicalEntrances() );
    PRE( i < nLogicalEntrances() );

    return physConstruction().isEntranceOpen( i, doOpen );
}

/* End CONSTRON.CPP *************************************************/
