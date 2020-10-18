/*
 * M C F L Y . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "mathex/eulerang.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/quatern.hpp"
#include "device/keyboard.hpp"
#include "phys/mcfly.hpp"

PhysFlyControl::PhysFlyControl( PhysMotionControlled* pMotionControlled )
:	PhysMotionControlWithTrans( pMotionControlled, defaultForwards() )
{
	ctor();
}

PhysFlyControl::PhysFlyControl( PhysMotionControlled* pMotionControlled,
								PhysMotionConstraint* pMotionConstraint )
:	PhysMotionControlWithTrans( pMotionControlled, pMotionConstraint, defaultForwards() )
{
	ctor();
}

PhysFlyControl::PhysFlyControl( PhysMotionControlled* pMotionControlled, const MexVec2& forwards )
:	PhysMotionControlWithTrans( pMotionControlled, forwards )
{
	ctor();
}

PhysFlyControl::PhysFlyControl( PhysMotionControlled* pMotionControlled,
								PhysMotionConstraint* pMotionConstraint,
								const MexVec2& forwards )
:	PhysMotionControlWithTrans( pMotionControlled, pMotionConstraint, forwards )
{
	ctor();
}

// Do the initialisation work common to all the constructors.
void PhysFlyControl::ctor()
{
	metresPerSecond(8.0);
	degreesPerSecond(3.0);

	// Two new commands required for motion/rotation stop
	commandList_.push_back( DevKeyToCommandTranslator::Command() );
	commandList_.push_back( DevKeyToCommandTranslator::Command() );

	useDefaultKeyboardMapping();
}

PhysFlyControl::~PhysFlyControl()
{}

void PhysFlyControl::useDefaultKeyboardMapping()
{
	pKeyTranslator_ = _NEW( DevKeyToCommandTranslator() );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::ENTER_PAD, PhysMotionControlWithTrans::FOWARD, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_DELETE, PhysMotionControlWithTrans::BACKWARD, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::END, PhysMotionControlWithTrans::SLIDE_LEFT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::PLUS_PAD, PhysMotionControlWithTrans::UP, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::MINUS_PAD, PhysMotionControlWithTrans::DOWN, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::HOME, PhysMotionControlWithTrans::ROLL_LEFT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::INSERT, STOP_MOVING, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::PAD_5, STOP_ROTATING, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_R, PhysMotionControlWithTrans::RESET_POS, DevKeyToCommand::EITHER, DevKeyToCommand::PRESSED, DevKeyToCommand::EITHER  ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::KEY_R, PhysMotionControlWithTrans::RESET_ORIENTATION, DevKeyToCommand::EITHER, DevKeyToCommand::RELEASED, DevKeyToCommand::EITHER  ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::LEFT_ARROW, PhysMotionControlWithTrans::ROTATE_LEFT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::RIGHT_ARROW, PhysMotionControlWithTrans::ROTATE_RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::DOWN_ARROW, PhysMotionControlWithTrans::PITCH_UP, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::UP_ARROW, PhysMotionControlWithTrans::PITCH_DOWN, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::PAGE_UP, PhysMotionControlWithTrans::ROLL_RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator_->addTranslation( DevKeyToCommand( DevKey::PAGE_DOWN, PhysMotionControlWithTrans::SLIDE_RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
}

void PhysFlyControl::useKeypadMapping()
{
    DevKeyToCommandTranslator* pKeyTranslator = _NEW( DevKeyToCommandTranslator() );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::ENTER_PAD, PhysMotionControlWithTrans::FOWARD, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::DELETE_PAD, PhysMotionControlWithTrans::BACKWARD, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_1, PhysMotionControlWithTrans::SLIDE_LEFT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_3, PhysMotionControlWithTrans::SLIDE_RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_4, PhysMotionControlWithTrans::ROTATE_LEFT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_6, PhysMotionControlWithTrans::ROTATE_RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PLUS_PAD, PhysMotionControlWithTrans::UP, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::MINUS_PAD, PhysMotionControlWithTrans::DOWN, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_2, PhysMotionControlWithTrans::PITCH_UP, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_8, PhysMotionControlWithTrans::PITCH_DOWN, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_7, PhysMotionControlWithTrans::ROLL_LEFT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_9, PhysMotionControlWithTrans::ROLL_RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_0, PhysFlyControl::STOP_MOVING, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAD_5, PhysFlyControl::STOP_ROTATING, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::KEY_R, PhysMotionControlWithTrans::RESET_POS, DevKeyToCommand::EITHER, DevKeyToCommand::PRESSED, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::KEY_R, PhysMotionControlWithTrans::RESET_ORIENTATION, DevKeyToCommand::EITHER, DevKeyToCommand::RELEASED, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::LEFT_ARROW, PhysMotionControlWithTrans::ROTATE_LEFT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::RIGHT_ARROW, PhysMotionControlWithTrans::ROTATE_RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::DOWN_ARROW, PhysMotionControlWithTrans::PITCH_UP, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::UP_ARROW, PhysMotionControlWithTrans::PITCH_DOWN, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAGE_UP, PhysMotionControlWithTrans::ROLL_RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );
	pKeyTranslator->addTranslation( DevKeyToCommand( DevKey::PAGE_DOWN, PhysMotionControlWithTrans::SLIDE_RIGHT, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER, DevKeyToCommand::EITHER  ) );

	setKeyTranslator( pKeyTranslator );
	initEventQueue();
}

void PhysFlyControl::updateMotion()
{
	if ( inputEnabled() )
	{
		double elapsedTime = keyTimer_.time() * 10.0;

		if ( commandList_[STOP_MOVING].on() )
			stopMoving();
		if ( commandList_[STOP_ROTATING].on() )
			stopRotating();

		if ( commandList_[PhysMotionControlWithTrans::FOWARD].on() )
			motion_.deltaSpeed(metresPerSecond() * elapsedTime);
		if ( commandList_[PhysMotionControlWithTrans::BACKWARD].on() )
			motion_.deltaSpeed(-metresPerSecond() * elapsedTime);

		if ( commandList_[PhysMotionControlWithTrans::SLIDE_RIGHT].on() )
			motion_.deltaDrift(metresPerSecond() * elapsedTime);
 		if ( commandList_[PhysMotionControlWithTrans::SLIDE_LEFT].on() )
			motion_.deltaDrift(-metresPerSecond() * elapsedTime);

		if ( commandList_[PhysMotionControlWithTrans::DOWN].on() )
			motion_.deltaClimb(-metresPerSecond() * elapsedTime);
		if ( commandList_[PhysMotionControlWithTrans::UP].on() )
			motion_.deltaClimb(metresPerSecond() * elapsedTime);

		if ( commandList_[PhysMotionControlWithTrans::ROTATE_RIGHT].on() )
			motion_.deltaHeading(radiansPerSecond() * elapsedTime);
		if ( commandList_[PhysMotionControlWithTrans::ROTATE_LEFT].on() )
			motion_.deltaHeading(-radiansPerSecond() * elapsedTime);

		if ( commandList_[PhysMotionControlWithTrans::PITCH_DOWN].on() )
			motion_.deltaPitch(-radiansPerSecond() * elapsedTime);
		if ( commandList_[PhysMotionControlWithTrans::PITCH_UP].on() )
			motion_.deltaPitch(radiansPerSecond() * elapsedTime);

		if ( commandList_[PhysMotionControlWithTrans::ROLL_LEFT].on() )
			motion_.deltaRoll(-radiansPerSecond() * elapsedTime);
		if ( commandList_[PhysMotionControlWithTrans::ROLL_RIGHT].on() )
			motion_.deltaRoll(radiansPerSecond() * elapsedTime);

		if ( commandList_[PhysMotionControlWithTrans::RESET_POS].on() )
			resetPosition();

		if ( commandList_[PhysMotionControlWithTrans::RESET_ORIENTATION].on() )
			resetOrientation();

	}
	keyTimer_.time(0);
}

// Look for keypresses and modify motion as appropriate.
// virtual
void PhysFlyControl::update()
{
	if (motionFrozen())
		return;

	updateMotion();

	// Allow the motion constraint to modify our movement for collision
	// detection etc.
	const double elapsed = frameTimer_.time();

	MexTransform3d xform = pMotionControlled_->globalTransform();
	pMotionConstraint_->move(xform, motion_, elapsed);
	pMotionControlled_->globalTransform(xform);

	IAIN_STREAM(std::setprecision(3) << std::setw(4) << motion_ << "\n");
	IAIN_STREAM("frame time=" << elapsed << "\n");

	resetCommands();

	// Begin timing the next frame.
	frameTimer_.time(0);
}

void PhysFlyControl::stopMoving()
{
	motion_.stopMoving();
}

void PhysFlyControl::stopRotating()
{
	motion_.stopRotating();
}

double PhysFlyControl::radiansPerSecond() const
{
	return degreesPerSecond() / 180 * Mathex::PI;
}
