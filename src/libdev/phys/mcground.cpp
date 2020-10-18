/*
 * G R F L Y C T L . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/base.hpp"
#include "device/keyboard.hpp"
#include "device/time.hpp"
#include "mathex/mathex.hpp"
#include "mathex/radians.hpp"
#include "mathex/degrees.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "phys/mcground.hpp"
#include "device/keytrans.hpp"

//////////////////////////////////////////////////////////////////////////////////////////
PhysGroundFlyControl::PhysGroundFlyControl( PhysMotionControlled* pMotionControlled )
:   PhysMotionControlWithTrans( pMotionControlled, defaultForwards() )
{
	PRE( pMotionControlled );
	ctor();
}

PhysGroundFlyControl::PhysGroundFlyControl( PhysMotionControlled* pMotionControlled,
											PhysMotionConstraint* pMotionConstraint )
:   PhysMotionControlWithTrans( pMotionControlled, pMotionConstraint, defaultForwards() )
{
	PRE( pMotionControlled );
	PRE( pMotionConstraint );
	ctor();
}

PhysGroundFlyControl::PhysGroundFlyControl( PhysMotionControlled* pMotionControlled,
											const MexVec2& forwards )
:   PhysMotionControlWithTrans( pMotionControlled, forwards )
{
	PRE( pMotionControlled );
	ctor();
}

PhysGroundFlyControl::PhysGroundFlyControl( PhysMotionControlled* pMotionControlled,
											PhysMotionConstraint* pMotionConstraint,
											const MexVec2& forwards )
:   PhysMotionControlWithTrans( pMotionControlled, pMotionConstraint, forwards )
{
	PRE( pMotionControlled );
	PRE( pMotionConstraint );
	ctor();
}

// Do the initialisation work common to all the constructors.
void PhysGroundFlyControl::ctor()
{
	metresPerSecond(8.0);
	degreesPerSecond(5.0);
 	setupDefaultKeyboardMapping();
	reversePitchUpDownKeys_ = false;

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysGroundFlyControl::~PhysGroundFlyControl()
{
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysGroundFlyControl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysGroundFlyControl::setupDefaultKeyboardMapping()
{
	pKeyTranslator_ = _NEW( DevKeyToCommandTranslator() );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::UP_ARROW, 	PhysMotionControlWithTrans::FOWARD, 		DevKeyToCommand::EITHER, 	DevKeyToCommand::RELEASED, 	DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::DOWN_ARROW, 	PhysMotionControlWithTrans::BACKWARD, 		DevKeyToCommand::EITHER, 	DevKeyToCommand::RELEASED, 	DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_DELETE, 	PhysMotionControlWithTrans::SLIDE_LEFT, 	DevKeyToCommand::EITHER, 	DevKeyToCommand::EITHER, 	DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::END, 			PhysMotionControlWithTrans::SLIDE_RIGHT, 	DevKeyToCommand::EITHER, 	DevKeyToCommand::EITHER, 	DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::LEFT_ARROW, 	PhysMotionControlWithTrans::ROTATE_LEFT, 	DevKeyToCommand::EITHER, 	DevKeyToCommand::EITHER, 	DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::RIGHT_ARROW, 	PhysMotionControlWithTrans::ROTATE_RIGHT, 	DevKeyToCommand::EITHER, 	DevKeyToCommand::EITHER, 	DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::PAGE_UP, 		PhysMotionControlWithTrans::UP, 			DevKeyToCommand::EITHER, 	DevKeyToCommand::EITHER, 	DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::PAGE_DOWN, 	PhysMotionControlWithTrans::DOWN, 			DevKeyToCommand::EITHER, 	DevKeyToCommand::EITHER, 	DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::PLUS_PAD, 	PhysMotionControlWithTrans::UP, 			DevKeyToCommand::EITHER, 	DevKeyToCommand::EITHER, 	DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::MINUS_PAD, 	PhysMotionControlWithTrans::DOWN, 			DevKeyToCommand::EITHER, 	DevKeyToCommand::EITHER, 	DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::UP_ARROW, 	PhysMotionControlWithTrans::PITCH_UP, 		DevKeyToCommand::EITHER, 	DevKeyToCommand::PRESSED, 	DevKeyToCommand::RELEASED ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::DOWN_ARROW, 	PhysMotionControlWithTrans::PITCH_DOWN, 	DevKeyToCommand::EITHER, 	DevKeyToCommand::PRESSED, 	DevKeyToCommand::RELEASED ) );
	//pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_R, 		PhysMotionControlWithTrans::RESET_POS, 		DevKeyToCommand::EITHER, 	DevKeyToCommand::PRESSED, 	DevKeyToCommand::RELEASED  ) );
	//pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_R, 		PhysMotionControlWithTrans::RESET_ORIENTATION, DevKeyToCommand::EITHER, DevKeyToCommand::RELEASED, 	DevKeyToCommand::RELEASED  ) );
}

//////////////////////////////////////////////////////////////////////////////////////////

void PhysGroundFlyControl::updateMotion()
{
	MexRadians radiansPerSecond( degreesPerSecond() );

	if ( inputEnabled() )
	{
		double elapsedTime = keyTimer_.time() * 10.0;
		if ( elapsedTime > 1.0 )
			elapsedTime = 1.0; // Knock elapsedTime back to a second to avoid strange motion decay


		// Speed
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
			double decay = ( motion_.speed() / 4.0 ) - motion_.speed();
	        motion_.speed( motion_.speed() + ( decay * elapsedTime ) );
		}

		// Strafe
	    if( commandList_[PhysMotionControlWithTrans::SLIDE_RIGHT].on() )
		{
			if ( motion_.drift() < 0.0 )
				 motion_.drift( abs(motion_.drift()) );
			motion_.deltaDrift( metresPerSecond() * elapsedTime );
		}
	    else if( commandList_[PhysMotionControlWithTrans::SLIDE_LEFT].on() )
	    {
	    	if ( motion_.drift() > 0.0 )
				 motion_.drift( -motion_.drift() );
		    motion_.deltaDrift( -metresPerSecond() * elapsedTime );
		}
	    else if( fabs( motion_.drift() ) < metresPerSecond() )
	    {
	        motion_.drift( 0.0 );
	    }
	    else
		{
			double decay = ( motion_.drift() / 4.0 ) - motion_.drift();
	        motion_.drift( motion_.drift() + ( decay * elapsedTime ) );
		}

		// Climb
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

		// Heading
	    if( commandList_[PhysMotionControlWithTrans::ROTATE_RIGHT].on() )
		{
		   	if ( motion_.heading() < 0.0 )
				 motion_.heading( abs(motion_.heading()) );
			motion_.deltaHeading( radiansPerSecond.asScalar() );
	    }
	    else if( commandList_[PhysMotionControlWithTrans::ROTATE_LEFT].on() )
	    {
	       	if ( motion_.heading() > 0.0 )
				 motion_.heading( -motion_.heading() );
		    motion_.deltaHeading( -radiansPerSecond.asScalar() );
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

		// Pitch
	    if( ( commandList_[PhysMotionControlWithTrans::PITCH_UP].on() and not reversePitchUpDownKeys_ ) or
	    	( commandList_[PhysMotionControlWithTrans::PITCH_DOWN].on() and reversePitchUpDownKeys_ ) )
		{
	       	if ( motion_.pitch() > 0.0 )
				 motion_.pitch( -motion_.pitch() );
	        motion_.deltaPitch( -radiansPerSecond.asScalar() );
		}
	    else if( ( commandList_[PhysMotionControlWithTrans::PITCH_DOWN].on() and not reversePitchUpDownKeys_ ) or
	    		 ( commandList_[PhysMotionControlWithTrans::PITCH_UP].on() and reversePitchUpDownKeys_ ) )
	    {
	       	if ( motion_.pitch() < 0.0 )
				 motion_.pitch( abs(motion_.pitch()) );
	        motion_.deltaPitch( radiansPerSecond.asScalar() );
	    }
	    else if( fabs( motion_.pitch() ) < radiansPerSecond.asScalar() )
	    {
	        motion_.pitch( 0.0 );
	    }
	    else
		{
			double decay = ( motion_.pitch() / 4.0 ) - motion_.pitch();
	        motion_.pitch( motion_.pitch() + ( decay * elapsedTime ) );
		}

		if ( commandList_[PhysMotionControlWithTrans::RESET_POS].on() )
			resetPosition();

		if ( commandList_[PhysMotionControlWithTrans::RESET_ORIENTATION].on() )
			resetOrientation();

		const DevKeyboard& keyboard = DevKeyboard::instance();
	    bool shifted = keyboard.shiftPressed();
		if ( not shifted )
		{
			// Reset elevation
			MexTransform3d newTransform = pMotionControlled_->globalTransform();
			MexEulerAngles angles;
			newTransform.rotation( &angles );
			angles.elevation( 0.0 );
			newTransform.rotation( angles );
	        pMotionControlled_->globalTransform( newTransform );
			motion_.pitch( 0.0 );
		}
	}

	// Begin timing the next keyboard repeat.
	keyTimer_.time(0);
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysGroundFlyControl::update()
{
    //Check not frozen
	if( not motionFrozen() )
    {
		//Update deltas from keyboard, and check we have some motion
    	updateMotion();

        //Compute time since last increment
        double elapsed = frameTimer_.time();
        if( DevKeyboard::instance().ctrlPressed() )
            elapsed *= 0.25;

		MexTransform3d newTransform = pMotionControlled_->globalTransform();
		pMotionConstraint_->move( newTransform, motion_, elapsed );
        pMotionControlled_->globalTransform( newTransform );
    }

	resetCommands();

	// Begin timing the next frame.
	frameTimer_.time(0);
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysGroundMotionConstraint::PhysGroundMotionConstraint()
:	maxHeight( 10.0 ),
	minHeight( 1.4 ),
	maxSpeed( 70.0 ),
	minSpeed( -70.0 ),
	maxTurnRate( MexDegrees( 120 ) ),
    minTurnRate( -MexDegrees( 120 ) ),
    maxPitchRate( MexDegrees( 30 ) ),
    minPitchRate( -MexDegrees( 30 ) ),
    maxPitch( MexDegrees( 70 ) ),
    minPitch( -MexDegrees( 70 ) )
{}

// virtual
void PhysGroundMotionConstraint::move( MexTransform3d& xform, PhysMotion& motion, double elapsedTime )
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

	if ( motion.pitch() > maxPitchRate().asScalar() )
		motion.pitch( maxPitchRate().asScalar() );
	else if ( motion.pitch() < minPitchRate().asScalar() )
	    motion.pitch( minPitchRate().asScalar() );

	// Work out new position using an UnconstrainedMotion
	MexTransform3d newTrans( xform );
	PhysUnconstrainedMotion noConstraint;
	noConstraint.move( newTrans, motion, elapsedTime );

	MexPoint3d position = newTrans.position();
	MexEulerAngles angles;
	newTrans.rotation( &angles );

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
	xform.position( position );
	xform.rotation( angles );
}

// virtual
bool PhysGroundMotionConstraint::snapTo( MexPoint3d* location )
{
	// Make sure height restrictions are not being violated
   	if ( location->z() < minHeight() )
		location->z( minHeight() );
	else if ( location->z() > maxHeight() )
		location->z( maxHeight() );

	return true;
}

// virtual
bool PhysGroundMotionConstraint::snapTo( MexTransform3d* trans )
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

void PhysGroundFlyControl::reversePitchUpDownKeys( bool newValue )
{
	reversePitchUpDownKeys_ = newValue;
}

bool PhysGroundFlyControl::reversePitchUpDownKeys() const
{
	return reversePitchUpDownKeys_;
}

/* End GRFLYCTL.CPP *************************************************/
