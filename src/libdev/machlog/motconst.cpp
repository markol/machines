/*
 * M O T C O N S T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/motconst.hpp"
#include "machlog/camera.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/races.hpp"
#include "machlog/machine.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/machine.hpp"
#include "mathex/point3d.hpp"
#include "mathex/line3d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/vec2.hpp"
#include "device/keytrans.hpp"
#include "world4d/domain.hpp"

#include "render/camera.hpp"

MachLogGroundCameraMotionConstraint::MachLogGroundCameraMotionConstraint( MachLogCamera* pCamera )
:	pCamera_( pCamera ),
	zTerrainDelta_( ( ( maxHeight() - minHeight() ) / 2.0 ) + minHeight() )
{

    TEST_INVARIANT;
}

MachLogGroundCameraMotionConstraint::~MachLogGroundCameraMotionConstraint()
{
    TEST_INVARIANT;

}

void MachLogGroundCameraMotionConstraint::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

// virtual
void MachLogGroundCameraMotionConstraint::move( MexTransform3d& trans, PhysMotion& motion, double elapsedTime )
{
	MexTransform3d oldTrans = trans;

	// First, make sure "motion" is not too fast in speed, climb, pitch etc.
	if ( motion.speed() > maxSpeed() )
		motion.speed( maxSpeed() );
	else if ( motion.speed() < minSpeed() )
		motion.speed( minSpeed() );

	if ( motion.drift() > maxSpeed() )
		motion.drift( maxSpeed() );
	else if ( motion.drift() < minSpeed() )
		motion.drift( minSpeed() );

	if ( motion.climb() > maxSpeed() )
		motion.climb( maxSpeed() );
	else if ( motion.climb() < minSpeed() )
		motion.climb( minSpeed() );

	if ( motion.heading() > maxTurnRate().asScalar() )
		motion.heading( maxTurnRate().asScalar() );
	else if ( motion.heading() < minTurnRate().asScalar() )
	    motion.heading( minTurnRate().asScalar() );

	if ( motion.pitch() > maxPitchRate().asScalar() )
		motion.pitch( maxPitchRate().asScalar() );
	else if ( motion.pitch() < minPitchRate().asScalar() )
	    motion.pitch( minPitchRate().asScalar() );

	// Work out new position using an UnconstrainedMotion
	MexTransform3d newTrans( trans );
	PhysUnconstrainedMotion noConstraint;
	noConstraint.move( newTrans, motion, elapsedTime );

	MexPoint3d position = newTrans.position();
	MexEulerAngles angles;
	newTrans.rotation( &angles );

	// Adjust pitch if necessary
	if ( angles.elevation() > maxPitch() )
	{
		angles.elevation( maxPitch() );
	}
	else if ( angles.elevation() < minPitch() )
	{
		angles.elevation( minPitch() );
	}

	// Update passed in transform. This is the new location that the
	// motionControlled object will be moved to.
	trans.position( position );
	trans.rotation( angles );

	MexPoint3d pos = trans.position();

	MATHEX_SCALAR changeInZ = pos.z() - oldTrans.position().z();
	zTerrainDelta_ += changeInZ;

	// Adjust height above terrain if necessary
	if ( zTerrainDelta_ < minHeight() )
	{
		zTerrainDelta_ =  minHeight();
		motion.climb( std::max( minSpeed(), 0.0 ) );
	}
	else if ( zTerrainDelta_ > maxHeight() )
	{
		zTerrainDelta_ = maxHeight();
		motion.climb( std::max( minSpeed(), 0.0 ) );
	}

	// Stop camera from going through building roof
	if ( pCamera_->insideConstruction() and zTerrainDelta_ > 4.1 )
	{
		zTerrainDelta_ = 4.1;
	}

    MachPhysPlanetSurface* pSurface = MachLogPlanet::instance().surface();
	MATHEX_SCALAR terrainHeight = pSurface->terrainHeight( pos.x(), pos.y(), pCamera_->floors() );

	// Make sure camera doesn't drop below min machine height
	terrainHeight = std::max( terrainHeight, pSurface->minMachineHeight() );

	pos.z( terrainHeight + zTerrainDelta_ );

	trans.position( pos );

	if ( not pCamera_->newPositionIsValid( trans ) )
	{
		if ( not pCamera_->alternativeNewPosition( &trans ) )
		{
			trans = oldTrans;
    		motion.speed( 0.0 );
			motion.drift( 0.0 );
		}
	}
}

// virtual
bool MachLogGroundCameraMotionConstraint::snapTo( MexPoint3d* pLocation )
{
	PhysGroundMotionConstraint::snapTo( pLocation );

	// Set up a transform that uses the new location but the old heading ( roll, elevation etc ).
	MexTransform3d trans = pCamera_->globalTransform();
	trans.position( *pLocation );

	// Use other version of "snapTo" because it
	// searches for alternative locations.
    if( snapTo( &trans ) )
    {
		*pLocation = trans.position();
    	return true;
	}
	return false;
}

// virtual
bool MachLogGroundCameraMotionConstraint::snapTo( MexTransform3d* pTrans )
{
	PhysGroundMotionConstraint::snapTo( pTrans );

	// Get planet extremes
	MATHEX_SCALAR maxPlanetX = MachLogPlanet::instance().surface()->xMax();
	MATHEX_SCALAR maxPlanetY = MachLogPlanet::instance().surface()->yMax();
	MATHEX_SCALAR minPlanetX = MachLogPlanet::instance().surface()->xMin();
	MATHEX_SCALAR minPlanetY = MachLogPlanet::instance().surface()->yMin();

	// Reset elevation in transform
	MexEulerAngles angles;
	pTrans->rotation( &angles );
	angles.elevation( 0.0 );
	pTrans->rotation( angles );

	MexPoint3d location( pTrans->position() );

    // Check that camera height is above ground by minHeight
    MachPhysPlanetSurface* pSurface = MachLogPlanet::instance().surface();

	MATHEX_SCALAR terrainHeight = pSurface->terrainHeight( location.x(), location.y(), pCamera_->floors() );

	location.z( terrainHeight + zTerrainDelta_ );

	enum FindDir { FOWARD, BACKWARD, LEFT, RIGHT };

	FindDir findDir = FOWARD;
	MexVec3 moveVec = pTrans->xBasis();
	moveVec *= 5;
	bool finished = false;

	// Find sensible location to position camera
    while( not finished and not pCamera_->canSnapToPosition( location ) )
    {
    	location += moveVec;

		// Check to see if we have stepped outside the planet boundary
		if ( location.x() >= maxPlanetX or location.x() <= minPlanetX or
			 location.y() >= maxPlanetY or location.y() <= minPlanetY )
		{
			// Reset location
			location = pTrans->position();

			switch ( findDir )
			{
				case FOWARD:
					findDir = BACKWARD;
					moveVec.x(-moveVec.x());
					moveVec.y(-moveVec.y());
					moveVec.z(-moveVec.z());
					break;
				case BACKWARD:
					findDir = LEFT;
					moveVec = pTrans->yBasis();
					moveVec *= 5;
					break;
				case LEFT:
					findDir = RIGHT;
					moveVec.x(-moveVec.x());
					moveVec.y(-moveVec.y());
					moveVec.z(-moveVec.z());
					break;
				case RIGHT:
					finished = true; // No other directions that can be tried. We have failed to find a new location
					break;
			}
		}
		else
		{
			// Check that camera height is above ground by minHeight
			terrainHeight = pSurface->terrainHeight( location.x(), location.y(), pCamera_->floors() );

			location.z( terrainHeight + zTerrainDelta_ );
		}
	}

	if ( not finished )
	{
		pTrans->position( location );
	}

	return not finished;
}

ostream& operator <<( ostream& o, const MachLogGroundCameraMotionConstraint& t )
{

    o << "MachLogGroundCameraMotionConstraint " << (void*)&t << " start" << std::endl;
    o << "MachLogGroundCameraMotionConstraint " << (void*)&t << " end" << std::endl;

    return o;
}


MachLogZenithCameraMotionConstraint::MachLogZenithCameraMotionConstraint( MachLogCamera* pCamera )
:	pCamera_( pCamera ),
    isUpToDate_( false ),
    xViewPoint_( 100.0 ),
    yViewPoint_( 100.0 ),
    nStoredTerrainHeights_( 0 ),
    storedTerrainHeightIndex_( 0 )
{
    //Get the required pitch angle from starting transform
	MexEulerAngles angles;
	pCamera->globalTransform().rotation( &angles );
	pitchAngle_ = angles.elevation();
    sinPitchAngle_ = sin( pitchAngle_ );

	// Store camera initial position from starting transform
    setCameraDataFromTransform();

    //Set up unit direction vectors pointing from camera origin through projection window
    //corners
    double halfTanVertical = tan( 0.5 * pCamera->verticalFOVAngle() );
    double halfTanHorizontal = tan( 0.5 * pCamera->horizontalFOVAngle() );
    cornerDirectionVectors_[0] = MexVec3( 1.0, halfTanHorizontal, halfTanVertical );
    cornerDirectionVectors_[1] = MexVec3( 1.0, halfTanHorizontal, -halfTanVertical );
    cornerDirectionVectors_[2] = MexVec3( 1.0, -halfTanHorizontal, halfTanVertical );
    cornerDirectionVectors_[3] = MexVec3( 1.0, -halfTanHorizontal, -halfTanVertical );

    for( size_t i = 0; i!= 4; ++i )
        cornerDirectionVectors_[i].makeUnitVector();

	TEST_INVARIANT;
}

// virtual
MachLogZenithCameraMotionConstraint::~MachLogZenithCameraMotionConstraint()
{

	TEST_INVARIANT;
}

void MachLogZenithCameraMotionConstraint::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

// virtual
void MachLogZenithCameraMotionConstraint::move( MexTransform3d& trans, PhysMotion& motion, double elapsedTime )
{
	// First, make sure "motion" is not too fast in speed, climb, pitch etc.
	if ( motion.speed() > maxSpeed() )
		motion.speed( maxSpeed() );
	else if ( motion.speed() < minSpeed() )
		motion.speed( minSpeed() );

	if ( motion.drift() > maxSpeed() )
		motion.drift( maxSpeed() );
	else if ( motion.drift() < minSpeed() )
		motion.drift( minSpeed() );

	if ( motion.climb() > maxSpeed() )
		motion.climb( maxSpeed() );
	else if ( motion.climb() < minSpeed() )
		motion.climb( minSpeed() );

	if ( motion.heading() > maxTurnRate().asScalar() )
		motion.heading( maxTurnRate().asScalar() );
	else if ( motion.heading() < minTurnRate().asScalar() )
	    motion.heading( minTurnRate().asScalar() );

    //If any of these values are moving we can't be up to date
    if( motion.speed() != 0.0 or motion.drift() != 0.0 or motion.climb() != 0.0 or motion.heading() != 0.0 )
        isUpToDate_ = false;

    if( not isUpToDate_ )
    {
        //Compute the min/max zoom distance
        MATHEX_SCALAR minZoomDistance = minHeight() / sinPitchAngle_;
        MATHEX_SCALAR maxZoomDistance = maxHeight() / sinPitchAngle_;

        //Adjust the zoom distance
        MATHEX_SCALAR deltaZoom = elapsedTime * motion.climb();
        Brake zBrake( minZoomDistance, maxZoomDistance, 0.05 );
        zoomDistance_ += zBrake.newDelta( zoomDistance_, deltaZoom );

		// The post-condition of Brake::newDelta ought to give a zoomDistance_ value
		// which falls within the limits.  This appears not to be the case, probably due
		// to floating-point tollerance errors.  So, check the result of the addition.
		if (zoomDistance_ < minZoomDistance)
			zoomDistance_ = minZoomDistance;
		else if (zoomDistance_ > maxZoomDistance)
			zoomDistance_ = maxZoomDistance;

        //Adjust the heading
        heading_ += MexRadians( motion.heading() * elapsedTime );

        //Compute the footprint with this height and angle
        footprint( zoomDistance_, heading_ );

        //Get the planet surface and its x/y limits
        const MachPhysPlanetSurface& surface = *(MachLogPlanet::instance().surface() );
        MATHEX_SCALAR xPlanetMin = surface.xMin() + 0.01;
        MATHEX_SCALAR yPlanetMin = surface.yMin() + 0.01;
        MATHEX_SCALAR xPlanetMax = surface.xMax() - 0.01;
        MATHEX_SCALAR yPlanetMax = surface.yMax() - 0.01;

        //Compute the x/y limits for the view centre position based on the footprint dimensions
        MATHEX_SCALAR xMin = xPlanetMin - footprintXMin_;
        MATHEX_SCALAR xMax = xPlanetMax - footprintXMax_;
        MATHEX_SCALAR yMin = yPlanetMin - footprintYMin_;
        MATHEX_SCALAR yMax = yPlanetMax - footprintYMax_;

        //Adjust the x/y position
        MATHEX_SCALAR sinHeading = sin( heading_ );
        MATHEX_SCALAR cosHeading = cos( heading_ );
        MATHEX_SCALAR dx = elapsedTime * (cosHeading * motion.speed() - sinHeading * motion.drift());
        MATHEX_SCALAR dy = elapsedTime * (sinHeading * motion.speed() + cosHeading * motion.drift());

        //Brake this within limits. If braking occurs we need to use the minimum resulting ratio in x and y.
        //Otherwise, we start sliding along the limit of the planet, which is undesirable.
        Brake xBrake( xMin, xMax, 0.03 );
        Brake yBrake( yMin, yMax, 0.03 );
        MATHEX_SCALAR xNewDelta = xBrake.newDelta( xViewPoint_, dx);
        MATHEX_SCALAR yNewDelta = yBrake.newDelta( yViewPoint_, dy);

        xViewPoint_ += xNewDelta;
        yViewPoint_ += yNewDelta;

        //Get the terrain height at this position, and ensure don't drop too low
        MATHEX_SCALAR terrainHeight = surface.terrainHeight( xViewPoint_, yViewPoint_ );
        if( terrainHeight < 0.0 )
            terrainHeight = 0.0;

        //Add this frame's height to the stored value list for smoothing
        aStoredTerrainHeights_[ storedTerrainHeightIndex_ ] = terrainHeight;
        PhysAbsoluteTime realNowTime = Phys::time();
        aStoredTimes_[ storedTerrainHeightIndex_ ] = realNowTime;
        if( ++storedTerrainHeightIndex_ == N_MAX_STORED_HEIGHTS )
            storedTerrainHeightIndex_ = 0;

        if( nStoredTerrainHeights_ < N_MAX_STORED_HEIGHTS )
            ++nStoredTerrainHeights_;

        //Get the average over the last few frames
        MATHEX_SCALAR averageTerrainHeight = 0.0;
        uint nHeightsUsed = 0;
        for( size_t i = 0; i != nStoredTerrainHeights_; ++i )
        {
            if( (aStoredTimes_[i] - realNowTime) < 1.0 )
            {
                averageTerrainHeight += aStoredTerrainHeights_[i];
                ++nHeightsUsed;
            }
        }

        //If this is lower than this frame's height, adjust the zoom back down
        MATHEX_SCALAR dropHeight = (nHeightsUsed == 0 ? 0.0 : terrainHeight - (averageTerrainHeight/nHeightsUsed));
        MATHEX_SCALAR zoomDelta = ( dropHeight > 0.01 ? dropHeight / sinPitchAngle_ : 0.0 );

        //Mark camera position up to date, only if we aren't adjusting the zoom.
        isUpToDate_ = zoomDelta == 0.0;

        //Now compute the new camera transform
        MexTransform3d work1( MexEulerAngles( heading_, pitchAngle_, 0.0 ),
                              MexPoint3d( xViewPoint_, yViewPoint_, terrainHeight) );
        MexTransform3d work2( MexPoint3d( zoomDelta - zoomDistance_, 0.0, 0.0 ) );
        work1.transform( work2, &trans );

        //Check the terrain height at the camera position. Camera must be min height above that.
        const MexPoint3d& cameraPosition = trans.position();
        MATHEX_SCALAR underCameraTerrainHeight = surface.terrainHeight( cameraPosition.x(), cameraPosition.y() );

		// This bit of code dynamically moves the near clip plane so that
		// Z-buffer artifacts are kept to a minimum.
		MATHEX_SCALAR minNearClipDistance = 1;
		MATHEX_SCALAR maxNearClipDistance = 15;

		MATHEX_SCALAR nearClipDistance = mexInterpolate(
			zoomDistance_,
			minZoomDistance,
			maxZoomDistance,
			minNearClipDistance,
			maxNearClipDistance);
		ASSERT(inIntervalOO(nearClipDistance, minNearClipDistance, maxNearClipDistance), "");

		pCamera_->pRenCamera()->hitherClipDistance(nearClipDistance);

        if( cameraPosition.z() - underCameraTerrainHeight < minHeight() )
        {
            //We need to pitch up at a steeper angle to keep clear of the terrain.
            //Compute the new pitch angle, but go no higher than 85 degrees.
            const double sinMaxPitchAngle = 0.995;
            MATHEX_SCALAR sinHigherPitchAngle =
                (underCameraTerrainHeight + minHeight() - terrainHeight) / (zoomDistance_ - zoomDelta);
            if( sinHigherPitchAngle > sinMaxPitchAngle )
                sinHigherPitchAngle = sinMaxPitchAngle;

            if( sinHigherPitchAngle > sinPitchAngle_ )
            {
                //Modify to use the higher pitch angle
                MexRadians higherPitchAngle = asin( sinHigherPitchAngle );
                work1.rotation( MexEulerAngles( heading_, higherPitchAngle, 0.0 ) );
                work1.transform( work2, &trans );
            }
        }
    }
    else
        trans = pCamera_->globalTransform();
}

void MachLogZenithCameraMotionConstraint::footprint( MATHEX_SCALAR zoomDistance, MexRadians headingAngle )
{
    //Set up transform representing position of camera relative to the view centre
    MexTransform3d work1( MexEulerAngles( headingAngle, pitchAngle_, 0.0 ) );
    MexTransform3d work2( MexPoint3d( -zoomDistance, 0.0, 0.0 ) );
    MexTransform3d cameraTransform;
    work1.transform( work2, &cameraTransform );

    //Compute the relative height of the camera aboev the view centre
    const MexPoint3d& cameraPosition = cameraTransform.position();

    //Initialise the footprint to teh camera position
    footprintXMin_ = footprintXMax_ = cameraPosition.x();
    footprintYMin_ = footprintYMax_ = cameraPosition.y();

    //Apply this to each direction vector for the projection window coordinates.
    for( size_t i = 0; i != 4; ++i )
    {
        //Apply transform to direction vector
        MexVec3 d( cornerDirectionVectors_[i] );
        cameraTransform.transform( &d );
        ASSERT( d[2] != 0.0, "" );

        //Now compute x, y coordinates of intersection of line through
        //camera origin and having this direction with z = 0
        MATHEX_SCALAR f = -cameraPosition.z() / d[2];
        MATHEX_SCALAR x = cameraPosition.x() + f * d[0];
        MATHEX_SCALAR y = cameraPosition.y() + f * d[1];

        if( x > footprintXMax_ )
            footprintXMax_ = x;
        else if( x < footprintXMin_ )
            footprintXMin_ = x;

        if( y > footprintYMax_ )
            footprintYMax_ = y;
        else if( y < footprintYMin_ )
            footprintYMin_ = y;
    }
}

void MachLogZenithCameraMotionConstraint::cameraPositionData
(
    MATHEX_SCALAR* pZoomDistance, MATHEX_SCALAR* pX, MATHEX_SCALAR* pY, MexRadians* pHeading
) const
{
    *pZoomDistance = zoomDistance_;
    *pX = xViewPoint_;
    *pY = yViewPoint_;
    *pHeading = heading_;
}

void MachLogZenithCameraMotionConstraint::cameraPositionData
(
    MATHEX_SCALAR zoomDistance, MATHEX_SCALAR x, MATHEX_SCALAR y, MexRadians heading
)
{
    //Save the data
    zoomDistance_ = zoomDistance;
    xViewPoint_ = x;
    yViewPoint_ = y;
    heading_ = heading;

    //Mark out of date
    isUpToDate_ = false;
    nStoredTerrainHeights_ = 0;
    storedTerrainHeightIndex_ = 0;
}

void MachLogZenithCameraMotionConstraint::setCameraPosition( const MexTransform3d& cameraTransform )
{
    //Set the camera position
    pCamera_->globalTransform( cameraTransform );

    //Compute the position data from the transform
    setCameraDataFromTransform();
}

void MachLogZenithCameraMotionConstraint::setCameraDataFromTransform()
{
    //get the camera transform. Construct a line from its origin along the x axis for 1000m.
    const MexTransform3d& cameraTransform = pCamera_->globalTransform();
    const MexPoint3d& startPoint = cameraTransform.position();
    MexVec3 sightDir = cameraTransform.xBasis();
    MATHEX_SCALAR limit = 1000.0;
    sightDir *= limit;
    MexPoint3d endPoint( startPoint );
    endPoint += sightDir;
    MexLine3d lineOfSight( startPoint, endPoint );

    //Get the domain containing the camera location
    W4dDomain* pCameraDomain = MachLogPlanetDomains::pDomainAt( MexPoint2d( startPoint ) );
    ASSERT( pCameraDomain != NULL, "Camera start position not in a domain" );

    //Find the first thing we hit along this line
    MATHEX_SCALAR distance;
    W4dEntity* pHitEntity;
    int filter = W4dDomain::EXCLUDE_NOT_SOLID | W4dDomain::EXCLUDE_NOT_STATIONARY;
    bool foundCentre =
        pCameraDomain->findNearerEntity( lineOfSight, limit, W4dEntity::nextCheckId(), W4dEntity::LOW,
                                         &pHitEntity, &distance, filter );

    //If no luck with the domain based check, just get the coords at z=0 along line of sight
    if( not foundCentre )
        distance = limit * ( endPoint.z() / ( startPoint.z() - endPoint.z() ) );

    //Set the centre coords
    MexPoint3d terrainPoint = lineOfSight.pointAtDistance( distance );
    xViewPoint_ = terrainPoint.x();
    yViewPoint_ = terrainPoint.y();
    zoomDistance_ = distance;

    //Set the heading
  	MexEulerAngles angles;
	cameraTransform.rotation( &angles );
	heading_ = angles.azimuth();

    //Ensure view updated next time
    isUpToDate_ = false;
    nStoredTerrainHeights_ = 0;
    storedTerrainHeightIndex_ = 0;
}

//virtual
bool MachLogZenithCameraMotionConstraint::snapTo( MexPoint3d* pLocation )
{
	PRE( pLocation );

    //Set the location
    xViewPoint_ = pLocation->x();
    yViewPoint_ = pLocation->y();
    isUpToDate_ = false;
    nStoredTerrainHeights_ = 0;
    storedTerrainHeightIndex_ = 0;

    //Force an update
	PhysMotion motion(MexVec2(1,0));
    MexTransform3d newTrans;
	move( newTrans, motion, 0 );

    //Move the camera
	pCamera_->globalTransform( newTrans );

    //return the actual position
    *pLocation = newTrans.position();

    //Reset the up-to-date flag, since the base class changes the transform of the camera,
    //using the returned location, possibly incorrectly.
    isUpToDate_ = false;

    return true;
}

//virtual
bool MachLogZenithCameraMotionConstraint::snapTo( MexTransform3d* pTrans )
{
	PRE( pTrans );

    //Set the location
    xViewPoint_ = pTrans->position().x();
    yViewPoint_ = pTrans->position().y();
    isUpToDate_ = false;
    nStoredTerrainHeights_ = 0;
    storedTerrainHeightIndex_ = 0;

    //Force an update
	PhysMotion motion(MexVec2(1,0));
    MexTransform3d newTrans;
	move( newTrans, motion, 0 );

    //Move the camera
	pCamera_->globalTransform( newTrans );

    //return the actual position
    *pTrans = newTrans;

    //Reset the up-to-date flag, since the base class changes the transform of the camera,
    //using the returned location, possibly incorrectly.
    isUpToDate_ = false;

    return true;
}

ostream& operator <<( ostream& o, const MachLogZenithCameraMotionConstraint& t )
{

    o << "MachLogZenithCameraMotionConstraint " << (void*)&t << " start" << std::endl;
    o << "MachLogZenithCameraMotionConstraint " << (void*)&t << " end" << std::endl;

    return o;
}


MachLogPlanetCameraConstraint::MachLogPlanetCameraConstraint()
{

    TEST_INVARIANT;
}

MachLogPlanetCameraConstraint::~MachLogPlanetCameraConstraint()
{
    TEST_INVARIANT;

}

//virtual
void MachLogPlanetCameraConstraint::move(MexTransform3d& transform, PhysMotion& motion, double elapsedTime)
{
    //Perform unconstrained motion
    PhysUnconstrainedMotion noConstraint;
	noConstraint.move( transform, motion, elapsedTime );

    //Get the planet surface
    MachPhysPlanetSurface& surface = *MachLogPlanet::instance().surface();

    //Set the x/y bounds
    MATHEX_SCALAR xMinBound = surface.xMin() + 1.0;
    MATHEX_SCALAR xMaxBound = surface.xMax() - 1.0;
    MATHEX_SCALAR yMinBound = surface.yMin() + 1.0;
    MATHEX_SCALAR yMaxBound = surface.yMax() - 1.0;

    //Get the transform position
    MexPoint3d locationVec;
    transform.position( &locationVec );

    //Limit the location to within the bounds
    MexPoint3d newPos;

    if( locationVec.x() < xMinBound )
    	newPos[0] = xMinBound;
    else if( locationVec.x() > xMaxBound )
    	newPos[0] = xMaxBound;
    else
    	newPos[0] = locationVec.x();

    if( locationVec.y() < yMinBound )
    	newPos[1] = yMinBound;
    else if( locationVec.y() > yMaxBound )
    	newPos[1] = yMaxBound;
    else
    	newPos[1] = locationVec.y();

    //Given the actual x/y location, get the terrain height, and ensure at least 1 metre
    //above ground
    MATHEX_SCALAR height = surface.terrainHeight( newPos[0], newPos[1] ) + 1.0;
    newPos[2] = locationVec.z();
    if( newPos[2] < height )
    	newPos[2] = height;

    transform.position( newPos );
}

// virtual
bool MachLogPlanetCameraConstraint::snapTo( MexPoint3d* location )
{
    //Get the planet surface
    MachPhysPlanetSurface& surface = *MachLogPlanet::instance().surface();

    //Set the x/y bounds
    MATHEX_SCALAR xMinBound = surface.xMin() + 1.0;
    MATHEX_SCALAR xMaxBound = surface.xMax() - 1.0;
    MATHEX_SCALAR yMinBound = surface.yMin() + 1.0;
    MATHEX_SCALAR yMaxBound = surface.yMax() - 1.0;

    //Limit the location to within the bounds
    MexPoint3d& newPos = *location;

    if( newPos.x() < xMinBound )
    	newPos[0] = xMinBound;
    else if( newPos.x() > xMaxBound )
    	newPos[0] = xMaxBound;
    else
    	newPos[0] = newPos.x();

    if( newPos.y() < yMinBound )
    	newPos[1] = yMinBound;
    else if( newPos.y() > yMaxBound )
    	newPos[1] = yMaxBound;
    else
    	newPos[1] = newPos.y();

    //Given the actual x/y location, get the terrain height, and ensure at least 1 metre
    //above ground
    MATHEX_SCALAR height = surface.terrainHeight( newPos[0], newPos[1] ) + 1.0;

    if( newPos[2] < height )
    	newPos[2] = height;

	return true;
}

//virtual
bool MachLogPlanetCameraConstraint::snapTo( MexTransform3d* trans )
{
	MexPoint3d location = trans->position();
	if ( snapTo( &location ) )
	{
		trans->position( location );
		return true;
	}
	return false;
}

void MachLogPlanetCameraConstraint::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogPlanetCameraConstraint& t )
{

    o << "MachLogPlanetCameraConstraint " << (void*)&t << " start" << std::endl;
    o << "MachLogPlanetCameraConstraint " << (void*)&t << " end" << std::endl;

    return o;
}

MachLogMachineThirdPerson::MachLogMachineThirdPerson( const MachLogMachine* pMachine )
: 	pMachine_( pMachine )
{}

// virtual
MachLogMachineThirdPerson::~MachLogMachineThirdPerson()
{}

void MachLogMachineThirdPerson::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

// virtual
bool MachLogMachineThirdPerson::thirdPersonExists() const
{
	return MachLogRaces::instance().machineExists( pMachine_ );
}

// virtual
void MachLogMachineThirdPerson::getIdealCameraLocation( MexTransform3d* pTransform )
{
	const W4dEntity& entity = pMachine_->physMachine();
   	entity.globalTransform().transform( thirdPersonOffset(), pTransform );
}

//static
MexTransform3d& MachLogMachineThirdPerson::thirdPersonOffset()
{
	static MexTransform3d thirdPersonOffsetTrans( MexPoint3d( -10.0, 2.0, 1.0 ) );
	return thirdPersonOffsetTrans;
}

//static
DevKeyToCommandTranslator& MachLogMachineThirdPerson::keyTranslator()
{
	static DevKeyToCommandTranslator    keyTranslator_;
    static  bool    first = true;

    if( first )
    {
        first = false;

		keyTranslator_.addTranslation( DevKeyToCommand( DevKey::PLUS_PAD, ZOOMOUT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
		keyTranslator_.addTranslation( DevKeyToCommand( DevKey::MINUS_PAD, ZOOMIN, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
		keyTranslator_.addTranslation( DevKeyToCommand( DevKey::PAGE_UP, UP, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
		keyTranslator_.addTranslation( DevKeyToCommand( DevKey::PAGE_DOWN, DOWN, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
		keyTranslator_.addTranslation( DevKeyToCommand( DevKey::LEFT_ARROW, LEFT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
		keyTranslator_.addTranslation( DevKeyToCommand( DevKey::RIGHT_ARROW, RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
		keyTranslator_.addTranslation( DevKeyToCommand( DevKey::KEY_R, RESET, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	}

	return keyTranslator_;
}

//static
bool MachLogMachineThirdPerson::processButtonEvent( const DevButtonEvent& buttonEvent )
{
	typedef DevKeyToCommand::CommandId CommandId;

	bool processed = false;

	CommandId commandId;
	processed = keyTranslator().translate( buttonEvent, &commandId );

	if ( processed )
	{
		switch ( commandId )
		{
			case ZOOMOUT:
   				thirdPersonOffset().translate( MexVec3( -1, 0, 0 ) );
				break;
			case ZOOMIN:
		  		thirdPersonOffset().translate( MexVec3( 1, 0, 0 ) );
				break;
			case UP:
				if ( thirdPersonOffset().position().z() < 11.0 )
					thirdPersonOffset().translate( MexVec3( 0, 0, 1 ) );
				break;
			case DOWN:
				if ( thirdPersonOffset().position().z() > 1.0 )
					thirdPersonOffset().translate( MexVec3( 0, 0, -1 ) );
				break;
			case LEFT:
				thirdPersonOffset().translate( MexVec3( 0, -1, 0 ) );
				break;
			case RIGHT:
				thirdPersonOffset().translate( MexVec3( 0, 1, 0 ) );
				break;
			case RESET:
				thirdPersonOffset() = MexPoint3d( -10.0, 2.0, 1.0 );
				break;
			default:
				ASSERT_FAIL( "unknown command (MachLogThirdPerson)" );
		}
	}
	return processed;
}

const MachLogMachine* MachLogMachineThirdPerson::machine() const
{
	return pMachine_;
}

ostream& operator <<( ostream& o, const MachLogMachineThirdPerson& t )
{

    o << "MachLogMachineThirdPerson " << (void*)&t << " start" << std::endl;
    o << "MachLogMachineThirdPerson " << (void*)&t << " end" << std::endl;

    return o;
}

MachLogThirdPersonCameraConstraint::MachLogThirdPersonCameraConstraint( PhysThirdPerson* pThirdPerson, MachLogCamera* pCamera )
:	PhysThirdPersonCameraConstraint( pThirdPerson ),
	pCamera_( pCamera )
{
    TEST_INVARIANT;
}

MachLogThirdPersonCameraConstraint::~MachLogThirdPersonCameraConstraint()
{
    TEST_INVARIANT;

}

void MachLogThirdPersonCameraConstraint::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
void MachLogThirdPersonCameraConstraint::move( MexTransform3d& transform, PhysMotion& motion, double elapsedTime )
{
	MexTransform3d newTransform( transform );
	PhysThirdPersonCameraConstraint::move( newTransform, motion, elapsedTime );

	if ( pCamera_->newPositionIsValid( newTransform ) )
	{
		transform = newTransform;
	}
}

ostream& operator <<( ostream& o, const MachLogThirdPersonCameraConstraint& t )
{

    o << "MachLogThirdPersonCameraConstraint " << (void*)&t << " start" << std::endl;
    o << "MachLogThirdPersonCameraConstraint " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogGroundCameraMotionConstraint::zTerrainDelta( MATHEX_SCALAR newZTerrainDelta )
{
	zTerrainDelta_ = newZTerrainDelta;

	// Adjust height above terrain if necessary
	if ( zTerrainDelta_ < minHeight() )
	{
		zTerrainDelta_ =  minHeight();
	}
	else if ( zTerrainDelta_ > maxHeight() )
	{
		zTerrainDelta_ = maxHeight();
	}
}

MATHEX_SCALAR MachLogGroundCameraMotionConstraint::zTerrainDelta() const
{
	return zTerrainDelta_;
}

/* End MOTCONST.CPP *************************************************/
