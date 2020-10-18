/*
 * M C Z E N I T H . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point3d.hpp"
#include "device/keyboard.hpp"
#include "phys/mczenith.hpp"

PhysZenithFlyControl::PhysZenithFlyControl( PhysMotionControlled* pMotionControlled )
:   PhysMotionControlWithTrans( pMotionControlled, defaultForwards() )
{
	PRE( pMotionControlled );
	ctor(pMotionControlled);
}

PhysZenithFlyControl::PhysZenithFlyControl( PhysMotionControlled* pMotionControlled,
   										    PhysMotionConstraint* pMotionConstraint )
:   PhysMotionControlWithTrans( pMotionControlled, pMotionConstraint, defaultForwards() )
{
	PRE( pMotionControlled );
	PRE( pMotionConstraint );
	ctor(pMotionControlled);
}

PhysZenithFlyControl::PhysZenithFlyControl( PhysMotionControlled* pMotionControlled,
											const MexVec2& forwards )
:   PhysMotionControlWithTrans( pMotionControlled, forwards )
{
	PRE( pMotionControlled );
	ctor(pMotionControlled);
}

PhysZenithFlyControl::PhysZenithFlyControl( PhysMotionControlled* pMotionControlled,
   										    PhysMotionConstraint* pMotionConstraint,
											const MexVec2& forwards )
:   PhysMotionControlWithTrans( pMotionControlled, pMotionConstraint, forwards )
{
	PRE( pMotionControlled );
	PRE( pMotionConstraint );
	ctor(pMotionControlled);
}

void PhysZenithFlyControl::ctor(PhysMotionControlled* pMotionControlled)
{
	metresPerSecond(35.0);
	degreesPerSecond(10.0);

    // We use the target's current location and pitch angle
    MexTransform3d defaultTransform( pMotionControlled->globalTransform() );

	// Setup pitch and default heading
	MexEulerAngles angles;
	defaultTransform.rotation( &angles );
	pitchAngle_ = angles.elevation();

	setupDefaultKeyboardMapping();

    TEST_INVARIANT;
}

PhysZenithFlyControl::~PhysZenithFlyControl()
{
    TEST_INVARIANT;

}

void PhysZenithFlyControl::pitchAngle( MexRadians& newPitch )
{
	MexTransform3d globalTrans = pMotionControlled_->globalTransform();

	MexEulerAngles angles;
	globalTrans.rotation( &angles );
	angles.elevation( newPitch );
	globalTrans.rotation( angles );

	pMotionControlled_->globalTransform( globalTrans );

	pitchAngle_ = newPitch;
}

// Look for keypresses and modify motion as appropriate.
// virtual
void PhysZenithFlyControl::update()
{
    //Check not frozen
	if( not motionFrozen() )
    {
    	updateMotion();

        //Compute time since last increment
        double elapsed = frameTimer_.time();
        if( DevKeyboard::instance().ctrlPressed() )
            elapsed *= 0.25;

		MexTransform3d newTransform = pMotionControlled_->globalTransform();

		// Reset the pitch angle so that foward/backward motion is
		// not along the pitch angle.
		MexEulerAngles angles;
		newTransform.rotation( &angles );
		angles.elevation( 0.0 );
		newTransform.rotation( angles );

		pMotionConstraint_->move( newTransform, motion_, elapsed );

		// Set pitch angle back
		newTransform.rotation( &angles );
		angles.elevation( pitchAngle_ );
		newTransform.rotation( angles );

        pMotionControlled_->globalTransform( newTransform );

		resetCommands();
    }

	// Begin timing the next frame.
	frameTimer_.time(0);
}
 //////////////////////////////////////////////////////////////////////////////////////////

void PhysZenithFlyControl::setupDefaultKeyboardMapping()
{
	pKeyTranslator_ = _NEW( DevKeyToCommandTranslator() );
 	//pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_R, PhysMotionControlWithTrans::RESET_POS, 		DevKeyToCommand::EITHER, DevKeyToCommand::PRESSED, 	DevKeyToCommand::RELEASED  ) );
	//pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_R, PhysMotionControlWithTrans::RESET_ORIENTATION, DevKeyToCommand::EITHER, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED  ) );
 	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::UP_ARROW, PhysMotionControlWithTrans::FOWARD, 		DevKeyToCommand::EITHER, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::DOWN_ARROW, PhysMotionControlWithTrans::BACKWARD, 	DevKeyToCommand::EITHER, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::LEFT_ARROW, PhysMotionControlWithTrans::SLIDE_LEFT, 	DevKeyToCommand::EITHER, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::RIGHT_ARROW, PhysMotionControlWithTrans::SLIDE_RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::LEFT_ARROW, PhysMotionControlWithTrans::ROTATE_LEFT, 	DevKeyToCommand::EITHER, DevKeyToCommand::PRESSED, 	DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::RIGHT_ARROW, PhysMotionControlWithTrans::ROTATE_RIGHT,DevKeyToCommand::EITHER, DevKeyToCommand::PRESSED, 	DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::PAGE_UP, PhysMotionControlWithTrans::UP, 				DevKeyToCommand::EITHER, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::PAGE_DOWN, PhysMotionControlWithTrans::DOWN, 			DevKeyToCommand::EITHER, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::PLUS_PAD, PhysMotionControlWithTrans::UP, 			DevKeyToCommand::EITHER, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::MINUS_PAD, PhysMotionControlWithTrans::DOWN, 			DevKeyToCommand::EITHER, DevKeyToCommand::RELEASED, DevKeyToCommand::RELEASED ) );
}

void PhysZenithFlyControl::updateMotion()
{
	MexRadians radiansPerSecond( degreesPerSecond() );

	if ( inputEnabled() )
	{
		double elapsedTime = keyTimer_.time() * 10.0;
		if ( elapsedTime > 1.0 )
			elapsedTime = 1.0; // Knock elapsedTime back to a second to avoid strange motion decay

		// Foward/Backward motion
        if( commandList_[PhysMotionControlWithTrans::FOWARD].on() )
		{
			if ( motion_.speed() < 0.0 )
				motion_.speed( abs(motion_.speed()) );
            motion_.deltaSpeed( metresPerSecond() * elapsedTime );
		}
        else if( commandList_[PhysMotionControlWithTrans::BACKWARD].on() )
		{
			if ( motion_.speed() > 0.0 )
				motion_.speed( -motion_.speed() );
         	motion_.deltaSpeed( -metresPerSecond() * elapsedTime );
		}
        else if( fabs( motion_.speed() ) < metresPerSecond() )
		{
            motion_.speed( 0.0 );
		}
        else
		{
			double decay = ( motion_.speed() / 8.0 ) - motion_.speed();
	        motion_.speed( motion_.speed() + ( decay * elapsedTime ) );
		}

		// Strafe motion
		if( commandList_[PhysMotionControlWithTrans::SLIDE_LEFT].on() )
        {
        	if ( motion_.drift() > 0.0 )
				motion_.drift( -motion_.drift() );
        	motion_.deltaDrift( -metresPerSecond() * elapsedTime );
		}
        else if( commandList_[PhysMotionControlWithTrans::SLIDE_RIGHT].on() )
        {
        	if ( motion_.drift() < 0.0 )
				motion_.drift( abs(motion_.drift()) );
            motion_.deltaDrift( metresPerSecond() * elapsedTime );
		}
        else if( fabs( motion_.drift() ) < metresPerSecond() )
        {
            motion_.drift( 0.0 );
        }
        else
		{
			double decay = ( motion_.drift() / 8.0 ) - motion_.drift();
	        motion_.drift( motion_.drift() + ( decay * elapsedTime ) );
		}

		// Up/Down motion
        if( commandList_[PhysMotionControlWithTrans::UP].on() )
		{
		   	if ( motion_.climb() < 0.0 )
				motion_.climb( abs(motion_.climb()) );
            motion_.deltaClimb( metresPerSecond() * elapsedTime );
        }
        else if( commandList_[PhysMotionControlWithTrans::DOWN].on() )
        {
            if ( motion_.climb() > 0.0 )
				motion_.climb( -motion_.climb() );
			motion_.deltaClimb( -metresPerSecond() * elapsedTime );
        }
        else if( fabs( motion_.climb() ) < metresPerSecond() )
        {
            motion_.climb( 0.0 );
        }
        else
		{
			double decay = ( motion_.climb() / 4.0 ) - motion_.climb();
	        motion_.climb( motion_.climb() + ( decay * elapsedTime ) );
		}

		// Rotation
        if( commandList_[PhysMotionControlWithTrans::ROTATE_LEFT].on() )
		{
		   	if ( motion_.heading() > 0.0 )
				motion_.heading( -motion_.heading() );
			motion_.deltaHeading( -radiansPerSecond.asScalar() * elapsedTime );
		}
        else if( commandList_[PhysMotionControlWithTrans::ROTATE_RIGHT].on() )
        {
           	if ( motion_.heading() < 0.0 )
				motion_.heading( abs(motion_.heading()) );
		    motion_.deltaHeading( radiansPerSecond.asScalar() * elapsedTime );
		}
        else if( fabs( motion_.heading() ) < radiansPerSecond.asScalar() )
        {
            motion_.heading( 0.0 );
        }
        else
		{
			double decay = ( motion_.heading() / 4.0 ) - motion_.heading();
	        motion_.heading( motion_.heading() + ( decay * elapsedTime ) );
		}

		if ( commandList_[PhysMotionControlWithTrans::RESET_POS].on() )
			resetPosition();

		if ( commandList_[PhysMotionControlWithTrans::RESET_ORIENTATION].on() )
			resetOrientation();
  	}

	keyTimer_.time(0);
}

void PhysZenithFlyControl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//////////////////////////////////////////////////////////////////////////////////////////

PhysZenithMotionConstraint::PhysZenithMotionConstraint()
:	maxHeight( 250.0 ),
	minHeight( 20.0 ),
	maxSpeed( 250.0 ),
	minSpeed( -250.0 ),
	maxTurnRate( MexDegrees( 120 ) ),
    minTurnRate( -MexDegrees( 120 ) )
{}

// virtual
void PhysZenithMotionConstraint::move( MexTransform3d& xform, PhysMotion& motion, double elapsedTime )
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

	// Work out new position using an UnconstrainedMotion
	MexTransform3d newTrans( xform );
	PhysUnconstrainedMotion noConstraint;
	noConstraint.move( newTrans, motion, elapsedTime );

	MexPoint3d position = newTrans.position();

	// Adjust height if necessary
	if ( position.z() < minHeight() )
	{
		position.z( minHeight() );
		motion.climb( std::max( minSpeed(), 0.0 ) );
	}
	else if ( position.z() > maxHeight() )
	{
		position.z( maxHeight() );
		motion.climb( std::max( minSpeed(), 0.0 ) );
	}

	// Update passed in transform. This is the new location that the
	// motionControlled object will be moved to.
	xform.position( position );
	MexEulerAngles newAngles;
	newTrans.rotation( &newAngles );
	xform.rotation( newAngles );
}

// virtual
bool PhysZenithMotionConstraint::snapTo( MexPoint3d* location )
{
	// Make sure height restrictions are not being violated
   	if ( location->z() < minHeight() )
		location->z( minHeight() );
	else if ( location->z() > maxHeight() )
		location->z( maxHeight() );

	return true;
}

// virtual
bool PhysZenithMotionConstraint::snapTo( MexTransform3d* trans )
{
	MexPoint3d location( trans->position() );

	// Make sure height restrictions are not being violated
   	if ( location.z() < minHeight() )
		location.z( minHeight() );
	else if ( location.z() > maxHeight() )
		location.z( maxHeight() );

	trans->position( location );

	return true;
}

/* End ZENITCAM.CPP *************************************************/
