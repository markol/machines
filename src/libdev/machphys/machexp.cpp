/*
 * M A C H E X P . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/list.hpp"

#include "mathex/vec3.hpp"
#include "mathex/point3d.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/quatern.hpp"
#include "mathex/random.hpp"

#include "phys/phys.hpp"
#include "phys/rampacce.hpp"
#include "phys/eulertx.hpp"
#include "phys/accetumb.hpp"
#include "phys/linemoti.hpp"
#include "phys/movespin.hpp"
#include "phys/lsclplan.hpp"

#include "world4d/entity.hpp"
#include "world4d/link.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/light.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/generic.hpp"
#include "world4d/soundman.hpp"

#include "sim/manager.hpp"

#include "machphys/machine.hpp"
#include "machphys/machexp.hpp"
#include "machphys/mexpdata.hpp"
#include "machphys/fireball.hpp"
#include "machphys/random.hpp"
#include "machphys/shockwav.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/wepdata.hpp"

static PhysScalarPlanPtr scalarPlanPtr(const PhysRelativeTime& time, const double& alpha)
{
    PhysLinearScalarPlan::ScalarVec times;
    	times.reserve(1);
		times.push_back(time);

    PhysLinearScalarPlan::ScalarVec scales;
    	scales.reserve(2);
        scales.push_back(alpha);
        scales.push_back(0);

	PhysLinearScalarPlan* pPlan = _NEW( PhysLinearScalarPlan(times, scales) );
    return PhysScalarPlanPtr(pPlan);
}

MachPhysMachineExplosion::MachPhysMachineExplosion(
    MachPhysMachine* pMachine,
    const MachPhysMachineExplosionData& data,
    const PhysAbsoluteTime& time )
{
    ////////////////////////////////////////////
    // TBD: this is a huge hack for ECTS.  Remove it.
	// If the composite has any shadow children, copy them.  We need to use
	// the child's name as a form of RTTI in order to down-cast to a shadow
	// interface (very crufty).
    const W4dEntity::W4dEntities& children = pMachine->children();
    for( W4dEntity::W4dEntities::const_iterator i = children.begin(); i != children.end(); ++i )
    {
	    if ((*i)->name() == "SHADOW_PROJ" || (*i)->name() == "SHADOW_FIXED")
	    {
            (*i)->visible( false );
		}
    }
    ////////////////////////////////////////////

	// create an initial fireball covering the whole machine

	PhysAbsoluteTime now = SimManager::instance().currentTime();
	PhysRelativeTime    mainFireballMinDuration = 0.3;
	PhysRelativeTime    mainFireballMaxDuration = 0.5;
	PhysRelativeTime    mainFireballDuration = randomDouble( mainFireballMinDuration, mainFireballMaxDuration );
	PhysRelativeTime	mainExplosionOffset = 0.05;

	// hide the held entities if any
	if (pMachine->hasHeldEntities())
	{
      const W4dComposite::HeldEntities& heldEntities = pMachine->heldEntities();
      for( W4dComposite::HeldEntities::const_iterator i = heldEntities.begin(); i != heldEntities.end(); ++i )
      {
            W4dVisibilityPlan* pVisibilityPlan = _NEW( W4dVisibilityPlan( false ) );

             (*i)->entityPlanForEdit().visibilityPlan(
              W4dVisibilityPlanPtr( pVisibilityPlan ),
              now+mainExplosionOffset );
	  }
    }


    createMainFireball( pMachine, mainExplosionOffset, mainFireballDuration );

	// find the link having the smallest size/weight
	MATHEX_SCALAR minSize=1.e5;
    for( MachPhysMachineExplosionData::LinkDatas::const_iterator i = data.links().begin();
      i != data.links().end(); ++i )
    {
	  MATHEX_SCALAR size = (*i).size();
	  if( ( size>0 ) and ( size < minSize ) )
	  {
	    minSize=size;
	  }
    }

    //  Send some of the links flying into the air
    size_t  nShotOff = 0;
    size_t  nLeft = data.links().size();

    for( MachPhysMachineExplosionData::LinkDatas::const_iterator i = data.links().begin();
      i != data.links().end(); ++i )
    {
//        cout << std::endl;
//        cout << (*i);

        bool    shoot;

        //  The list should have been arranged so that the links
        //  most likely to be shot off are at the beginning. If this
        //  is not the case we might get part way through the list and
        //  reach our maximum shot off limit whilst still leaving some
        //  parts that should always be shot off unshot.

        if( data.maxToShootOff() != MachPhysMachineExplosionData::ALL_LINKS and nShotOff >= data.maxToShootOff() )
            shoot = false;
        else if( nLeft + nShotOff <= data.minToShootOff() )
            shoot = true;
        else if( randomDouble( 0.0, 1.0 ) < (*i).shootOffProbability() )
            shoot = true;
        else
            shoot = false;

        if( shoot )
        {
			// compute the pseudoweight of the link (from its relative volume to the biggest link)
			// used to compute its explosion speed and rotation
			MATHEX_SCALAR pseudoWeight=abs( pow( ( (*i).size()/minSize ), 3) );
			pseudoWeight=(pseudoWeight<1.0) ? 1.0 : pseudoWeight;
            shootOffLink( pMachine, (*i), mainExplosionOffset, time, pseudoWeight, data.maxSize(), data.explosionCenter() );
            ++nShotOff;
        }
        else
        {
            //  Make sure that the link has vanished by the end of the explosion

            W4dVisibilityPlan* pVisibilityPlan = _NEW( W4dVisibilityPlan( false ) );
//            pVisibilityPlan->add( false, now );

			W4dLink* pLink = pMachine->links()[ (*i).linkId() ];
            pLink->entityPlanForEdit().visibilityPlan(
              W4dVisibilityPlanPtr( pVisibilityPlan ),
              now+mainExplosionOffset );
        }

        --nLeft;
    }

	//create a shockwave
	MachPhysShockWave* pShockWave  = _NEW( MachPhysShockWave(pMachine, MexTransform3d() ) );

	MATHEX_SCALAR sizeX = pMachine->compositeBoundingVolume().xLength();
	MATHEX_SCALAR sizeY = pMachine->compositeBoundingVolume().yLength();

    MATHEX_SCALAR fromRadius = std::min(sizeX, sizeY);
    MATHEX_SCALAR toRadius = std::max(sizeX, sizeY);
	const MATHEX_SCALAR zScale = 0.1;

	PhysRelativeTime duration = 0.25;

	const MachPhysWeaponData& WeapData = MachPhysData::instance().weaponData(MachPhys::NUCLEAR_MISSILE);
	fromRadius *=  WeapData.extras()[0];
	toRadius *=  WeapData.extras()[1];
	duration = WeapData.extras()[2];

	// make sure the mininmum radius is not less than 3m without changing the radius ratio
	MATHEX_SCALAR minFromRadius=2.0;
	if( fromRadius < minFromRadius)
	{
	  MATHEX_SCALAR radiusRatio=toRadius/fromRadius;
	  fromRadius=minFromRadius;
	  toRadius=fromRadius*radiusRatio;
	}

	pShockWave->startShockWave( now+mainExplosionOffset, duration, fromRadius, toRadius, zScale);
    W4dGarbageCollector::instance().add( pShockWave, now + duration );

	// Add a brief light for the duration of the shockwave.
	const MATHEX_SCALAR lightRange = 0.5 * (fromRadius + toRadius);
	W4dUniformLight* pLight = _NEW(W4dUniformLight(pMachine, MexVec3(0, 0, 1), lightRange));
	pLight->colour(RenColour(2.3, 1.5, 0.4));
	pLight->constantAttenuation(0);
	pLight->linearAttenuation(0.2);
	pLight->quadraticAttenuation(0.79);
	pLight->scope(W4dLight::DYNAMIC);

    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( false ) ) );
	visibilityPlanPtr->add(true, mainExplosionOffset);
	visibilityPlanPtr->add(false, duration+mainExplosionOffset);
    pLight->entityPlanForEdit().visibilityPlan( visibilityPlanPtr, now );

	PhysScalarPlanPtr intensityPlanPtr = scalarPlanPtr(duration, 1);
	pLight->intensityPlan(intensityPlanPtr, now+mainExplosionOffset );
}

MachPhysMachineExplosion::~MachPhysMachineExplosion()
{
    TEST_INVARIANT;

}

void MachPhysMachineExplosion::shootOffLink(
    MachPhysMachine* pMachine,
    const MachPhysMachineExplosionData::LinkData& linkData,
    const PhysRelativeTime& startTimeOffset,
    const PhysRelativeTime& maxExplosionTime,
    MATHEX_SCALAR pseudoWeight,
    MATHEX_SCALAR maxSize,
    const MexPoint3d& explosionCenter )
{
    //  Calculate the start time and duration for this link's motion
    ASSERT_INFO( linkData.minExplosionTime() );
    ASSERT_INFO( linkData.maxExplosionTime() );

	// the link is dismantled at time 0
	PhysRelativeTime    minExplosionOffset = linkData.minExplosionTime();
	PhysRelativeTime    maxExplosionOffset = linkData.maxExplosionTime();
	PhysRelativeTime    fireballStartOffset;
	// I don't know why : sometimes maxExplosionTime is less than minExplosionTime
	if ( maxExplosionOffset > minExplosionOffset )
	   fireballStartOffset = startTimeOffset+randomDouble( minExplosionOffset, maxExplosionOffset );
	else
	   fireballStartOffset = startTimeOffset+minExplosionOffset;

	PhysRelativeTime    fireballMinDuration = 0.2;
	PhysRelativeTime    fireballMaxDuration = 0.4;
	PhysRelativeTime    fireballDuration = randomDouble( fireballMinDuration, fireballMaxDuration );

    //  Attach the link directly to the composite so that its
    //  transform will not be relative to another link
	W4dLink* pLink = pMachine->links()[ linkData.linkId() ];
    pLink->attachTo( pMachine );

    createSecondaryFireball( pMachine, linkData, fireballStartOffset, fireballDuration, maxSize );

    setupMotionPlans( pMachine, linkData, startTimeOffset, maxExplosionTime, pseudoWeight, explosionCenter );

    //  Make the link vanish when it comes to rest
    W4dVisibilityPlan* pVisibilityPlan = _NEW( W4dVisibilityPlan( false ) );

    pLink->entityPlanForEdit().visibilityPlan(
      W4dVisibilityPlanPtr( pVisibilityPlan ),
      SimManager::instance().currentTime()+fireballStartOffset );
}

void MachPhysMachineExplosion::setupMotionPlans(
    MachPhysMachine* pMachine,
    const MachPhysMachineExplosionData::LinkData& linkData,
    const PhysRelativeTime& startTimeOffset,
    const PhysRelativeTime& explosionDuration,
    MATHEX_SCALAR pseudoWeight,
    const MexPoint3d& explosionCenter )
{
    //  Set up the initial transform, then the transforms of where the link
    //  is going to bounce and end up.

	PRE(pseudoWeight>=0.1);

	W4dLink* pLink = pMachine->links()[ linkData.linkId() ];

	// the initial transform with respect to the explosion center
    const MexTransform3d initialTransform( pLink->localTransform() );

	const PhysRelativeTime duration=explosionDuration-startTimeOffset;

	 // set the rotation speed from the part's mass
	// the max rotation speed is obtained for pseudoWeight=1 (the heaviest object)
	static MATHEX_SCALAR lightestPartMinAngularSpeed=100;
	static MATHEX_SCALAR lightestPartMaxAngularSpeed=200;
	MATHEX_SCALAR minAngularCineticEnergy=pow(lightestPartMinAngularSpeed,2)/2;
	MATHEX_SCALAR maxAngularCineticEnergy=pow(lightestPartMaxAngularSpeed,2)/2;
	// the max angular speed for the current part is computed from its weight
	MATHEX_SCALAR minAngularSpeed=sqrt( minAngularCineticEnergy*2/pseudoWeight );
	MATHEX_SCALAR maxAngularSpeed=sqrt( maxAngularCineticEnergy*2/pseudoWeight );
    MATHEX_SCALAR tumbleRateRadiansPerSecond = randomDouble( minAngularSpeed, maxAngularSpeed );
	tumbleRateRadiansPerSecond = (tumbleRateRadiansPerSecond <0.5) ? 0.5 : tumbleRateRadiansPerSecond;

	MexVec3 rotationAxis( randomDouble( -1.0, 1.0 ), randomDouble( -1.0, 1.0 ), randomDouble( -1.0, 1.0 ) );
    rotationAxis.makeUnitVector();

	// Get the link center in order to compute the direction vector of the explosion
	const MexAlignedBox3d& linkBoundingVol = pLink->boundingVolume();
    MexPoint3d linkCentroid = linkBoundingVol.centroid();
	initialTransform.transform( &linkCentroid );

	// the direction vector  provides the direction and speed of
	// the link dispersion
    MexVec3 directionVector( linkCentroid );
	directionVector.x( directionVector.x() - explosionCenter.x() );
	directionVector.y( directionVector.y() - explosionCenter.y() );
	directionVector.z( directionVector.z() - explosionCenter.z() );
	directionVector.x( directionVector.x() * randomDouble( 0.9, 1.1 ) );
	directionVector.y( directionVector.y() * randomDouble( 0.9, 1.1 ) );
	directionVector.z( directionVector.z() * randomDouble( 0.9, 1.1 ) );
    MexVec3 speedVector( directionVector );
	MATHEX_SCALAR speedFactor = 7 * randomDouble(0.7, 1.3);
	speedVector*=speedFactor;

	// create and register the motion plan
    PhysMoveSpinPlan* pPlan = _NEW( PhysMoveSpinPlan( initialTransform, speedVector, rotationAxis, tumbleRateRadiansPerSecond, duration ) );
    PhysMotionPlanPtr planPtr( pPlan );
    pLink->entityPlanForEdit().absoluteMotion( planPtr, SimManager::instance().currentTime() + startTimeOffset, 0 );
}

// create the fireball of the machine itself
void MachPhysMachineExplosion::createMainFireball(
    MachPhysMachine* pMachine,
    const PhysRelativeTime& fireballStartTime,
    const PhysRelativeTime& duration )
{

	const MexAlignedBox3d& machineBoundingVol = pMachine->compositeBoundingVolume();
    MexVec3 vector = machineBoundingVol.maxCorner();
    vector  -= machineBoundingVol.minCorner();
    MATHEX_SCALAR machineSize = vector.modulus();

    MATHEX_SCALAR fireballSize = randomDouble( machineSize * 4.5, machineSize * 6 );

    if( fireballSize > 0 )
    {

        MATHEX_SCALAR fireballDepthOffset = -machineSize/2;

        MexPoint3d fireballPosition(0.0,0.0,0.0);

        fireballPosition.x( ( machineBoundingVol.maxCorner().x() + machineBoundingVol.minCorner().x() ) / 2 + machineBoundingVol.xLength()*randomDouble( -0.2, 0.2 ) );
        fireballPosition.y( ( machineBoundingVol.maxCorner().y() + machineBoundingVol.minCorner().y() ) / 2 + machineBoundingVol.yLength()*randomDouble( -0.2, 0.2 ) );
        fireballPosition.z( ( machineBoundingVol.maxCorner().z() + machineBoundingVol.minCorner().z() ) / 2 + machineBoundingVol.zLength()*randomDouble( -0.2, 0.2 ) );

        MachPhysFireball* pFireball = _NEW( MachPhysFireball(
            pMachine,
            fireballPosition,
            MachPhysFireball::randomFireball(),
            fireballSize,
            fireballDepthOffset,
            SimManager::instance().currentTime() + fireballStartTime,
            duration ) );

        //W4dGeneric* pExplosionSite = _NEW(W4dGeneric(_STATIC_CAST(W4dEntity*, pMachine->containingDomain()), pMachine->localTransform()));
        W4dGeneric* pExplosionSite = _NEW(W4dGeneric(_REINTERPRET_CAST(W4dEntity*, pMachine->containingDomain()), pMachine->localTransform()));

		SOUND_STREAM("Real fireball size " << fireballSize << std::endl);
		SoundId thisFireballSound = SID_XPLODE1_MACHINE;
		MexBasicRandom tempRandom(MexBasicRandom::constructSeededFromTime());
		int randomNumber =
			mexRandomInt(&tempRandom, 0, 100);
		if(fireballSize < 50.0)
		{
			if(randomNumber < 50)
			{
				SOUND_STREAM("Fireball size 1" << std::endl);
				thisFireballSound = SID_XPLODE1_MACHINE;
			}
			else
			{
				SOUND_STREAM("Fireball size 2" << std::endl);
				thisFireballSound = SID_XPLODE2_MACHINE;
			}
		}
		else if(fireballSize < 100)
		{
			if(randomNumber < 50)
			{
				SOUND_STREAM("Fireball size 3" << std::endl);
				thisFireballSound = SID_XPLODE3_MACHINE;
			}
			else
			{
				SOUND_STREAM("Fireball size 4" << std::endl);
				thisFireballSound = SID_XPLODE4_MACHINE;
			}
		}
		else
		{
			if(randomNumber < 50)
			{
				SOUND_STREAM("Fireball size 5" << std::endl);
				thisFireballSound = SID_XPLODE5_MACHINE;
			}
			else
			{
				SOUND_STREAM("Fireball size 6" << std::endl);
				thisFireballSound = SID_XPLODE6_MACHINE;
			}
		}

  		W4dSoundManager::instance().play(pExplosionSite, thisFireballSound, fireballStartTime, 1);

        W4dGarbageCollector::instance().add(pExplosionSite, SimManager::instance().currentTime() + fireballStartTime + duration + 10);

		//If we have a hip link on this machine then stop the sound (if any) associated with it
		//This is necessary for gliders
		W4dLink* pHipLink;
    	if(pMachine->findLink( "hip", &pHipLink ))
			W4dSoundManager::instance().stop( pHipLink );



	   MATHEX_SCALAR rotationAngle = MachPhysRandom::randomDouble(-(Mathex::PI)/8, 7*(Mathex::PI)/8 );
	   pFireball->rotate( MexRadians( rotationAngle) );

    }
}

void MachPhysMachineExplosion::createSecondaryFireball(
    MachPhysMachine* pMachine,
    const MachPhysMachineExplosionData::LinkData& linkData,
    const PhysAbsoluteTime& fireballStartTime,
    const PhysRelativeTime& duration,
    MATHEX_SCALAR maxSize )
{
    //  Attach a fireball to the link so that it blows up at the correct time

    MATHEX_SCALAR fireballSize = randomDouble( linkData.size() * 2, linkData.size() * 3 );

	W4dLink* pLink = pMachine->links()[ linkData.linkId() ];

    if( fireballSize > 0 )
    {
        //  Move the smaller fireballs further forward than the large ones so that the
        //  will tend to appear in fron of the large ones. We still want all of the
        //  fireballs to appear in front of the thing that they are blowing up.

//        MATHEX_SCALAR fireballDepthOffset = maxSize - fireballSize / 2;
        MATHEX_SCALAR fireballDepthOffset = - maxSize;

        MachPhysFireball* pFireball = _NEW( MachPhysFireball(
            pLink,
            MexPoint3d( 0.0, 0.0, 0.0 ),
            MachPhysFireball::randomFireball(),
            fireballSize,
            fireballDepthOffset,
            SimManager::instance().currentTime() + fireballStartTime,
            duration ) );

        pMachine->attachFireball( pFireball, pLink, MexPoint3d( 0.0, 0.0, 0.0 ) );
	    MATHEX_SCALAR rotationAngle = randomDouble(0, 2*(Mathex::PI) );
	    pFireball->rotate( MexRadians( rotationAngle) );

    }
}

// void MachPhysMachineExplosion::shootOffLink(
//     MachPhysMachine* pMachine,
//     const MachPhysMachineExplosionData::LinkData& linkData,
//     const PhysTime& maxSeparationTime,
//     MATHEX_SCALAR maxSize )
// {
//     W4dLink* pLink = linkData.pLink();
//
//     PhysTime    startTimeOffset = randomDouble( linkData.minTime(), linkData.maxTime() );
//
//     startTimeOffset = min( startTimeOffset, maxSeparationTime );
//
//     //  The duration for this set of bounces will depend on the size of the link
//
//     PhysTime    duration = randomDouble( maxSeparationTime * 1.0 / 4.0, maxSeparationTime * 3.0 / 4.0 ) - startTimeOffset;
//
//     duration /= ( linkData.discreteSize() * 2.0 );
//
//     duration = max( 0.0, duration );
//     duration = min( maxSeparationTime - startTimeOffset, duration );
//
//     //  Attach the link directly to the composite so that its
//     //  transform will not be relative to another link
//     pLink->attachTo( pMachine );
//
//     //  Create a fireball for the link as it is blown off
//
//     MexTransform3d  fireballTransform( pLink->localTransform() );
//     MexPoint3d      fireballPosition( fireballTransform.position() );
//
//     fireballPosition.x( fireballPosition.x() + randomDouble( -0.5, 0.5 ) );
//     fireballPosition.y( fireballPosition.y() + randomDouble( -0.5, 0.5 ) );
//     fireballPosition.z( fireballPosition.z() + randomDouble( -0.5, 0.5 ) );
//
//     MATHEX_SCALAR fireballSize = randomDouble( linkData.size() * 0.9, linkData.size() * 1.3 );
//
//     //  Move the smaller fireballs further forward than the large ones so that the
//     //  will tend to appear in fron of the large ones. We still want all of the
//     //  fireballs to appear in front of the thing that they are blowing up.
//
//     MATHEX_SCALAR fireballDepthOffset = maxSize - fireballSize / 2;
//
//     _NEW( MachPhysFireball(
//         pMachine,
//         fireballPosition,
//         randomInt( 2 ) ? MachPhysFireball::FIREBALL_1 : MachPhysFireball::FIREBALL_2,
//         fireballSize,
//         fireballDepthOffset,
//         startTimeOffset,
//         duration ) );
//
//     //  Create the various plans that will take the link from its
//     //  initial position to its final demise.
//
//     const MexVec3   acceleration( 0.0, 0.0, -9.81 );
//
//     bool    goingUnderground = false;
//     size_t  nBounces;
//
//     if( randomDouble( 0.0, 1.0 ) < linkData.undergroundProbability() )
//     {
//         goingUnderground = true;
//         nBounces = 1;
//     }
//     else
//         nBounces = randomInt( 0, linkData.maxBounces() + 1 );
//
//     //  Set up the times at which the link will bounce
//     PhysMotionPlan::Times*  pTimes = _NEW( PhysMotionPlan::Times );
//
//     generateDecreasingTimeIntervals( nBounces + 1, 0.0, duration, pTimes );
//
//     MATHEX_SCALAR   finalZ = 0.0;
//     MATHEX_SCALAR   bounceZ = linkData.bounceHeight();
//
//
//     if( goingUnderground )
//     {
//         //  The link is going to vanish underground - make sure it gets completely underground
//
//         MATHEX_SCALAR currentZ = pLink->globalTransform().position().z();
//
//         finalZ = -linkData.size() * 2;
//
//         //  Links vanishing underground should only bounce once
//         nBounces = 1;
//
//         //  Extend the time by the correct amount for the link to reach its underground position
//
//         PhysTime lastInterval;
//
//         if( pTimes->size() == 1 )
//             lastInterval = pTimes->end()[ -1 ];
//         else
//             lastInterval = pTimes->end()[ -1 ] - pTimes->end()[ -2 ];
//
//         //  This is the equation you get if you solve the equations of
//         //  motion for the change in time necessary to cause a given change
//         //  in position
//
//         lastInterval += ( lastInterval +
//           sqrt( lastInterval * lastInterval + 8 * ( ( finalZ - currentZ ) - bounceZ ) / acceleration.z() ) ) / 2;
//
//         if( pTimes->size() == 1 )
//             pTimes->back() = lastInterval;
//         else
//             pTimes->back() = pTimes->end()[ -2 ] + lastInterval;
//     }
//     else
//     {
//         //  The link is going to vanish in midair. This means we need to calculate
//         //  the final height and time for the link and also add a fireball to happen
//         //  at the appropriate time.
//
//         PhysTime lastInterval;
//
//         if( pTimes->size() == 1 )
//             lastInterval = pTimes->end()[ -1 ];
//         else
//             lastInterval = pTimes->end()[ -1 ] - pTimes->end()[ -2 ];
//
//         PhysTime newLastInterval = randomDouble( 0.0, lastInterval );
//
//         if( pTimes->size() == 1 )
//             pTimes->back() = newLastInterval;
//         else
//             pTimes->back() = pTimes->end()[ -2 ] + newLastInterval;
//
//         finalZ = ( acceleration.z() * newLastInterval / 2.0 ) * ( newLastInterval - lastInterval );
//
//         //  Attach a fireball to the link so that it blows up at the correct time
//
//         fireballSize = randomDouble( linkData.size() * 1.2, linkData.size() * 1.4 );
//
//         //  Move the smaller fireballs further forward than the large ones so that the
//         //  will tend to appear in fron of the large ones. We still want all of the
//         //  fireballs to appear in front of the thing that they are blowing up.
//
//         fireballDepthOffset = maxSize - fireballSize / 2;
//
//         MachPhysFireball* pFireball = _NEW( MachPhysFireball(
//             pLink,
//             MexPoint3d( 0.0, 0.0, 0.0 ),
//             randomInt( 2 ) ? MachPhysFireball::FIREBALL_1 : MachPhysFireball::FIREBALL_2,
//             fireballSize,
//             fireballDepthOffset,
//             startTimeOffset + pTimes->back(),
//             maxSeparationTime - ( startTimeOffset + pTimes->back() ) ) );
//
//         pMachine->attachFireball( pFireball, pLink, MexPoint3d( 0.0, 0.0, 0.0 ) );
//     }
//
//     //  Set up the initial transform, then the transforms of where the link
//     //  is going to bounce and end up.
//
//     PhysAccelerateTumblePlan::EulerTransforms*  pTransforms = _NEW( PhysAccelerateTumblePlan::EulerTransforms );
//
//     pTransforms->push_back( pLink->localTransform() );
//
//     MATHEX_SCALAR tumbleRateRadiansPerSecond = randomDouble( 0.0, 2.0 );
//
//     generateTransforms( pTransforms, *pTimes, tumbleRateRadiansPerSecond, bounceZ, finalZ );
//
//     PhysAccelerateTumblePlan::EulerTransformsPtr   transformsPtr( pTransforms );
//     PhysMotionPlan::TimesPtr        timesPtr( pTimes );
//
//     PhysAccelerateTumblePlan* pPlan = _NEW( PhysAccelerateTumblePlan(
//         transformsPtr,
//         timesPtr,
//         acceleration ) );
//
//     PhysMotionPlanPtr planPtr( pPlan );
//
//     pLink->absoluteMotion( planPtr, SimManager::instance().currentTime() + startTimeOffset, 0 );
//
//     //  Make the link vanish when it comes to rest
//     W4dVisibilityPlan* pVisibilityPlan = _NEW( W4dVisibilityPlan( true ) );
//
//     pVisibilityPlan->add( false, pTimes->back() );
//
//     W4dVisibilityPlanPtr    visibilityPlanPtr( pVisibilityPlan );
//
//     pLink->visibilityPlan( visibilityPlanPtr, SimManager::instance().currentTime() + startTimeOffset );
//
//     TEST_INVARIANT;
// }

//  Create a random transform for an object that is undergoing a
//  force as a result of an explosion to end up at.
void MachPhysMachineExplosion::createRandomTransform(
  const MexPoint3d& explosionPosition,
  const MexTransform3d& objectTransform,
  MexTransform3d* pResult )
{
    MexPoint3d  objectPosition( objectTransform.position() );
    MexVec3     direction( objectPosition );
    direction -= explosionPosition;

//    direction *= ( ( (float)rand() / 32767.0 ) * 2.0 );
    direction *= ( ( (float)rand() / (float)RAND_MAX ) * 2.0 );

    objectPosition += direction;
//    objectPosition.x( objectPosition.x() + ( (float)rand() / 32767.0 - 0.5 ) );
    objectPosition.x( objectPosition.x() + ( (float)rand() / (float)RAND_MAX - 0.5 ) );
//    objectPosition.y( objectPosition.y() + ( (float)rand() / 32767.0 - 0.5 ) );
    objectPosition.y( objectPosition.y() + ( (float)rand() / (float)RAND_MAX - 0.5 ) );
    objectPosition.z( 0.0 );

//    MexEulerAngles  rotation( (float)rand() / 32767.0, (float)rand() / 32767.0, (float)rand() / 32767.0 );
    MexEulerAngles  rotation( (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX );
    *pResult = MexTransform3d( rotation, objectPosition );
}

// void MachPhysMachineExplosion::generateDecreasingTimeIntervals(
//     size_t nIntervals,
//     const PhysTime& startTime,
//     const PhysTime& endTime,
//     PhysMotionPlan::Times* pTimes )
// {
//     PRE( pTimes->size() == 0 );
//
//     //  Fake a large previous interval for the first time round the loop
//     MATHEX_SCALAR  previousInterval = endTime;
//     PhysTime    timeLeft = endTime - startTime;
//     PhysTime    currentTime = startTime;
//
//     MATHEX_SCALAR   reductionFactor = randomDouble( 0.3, 0.7 );
//
//     //  Calculate the initial time based on the reduction factor
//     for( size_t i = 0; i < nIntervals; ++i )
//     {
//         MATHEX_SCALAR   interval;
//
//         if( i == nIntervals - 1 )
//             interval = timeLeft;
//         else
//         {
//             MATHEX_SCALAR lowerLimit = 1.1 * timeLeft / ( nIntervals - i );
//             MATHEX_SCALAR upperLimit = 1.5 * timeLeft / ( nIntervals - i );
//
//             ASSERT_INFO( lowerLimit );
//             ASSERT_INFO( upperLimit );
//
//             upperLimit = min( upperLimit, previousInterval );
//
//             interval = randomDouble( lowerLimit, upperLimit );
//         }
//
//         currentTime += interval;
//
//         pTimes->push_back( currentTime );
//
//         ASSERT_INFO( interval );
//         ASSERT_INFO( previousInterval );
//         ASSERT( interval < previousInterval, "" );
//
//         timeLeft -= interval;
//
//         ASSERT_INFO( timeLeft );
//         ASSERT( timeLeft >= 0.0, "" );
//
//     }
// }
//

double MachPhysMachineExplosion::randomDouble(
    MATHEX_SCALAR lowerLimit,
    MATHEX_SCALAR upperLimit )
{
    PRE_INFO( lowerLimit );
    PRE_INFO( upperLimit );
    PRE( lowerLimit <= upperLimit );

    double result;

//    result = lowerLimit + ( (float)(rand()) / 32767.0 ) * ( upperLimit - lowerLimit );
    result = lowerLimit + ( (float)(rand()) / (float)RAND_MAX ) * ( upperLimit - lowerLimit );

    POST( lowerLimit <= result and result <= upperLimit );

    return result;
}

int MachPhysMachineExplosion::randomInt(
    int lowerLimit,
    int upperLimit )
{
    PRE( lowerLimit < upperLimit );

    int result;

    result = lowerLimit + rand() % ( upperLimit - lowerLimit );

    POST( lowerLimit <= result and result < upperLimit );

    return result;
}

int MachPhysMachineExplosion::randomInt(
    int upperLimit )
{
    return randomInt( 0, upperLimit );
}

// static
MexVec3& MachPhysMachineExplosion::acceleration( void )
{
    static MexVec3   acceleration_( 0.0, 0.0, -9.81 );

    return acceleration_;
}

void MachPhysMachineExplosion::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}


/* End MACHEXP.CPP **************************************************/
