/*
 * L I N E P R O J . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "stdlib/string.hpp"
#include "ctl/pvector.hpp"
#include "ctl/list.hpp"
#include "utility/bndidgen.hpp"
#include "mathex/line3d.hpp"
#include "mathex/line2d.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/abox3d.hpp"
#include "phys/rampacce.hpp"
#include "phys/motchunk.hpp"
#include "world4d/domain.hpp"
#include "world4d/entity.hpp"
#include "world4d/manager.hpp"
#include "sim/manager.hpp"
#include "machphys/lineproj.hpp"
#include "machphys/wepdata.hpp"

#include "machlog/lineproj.hpp"
#include "machlog/race.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/dyingent.hpp"
#include "machlog/actor.hpp"
#include "machlog/races.hpp"
#include "machlog/spacial.hpp"
#include "machlog/constron.hpp"
#include "machlog/network.hpp"
#include "machlog/artefact.hpp"
#include "machlog/planet.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"

PER_DEFINE_PERSISTENT( MachLogLinearProjectile );

MachLogLinearProjectile::MachLogLinearProjectile(
    MachLogRace* pRace,
    MachPhysLinearProjectile* pPhysProjectile,
    MachActor* pOwner,
    const MachPhysWeaponData& weaponData,
	DetectIntersections detectIntersections )
: SimProjectile( pRace, pPhysProjectile ) ,
	speed_( weaponData.projectileSpeed() ),
	maxRange_( weaponData.range() ),
	pOwner_( pOwner ),
	strikeType_( MachPhys::IN_AIR ),
	damage_( weaponData.damagePoints() ),
	currentlyAttached_( false )

{
	HAL_STREAM("MachLogLinearProjectile::MachLogLinearProjectile" );
	HAL_INDENT( 2 );
	MATHEX_SCALAR lineprojRange = maxRange_;
	HAL_STREAM( "range " << lineprojRange << std::endl );

    //Set the id
	if( MachLogNetwork::instance().isNetworkGame() )
	{
		id( MachLogRaces::idGenerator().nextId( pRace->race() * 250 + 1, pRace->race() * 250 + 251 ) );
	}
	else
	{
    	id( MachLogRaces::idGenerator().nextId() );
	}
	HAL_STREAM("id (" << id() << ")\n");
	pOwner_->attach( this );
	currentlyAttached_ = true;

	MexPoint3d globalStartPosition( pPhysProjectile->flightPath().end1() );
	MexPoint3d globalEndPosition( pPhysProjectile->flightPath().end2() );
	HAL_STREAM("global start pos from flightPath " << globalStartPosition << std::endl );
	HAL_STREAM("global end pos from flightPath " << globalEndPosition << std::endl );
	HAL_STREAM("euc. start to end " << globalStartPosition.euclidianDistance( globalEndPosition ) << std::endl );
	lineprojRange = globalStartPosition.euclidianDistance( globalEndPosition );

	if( detectIntersections == DO_DETECT_INTERSECTIONS )
	{
		MexVec3 globalDirection( globalStartPosition, globalEndPosition );
		globalDirection.makeUnitVector();
		MexVec3 direction( globalDirection );

	    lineprojRange = rangeToStationaryObject( globalStartPosition, globalEndPosition, lineprojRange );

		globalEndPosition = globalStartPosition;
		direction *= lineprojRange;
		globalEndPosition += direction;

	    lineprojRange = rangeToMovingObject( globalStartPosition, globalEndPosition, lineprojRange );

		globalEndPosition = globalStartPosition;
		direction = globalDirection;
		direction *= lineprojRange;
		globalEndPosition += direction;
	}

    //  Set the global destruction position
    globalDestructionPosition_ = globalEndPosition;


    PhysRelativeTime duration;

	PhysAbsoluteTime now = SimManager::instance().currentTime();
    if( lineprojRange > 0 )
    {
		duration = lineprojRange / speed_;
    }
    else
        duration = 0;

	//is the linear projectile such that we do not want to detect intersections and hence precomputed flight time.
    if( detectIntersections == DO_NOT_DETECT_INTERSECTIONS )
    	duration = physLinearProjectile().flightDuration();

    destructionTime_ = physLinearProjectile().flightStartTime() + duration;
    destroyAt( destructionTime_ );
	MexLine2d line2d( globalStartPosition, globalEndPosition );
	HAL_STREAM("setting up intersecting domains relationships.\n");
	MachLogPlanetDomains::Domains intersectingDomains;
	MachLogPlanetDomains::intersecting( line2d, &intersectingDomains );
	HAL_STREAM("number of intersecting domains is: " << intersectingDomains.size() << std::endl );
	ASSERT( intersectingDomains.size() > 0, "Linear projectile MUST interset with at least one domain\n" );
	for( MachLogPlanetDomains::Domains::iterator i = intersectingDomains.begin(); i != intersectingDomains.end(); ++i )
	{
		if( (*i) != pPhysProjectile->pParent() )
			pPhysProjectile->intersects( (*i), true );
	}
	HAL_INDENT( -2 );
	HAL_STREAM("(" << id() << ") MLLinearProjectile::MLLinearProjectile DONE\n" );

    TEST_INVARIANT;
}

MachLogLinearProjectile::~MachLogLinearProjectile()
{
    MachLogRaces::idGenerator().freeId( id() );
	if( currentlyAttached_ )
		pOwner_->detach( this );
    TEST_INVARIANT;

}

MATHEX_SCALAR   MachLogLinearProjectile::rangeToStationaryObject(
    const MexPoint3d& globalStartPosition,
    const MexPoint3d& globalEndPosition,
    MATHEX_SCALAR range )
{
    PRE( fabs( range - globalStartPosition.euclidianDistance( globalEndPosition ) )
         < MexEpsilon::instance() );

    POST_DATA( MATHEX_SCALAR originalRange = range; );

    MexLine3d       line( globalStartPosition, globalEndPosition );

    //  Find out the maximum possible range of the lineproj - this is the
    //  range at which it will run into a building or the terrain.

    //W4dDomain* pDomain = pCurrentDomain();
	W4dDomain* pDomain = MachLogPlanetDomains::pDomainAt( MexPoint2d( globalStartPosition ) );

    W4dEntity*      pIntersectingEntity;
    MATHEX_SCALAR   distanceToIntersection;

    //  Check for construction or terrain intersection

	ulong checkId = W4dEntity::nextCheckId();
	pOwner_->physObject().checkId( checkId );

    if( pDomain->findNearerEntity(
      line, range, checkId,
      W4dEntity::HIGH, &pIntersectingEntity, &distanceToIntersection,
      W4dDomain::EXCLUDE_NOT_SOLID | W4dDomain::EXCLUDE_NOT_STATIONARY ) )
    {
//         INSPECT_ON( Diag::bobStream(), pIntersectingEntity->globalTransform() );
//
//         if( pIntersectingEntity->hasSubject() )
//         {
//             //Get the subject
//             W4dSubject& subject = pIntersectingEntity->subject();
//
//             //Convert to an actor.
//             //TBD - do this properly using id map etc
//             MachActor* pActor = &(_CONST_CAST( MachActor&, subject ) );
//
//             INSPECT_ON( Diag::bobStream(), *pActor );
//         }
        if( pIntersectingEntity->hasSubject() )
			strikeType_ = MachPhys::ON_OBJECT;
		else
			strikeType_ = MachPhys::ON_TERRAIN;
        range = distanceToIntersection;
    }

    POST( range <= originalRange );

    return range;
}

MATHEX_SCALAR   MachLogLinearProjectile::rangeToMovingObject(
    const MexPoint3d& globalStartPosition,
    const MexPoint3d& globalEndPosition,
    MATHEX_SCALAR range3d )
{
    PRE( fabs( range3d - globalStartPosition.euclidianDistance( globalEndPosition ) )
         < MexEpsilon::instance() );

    //  The configuration space code works entirely in 2D so get the start / end
    //  positions and speed in 2D.
    MexPoint2d  startPosition2d( globalStartPosition );
    MexPoint2d  endPosition2d( globalEndPosition );

    const PhysRelativeTime duration = range3d / speed_;
    MATHEX_SCALAR   speed2d = startPosition2d.euclidianDistance( endPosition2d ) / duration;

    //Set up the start and end times for the flight
    PhysAbsoluteTime startTime = SimManager::instance().currentTime();
    PhysAbsoluteTime endTime = startTime + duration;

    //Construct a ramp acceleration for the projectile to use in making a motion chunk.
    //  The actual value of the acceleration is irrelevent because it will be applied for zero time.
    const MATHEX_SCALAR dummyAcceleration = 1.0;
    const MATHEX_SCALAR accelerationTime = 0.0;

    PhysRampAcceleration    rampAcceleration(
        speed2d,
        dummyAcceleration, //Signed: -ve indicates deceleration
        dummyAcceleration, //Signed: -ve indicates deceleration
        duration,
        accelerationTime,
        accelerationTime );

    //Make a motion chunk for the missile's flight
    const   MATHEX_SCALAR   clearance = 1.0;
    const   PhysRelativeTime    timeOffset = 0.0;

    PhysMotionChunk motionChunk(
        startPosition2d,
        endPosition2d,
        clearance,
        rampAcceleration,
        startTime,
        timeOffset,
        MexDouble::minusInfinity(),
        MexDouble::plusInfinity()
    );

    //See which machines the missile's flight path intersects using the config space.
    //Get the correct config space to use. A machine might be inside a building.
    PhysConfigSpace2d::ChunkIntersections intersections;
    const PhysConfigSpace2d& configSpace =
        (pOwner_->objectIsMachine() ? pOwner_->asMachine().motionSeq().currentConfigSpace()
                                    : MachLogPlanet::instance().configSpace());

    //Exclude the owner's id from the check's, so we don't detect launching position as a collision
    configSpace.findIntersections( motionChunk, pOwner_->id(), &intersections );
    size_t nPossibleCollisions = intersections.size();

    //Check we have some collisions
    if( nPossibleCollisions != 0 )
    {
        MachLogRaces& races = MachLogRaces::instance();
        W4dManager& w4dManager = W4dManager::instance();
        MexLine3d fullPath( globalStartPosition, globalEndPosition, range3d );
        const PhysRelativeTime timeIncrement = 0.25;
        bool haveCollision = false;

        while( not haveCollision  and  nPossibleCollisions != 0 )
        {
            //Find the earliest possible collision
            PhysAbsoluteTime earliestTime = intersections.front().collisionTime_;
            size_t index = 0;
            for( size_t i = 1; i != nPossibleCollisions; ++i )
            {
                PhysAbsoluteTime collisionTime = intersections[i].collisionTime_;
                if( collisionTime < earliestTime )
                {
                    earliestTime = collisionTime;
                    index = i;
                }
            }

            if( earliestTime < startTime )
                earliestTime = startTime;

            //Get the motion chunk and actor data for this collision
	        UtlId actorId = intersections[ index ].collisionObjectId_.asScalar();
	        PhysConfigSpace2d::MotionChunkId& chunkId = intersections[ index ].collisionChunkId_;

			ASSERT_INFO( actorId );
			ASSERT( races.actorExists( actorId ), "Actor with postulated ID no longer exists!" );

            //Get the moving actor with the collision object id
            MachActor& possibleHitActor = races.actor( actorId );
            W4dEntity& hitEntity = possibleHitActor.physObject();

            //See if the chunk represents a moving or stationary machine
            const PhysMotionChunk& motionChunk = configSpace.motionChunk( chunkId );
            bool isStationary = motionChunk.startPoint().sqrEuclidianDistance( motionChunk.endPoint() ) < 0.01;

            //Check to see if and when a collision occurs with this machine, by testing the relevant
            //section of the flight path against the bounding box of the machine at discrete time intervals.
            //If not moving we can check the full length of the path.
            MATHEX_SCALAR segmentStartDistance = range3d * ((earliestTime - startTime) / duration);
            MATHEX_SCALAR segmentLength = ( isStationary ? range3d - segmentStartDistance
                                                         : range3d * (timeIncrement / duration) );
            MexPoint3d segmentStartPoint = fullPath.pointAtDistance( segmentStartDistance );
            MATHEX_SCALAR sqrLastDistanceFromObject =
                globalStartPosition.sqrEuclidianDistance( possibleHitActor.position() );

            for( PhysAbsoluteTime testTime = earliestTime; testTime < endTime; testTime += timeIncrement )
            {
                //get the line segment for this time slot
                MATHEX_SCALAR segmentEndDistance = segmentStartDistance + segmentLength;
                MexPoint3d segmentEndPoint = fullPath.pointAtDistance( segmentEndDistance );
                MexLine3d segment( segmentStartPoint, segmentEndPoint );

                //set the artificial time to the test time unless actor is not moving
                if( not isStationary )
                    w4dManager.artificialTime( testTime );

                //check the line segment for intersection with the machine
                MATHEX_SCALAR entryDistance;
                if( hitEntity.intersectsLine( segment, &entryDistance, W4dEntity::LOW ) )
                {
                    //We have a hit.
                    haveCollision = true;
                	strikeType_ = MachPhys::ON_OBJECT;

                    //Adjust the range to the entry point.
              	    //little tweak to ensure it just breaches bounding box of intersected object
					//without the tweak, get situations where an apparent physical hit actually
					//does no damage, plus you can quite easily damage yourself (esp. with electric
					//charge) - logical detonation point not close enough to target and too near you
                    MATHEX_SCALAR newRange = segmentStartDistance + entryDistance + 2.0;
                    if( newRange < range3d )
                        range3d = newRange;

                    break;
                }

                //Only one test needed for stationary actors
                if( isStationary )
                    break;

                //Get the distance of the segment start point from the object origin, and if increasing,
                //we must be moving away from it now.
                MATHEX_SCALAR sqrDistanceFromObject = segmentStartPoint.sqrEuclidianDistance( possibleHitActor.position() );
                if( sqrDistanceFromObject > sqrLastDistanceFromObject )
                    break;
                else
                    sqrLastDistanceFromObject = sqrDistanceFromObject;

                //Advance the segment data
                segmentStartDistance = segmentEndDistance;
                segmentStartPoint = segmentEndPoint;
            }

            //Scrap the artificial time
            w4dManager.clearArtificialTime();

            //Remove this entry from the collision list
            intersections.erase( intersections.begin() + index );
            --nPossibleCollisions;
        }
    }

    return range3d;
}

PhysRelativeTime
MachLogLinearProjectile::update( const PhysRelativeTime& /* maxCPUTime */, MATHEX_SCALAR )
{
	PhysRelativeTime result = 0.0;

    //  This check is a hack which needs to be here until the logical lineproj
    //  is properly destroyed when the physical lineproj blows up.

    if( not isDead() )
    {
        if( SimManager::instance().currentTime() > destructionTime_ )
        {
			doBeDestroyed();
            isDead( true );
        }
    }

	return result;
}

