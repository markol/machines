/*
 * A X I S T U R N . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dAxisTurnerPlan

    A motion plan that causes rotation about a principal axis to occur.
    Supply base transform.
    This is a semi-reactive plan, and can be updated on the fly.
    Uses current time to evaluate result.
*/

#ifndef _W4D_AXISTURN_HPP
#define _W4D_AXISTURN_HPP

#include "base/base.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/radians.hpp"
#include "phys/motplan.hpp"
#include "world4d/world4d.hpp"

class W4dAxisTurnerPlan : public PhysMotionPlan
// Canonical form revoked
{
public:
    //Defines rotation about the turningAxis principal axis of baseTransform.
    //The motion starts at startTime, turns through turnAngle (which can be signed and non-integral
    //multiples of 360 degrees), turning with rate turnRate.
    //the plan will persist even after motion has ended for duration.
    W4dAxisTurnerPlan( const MexTransform3d& baseTransform, W4d::Axis turningAxis, const PhysAbsoluteTime& startTime,
                    MexRadians startAngle, MexRadians turnAngle, MexRadians turnRate,
                    const PhysRelativeTime& duration );
    //PRE( axisIndex < 3 );

    virtual ~W4dAxisTurnerPlan();

    //Change motion to use starting angle and time as would be at startTime, and turn through turnAngle.
    //returns time it will take to make the turn.
    PhysRelativeTime turnBy( const PhysAbsoluteTime& startTime, MexRadians turnAngle );

    //Change motion to use starting angle and time as would be st startTime from old motion,
    //and then turn through shortest route to requiredAngle, rounding with 360 degrees.
    //returns time it will take to make the turn.
    PhysRelativeTime turnTo( const PhysAbsoluteTime& startTime, MexRadians requiredAngle );

    //Cancel any current motion and set the angle and start time
    void reset( const PhysAbsoluteTime& startTime, MexRadians startAngle );

    //set the turn rate from startTime
    void turnRate( const PhysAbsoluteTime& startTime, MexRadians rate );

    //The angle of rotation as at time.
    MexRadians angle( const PhysAbsoluteTime& time ) const;

    //Reset base transform to newTransform
    void baseTransform( const MexTransform3d& newTransform );

    //Accessors
    const MexTransform3d& baseTransform() const;
    W4d::Axis turningAxis() const;
    const PhysAbsoluteTime& startTime() const;
    MexRadians startAngle() const;
    MexRadians turnAngle() const;

    /////////////////////////////////////////////////
    // Inherited from PhysMotionPlan

    //Override to define result as a function of timeOffset.
    //If time is greater than duration, the transform at time duration is to be returned.
    virtual void transform( const PhysRelativeTime& timeOffset, MexTransform3d* result) const;

    /////////////////////////////////////////////////

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const W4dAxisTurnerPlan& t );


    W4dAxisTurnerPlan( const W4dAxisTurnerPlan& );
    W4dAxisTurnerPlan& operator =( const W4dAxisTurnerPlan& );

    //Update for current motion at time
    void updateData( const PhysAbsoluteTime& time );

    //The angle would have turned through at time
    MexRadians currentDeltaAngle( const PhysAbsoluteTime& time ) const;

    //data members
    MexTransform3d baseTransform_; //Starting transform about one of whose principal axes we rotate
    W4d::Axis turningAxis_; //The index of the turn axis
    PhysAbsoluteTime startTime_; //Start time for current motion
    MexRadians startAngle_; //Base angle for current motion
    MexRadians turnAngle_; //Angle we want to turn through
    MexRadians turnRate_; //rate at which we can turn
};


#endif

/* End AXISTURN.HPP *************************************************/
