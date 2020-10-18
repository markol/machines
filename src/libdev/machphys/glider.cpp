/*
 * G L I D E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "mathex/radians.hpp"
#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"

#include "machphys/glider.hpp"
#include "machphys/internal/glideri.hpp"
#include "machphys/machine.hpp"
#include "machphys/machdata.hpp"
#include "machphys/mcmovinf.hpp"

#include "world4d/link.hpp"
#include "world4d/composit.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/light.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/soundman.hpp"

#include "render/colour.hpp"

#include "phys/rampacce.hpp"
#include "phys/linemoti.hpp"
#include "phys/linetrav.hpp"
#include "phys/motplan.hpp"

#include "mathex/eulerang.hpp"
#include "mathex/quatern.hpp"
#include "ctl/vector.hpp"

#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT( MachPhysGlider );

static W4dVisibilityPlanPtr thrustAndBrake( W4dEntity* entity, const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration )
{
    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	visibilityPlanPtr->add(false, duration);

	entity->entityPlanForEdit().visibilityPlan(visibilityPlanPtr, startTime );

	return visibilityPlanPtr;
}

MachPhysGlider::MachPhysGlider( MachPhysMachine* pMachine, MATHEX_SCALAR height )
: MachPhysLocomotionMethod( pImpl_ = _NEW( MachPhysGliderImpl( pMachine, this, height ) ) )
{
	CB_DEPIMPL(W4dLink*, pBody_);
	CB_DEPIMPL(W4dLink*, pHip_);
	CB_DEPIMPL(W4dLink*, pBrake_);
	CB_DEPIMPL(W4dLink*, pThrust_);
	CB_DEPIMPL(W4dLink*, pLFan_);
	CB_DEPIMPL(W4dLink*, pRFan_);
	CB_DEPIMPL(W4dLink*, pLTurb_);
	CB_DEPIMPL(W4dLink*, pRTurb_);
	CB_DEPIMPL(MexPoint3d, hipPosition_);

    bool bodyFound = pMachine->findLink( "body", &pBody_ );
	ASSERT( bodyFound, "body link not found.	");

    bool hipFound = pMachine->findLink( "hip", &pHip_ );
	ASSERT( hipFound, "hip link not found.	");

	hipPosition_ = pHip_->localTransform().position();
    bool brakeFound = pMachine->findLink( "brake", &pBrake_ );
	ASSERT( brakeFound, "brake link not found.	");
	pBrake_->visible( false );

    bool thrustFound = pMachine->findLink( "thrust", &pThrust_ );
	ASSERT( thrustFound, "thrust link not found.	");
	pThrust_->visible( false );

	pMachine->findLink( "l_fan", &pLFan_ );
	pMachine->findLink( "r_fan", &pRFan_ );
	pMachine->findLink( "l_turb", &pLTurb_ );
	pMachine->findLink( "r_turb", &pRTurb_ );

    startBobbing(SimManager::instance().currentTime());

    //Moved from startBobbing() - put it back if any problems
        if( pLFan_ )
        {
	        MexTransform3d spinTransform( MexEulerAngles( MexDegrees(120), 0.0, 0.0 ) );

            //Set start and first position transforms
            const MexTransform3d& startPosition = pLFan_->localTransform();
            MexTransform3d endPosition( startPosition );
            endPosition.transform( spinTransform );

            PhysLinearMotionPlan* pPlan =
                _NEW( PhysLinearMotionPlan( startPosition, endPosition, 0.25 ) );

            //Add a further rotation of 120 degrees
            endPosition.transform( spinTransform );
            pPlan->add( endPosition, 0.5 );

            //Finally move back to start position
            pPlan->add( startPosition, 0.75 );

            //Add the plan
		    PhysMotionPlanPtr planPtr( pPlan );
           	pLFan_->entityPlanForEdit().absoluteMotion(planPtr,
            						  SimManager::instance().currentTime(),
                                      1000000);
        }

        if( pRFan_ )
        {
	        MexTransform3d spinTransform( MexEulerAngles(  MexDegrees(-120), 0.0, 0.0  ) );

            //Set start and first position transforms
            const MexTransform3d& startPosition = pRFan_->localTransform();
            MexTransform3d endPosition( startPosition );
            endPosition.transform( spinTransform );

            PhysLinearMotionPlan* pPlan =
                _NEW( PhysLinearMotionPlan( startPosition, endPosition, 0.25 ) );

            //Add a further rotation of 120 degrees
            endPosition.transform( spinTransform );
            pPlan->add( endPosition, 0.5 );

            //Finally move back to start position
            pPlan->add( startPosition, 0.75 );

            //Add the plan
		    PhysMotionPlanPtr planPtr( pPlan );
           	pRFan_->entityPlanForEdit().absoluteMotion(planPtr,
            						  SimManager::instance().currentTime(),
                                      1000000);
        }


        if( pLTurb_ )
        {
	        MexTransform3d spinTransform( MexEulerAngles( MexDegrees(120), 0.0, 0.0  ) );

            //Set start and first position transforms
            const MexTransform3d& startPosition = pLTurb_->localTransform();
            MexTransform3d endPosition( startPosition );
            endPosition.transform( spinTransform );

            PhysLinearMotionPlan* pPlan =
                _NEW( PhysLinearMotionPlan( startPosition, endPosition, 0.25 ) );

            //Add a further rotation of 120 degrees
            endPosition.transform( spinTransform );
            pPlan->add( endPosition, 0.5 );

            //Finally move back to start position
            pPlan->add( startPosition, 0.75 );

            //Add the plan
		    PhysMotionPlanPtr planPtr( pPlan );
           	pLTurb_->entityPlanForEdit().absoluteMotion(planPtr,
            						  SimManager::instance().currentTime(),
                                      1000000);
        }

        if( pRTurb_ )
        {
	        MexTransform3d spinTransform( MexEulerAngles( MexDegrees(-120), 0.0, 0.0  ) );

            //Set start and first position transforms
            const MexTransform3d& startPosition = pRTurb_->localTransform();
            MexTransform3d endPosition( startPosition );
            endPosition.transform( spinTransform );

            PhysLinearMotionPlan* pPlan =
                _NEW( PhysLinearMotionPlan( startPosition, endPosition, 0.25 ) );

            //Add a further rotation of 120 degrees
            endPosition.transform( spinTransform );
            pPlan->add( endPosition, 0.5 );

            //Finally move back to start position
            pPlan->add( startPosition, 0.75 );

            //Add the plan
		    PhysMotionPlanPtr planPtr( pPlan );
           	pRTurb_->entityPlanForEdit().absoluteMotion(planPtr,
            						  SimManager::instance().currentTime(),
                                      1000000);
        }

    TEST_INVARIANT;
}

MachPhysGlider::~MachPhysGlider()
{
    TEST_INVARIANT;

    // pImpl_ will be removed by MachPhysLocomotionMethod destructor
    //_DELETE( pImpl_ );
}

// virtual
MachPhysLocomotionMethod* MachPhysGlider::clone( MachPhysMachine* pMachine, const W4dLinks& )
{
    return _NEW( MachPhysGlider( pMachine, pImpl_->height_ ) );
}

// virtual
void MachPhysGlider::moveAnimations
(
    const PhysLinearTravelPlan& linearTravelPlan, const TurnAngles& turnAngles,
    const MachPhysMachineMoveInfo& info
)
{
	CB_DEPIMPL(W4dLink*, pBody_);
	CB_DEPIMPL(W4dLink*, pHip_);
	CB_DEPIMPL(W4dLink*, pBrake_);
	CB_DEPIMPL(W4dLink*, pThrust_);
	CB_DEPIMPL(MexPoint3d, hipPosition_);

    PRE( linearTravelPlan.nSegments() == turnAngles.size() );
	PRE( pBody_ != NULL );

	stopBobbing();

	const PhysAbsoluteTime startTime = info.startTime();

    const PhysMotionPlan::RampAccelerations& rampAccelerations = (*linearTravelPlan.rampAccelerations());

    size_t  nSegments = linearTravelPlan.nSegments();

    //Add angles for each segment
    MexRadians maxRollAngle = MexDegrees( 30.0 );
    MexRadians maxPitchAngle = MexDegrees( 20.0 );

	//create a thrust/brake light
	W4dPointLight* pBrakeLight = _NEW(W4dPointLight(pBrake_, MexVec3(1, 0, 0), 30));

	pMachine()->hold(pBrakeLight, pBrake_, MexTransform3d(MexPoint3d(0, 0, -10)));
	pBrakeLight->colour( RenColour( 3, 2.4, 0) );
	pBrakeLight->constantAttenuation(0);
	pBrakeLight->linearAttenuation(0.7);
	pBrakeLight->quadraticAttenuation(0.3);
	pBrakeLight->scope(W4dLight::DYNAMIC);
	pBrakeLight->visible(false);
 	pBrakeLight->illuminate(pMachine());

	W4dPointLight* pThrustLight = _NEW(W4dPointLight(pThrust_, MexVec3(1, 0, 0), 30));

	pMachine()->hold(pThrustLight, pThrust_, MexTransform3d(MexPoint3d(-10, 0, -5)));
	pThrustLight->colour( RenColour( 3, 0, 0) );
	pThrustLight->constantAttenuation(0);
	pThrustLight->linearAttenuation(0.7);
	pThrustLight->quadraticAttenuation(0.3);
	pThrustLight->scope(W4dLight::DYNAMIC);
	pThrustLight->visible(false);
	pThrustLight->illuminate(pMachine());

	MexTransform3d  brakeLightXform( MexPoint3d( -10, 0, -10 ));
	MexTransform3d thrustLightXform( MexPoint3d( -10, 0, -7 ));

	pBrakeLight->localTransform( brakeLightXform );
	pThrustLight->localTransform( thrustLightXform );

	pBrakeLight->maxRange( 20 );
	pThrustLight->maxRange( 30 );

    //Set up collections for the transforms and times
    ctl_vector< MexTransform3d >  transforms;
    PhysMotionPlan::Times   times;

	transforms.reserve( 1 + nSegments * 5 );
	times.reserve( nSegments * 5 );

	transforms.push_back( pBody_->localTransform() );

	//define the transforms of the body link with respect to the machine,
	//then transform them to its local system
	const MexTransform3d parentXform = pBody_->pParent()->globalTransform();
	const MexTransform3d machineXform = pMachine()->globalTransform();

	MexTransform3d bodyXform;	 //in the machine system
	machineXform.transformInverse( pBody_->globalTransform(), &bodyXform );
	const MexPoint3d position = bodyXform.position(); //in the machine system

	static const MexVec3 pitchAxis(0, 1, 0);	 //in the machine system
	static const MexVec3 rollAxis(1, 0, 0);	 //in the machine system

    PhysRelativeTime segmentStartTime = 0.0;
    PhysRelativeTime translationStartTime = segmentStartTime;
    PhysRelativeTime translationTime = 0;

    for( size_t i = 0; i < nSegments; ++i )
    {
        //Get the times spent at constant velocity, etc

        PhysRelativeTime segmentEndTime = segmentStartTime + rampAccelerations[ i ].totalTime();
        PhysRelativeTime accelerationTime = rampAccelerations[ i ].firstAccelerationTime();
        PhysRelativeTime decelerationTime = rampAccelerations[ i ].secondAccelerationTime();
        PhysRelativeTime constantVelocityTime = (segmentEndTime - segmentStartTime) -
                                        (accelerationTime + decelerationTime);

	    MATHEX_SCALAR acceleration = rampAccelerations[ i ].firstAcceleration();
	    MATHEX_SCALAR deceleration = rampAccelerations[ i ].secondAcceleration();

        ASSERT( constantVelocityTime >= -MexEpsilon::instance(), "" );

        if( constantVelocityTime < 0 ) constantVelocityTime = 0;

        //Decide whether turning or translating
        if( fabs( turnAngles[ i ].asScalar() ) < 0.01 )
        {
			translationTime += rampAccelerations[ i ].totalTime();
		}
        else
        {
			if( translationTime > 0 )
			{
				//pitching TODO check
				//MexRadians pitchAngle = translationTime>1.5? maxPitchAngle: MexDegrees( 5 );
				MexRadians pitchAngle = translationTime>1.5? maxPitchAngle: _STATIC_CAST( MexRadians, MexDegrees( 5 ));
				PhysRelativeTime pitchTime = translationTime>1.5? 0.7: translationTime*0.2;

				PhysRelativeTime pitchUpTime = translationStartTime + pitchTime;
				PhysRelativeTime levelTime = translationStartTime + translationTime*0.5;
				PhysRelativeTime pitchDownTime = translationStartTime + translationTime - pitchTime;

				MexTransform3d xForm = transform( pitchAngle, pitchAxis, position); //in the machine system
				xForm.preTransform( machineXform );	 //this = t * this	result in the global transform of the body link

				MexTransform3d currentXform;
				parentXform.transformInverse( xForm, &currentXform);

		        transforms.push_back( currentXform  );
	            times.push_back( pitchUpTime );

				PhysAbsoluteTime lightStartTime = startTime+translationStartTime;
				pThrustLight->entityPlanForEdit().visibilityPlan( thrustAndBrake(pThrust_, lightStartTime, pitchTime ), lightStartTime);

				xForm = transform( 0, pitchAxis, position);
				xForm.preTransform( machineXform );
				parentXform.transformInverse( xForm, &currentXform);

		        transforms.push_back( currentXform  );
	            times.push_back( levelTime );

				xForm = transform( -pitchAngle, pitchAxis, position);
				xForm.preTransform( machineXform );
				parentXform.transformInverse( xForm, &currentXform);

		        transforms.push_back( currentXform  );
	            times.push_back( pitchDownTime );

				xForm = transform( 0, pitchAxis, position);
				xForm.preTransform( machineXform );
				parentXform.transformInverse( xForm, &currentXform);

		        transforms.push_back( currentXform  );
	            times.push_back( translationStartTime + translationTime );

				pBrakeLight->entityPlanForEdit().visibilityPlan( thrustAndBrake(pBrake_, startTime+pitchDownTime, pitchTime ), startTime+pitchDownTime);
			}
			//reset time for the next translation  section
			translationStartTime = segmentEndTime;
			translationTime = 0;

			//rolling
            MexRadians rollAngle = -turnAngles[ i ];
            if( rollAngle > maxRollAngle ) rollAngle = maxRollAngle;
            else if( rollAngle < -maxRollAngle ) rollAngle = -maxRollAngle;

			MexTransform3d xForm = transform( rollAngle, rollAxis, position); //in the machine system
			xForm.preTransform( machineXform );	 //this = t * this	result in the global transform of the body link

			MexTransform3d currentXform;
			parentXform.transformInverse( xForm, &currentXform);

	        transforms.push_back( currentXform  );
            times.push_back( (segmentStartTime + segmentEndTime) * 0.5 );

			xForm = transform( 0, rollAxis, position);
			xForm.preTransform( machineXform );
			parentXform.transformInverse( xForm, &currentXform);

	        transforms.push_back( currentXform  );
            times.push_back( segmentEndTime );
        }

        segmentStartTime = segmentEndTime;
	}

	if( translationTime > 0.0 )
	{
		//pitching
		//MexRadians pitchAngle = translationTime>1.5? maxPitchAngle: MexDegrees( 5 );
		MexRadians pitchAngle = translationTime>1.5? maxPitchAngle: _STATIC_CAST( MexRadians, MexDegrees( 5 ));
		PhysRelativeTime pitchTime = translationTime>1.5? 0.7: translationTime*0.2;

		PhysRelativeTime pitchUpTime = translationStartTime + pitchTime;
		PhysRelativeTime levelTime = translationStartTime + translationTime*0.5;
		PhysRelativeTime pitchDownTime = translationStartTime + translationTime - pitchTime;

		MexTransform3d xForm = transform( pitchAngle, pitchAxis, position); //in the machine system
		xForm.preTransform( machineXform );	 //this = t * this	result in the global transform of the body link

		MexTransform3d currentXform;
		parentXform.transformInverse( xForm, &currentXform);

        transforms.push_back( currentXform  );
        times.push_back( pitchUpTime );

		PhysAbsoluteTime lightStartTime = startTime+translationStartTime;
		pThrustLight->entityPlanForEdit().visibilityPlan( thrustAndBrake(pThrust_, lightStartTime, pitchTime ), lightStartTime);

		//Work out which brake and blast sounds are to be played
		SoundId brakeId = SID_FLY_BRAKE_LARGE;
		SoundId blastId = SID_FLY_BLAST_LARGE;
		const MachPhysMachineData& data = pMachine()->machineData();
		if(data.machineType() == MachPhys::GEO_LOCATOR &&
			data.hwLevel() == 3)
		{
			brakeId = SID_FLY_BRAKE_SMALL;
			blastId = SID_FLY_BLAST_SMALL;
		}

		W4dSoundManager::instance().play(pMachine(), blastId, lightStartTime, 1);

		xForm = transform( 0, pitchAxis, position);
		xForm.preTransform( machineXform );
		parentXform.transformInverse( xForm, &currentXform);

        transforms.push_back( currentXform  );
        times.push_back( levelTime );

		xForm = transform( -pitchAngle, pitchAxis, position);
		xForm.preTransform( machineXform );
		parentXform.transformInverse( xForm, &currentXform);

        transforms.push_back( currentXform  );
        times.push_back( pitchDownTime );

		xForm = transform( 0, pitchAxis, position);
		xForm.preTransform( machineXform );
		parentXform.transformInverse( xForm, &currentXform);

        transforms.push_back( currentXform  );
        times.push_back( translationStartTime + translationTime );

		pBrakeLight->entityPlanForEdit().visibilityPlan( thrustAndBrake(pBrake_, startTime+pitchDownTime, pitchTime ), startTime+pitchDownTime);

		W4dSoundManager::instance().play(pMachine(), brakeId, startTime+pitchDownTime, 1);
	}

	uint nTimes = times.size();
	uint nTransforms = transforms.size();
	PhysAbsoluteTime bobTime = startTime;

    //Allow for a negligble turn
    if( nTimes != 0 )
    {
    	ASSERT( nTransforms-nTimes ==1, "transform and time numbers do not match" );
        PhysLinearMotionPlan* pPlan =
            _NEW( PhysLinearMotionPlan( transforms[0], transforms[1],  times[0] ) );

    	for (uint i=1; i<nTimes; ++i)
    	{
    		pPlan->add( transforms[i+1], times[i] );
    	}

        PhysMotionPlanPtr planPtr( pPlan );

        pBody_->entityPlanForEdit().absoluteMotion( planPtr, startTime);

        bobTime += planPtr->duration();
    }

	startBobbing( bobTime );

    W4dGarbageCollector::instance().add( pBrakeLight, bobTime );
    W4dGarbageCollector::instance().add( pThrustLight, bobTime );
}

// virtual
void MachPhysGlider::stopDead()
{
	const PhysRelativeTime duration = 1.0;

	//glider rocks forward, backward, then come to rest
	pitchOrRollAnimation(pImpl_->pBody_, MexDegrees( 20 ), MexVec3(0, 1, 0), duration );
}

// virtual
void MachPhysGlider::reviseProfile( TransformsPtr transformsPtr )
{
    forceUpright( transformsPtr );

	//also smoothes out the surface changes

	const size_t nTransforms = transformsPtr->size();
	MATHEX_SCALAR altitude = (*transformsPtr)[0].position().z();

    for( size_t i = 1; i < nTransforms; ++i )
    {
		const MATHEX_SCALAR surfaceHeight = (*transformsPtr)[i].position().z();

		if ( fabs(altitude - surfaceHeight) <= 5 )
		{
			(*transformsPtr)[i].position().z(altitude);
		}
		else
		{
			altitude = surfaceHeight;
		}
    }

}

void MachPhysGlider::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysGlider& t )
{

    o << "MachPhysGlider " << (void*)&t << " start" << std::endl;
    o << "MachPhysGlider " << (void*)&t << " end" << std::endl;

    return o;
}

void MachPhysGlider::stopBobbing()
{
	CB_DEPIMPL(W4dLink*, pHip_);

    PRE( pHip_ != NULL );
    pHip_->entityPlanForEdit().clearAnimation( BOBBING );
}

void MachPhysGlider::startBobbing( const PhysAbsoluteTime& startTime )
{
	CB_DEPIMPL(W4dLink*, pBody_);
	CB_DEPIMPL(W4dLink*, pHip_);
	CB_DEPIMPL(MATHEX_SCALAR, height_);
	CB_DEPIMPL(MexPoint3d, hipPosition_);
	CB_DEPIMPL(W4dLink*, pLFan_);
	CB_DEPIMPL(W4dLink*, pRFan_);
	CB_DEPIMPL(W4dLink*, pLTurb_);
	CB_DEPIMPL(W4dLink*, pRTurb_);

    PRE( pHip_ != NULL );

    if( height_ > 0.0 )
    {
        //  Set up a general "bobbing up and down" motion for the glider machine

        //Get the initial transform
        MexPoint3d upPosition = hipPosition_;
        upPosition.z( upPosition.z() + height_ );

 		const MexVec3 bobAxis(1, 0, 0);
		const MexTransform3d downXform = transform(MexDegrees(15), bobAxis, hipPosition_);
		const MexTransform3d upXform = transform(MexDegrees(-15), bobAxis, upPosition);
		const MexTransform3d startXform = pHip_->localTransform();

 		const MATHEX_SCALAR refSpeed = 0.2;
		const PhysRelativeTime duration = height_/refSpeed;

		ctl_vector< MexTransform3d >* pPrepTransforms = _NEW( ctl_vector< MexTransform3d > );
		pPrepTransforms->reserve(2);
		pPrepTransforms->push_back( startXform );
		pPrepTransforms->push_back( downXform );

		ctl_vector< PhysRampAcceleration >* pPrepRampAccelerations = _NEW( ctl_vector< PhysRampAcceleration > );;
		pPrepRampAccelerations->reserve( 1 );

		MATHEX_SCALAR prepDis = startXform.position().euclidianDistance( downXform.position() );
	    MATHEX_SCALAR transitTime = prepDis/refSpeed;

		if( transitTime > 0 )
		{
			MATHEX_SCALAR distance = prepDis;
		    MATHEX_SCALAR startSpeed = 0;
		    MATHEX_SCALAR desiredSpeed = 3.0;
		    MATHEX_SCALAR endSpeed = 0;
		    MATHEX_SCALAR maxAcceleration = 0.4;
		    MATHEX_SCALAR maxDeceleration = 0.4;

			pPrepRampAccelerations->push_back( PhysRampAcceleration(  distance,
										 								startSpeed,
																		desiredSpeed,
																		endSpeed,
																		transitTime,
																		maxAcceleration,
																		maxDeceleration,
																		PhysRampAcceleration::BY_DISTANCE_TIME ) );

			PhysMotionPlan::TransformsPtr prepTransformsPtr = pPrepTransforms;
			PhysMotionPlan::RampAccelerationsPtr prepRampAccelerationsPtr= pPrepRampAccelerations;

		    PhysLinearTravelPlan* pTransitPlan =
		        _NEW( PhysLinearTravelPlan( prepTransformsPtr, prepRampAccelerationsPtr ) );

	        pHip_->entityPlanForEdit().absoluteMotion( PhysMotionPlanPtr( pTransitPlan ), startTime, 0, BOBBING );
		}
        else
        {
            _DELETE( pPrepTransforms );
            _DELETE( pPrepRampAccelerations );
        }

		//the true bobbing animation
		ctl_vector< MexTransform3d >*  pBobTransforms = _NEW( ctl_vector< MexTransform3d > );
		pBobTransforms->reserve(3);
		pBobTransforms->push_back( downXform );
		pBobTransforms->push_back( upXform );
		pBobTransforms->push_back( downXform );

		const PhysRampAcceleration acce( height_, 0, 3.0, 0, duration, 0.2, 0.2, PhysRampAcceleration::BY_DISTANCE_TIME );

		ctl_vector< PhysRampAcceleration >*	pBobRampAccelerations = _NEW( ctl_vector< PhysRampAcceleration > );
		pBobRampAccelerations->reserve( 2 );
		pBobRampAccelerations->push_back( acce );
		pBobRampAccelerations->push_back( acce );

		PhysMotionPlan::TransformsPtr bobTransformsPtr = pBobTransforms;
		PhysMotionPlan::RampAccelerationsPtr bobRampAccelerationsPtr = pBobRampAccelerations;

	    PhysLinearTravelPlan* pBobPlan =
	        _NEW( PhysLinearTravelPlan( bobTransformsPtr, bobRampAccelerationsPtr ) );

        static const uint repeatForever = 1000000;
	    pHip_->entityPlanForEdit().absoluteMotion( PhysMotionPlanPtr( pBobPlan ), startTime+transitTime, repeatForever, BOBBING );

	}
}

MachPhysGlider::MachPhysGlider( PerConstructor con )
: MachPhysLocomotionMethod( con ),
  pImpl_( NULL )
{
}

void perWrite( PerOstream& ostr, const MachPhysGlider& glider )
{
    const MachPhysLocomotionMethod& base = glider;

    ostr << base;

    ostr << glider.pImpl_;
}

void perRead( PerIstream& istr, MachPhysGlider& glider )
{
    MachPhysLocomotionMethod& base = glider;

    istr >> base;

    istr >> glider.pImpl_;
}

void MachPhysGlider::doFirstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state )
{
	CB_DEPIMPL(W4dLink*, pBody_);
	CB_DEPIMPL(W4dLink*, pHip_);

    //We don't want to play turning animatins, because they make first person too difficult.
    //So locally map turning states into stopped state.
    //But we do want to do them if remotely controlled
    MachPhysLocomotionMethod::FirstPersonMotionState oldState = pImpl_->lastState();

    if( not isRemotelyControlledFirstPerson() )
    {
        if( oldState == MachPhysLocomotionMethod::TURNING_LEFT or
            oldState == MachPhysLocomotionMethod::TURNING_RIGHT )
            oldState = MachPhysLocomotionMethod::STOPPED;

        if( state == MachPhysLocomotionMethod::TURNING_LEFT or
            state == MachPhysLocomotionMethod::TURNING_RIGHT )
            state = MachPhysLocomotionMethod::STOPPED;
    }

    //If we haven't changed state do nothing
    if( state != oldState )
    {
        switch( state )
        {
            case MachPhysLocomotionMethod::ENTERING:
            {
                //Need to stop the bobbing, because it causes the camera to wander from side to side
                snapToMidBob();
                //pHip_->entityPlanForEdit().finishAnimation( BOBBING );
                break;
            }

            case MachPhysLocomotionMethod::LEAVING:
            {
                startBobbing(SimManager::instance().currentTime());
                break;
            }

            case MachPhysLocomotionMethod::STOPPED:
            {
				//glider rocks forward, backward, then come to rest
				pitchOrRollAnimation(pBody_, MexDegrees( 5 ), MexVec3(0, 1, 0), 0.3 );
                break;
            }

            case MachPhysLocomotionMethod::MOVING_FORWARDS:
			{
				//glider pitches down
				MexTransform3d currentTransform = pBody_->localTransform();
				MexPoint3d bodyPosition = currentTransform.position();
				MexTransform3d normalTransform = transform(MexDegrees( 0 ), MexVec3( 0, 1, 0), bodyPosition );
				MexTransform3d forwardsTransform = transform(MexDegrees( -5 ), MexVec3( 0, 1, 0), bodyPosition );

			    PhysLinearMotionPlan* pPlan =
			        _NEW( PhysLinearMotionPlan( currentTransform, normalTransform,  0.3 ) );
				pPlan->add( forwardsTransform, 0.6);

			    PhysMotionPlanPtr planPtr( pPlan );

			    pBody_->entityPlanForEdit().absoluteMotion( planPtr, SimManager::instance().currentTime() );
                break;
			}
            case MachPhysLocomotionMethod::MOVING_BACKWARDS:
            {
				//glider pitches up
				MexTransform3d currentTransform = pBody_->localTransform();
				MexPoint3d bodyPosition = currentTransform.position();
				MexTransform3d normalTransform = transform(MexDegrees( 0 ), MexVec3( 0, 1, 0), bodyPosition );
				MexTransform3d forwardsTransform = transform(MexDegrees( 5 ), MexVec3( 0, 1, 0), bodyPosition );

			    PhysLinearMotionPlan* pPlan =
			        _NEW( PhysLinearMotionPlan( currentTransform, normalTransform,  0.3 ) );
				pPlan->add( forwardsTransform, 0.6);

			    PhysMotionPlanPtr planPtr( pPlan );

			    pBody_->entityPlanForEdit().absoluteMotion( planPtr, SimManager::instance().currentTime() );
                break;
            }

            case MachPhysLocomotionMethod::TURNING_LEFT:
            {
				//glider rolls left
				MexTransform3d currentTransform = pBody_->localTransform();
				MexPoint3d bodyPosition = currentTransform.position();
				MexTransform3d normalTransform = transform(MexDegrees( 0 ), MexVec3( 1, 0, 0), bodyPosition );
				MexTransform3d leftTransform = transform(MexDegrees( 15 ), MexVec3( 1, 0, 0), bodyPosition );

			    PhysLinearMotionPlan* pPlan =
			        _NEW( PhysLinearMotionPlan( currentTransform, normalTransform,  0.25 ) );
				pPlan->add( leftTransform, 0.5);

			    PhysMotionPlanPtr planPtr( pPlan );

			    pBody_->entityPlanForEdit().absoluteMotion( planPtr, SimManager::instance().currentTime() );

				//pBody_->localTransform( leftTransform );
                break;
            }

            case MachPhysLocomotionMethod::TURNING_RIGHT:
            {
				//glider rolls right

				MexTransform3d currentTransform = pBody_->localTransform();
				MexPoint3d bodyPosition = currentTransform.position();
				MexTransform3d normalTransform = transform(MexDegrees( 0 ), MexVec3( 1, 0, 0), bodyPosition );
				MexTransform3d rightTransform = transform(MexDegrees( -15 ), MexVec3( 1, 0, 0), bodyPosition );

			    PhysLinearMotionPlan* pPlan =
			        _NEW( PhysLinearMotionPlan( currentTransform, normalTransform,  0.25 ) );
				pPlan->add( rightTransform, 0.5);

			    PhysMotionPlanPtr planPtr( pPlan );

			    pBody_->entityPlanForEdit().absoluteMotion( planPtr, SimManager::instance().currentTime() );

				//pBody_->localTransform( rightTransform );
                break;
            }

        }
    }
}

void MachPhysGlider::snapToMidBob()
{
	CB_DEPIMPL(W4dLink*, pHip_);
	CB_DEPIMPL(MATHEX_SCALAR, height_);
	CB_DEPIMPL(MexPoint3d, hipPosition_);

    PRE( pHip_ != NULL );

    if( height_ > 0.0 )
    {
        //Scrap any current motion plans on the hip
        pHip_->entityPlanForEdit().clearMotionPlans();

        //Get the initial transform
        MexPoint3d midPosition = hipPosition_;
        midPosition.z( midPosition.z() + 0.5 * height_ );

 		const MexVec3 bobAxis(1, 0, 0);
		const MexTransform3d midXform = transform(MexDegrees(0.0), bobAxis, midPosition);
        pHip_->localTransform( midXform );
	}
}

/* End GLIDER.CPP ***************************************************/