void MachLogLinearProjectile::destroyAt( const PhysAbsoluteTime& time )
{
	//HAL_STREAM("MLLP::destroyAt " << time << std::endl );
    PhysRelativeTime explosionDuration = physLinearProjectile().beDestroyedAt( time, strikeType_ );


//    CtlConstCountedPtr< W4dEntity > entityPtr = pPhysLinearProjectile_;

    MachLogConstruction*  pConstruction = NULL;
	MachLogDyingEntityEvent::InsideBuilding inside = MachLogDyingEntityEvent::NOT_INSIDE_BUILDING;
    const PhysRelativeTime  destructionTime = time + explosionDuration - SimManager::instance().currentTime();

//WHERE;
	if( pOwner_->objectIsMachine() and pOwner_->asMachine().insideBuilding() )
	{
		inside = MachLogDyingEntityEvent::INSIDE_BUILDING;
		pConstruction = &pOwner_->asMachine().insideWhichBuilding();
	}

    MachLogDyingEntityEvent* pEvent = _NEW(
        MachLogDyingEntityEvent(
         physObjectPtr(),
    	 NULL,
    	 destructionTime,
    	 inside,
    	 pConstruction ) );

    SimManager::instance().add( pEvent );
//WHERE;
}

MachPhysLinearProjectile& MachLogLinearProjectile::physLinearProjectile()
{
    //  Permissable because we know we passed a lineproj into W4dSubject

	return _STATIC_CAST( MachPhysLinearProjectile&, physObject() );
}

