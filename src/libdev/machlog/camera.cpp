/*
 * C A M E R A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "device/time.hpp"

#include "ctl/pvector.hpp"

#include "mathex/point3d.hpp"
#include "mathex/line3d.hpp"
#include "mathex/cvexpgon.hpp"
#include "mathex/abox2d.hpp"

#include "phys/cspace2.hpp"

#include "render/colour.hpp"

#include "world4d/domain.hpp"
#include "world4d/scenemgr.hpp"

#include "machphys/plansurf.hpp"
#include "machphys/machine.hpp"
#include "machphys/consdata.hpp"
#include "machphys/constron.hpp"

#include "machlog/camera.hpp"
#include "machlog/internal/camerai.hpp"
#include "machlog/planet.hpp"
#include "machlog/presspad.hpp"
#include "machlog/constron.hpp"
#include "machlog/races.hpp"
#include "machlog/machine.hpp"
#include "profiler/stktrace.hpp"

MachLogCamera::MachLogCamera(   W4dSceneManager* pMgr,
							    W4dEntity* pParent,
							    const W4dTransform3d& localTransform,
							    Type cameraType )
: 	W4dCamera( pMgr, pParent, localTransform ),
	pImpl_( _NEW( MachLogCameraImpl() ) )
{
	CB_MachLogCamera_DEPIMPL();

	onPad_ = false;
	type_ = cameraType;
	insideConstruction_ = false;
	pConfigSpace_ = &MachLogPlanet::instance().configSpace();
	pCurrentPadConstruction_ = NULL;
	pForceDomain_ = NULL;
	observingConstruction_ = false;

    PRE( pMgr != NULL );
    PRE( pParent != NULL );

    switch( cameraType )
    {
        case GROUND:
		    yonClipDistance( 2000 );
            verticalFOVAngle( 40 * Mathex::PI / 180.0 );
            break;
        case FREE_MOVING:
		    yonClipDistance( 2000 );
            verticalFOVAngle( 40 * Mathex::PI / 180.0 );
            break;
        case ZENITH:
		    yonClipDistance( 1000 );
            verticalFOVAngle( 40 * Mathex::PI / 180.0 );
            break;
        case THIRD_PERSON:
		    yonClipDistance( 2000 );
            verticalFOVAngle( 40 * Mathex::PI / 180.0 );
            break;
        case FIRST_PERSON:
		    yonClipDistance( 2000 );
			hitherClipDistance( 0.2 );
            verticalFOVAngle( 60 * Mathex::PI / 180.0 );
            break;
    }
}

MachLogCamera::~MachLogCamera()
{
	CB_MachLogCamera_DEPIMPL();

    TEST_INVARIANT;

    //Remove any observer relation
    if( observingConstruction_ )
        stopObservingConstruction();

	_DELETE( pImpl_ );
}

// Activating a camera may produce a change in the inside building state.
// If the previously active camera was inside and we are now outside, or
// vice versa, we need to switch environments etc.
void MachLogCamera::enable(MachLogCamera* lastCamera)
{
	CB_MachLogCamera_DEPIMPL();

	if (lastCamera)
	{
		if (lastCamera->insideConstruction() && !insideConstruction_)
			lastCamera->leaveConstruction();
		else if (!lastCamera->insideConstruction() && insideConstruction_)
			enterConstruction();
	}
}

void MachLogCamera::update()
{
	CB_MachLogCamera_DEPIMPL();

    MexPoint3d locationPoint;
    globalTransform().position( &locationPoint );

    if( type_ == GROUND )
        updatePressurePad( locationPoint );

    if( not insideConstruction_ )
    {
        updatePlanetDomain( locationPoint );
    }
}

void MachLogCamera::updatePlanetDomain( const MexPoint3d& position )
{
	CB_MachLogCamera_DEPIMPL();

    //If first person get the controlled entity
    MachLogMachine* p1stPersonMachine = NULL;
    W4dEntity* p1stPersonEntity = NULL;

    if( type_ == FIRST_PERSON )
    {
        p1stPersonEntity = this;
        while( p1stPersonEntity->hasParent() )
        {
            p1stPersonEntity = p1stPersonEntity->pParent();
            if( p1stPersonEntity->isComposite() )
            {
                MachActor& actor = MachLogRaces::instance().actor( p1stPersonEntity->id() );
                if( actor.objectIsMachine() )
                    p1stPersonMachine = &actor.asMachine();
            }
        }
    }


    // Get this camera into the correct domain on the planet's surface
    //Get the domain defined by its current position
    W4dDomain* pNewDomain = NULL;
    if( p1stPersonMachine  and  p1stPersonMachine->insideBuilding() )
    {
        pNewDomain = &p1stPersonMachine->insideWhichBuilding().interiorDomain();
    }
    else
    {
        MachPhysPlanetSurface* pSurface = MachLogPlanet::instance().surface();
        pNewDomain = (pForceDomain_ == NULL ? pSurface->domainAt( position )
                                            : pForceDomain_ );
    }
    ASSERT( pNewDomain != NULL, "" );

    //No need to change if camera already in this domain
    if( not hasParent()                                     //Camera has no parent
        or pParent() == MachLogPlanet::instance().pWorld()  //Camera owned directly by root
        or containingDomain() != pNewDomain )               //Different domain
    {
        //If a 1st person machine we attach the machine, otherwise the camera
        if( p1stPersonMachine )
        {
            p1stPersonMachine->physMachine().attachTo( pNewDomain );
        }
        else
        {
            attachTo( pNewDomain );
        }
    }
}

void    MachLogCamera::updatePressurePad( const MexPoint3d& position )
{
	CB_MachLogCamera_DEPIMPL();

    MachLogPressurePads& pressurePad = MachLogPlanet::instance().pressurePads();

    MachLogConstruction*    pConstruction;
    size_t  entrance;

    if( pressurePad.onPad( cameraPolygon( position ), &pConstruction, &entrance ) )
    {
        //  We are currently on a pad. There are three options:

        enum State { STAYING_ON, MOVING_FROM_ANOTHER_PAD, NEW_PAD };

        State   state;

        if( onPad_ )
        {
            if( ( pCurrentPadConstruction_ == pConstruction ) and
                ( currentPadEntrance_ == entrance ) )
                state = STAYING_ON;
            else
                state = MOVING_FROM_ANOTHER_PAD;
        }
        else
        {
            state = NEW_PAD;
        }

        //  If we have moved from another pad make sure we close the
        //  entrance for the other pad.
        if( state == MOVING_FROM_ANOTHER_PAD )
            pCurrentPadConstruction_->isEntranceOpen( currentPadEntrance_, false );

        //  Because we might move directly from one pad onto another
        //  we must always make sure these values are updated.
        //  We need to maintain the observer relation with the current construction
        if( observingConstruction_  and  pCurrentPadConstruction_ != pConstruction )
            stopObservingConstruction();

        pCurrentPadConstruction_ = pConstruction;
        currentPadEntrance_ = entrance;

        //Set up an observer relation with the new construction
        if( not observingConstruction_ )
            startObservingConstruction();

        if( state == STAYING_ON )
        {
            checkThreshold();
        }
        else
        {
            pConstruction->isEntranceOpen( entrance, true );
            onPad_ = true;

            //  In a pathologically bad case this could lead to problems
            //  if the camera moved onto a pad but so close to the threshold
            //  that it counted as being on the threshold.
            if( insideConstruction_ )
            {
                if( thresholdSide() == Mathex::VANILLA )
                    outsideThreshold_ = Mathex::CHOCOLATE;
                else
                    outsideThreshold_ = Mathex::VANILLA;
            }
            else
                outsideThreshold_ = thresholdSide();
        }


    }
    else
    {
        if( onPad_ )
        {
            pCurrentPadConstruction_->isEntranceOpen( currentPadEntrance_, false );
            onPad_ = false;
        }

        //If we're not on the pad, and not inside the construction, we can remove the observer relation
        if( not insideConstruction_  and  observingConstruction_ )
        {
            stopObservingConstruction();
            pCurrentPadConstruction_ = NULL;
        }
    }
}

bool    MachLogCamera::newPositionIsValid( const MexTransform3d& newPosition ) const
{
	CB_MachLogCamera_DEPIMPL();

    bool    positionAccepted = true;

    if( type_ == GROUND )
    {
        positionAccepted = groundPositionLegal( newPosition );
    }

    return positionAccepted;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogCamera::groundPositionLegal( const MexTransform3d& position ) const
{
	CB_MachLogCamera_DEPIMPL();

    const MexPoint3d& groundPosition = position.position();
    MATHEX_SCALAR cameraHeight = groundPosition.z();
	MexConvexPolygon2d cameraPoly =	cameraPolygon( groundPosition );

    MachLogPressurePads& pressurePad = MachLogPlanet::instance().pressurePads();

    MachLogConstruction*    pConstruction;
    size_t  entrance;

	bool targetOnPad = pressurePad.onPad( cameraPoly, &pConstruction, &entrance );

    if( targetOnPad )
    {
        //Get the maximum height above ground for the camera.
	    // Garrisons have larger doors than all other buildings
        MATHEX_SCALAR maxHeight = (pConstruction->objectType() == MachLog::GARRISON ? 7.5 : 4.1 );

        //Check this height against the base height of the building
        MATHEX_SCALAR constructionGroundHeight = pConstruction->position().z();
        targetOnPad = (cameraHeight > constructionGroundHeight) and
                      (cameraHeight < constructionGroundHeight + maxHeight);
    }

	if( targetOnPad and onPad_ )
    {
        //  Make sure that the target is actually on the same pad as our current position
        if( pConstruction != pCurrentPadConstruction_ or entrance != currentPadEntrance_ )
            targetOnPad = false;
    }

    bool positionLegal = false;

    if( targetOnPad )
    {
        positionLegal = true;
    }
    else
    {
        PhysConfigSpace2d::PolygonId id;

        positionLegal = pConfigSpace_->contains( cameraPoly, MachLog::OBSTACLE_LOW | MachLog::OBSTACLE_WATER, &id );

		// Double check to see if position is legal; We might be flying over
		// the top of the obstacle ( not taken into account by above "contains"
		// function.
		if ( not positionLegal )
		{
			positionLegal = true;

			// Check camera poly to see if it is over the top of the obstacle
			for ( int i = 0; i < 2 and positionLegal; ++i )
			{
				// Construct camera poly edge
				MexPoint3d edgePoint1 = cameraPoly.vertex( i );
				MexPoint3d edgePoint2 = cameraPoly.vertex( i + 2 );
				edgePoint1.z( cameraHeight );
				edgePoint2.z( cameraHeight );

				positionLegal = pConfigSpace_->contains( edgePoint1, edgePoint2, MachLog::OBSTACLE_LOW | MachLog::OBSTACLE_WATER, &id );
			}
		}
    }

    return positionLegal;
}

bool MachLogCamera::canSnapToPosition( const MexPoint3d& newPosition )
{
	CB_MachLogCamera_DEPIMPL();

    //  We can only snap to a position if it isn't in a building.
    //  Check it against our planet configuration space.
    MexConvexPolygon2d polygon( MexAlignedBox2d( MexPoint2d(
						        newPosition.x(),
						        newPosition.y() ), 1.5 ) );

    PhysConfigSpace2d::PolygonId id;

	bool positionLegal = false;

	positionLegal = MachLogPlanet::instance().configSpace().contains( polygon, MachLog::OBSTACLE_LOW | MachLog::OBSTACLE_WATER, &id );

	// Double check to see if position is legal; We might be flying over
	// the top of the obstacle ( not taken into account by above "contains"
	// function.
	if ( not positionLegal )
	{
		positionLegal = true;

		// Check camera poly to see if it is over the top of the obstacle
		for ( int i = 0; i < 2 and positionLegal; ++i )
		{
			// Construct camera poly edge
			MexPoint3d edgePoint1 = polygon.vertex( i );
			MexPoint3d edgePoint2 = polygon.vertex( i + 2 );
			edgePoint1.z( globalTransform().position().z() );
			edgePoint2.z( globalTransform().position().z() );

			positionLegal = pConfigSpace_->contains( edgePoint1, edgePoint2, MachLog::OBSTACLE_LOW | MachLog::OBSTACLE_WATER, &id );
		}
	}

    if( positionLegal )
    {
        if( insideConstruction_ )
			leaveConstruction();

        MexTransform3d  newTx( globalTransform() );

        //  Don't use the given height, use our previous height
        MexPoint3d  pos( newPosition.x(), newPosition.y(), newTx.position().z() );

        newTx.position( pos );

        globalTransform( newTx );
    }

    return positionLegal;
}


void MachLogCamera::checkThreshold()
{
	CB_MachLogCamera_DEPIMPL();

    //  Have we crossed the threshold of the current building whether
    //  entering or exiting?

    PRE( onPad_ );

    Mathex::Side    side = thresholdSide();

    //  Only check for crossing the threshold if we are definitely on
    //  one side or the other - ignore the case where we are right on
    //  the line.

    if( side == Mathex::VANILLA or side == Mathex::CHOCOLATE )
    {
        if( insideConstruction_ )
        {
            if( side == outsideThreshold_ )
				leaveConstruction();
        }
        else
        {
            if( side != outsideThreshold_ )
				enterConstruction();
        }
    }
}

Mathex::Side MachLogCamera::thresholdSide()
{
	CB_MachLogCamera_DEPIMPL();

    PRE( onPad_ );

	const MachPhysConstructionData::EntranceData& entranceData =
      pCurrentPadConstruction_->constructionData().entrances()[ currentPadEntrance_ ];

    MexPoint2d  position = globalTransform().position();

    Mathex::Side result = MexLine2d::side( entranceData.doorLine.end1(), entranceData.doorLine.end2(), position );

    return result;
}

void    MachLogCamera::pushParent( W4dEntity* pNewParent, PhysConfigSpace2d* pConfigSpace )
{
	CB_MachLogCamera_DEPIMPL();

    PRE( pParent() != NULL );
    PRE( pNewParent != NULL );
    PRE( pConfigSpace != NULL );

    parentStack_.push( std::pair< W4dEntity*, PhysConfigSpace2d* >( pParent(), pConfigSpace_ ) );

    attachTo( pNewParent );

    pConfigSpace_ = pConfigSpace;

}

void    MachLogCamera::popParent()
{
	CB_MachLogCamera_DEPIMPL();

    PRE( not parentStack_.empty() );

    attachTo( parentStack_.top().first );
    pConfigSpace_ = parentStack_.top().second;

    parentStack_.pop();
}

void MachLogCamera::CLASS_INVARIANT
{
	INVARIANT(this);
}

MexConvexPolygon2d MachLogCamera::cameraPolygon( const MexPoint2d& position ) const
{

    MexConvexPolygon2d polygon(
        MexAlignedBox2d( MexPoint2d(
        position.x(),
        position.y() ), 1.5 ) );

    return polygon;
}

void MachLogCamera::forceDomain( W4dDomain* pDomain )
{
	CB_MachLogCamera_DEPIMPL();

    pForceDomain_ = pDomain;
}

void MachLogCamera::leaveConstruction()
{
	CB_MachLogCamera_DEPIMPL();

	if (pCurrentPadConstruction_)
    	pCurrentPadConstruction_->leaveConstruction( this );

    insideConstruction_ = false;

    floors_.erase( floors_.begin(), floors_.end() );

    manager()->popBackgroundData();

    POST( floors_.size() == 0 );
}

void MachLogCamera::enterConstruction()
{
	CB_MachLogCamera_DEPIMPL();

	if (pCurrentPadConstruction_)
	    pCurrentPadConstruction_->enterConstruction( this );

	insideConstruction_ = true;

    //  While inside the building don't bother drawing the
    //  sky, just clear the background to black
    manager()->pushBackgroundData();

    manager()->clearBackground( true );
    manager()->backgroundColour( RenColour::black() );
    manager()->useBackground( NULL );

    floors_.erase( floors_.begin(), floors_.end() );

    if( pCurrentPadConstruction_ )
    {
        MachPhysConstruction& physConstruction = pCurrentPadConstruction_->physConstruction();

        const MachPhysPlanetSurface::Floor floor(
            physConstruction.externalGlobalBoundary(),
            physConstruction.globalTransform().position().z() );

        floors_.push_back( floor );
    }
}

bool MachLogCamera::insideConstruction() const
{
	CB_MachLogCamera_DEPIMPL();

	return insideConstruction_;
}

//virtual
bool MachLogCamera::beNotified( W4dSubject* pSubject, W4dSubject::NotificationEvent event, int )
{
	CB_MachLogCamera_DEPIMPL();

    bool stayAsObserver = true;

    //Only interested in a construction that the camera is in or at an entrance of being deleted
    if( event == W4dSubject::DELETED  and  (onPad_ or insideConstruction_) and
        pSubject == pCurrentPadConstruction_ )
    {
        //If inside the building we must leave it immediately
        if( insideConstruction_ )
            leaveConstruction();

        //If marked as on a pad, will no longer be true
        onPad_= false;
        pCurrentPadConstruction_ = NULL;

        //No longer want to observe
        stayAsObserver = false;
        observingConstruction_ = false;
    }

    return stayAsObserver;
}

//virtual
void MachLogCamera::domainDeleted( W4dDomain* )
{
    //No action needed
}

void MachLogCamera::startObservingConstruction()
{
	CB_MachLogCamera_DEPIMPL();

    PRE( not observingConstruction_ );
    PRE( pCurrentPadConstruction_ != NULL );

    pCurrentPadConstruction_->attach( this );
    observingConstruction_ = true;
}

void MachLogCamera::stopObservingConstruction()
{
	CB_MachLogCamera_DEPIMPL();

    PRE( observingConstruction_ );
    PRE( pCurrentPadConstruction_ != NULL );

    pCurrentPadConstruction_->detach( this );
    observingConstruction_ = false;
}

struct ObstacleCollisionInfo
{
	MexPoint2d intersectionPoint_;
	MexLine2d obstacleEdge_;
	MATHEX_SCALAR collisionDistance_;
};

void MachLogCamera::findObstacleCollisionPoints( const MexPolygon2d& obstacle,
												 ctl_pvector<ObstacleCollisionInfo>* pCollisionInfo,
												 const MexLine2d& motionDir ) const
{
	CB_MachLogCamera_DEPIMPL();

	for ( int i = 0; i < obstacle.nVertices(); ++i )
	{
		// Construct obstacle edge
		MexPoint2d edgePoint1 = obstacle.vertex( i );
		MexPoint2d edgePoint2 = ( i == obstacle.nVertices() - 1 ) ? obstacle.vertex( 0 ) : obstacle.vertex( i + 1 );

		MexLine2d obstacleEdge = MexLine2d( edgePoint1, edgePoint2 );

		MexPoint2d intersectPoint;

		// Find out if the motion direction intersects this edge
		if ( obstacleEdge.intersects( motionDir, &intersectPoint ) )
		{
	 		// Collision detected...
			MexVec2 motionEndToIntersectVec( motionDir.end2(), intersectPoint );

		    ctl_pvector<ObstacleCollisionInfo>::iterator insertIter = pCollisionInfo->begin();
			bool inserted = false;

			// Calculate how far we area from colliding with this edge
			MATHEX_SCALAR collisionDistance = motionEndToIntersectVec.squareModulus();

			// Insert collision info into sorted list. Closest collisions at front.
			while ( not inserted and insertIter != pCollisionInfo->end() )
			{
				if ( collisionDistance < (*insertIter)->collisionDistance_ )
				{
					inserted = true;
					ObstacleCollisionInfo* pNewCollisionInfo = _NEW( ObstacleCollisionInfo );

					pNewCollisionInfo->intersectionPoint_ = intersectPoint;
					pNewCollisionInfo->obstacleEdge_ = obstacleEdge;
					pNewCollisionInfo->collisionDistance_ = collisionDistance;

					pCollisionInfo->insert( insertIter, pNewCollisionInfo );
				}

				++insertIter;
			}

			if ( not inserted )
			{
				ObstacleCollisionInfo* pNewCollisionInfo = _NEW( ObstacleCollisionInfo );

				pNewCollisionInfo->intersectionPoint_ = intersectPoint;
				pNewCollisionInfo->obstacleEdge_ = obstacleEdge;
				pNewCollisionInfo->collisionDistance_ = collisionDistance;

				pCollisionInfo->push_back( pNewCollisionInfo );
			}
		}
	}
}

bool MachLogCamera::alternativeNewPosition( MexTransform3d* pPosition )	const
{
	CB_MachLogCamera_DEPIMPL();

	PRE( pPosition );
	PRE( not newPositionIsValid( *pPosition ) );

    bool positionAccepted = true;

	MexLine3d travelingFromTo( globalTransform().position(), pPosition->position() );

	MexVec2 travelingVector( travelingFromTo.end1(), travelingFromTo.end2() );
	MexLine2d travelingFromTo2d( travelingFromTo.end1(), travelingFromTo.end2() );

    if( type_ == GROUND )
    {
		PhysConfigSpace2d::PolygonId id;

		// Find the obstacle that we are colliding with...
        positionAccepted = pConfigSpace_->contains( cameraPolygon( pPosition->position() ), MachLog::OBSTACLE_LOW | MachLog::OBSTACLE_WATER, &id );

		if ( not positionAccepted )
		{
			// Get the obstacle
			const MexPolygon2d& obstacle = pConfigSpace_->polygon( id );

			ctl_pvector<ObstacleCollisionInfo> collisionInfo;

			// Get a list of collision points sorted by distance
	 		findObstacleCollisionPoints( obstacle, &collisionInfo, travelingFromTo2d );

			int index = 0;
			// Iterate through the collision points until we find one that slides the
			// camera into a new acceptable position
			while ( not positionAccepted and index < collisionInfo.size() )
			{
				// Split the obstacle edge in two from the point of intersection
				MexVec2 obstEdge1( collisionInfo[index]->intersectionPoint_, collisionInfo[index]->obstacleEdge_.end1() );

				obstEdge1.makeUnitVector();

				MexVec2 obstEdge2( -obstEdge1.x(), -obstEdge1.y() );

				MexPoint3d currentPosition = globalTransform().position();

				// Move camera along edge towards the vertex that we a facing
				if ( travelingVector.dotProduct( obstEdge1 ) > travelingVector.dotProduct( obstEdge2 ) )
				{
					currentPosition += obstEdge1;
				}
				else
				{
					currentPosition += obstEdge2;
				}

				// Update passed in transform to use new position
				pPosition->position( currentPosition );

				// Are we happy with the new position?
				positionAccepted = groundPositionLegal( *pPosition );

				++index;
			}

			// Clean up list of collision info
			while ( not collisionInfo.empty() )
			{
				_DELETE( collisionInfo.back() );
				collisionInfo.pop_back();
			}
		}
    }

    return positionAccepted;
}

const MachPhysPlanetSurface::Floors& MachLogCamera::floors() const
{
	CB_MachLogCamera_DEPIMPL();

    return floors_;
}


/* End CAMERA.CPP ***************************************************/
