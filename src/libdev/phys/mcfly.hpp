/*
 * M C F L Y . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _PHYS_MCFLY_HPP
#define _PHYS_MCFLY_HPP

#include "base/base.hpp"
#include "ctl/list.hpp"
#include "utility/property.hpp"
#include "device/timer.hpp"
#include "mathex/transf3d.hpp"
#include "phys/mcwtrans.hpp"

//class ostream;

// A flight-simulation type of control with six degrees of freedom.
class PhysFlyControl : public PhysMotionControlWithTrans
{
public:
	enum Command { STOP_ROTATING = PhysMotionControlWithTrans::NUM_COMMANDS, STOP_MOVING };

	// *All* ctors share these pre- and post-conditions.
	// PRE(PhysMotion::coordinateSystem().isSet()); (from base)
	// PRE( target );
	// POST( not MotionControl::enabled_ );
	// POST( metresPerSecond() == 8.0 );
	// POST( degreesPerSecond() == 3.0 );
	// The forwards direction defaults to the +ve x axis.
    PhysFlyControl( PhysMotionControlled* target);
    PhysFlyControl( PhysMotionControlled* target, const MexVec2& forwards);

	// PRE( pMotionConstraint );
	PhysFlyControl( PhysMotionControlled* target, PhysMotionConstraint* pMotionConstraint);
	// PRE( pMotionConstraint );
	PhysFlyControl( PhysMotionControlled* target, PhysMotionConstraint* pMotionConstraint, const MexVec2& forwards);

    virtual ~PhysFlyControl();

	// Look for keypresses and modify motion as appropriate.
	virtual void update();

	void stopMoving();
	void stopRotating();

	// Rates of movement.
	// The rate at which we translate may need to be specified by the application,
	// depending on the scale of the world.  Rate of rotation is not affected by
	// scale in this fashion, but you can set it anyway for completeness.
	UtlProperty<double> degreesPerSecond;
	UtlProperty<double> metresPerSecond;

	// This class offers two basic keyboard layouts: the original based around
	// the keypad and the Machines non-keypad version.  You can in any case
	// pick something entirely different by calling setKeyTranslator.
	void useDefaultKeyboardMapping();
	void useKeypadMapping();

private:
	void updateMotion();
	double radiansPerSecond() const;
	void ctor();

    // Operation deliberately revoked
    PhysFlyControl( const PhysFlyControl& );
    const PhysFlyControl& operator =( const PhysFlyControl& );
    bool operator ==( const PhysFlyControl& ) const;
};

#endif