const MachPhysLinearProjectile& MachLogLinearProjectile::physLinearProjectile() const
{
    //  Permissable because we know we passed a lineproj into W4dSubject

	return _STATIC_CAST( const MachPhysLinearProjectile&, physObject() );
}

const MexPoint3d&   MachLogLinearProjectile::globalDestructionPosition() const
{
    return globalDestructionPosition_;
}

void MachLogLinearProjectile::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

int MachLogLinearProjectile::damage() const
{
	return damage_;
}

void MachLogLinearProjectile::checkForDamage( MATHEX_SCALAR range, MachLogLinearProjectile::DamageCurveShape curve, MachPhys::WeaponType wt )
{
	MachActor* pActor = NULL;
	if( currentlyAttached_ )
		pActor = pOwner_;
	genericCheckForDamage( globalDestructionPosition(),
							range,
							curve,
							damage(),
							wt,
							NULL,
							pActor );
    isDead( true );
}

//static
void MachLogLinearProjectile::genericCheckForDamage
(
	const MexPoint3d& expPosition,
	MATHEX_SCALAR range,
	MachLogLinearProjectile::DamageCurveShape curve,
	int damage, MachPhys::WeaponType wt,
	MexLine3d* pDirection,
	MachActor* pActor
)
{
    //Get the planet's main config space
    PhysConfigSpace2d& planetConfigSpace = MachLogPlanet::instance().configSpace();

    //Useful values for the range checking
	MexAlignedBox2d tempBoundary;
    MATHEX_SCALAR sqrDoubleRange = range * range * 4.0;
    MATHEX_SCALAR sqrRange = range * range;

	//for each actor in the system see if the explosion sphere intersects
	//with the bounding volume of its entity. This is a multiple stage process
	//with the quickest checks first followed by more refined checks.
	for( MachLogRaces::Objects::iterator i = MachLogRaces::instance().objects().begin();
		i != MachLogRaces::instance().objects().end(); ++i )
	{
		//check any special cases first.
        //Flame thrower mustn't damage itself
		//Same goes for electric charge
		if( pActor == (*i)
			and ( wt == MachPhys::FLAME_THROWER1
			      or wt == MachPhys::FLAME_THROWER2
			      or wt == MachPhys::ELECTRIC_CHARGE ) )
	    {
	     	continue;
	    }

		//construct a temporary bounding box
		MexAlignedBox3d& box = _CONST_CAST( MexAlignedBox3d&, (*i)->physObject().boundingVolume() );
		if( (*i)->physObject().isComposite() )
			box = _CONST_CAST( MexAlignedBox3d&, (*i)->physObject().asComposite().compositeBoundingVolume() );

		MexPoint3d tempPosition( expPosition );
		//get explosion position in local space.
		(*i)->globalTransform().transformInverse( &tempPosition );
        MachActor& iActor = *(*i);
		//now point is in same coord system as box we can look for closest approach.
		MATHEX_SCALAR& useRange = sqrRange;
		if( iActor.objectIsLandMine() )
			useRange = sqrDoubleRange;
		MATHEX_SCALAR boxSqrRange = box.sqrEuclidianDistance( tempPosition );
		if( boxSqrRange < useRange )
		{
			MexLine3d line;
			if( pDirection )
				line = *pDirection;
			else
				line = MexLine3d( expPosition, (*i)->position() );

			if( iActor.objectIsConstruction() )
			{
				switch( curve )
				{
					case CONSTANT_DAMAGE:
					case R_RECIPROCAL_DAMAGE:
					case R_SQUARED_DAMAGE:
					case LINEAR_DAMAGE:
						iActor.beHit( damage, wt, pActor, &line );
						break;
					default:
						iActor.beHit( damage, wt, pActor, &line );
						break;
				}
			}
			else if( iActor.objectIsMachine() )
			{
 				const MATHEX_SCALAR& epsilon = MexEpsilon::instance();
				switch( curve )
				{
					case CONSTANT_DAMAGE:
						iActor.beHit( damage, wt, pActor, &line );
						break;
					case R_RECIPROCAL_DAMAGE:
					{
						if( boxSqrRange > epsilon )
						{
							MATHEX_SCALAR actualRange = expPosition.euclidianDistance( iActor.position() );
							if( actualRange <= 1 )
								iActor.beHit( damage, wt, pActor, &line );
							else
								iActor.beHit( damage / actualRange, wt, pActor, &line );
							}
						else
							iActor.beHit( damage, wt, pActor, &line );
						break;
					}
					case R_SQUARED_DAMAGE:
					{
						if( boxSqrRange > epsilon )
						{
							MATHEX_SCALAR sqrRange = expPosition.sqrEuclidianDistance( iActor.position() );
							if( sqrRange <= 1 )
								iActor.beHit( damage, wt, NULL, &line );
							else
								iActor.beHit( damage / sqrRange, wt, pActor, &line );
						}
						else
							iActor.beHit( damage, wt, NULL, &line );
						break;
					}
					case LINEAR_DAMAGE:
					{
						if( boxSqrRange > epsilon )
						{
							MATHEX_SCALAR actualRange = expPosition.euclidianDistance( iActor.position() );
							if( actualRange <= 1 or range <= 1 )
								iActor.beHit( damage, wt, pActor, &line );
							else
							{
								MATHEX_SCALAR gradient = damage / range;
								int actualDamage = damage - actualRange * gradient;
								actualDamage = std::max( 1, actualDamage );
								iActor.beHit( actualDamage, wt, pActor, &line );
							}
						}
						else
						{
							iActor.beHit( damage, wt, pActor, &line );
						}
						break;
					}
					default:
						iActor.beHit( damage, wt, pActor, &line );
						break;
				}
			}
	        else if( iActor.objectIsArtefact() )
	        {
	            //Register the damage
	            iActor.beHit( damage, wt, pActor, &line );
	        }
	        else if( iActor.objectIsLandMine() )
	        {
				iActor.beHit( damage, wt, pActor, &line );
	        }
		}
	}
}

