/*
 * Z E N I T H F L . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysZenithFlyControl

    Controls for moving a transform around in a zenith view
*/

#ifndef _PHYS_ZENITFL_HPP
#define _PHYS_ZENITFL_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "mathex/eulerang.hpp"
#include "phys/mcwtrans.hpp"
#include "phys/brake.hpp"

#include "gui/gui.hpp"
#include "machgui/cameras.hpp"

class PhysZenithFlyControl : public PhysMotionControlWithTrans
{
public:
	// *All* ctors share these pre- and post-conditions.
	// The forwards direction defaults to the +ve x axis.
	// PRE(PhysMotion::coordinateSystem().isSet()); (from base)
	// PRE( pMotionControlled );
	// POST( metresPerSecond() == 10.0 );
	// POST( degreesPerSecond() == 5.0 );
    PhysZenithFlyControl( PhysMotionControlled* );
    PhysZenithFlyControl( PhysMotionControlled*, const MexVec2& forwards );

	// PRE( pMotionConstraint );
	PhysZenithFlyControl( PhysMotionControlled*, PhysMotionConstraint* );
	// PRE( pMotionConstraint );
	PhysZenithFlyControl( PhysMotionControlled*, PhysMotionConstraint*, const MexVec2& forwards );
  
    virtual ~PhysZenithFlyControl( void );

	// The speed at which the motionControlled object will accelerate
	UtlProperty<double> metresPerSecond;

	// Controls the rotation acceleration of the motionControlled object
	UtlProperty<MexDegrees> degreesPerSecond;

	virtual void update( void );

	void pitchAngle( MexRadians& );

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    PhysZenithFlyControl( const PhysZenithFlyControl& );
    PhysZenithFlyControl& operator =( const PhysZenithFlyControl& );
    bool operator ==( const PhysZenithFlyControl& );

    void updateMotion( void );
	void setupDefaultKeyboardMapping( void );
	void ctor(PhysMotionControlled* pMotionControlled);

	// data members
    MexRadians pitchAngle_; //Fixed pitch angle

    friend void MachCameras::scrollWithWheel(const Gui::ScrollState, const double);
};

class PhysZenithMotionConstraint : public PhysMotionConstraint
{
public:
	PhysZenithMotionConstraint();
	virtual ~PhysZenithMotionConstraint() {}
	
	virtual void move(MexTransform3d&, PhysMotion&, double elapsedTime);

	// Called when there is a request to move an actor to a new location. Gives the 
	// motion constraint the chance to either modify the location slightly or
	// return false if the location is not acceptable.
	virtual bool snapTo( MexPoint3d* location );
	virtual bool snapTo( MexTransform3d* trans );

	UtlProperty< MATHEX_SCALAR > maxHeight;
	UtlProperty< MATHEX_SCALAR > minHeight;
	UtlProperty< MATHEX_SCALAR > maxSpeed;
	UtlProperty< MATHEX_SCALAR > minSpeed;
	UtlProperty< MexRadians > maxTurnRate;
	UtlProperty< MexRadians > minTurnRate;
};

#endif

/* End ZENITCAM.HPP *************************************************/
