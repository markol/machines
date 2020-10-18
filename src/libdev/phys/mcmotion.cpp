/*
 * M C M O T I O N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include <iostream>
#include <iomanip>
#include <algorithm>
#include "mathex/eulerang.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec2.hpp"
#include "mathex/vec3.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/quatern.hpp"
#include "mathex/coordsys.hpp"
#include "device/keyboard.hpp"
#include "device/butevent.hpp"
#include "phys/mcmotion.hpp"
#include "device/keytrans.hpp"

///////////////////////////////////////////////////////////////////////////
// static
ctl_list<PhysMotionControl*> PhysMotionControl::allControls_;
PhysMotionControl* PhysMotionControl::pActiveMotionControl_ = NULL;

PhysMotionControlled::PhysMotionControlled()
{}

// virtual
PhysMotionControlled::~PhysMotionControlled()
{}

PhysMotionControl::PhysMotionControl(PhysMotionControlled* t):
	pMotionControlled_(t),
	enabled_(false),
	frozen_(false)
{
	PRE(MexCoordSystem::instance().isSet());
	PRE( t );

	allControls_.push_front(this);
	default_ = t->globalTransform();

	POST(not enabled_);
}

PhysMotionControl::~PhysMotionControl()
{
	ASSERT(find(allControls_.begin(), allControls_.end(), this) != allControls_.end(), logic_error());
	allControls_.remove(this);
	_DELETE( pMotionControlled_ );
}

static void update_ctrl(PhysMotionControl* ctrl)
{
	if (ctrl)
		ctrl->update();
}

// Update all the controls, so that their targets continue
// to move, even if the controls are decoupled from input.

// static
void PhysMotionControl::updateAll()
{
	for_each(allControls_.begin(), allControls_.end(), update_ctrl);
}

static void disable_ctrl(PhysMotionControl* ctrl)
{
	if (ctrl)
		ctrl->disableInput();
}

// static
bool PhysMotionControl::processButtonEvent( const DevButtonEvent& buttonEvent )
{
	if ( pActiveMotionControl_ )
	{
		 return pActiveMotionControl_->doProcessButtonEvent( buttonEvent );
	}
	else
		return false;
}

// virtual
bool PhysMotionControl::doProcessButtonEvent( const DevButtonEvent& )
{
    return false;
}

// A control can be decoupled from the keyboard, effectivley
// becoming inactive.  When one control is enabled, all others
// in existance are disabled -- they can't share the keyboard.
// virtual
void PhysMotionControl::enableInput()
{
	for_each(allControls_.begin(), allControls_.end(), disable_ctrl);
	enabled_ = true;
	pActiveMotionControl_ = this;
}

// virtual
void PhysMotionControl::disableInput()
{
	enabled_ = false;
	if ( pActiveMotionControl_ == this )
		pActiveMotionControl_ = NULL;
}

bool PhysMotionControl::inputEnabled() const
{
	return enabled_;
}

void PhysMotionControl::setDefaultPosition(const MexTransform3d& xform)
{
	default_ = xform;
}

void PhysMotionControl::resetPosition()
{
	pMotionControlled_->setPosition( default_ );
}

void PhysMotionControl::resetOrientation()
{
	pMotionControlled_->setOrientation( default_ );
}

void PhysMotionControl::freezeMotion()
{
	frozen_ = true;
}

void PhysMotionControl::resumeMotion()
{
	frozen_ = false;
}

bool PhysMotionControl::motionFrozen() const
{
	return frozen_;
}

// static
const MexVec2& PhysMotionControl::defaultForwards()
{
	static const MexVec2 f(1,0);
	return f;
}

// Left is the cross product of forwards and up unless the co-ordinate
// system is left-handed.
static MexVec3 computeLeft(const MexVec3& forwards)
{
	MexVec3 result = forwards, up(0,0,1);
	result.crossProduct(up);

	if (MexCoordSystem::instance() == MexCoordSystem::LEFT_HANDED)
		result *= -1;

	return result;
}

// Defines an axis for pitch such that a +ve rotation will move from
// forwards towards +ve z.
static MexVec3 computePitchAxis(const MexVec3& forwards)
{
	MexVec3 result = forwards, up(0,0,1);
	result.crossProduct(up);
	return result;
}

// Defines an axis for heading such that a -ve rotation will move from
// forwards towards left.
static MexVec3 computeHeadingAxis(const MexVec3& forwards, const MexVec3& left)
{
	MexVec3 result = forwards;
	result.crossProduct(left);
	result *= -1;
	return result;
}

PhysMotion::PhysMotion(const MexVec2& forwards):
	forwards_(forwards.x(), forwards.y(), 0),
	left_(computeLeft(forwards_)),
	pitchAxis_(computePitchAxis(forwards_)),
	headingAxis_(computeHeadingAxis(forwards_, left_))
{
	PRE(MexCoordSystem::instance().isSet());
	stopMoving();
}

void PhysMotion::stopMoving()
{
	speed(0);
	climb(0);
	drift(0);
	stopRotating();
}

void PhysMotion::stopRotating()
{
	heading(0);
	pitch(0);
	roll(0);
}

void PhysMotion::deltaSpeed(double d)
{
	speed(speed() + d);
}

void PhysMotion::deltaClimb(double d)
{
	climb(climb() + d);
}

void PhysMotion::deltaDrift(double d)
{
	drift(drift() + d);
}

void PhysMotion::deltaHeading(double d)
{
	heading(heading() + d);
}

void PhysMotion::deltaPitch(double d)
{
	pitch(pitch() + d);
}

void PhysMotion::deltaRoll(double d)
{
	roll(roll() + d);
}

const MexVec3& PhysMotion::forwards() const
{
	return forwards_;
}

const MexVec3& PhysMotion::left() const
{
	return left_;
}

const MexVec3& PhysMotion::headingAxis() const
{
	return headingAxis_;
}

const MexVec3& PhysMotion::pitchAxis() const
{
	return pitchAxis_;
}

ostream& operator<<(ostream& ostr, const PhysMotion& m)
{
	ostr <<  "s=" << m.speed()   << " c=" << m.climb() << " d=" << m.drift();
	ostr << " h=" << m.heading() << " p=" << m.pitch() << " r=" << m.roll();
	return ostr;
}

// virtual
bool PhysUnconstrainedMotion::snapTo( MexPoint3d* )
{
	return true;
}

// virtual
bool PhysUnconstrainedMotion::snapTo( MexTransform3d* )
{
	return true;
}

// virtual
void PhysUnconstrainedMotion::move(	MexTransform3d& xform,		// what to move
									PhysMotion&   	motion,		// how to move it
									double    		elapsed	)	// the elapsed time since last move
{
	MexVec3 pos = xform.position();

	// All the speeds are in units per second, so multiply them
	// by the elapsed time since the last update.

	// Pivot around a vertical axis through our current position: translate
	// to the origin, rotate, translate back again (and throw in climb at
	// the same time).
	// To achieve the correct effect, the matrix multiplication order is
	// reversed.  This means we must also negate the sense of the rotation,
	// hence the minus sign.
	xform.position(MexVec3(0,0,0));
	MexQuaternion headQuat(motion.headingAxis(), -motion.heading() * elapsed);
	MexTransform3d rotZXform(headQuat);
	rotZXform.transform(xform);
	pos.z(pos.z() + motion.climb() * elapsed);
	rotZXform.position(pos);

	MexVec3 forwards = motion.forwards();
	MexVec3 drift   = motion.left();
	forwards *= (motion.speed() * elapsed);
	drift    *= (motion.drift() * elapsed);

    MexVec3 p = forwards;
	p += drift;
	rotZXform.translate(p);

	MexQuaternion pitchQuat(motion.pitchAxis(), motion.pitch() * elapsed);
	MexQuaternion  rollQuat(motion.forwards(),  motion.roll()  * elapsed);
	rotZXform.rotate(pitchQuat);
	rotZXform.rotate(rollQuat);

	xform = rotZXform;
}

PhysAltitudeBrake::PhysAltitudeBrake(double min, double max, double t):
	minAlt_(min),
	maxAlt_(max),
	tolerance_(t)
{
}

// virtual
void PhysAltitudeBrake::move
(
	MexTransform3d& xform,	// what to move
	PhysMotion& 	motion,	// how to move it
	double    		elapsed	// the elapsed time since last move
)
{
	const double old_z = xform.position()[2];
	MexTransform3d copy = xform;
	unconstrained_.move(copy, motion, elapsed);
	double new_z = copy.position()[2];

	bool applyBrakes = false;
	double scale;

	// Don't brake if we're at minAlt_ and start ascending.
	if ((new_z - minAlt_) < tolerance_ && new_z < old_z)
	{
		applyBrakes = true;

		double margin = new_z - minAlt_;
		if (margin < 0)
			margin = 0;
		scale = margin / tolerance_;
	}

	if ((maxAlt_ - new_z) < tolerance_ && new_z > old_z)
	{
		applyBrakes = true;

		double margin = maxAlt_ - new_z;
		if (margin < 0)
			margin = 0;
		scale = margin / tolerance_;
	}

	// If we're within either slow-down band, apply the brakes and do the
	// calculation again.
	if (!applyBrakes)
	{
		xform = copy;
	}
	else
	{
		motion.speed(motion.speed() * scale);
		motion.climb(motion.climb() * scale);
		motion.drift(motion.drift() * scale);
		unconstrained_.move(xform, motion, elapsed);
	}

	// Clamp the altitude just in case the above calculation doesn't properly
	// constrain the motion (it could if the initial speed is huge).
	if (new_z < minAlt_)
		new_z = minAlt_;
	if (new_z > maxAlt_)
		new_z = maxAlt_;

	MexVec3 pos = xform.position();
	pos[2] = new_z;
	xform.position(pos);
}

PhysAltitudeClamp::PhysAltitudeClamp(double min, double max):
	minAlt_(min),
	maxAlt_(max)
{
}

// virtual
void PhysAltitudeClamp::move
(
	MexTransform3d& xform,	// what to move
	PhysMotion&   	motion,	// how to move it
	double    		elapsed	// the elapsed time since last move
)
{
	unconstrained_.move(xform, motion, elapsed);
	MexVec3 pos = xform.position();

	// Clamp the altitude.
	if (pos[2] < minAlt_)
		pos[2] = minAlt_;
	if (pos[2] > maxAlt_)
		pos[2] = maxAlt_;

	xform.position(pos);
}

///////////////////////////////////////////////////////////////////////////
PhysPlanControl::PhysPlanControl(PhysMotionControlled* t):
	PhysMotionControl(t),
	dx_(0),
	dy_(0)
{
	metersPerSecond(8);
}

void PhysPlanControl::readKeyboard()
{
	const double repeatInterval = 0.1;

	if (inputEnabled() and keyTimer_.time() > repeatInterval)
	{
		const DevKeyboard& keyboard = DevKeyboard::instance();

		if (keyboard.keyCode(DevKey::PAD_0) || keyboard.keyCode(DevKey::PAD_5))
		{
			dx_ = 0;
			dy_ = 0;
		}

		if (keyboard.keyCode(DevKey::RIGHT_ARROW_PAD))
			dx_ += metersPerSecond();
		if (keyboard.keyCode(DevKey::LEFT_ARROW_PAD))
			dx_ -= metersPerSecond();

		if (keyboard.keyCode(DevKey::UP_ARROW_PAD))
			dy_ += metersPerSecond();
		if (keyboard.keyCode(DevKey::DOWN_ARROW_PAD))
			dy_ -= metersPerSecond();

		// If any key is pressed, begin timing the next keyboard repeat.
		if (keyboard.anyKey())
			keyTimer_.time(0);
	}
}

// virtual
void PhysPlanControl::update()
{
	if (motionFrozen())
		return;

	readKeyboard();

	MexTransform3d xform = pMotionControlled_->localTransform();
	MexVec3 pos = xform.position();
	pos[0] += dx_;
	pos[1] += dy_;

	xform.position(pos);
	pMotionControlled_->localTransform(xform);
}

PhysMotionControlledTransform::PhysMotionControlledTransform( MexTransform3d* pTransform )
: pMotionControlledTransform_( pTransform )
{
	PRE( pTransform );
}

// virtual
PhysMotionControlledTransform::~PhysMotionControlledTransform()
{}

// virtual
void PhysMotionControlledTransform::setPosition( const MexTransform3d& trans )
{
	*pMotionControlledTransform_ = trans;
}

// virtual
void PhysMotionControlledTransform::setOrientation( const MexTransform3d& trans )
{
	MexPoint3d oldPos = pMotionControlledTransform_->position();

	MexEulerAngles transAngles;
	trans.rotation( &transAngles );

	MexTransform3d newXform( transAngles, oldPos );
	*pMotionControlledTransform_ = newXform;
}

// virtual
MexTransform3d PhysMotionControlledTransform::globalTransform( void ) const
{
	return *pMotionControlledTransform_;
}

// virtual
void PhysMotionControlledTransform::globalTransform( const MexTransform3d& trans )
{
	*pMotionControlledTransform_ = trans;
}

// virtual
MexTransform3d PhysMotionControlledTransform::localTransform( void ) const
{
	return *pMotionControlledTransform_;
}

// virtual
void PhysMotionControlledTransform::localTransform( const MexTransform3d& trans )
{
	*pMotionControlledTransform_ = trans;
}