//virtual
bool MachLogLinearProjectile::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int )
{
	PRE ( currentlyAttached_ and pSubject == pOwner_ );

	switch( event )
	{
	case W4dSubject::DELETED:
		{
			pOwner_ = NULL;
			currentlyAttached_ = false;
		}
		break;
	default:
		;
	}
	return currentlyAttached_;
}

const PhysAbsoluteTime& MachLogLinearProjectile::destructionTime() const
{
	return destructionTime_;
}

MachPhys::StrikeType 	MachLogLinearProjectile::strikeType() const
{
	return strikeType_;
}

ostream& operator <<( ostream& o, const MachLogLinearProjectile& t )
{

    o << "MachLogLinearProjectile " << (void*)&t << " start" << std::endl;
    o << "MachLogLinearProjectile " << (void*)&t << " end" << std::endl;

    return o;
}

bool MachLogLinearProjectile::hasOwner() const
{
	return pOwner_ != NULL;
}

MachActor& MachLogLinearProjectile::owner()
{
	return *pOwner_;
}

MachActor* MachLogLinearProjectile::pOwner()
{
	return pOwner_;
}


void perWrite( PerOstream& ostr, const MachLogLinearProjectile& actor )
{
	const SimProjectile& base1 = actor;

	ostr << base1;
    ostr << actor.destructionTime_;
    ostr << actor.globalDestructionPosition_;
	ostr << actor.speed_;
	ostr << actor.maxRange_;
	ostr << actor.pOwner_;
	ostr << actor.strikeType_;
	ostr << actor.damage_;
	ostr << actor.currentlyAttached_;
}

void perRead( PerIstream& istr, MachLogLinearProjectile& actor )
{
	SimProjectile& base1 = actor;

	istr >> base1;

    istr >> actor.destructionTime_;
    istr >> actor.globalDestructionPosition_;
	istr >> actor.speed_;
	istr >> actor.maxRange_;
	istr >> actor.pOwner_;
	istr >> actor.strikeType_;
	istr >> actor.damage_;
	istr >> actor.currentlyAttached_;
	if( actor.currentlyAttached_ )
		actor.pOwner_->attach( &actor );
}

MachLogLinearProjectile::MachLogLinearProjectile( PerConstructor con )
:	SimProjectile( con )
{
}

/* End MISSILE.CPP **************************************************/


