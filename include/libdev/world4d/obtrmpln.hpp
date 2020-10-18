/*
 * O B T R M P L N . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dObjectTrackerMotionPlan

    A motion plan based on tracking a target object
*/

#ifndef _WORLD4D_OBTRMPLN_HPP
#define _WORLD4D_OBTRMPLN_HPP

#include "base/base.hpp"
#include "phys/motplan.hpp"
#include "world4d/objtrack.hpp"

class W4dObjectTrackerMotionPlan : public PhysMotionPlan
// Canonical form revoked
{
public:
    //ctor. objectTrackerPtr defines the motion.
    //The tracking is actiev for duration.
    W4dObjectTrackerMotionPlan( const W4dObjectTrackerPtr& objectTrackerPtr,
                                const PhysRelativeTime& duration );

    //dtor
    virtual ~W4dObjectTrackerMotionPlan();

    /////////////////////////////////////////////////////
    // Inherited from PhysMotionPlan

    //Override to define result as a function of timeOffset.
    //If time is greater than duration, the transform at time duration is to be returned.
    virtual void transform( const PhysRelativeTime& timeOffset, MexTransform3d* result) const;
    /////////////////////////////////////////////////////

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dObjectTrackerMotionPlan& t );

private:
    W4dObjectTrackerMotionPlan( const W4dObjectTrackerMotionPlan& );
    W4dObjectTrackerMotionPlan& operator =( const W4dObjectTrackerMotionPlan& );
    bool operator ==( const W4dObjectTrackerMotionPlan& );

    //data members
    W4dObjectTrackerPtr objectTrackerPtr_; //The object tracker defining the motion
};


#endif

/* End OBTRMPLN.HPP *************************************************/
