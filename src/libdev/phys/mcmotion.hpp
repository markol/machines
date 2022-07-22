/*
 * M C M O T I O N . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


#ifndef _PHYS_MCMOTION_HPP
#define _PHYS_MCMOTION_HPP

#include "base/base.hpp"
#include "ctl/list.hpp"
#include "utility/property.hpp"
#include "device/timer.hpp"
#include "device/butevent.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/vec3.hpp"

//class ostream;
class PhysMotionConstraint;	// defined below
class MexTransform3d;
//class DevButtonEvent;
class MexVec2;

// This needs a better name because it's very plausible that you
// will need different motions, alligned along different axes,
// say, x-y-z.
class PhysMotion
{
public:
	// Various speeds of movement.  Speed is forwards/backwards
	// along the line of sight.  Drift is sideways, i.e. perpendicular
	// to this speed and to the up vector.
	UtlProperty<double> speed, climb, drift, heading, roll, pitch;

	// Directions of motion defined:
	// 1) forward motion is along the forwards vector
	// 2) up is hardwired to be along +ve z
	// 3) -ve drift is leftwards; left is along the cross product of
	//    forwards and +z, unless the co-ordinate system is left-handed
	//    which will negate the direction
	// 4) +ve pitch moves from forwards towards +z
	// 5) -ve heading moves from forwards to left
	// PRE(MexCoordSystem::instance().isSet());
	PhysMotion(const MexVec2& forwards);

	void stopMoving();
	void stopRotating();

	void deltaSpeed(double);
	void deltaClimb(double);
	void deltaDrift(double);
	void deltaHeading(double);
	void deltaRoll(double);
	void deltaPitch(double);

	const MexVec3& forwards() const;
	const MexVec3& left() const;

	const MexVec3& headingAxis() const;
	const MexVec3& pitchAxis() const;

private:
	friend ostream& operator<<( ostream&, const PhysMotion& );

	const MexVec3 forwards_, left_, headingAxis_, pitchAxis_;
};

// Abstract base class for different things that are moved around via a PhysMotionControl.
class PhysMotionControlled
{
public:
	PhysMotionControlled();
	virtual ~PhysMotionControlled();

	virtual void setPosition( const MexTransform3d& ) = 0;
	virtual void setOrientation( const MexTransform3d& ) = 0;

	virtual MexTransform3d globalTransform() const = 0;
	virtual void globalTransform( const MexTransform3d& ) = 0;
	virtual MexTransform3d localTransform() const = 0;
	virtual void localTransform( const MexTransform3d& ) = 0;

private:
	// Operations deliberately revoked
    PhysMotionControlled( const PhysMotionControlled& );
    const PhysMotionControlled& operator =( const PhysMotionControlled& );
    bool operator ==( const PhysMotionControlled& ) const;
};

class PhysMotionControlledTransform	: public PhysMotionControlled
{
public:
	PhysMotionControlledTransform( MexTransform3d* pTransform );
	//PRE( pTransform );

	virtual ~PhysMotionControlledTransform();

	virtual void setPosition( const MexTransform3d& );
	virtual void setOrientation( const MexTransform3d& );

	virtual MexTransform3d globalTransform() const;
	virtual void globalTransform( const MexTransform3d& );
	virtual MexTransform3d localTransform() const;
	virtual void localTransform( const MexTransform3d& );

private:
	MexTransform3d* pMotionControlledTransform_;

	// Operations deliberately revoked
    PhysMotionControlledTransform( const PhysMotionControlledTransform& );
    const PhysMotionControlledTransform& operator =( const PhysMotionControlledTransform& );
    bool operator ==( const PhysMotionControlledTransform& ) const;
};

// Base class for different types of keyboard-driven movement controls.
class PhysMotionControl
{
public:
    virtual ~PhysMotionControl();

	// Look for keypresses and modify motion as appropriate.
	virtual void update() = 0;

	// Update all the controls, so that their targets continue
	// to move, even if the controls are decoupled from input.
	static void updateAll();

	// A control can be decoupled from the keyboard, effectivley
	// becoming inactive.  When one control is enabled, all others
	// in existance are disabled -- they can't share the keyboard.
	virtual void enableInput();
	virtual void disableInput();
	bool inputEnabled() const;

	// Directs button events to currently active MotionControl
	static bool processButtonEvent( const DevButtonEvent& );

	void freezeMotion();
	void resumeMotion();
	bool motionFrozen() const;

	// Provide a default position.
	void setDefaultPosition(const MexTransform3d& xform);

	// Resets both location (i.e. x,y,z) and orientation
	void resetPosition();

	// Resets just the orientation (i.e. does not affect x,y,z)
	void resetOrientation();

	// The forwards direction defaults to MexVec2(1,0).
	static const MexVec2& defaultForwards();

protected:
	// Note that target will be deleted by PhysMotionControl and should be
	// allocated on the heap.
    PhysMotionControl(PhysMotionControlled* target);
	// PRE(MexCoordSystem::instance().isSet());
	// PRE( target );
	// POST( not enabled_ );

	virtual bool doProcessButtonEvent( const DevButtonEvent& );

	// data members
	PhysMotionControlled* const pMotionControlled_; // What this control is acting upon.
	DevTimer frameTimer_, keyTimer_;

private:
	// data members
	bool enabled_, frozen_;
	MexTransform3d default_;

	static ctl_list< PhysMotionControl* > allControls_;
	static PhysMotionControl* pActiveMotionControl_;

    // Operations deliberately revoked
    PhysMotionControl( const PhysMotionControl& );
    const PhysMotionControl& operator =( const PhysMotionControl& );
    bool operator ==( const PhysMotionControl& ) const;
};

// Apply a motion in order to move an actor.  The movement function
// can be impelmented so that the actor is prevented from moving in
// an arbirary way, as defined by dervied classes.
class PhysMotionConstraint
{
public:
	virtual ~PhysMotionConstraint() {}
	virtual void move(MexTransform3d&, PhysMotion&, double elapsedTime) = 0;

	// Called when there is a request to move an actor to a new location. Gives the
	// motion constraint the chance to either modify the location slightly or
	// return false if the location is not acceptable.
	virtual bool snapTo( MexPoint3d* location ) = 0;

	virtual bool snapTo( MexTransform3d* trans ) = 0;
};

// Moves the actor as described by the given motion with no
// constraints of any sort.
class PhysUnconstrainedMotion : public PhysMotionConstraint
{
public:
	virtual void move( MexTransform3d&, PhysMotion&, double elapsedTime );
	virtual bool snapTo( MexPoint3d* location );
	virtual bool snapTo( MexTransform3d* trans );
};

// Don't let the target object travel outside of the given altitude range.
// Gracefully brake when it is about to exceed the limits.
class PhysAltitudeBrake : public PhysMotionConstraint
{
public:
	// Use HUGE_VAL if only an upper or lower constrain is required.
	PhysAltitudeBrake(double minAlt, double maxAlt, double tolerance);
	virtual void move(MexTransform3d&, PhysMotion&, double elapsed_time);

private:
	const double minAlt_, maxAlt_, tolerance_;
	PhysUnconstrainedMotion unconstrained_;
};

// Don't let the target object travel outside of the given altitude range.
// Just clamp the vertical position when the constrains are violated.
class PhysAltitudeClamp : public PhysMotionConstraint
{
public:
	// Use HUGE_VAL if only an upper or lower constrain is required.
	PhysAltitudeClamp(double minAlt, double maxAlt);
	virtual void move(MexTransform3d&, PhysMotion&, double elapsed_time);

private:
	const double minAlt_, maxAlt_;
	PhysUnconstrainedMotion unconstrained_;
};

// A keyboard-driven movement control which a 2D interface for looking
// straight down at the ground in plan-view.
class PhysPlanControl : public PhysMotionControl
{
public:
    PhysPlanControl(PhysMotionControlled* target);
	// POST(not MotionControl::enabled_);

	// Look for keypresses and modify motion as appropriate.
	virtual void update();

	UtlProperty<double> metersPerSecond;

private:
	void readKeyboard();
	double dx_, dy_;
};

#endif
