/*
 * M C W T R A N S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysMotionControlWithTrans

    A brief description of the class should go in here
*/

#ifndef _PHYS_MCWTRANS_HPP
#define _PHYS_MCWTRANS_HPP

#include "base/base.hpp"
#include "phys/mcmotion.hpp"
#include "device/keytrans.hpp"

class PhysMotionControlWithTrans : public PhysMotionControl
// Canonical form revoked
{
public:
	enum Command { 	FOWARD, BACKWARD, 
					UP, DOWN, 
					SLIDE_LEFT, SLIDE_RIGHT, 
					PITCH_UP, PITCH_DOWN, 
					ROTATE_LEFT, ROTATE_RIGHT, 
					ROLL_LEFT, ROLL_RIGHT,
					RESET_POS, RESET_ORIENTATION,
					NUM_COMMANDS /*Must come last*/ };

    virtual ~PhysMotionControlWithTrans();

	void setKeyTranslator( DevKeyToCommandTranslator* );
	// PRE( pTranslator );

	// Supply an abstract object which will limit the object's movement.
	// The control will delete the constraint when finished with it.
	// Don't share constraints amongst controls.
	void imposeConstraint( PhysMotionConstraint* );
	// PRE( pMotionConstraint );

    //Snap the motionControlled object to location. Retain old angles.
    virtual void snapTo( const MexPoint3d& location );

    //Snap the motionControlled object to location.
    virtual void snapTo( const MexTransform3d& trans );

	// Must be called before MotionControl is used to ensure that the correct
	// keyboard events are queued in DevEventQueue. 
	void initEventQueue();

	virtual void disableInput();

	void forceCommandOn( Command );

    void CLASS_INVARIANT;

protected:
    PhysMotionControlWithTrans( PhysMotionControlled*, const MexVec2& forwards );
	// PRE( pMotionControlled );

	PhysMotionControlWithTrans( PhysMotionControlled*, PhysMotionConstraint*, const MexVec2& forwards );
	// PRE( pMotionControlled );
	// PRE( pMotionConstraint );

	virtual bool doProcessButtonEvent( const DevButtonEvent& );

	void resetCommands();

	// data members
	DevKeyToCommandTranslator* pKeyTranslator_;
    PhysMotionConstraint* pMotionConstraint_;
    PhysMotion motion_;
    
    DevKeyToCommandTranslator::CommandList commandList_;

private:
	// operations revoked
    PhysMotionControlWithTrans( const PhysMotionControlWithTrans& );
    PhysMotionControlWithTrans& operator =( const PhysMotionControlWithTrans& );
    bool operator ==( const PhysMotionControlWithTrans& );

    friend ostream& operator <<( ostream& o, const PhysMotionControlWithTrans& t );
};


#endif

/* End MCWTRANS.HPP *************************************************/
