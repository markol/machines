/*
 * M O T P L A N . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysMotionPlan

    This class is an abstract base class for functions which map a time value in the
    range [0, duration] to a 3d transform.
*/

#ifndef _MOTPLAN_HPP
#define _MOTPLAN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"

class MexTransform3d;
class MexRadians;
class PhysRampAcceleration;

template < class T > class CtlCountedPtr;
template < class T > class ctl_vector;

//abstract, canonical (revoked)
class PhysMotionPlan
{
public:
    //Some useful typedefs
    typedef ctl_vector< MATHEX_SCALAR >         Times;
    typedef CtlCountedPtr< Times >              TimesPtr;
    typedef ctl_vector< MATHEX_SCALAR >         Speeds;
    typedef CtlCountedPtr< Speeds >             SpeedsPtr;
    typedef ctl_vector< MexTransform3d >        Transforms;
    typedef CtlCountedPtr< Transforms >         TransformsPtr;
    typedef ctl_vector< MexRadians >            Angles;
    typedef CtlCountedPtr< Angles >             AnglesPtr;
    typedef ctl_vector< PhysRampAcceleration >  RampAccelerations;
    typedef CtlCountedPtr< RampAccelerations >  RampAccelerationsPtr;

    //duration specifies the maximum time at which the motion is defined.
    //type specifies whether motion is absolute or relative.
    PhysMotionPlan( const PhysRelativeTime& duration );

    virtual ~PhysMotionPlan();

    //Return the motion duration
    const PhysRelativeTime& duration() const;

    //Override to define result as a function of timeOffset.
    //If time is greater than duration, the transform at time duration is to be returned.
    virtual void transform( const PhysRelativeTime& timeOffset, MexTransform3d* result) const = 0;

    //True iff the duration of the plan is <= timeOffset
    bool isDone( const PhysRelativeTime& timeOffset ) const;

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_ABSTRACT( PhysMotionPlan );
    PER_FRIEND_READ_WRITE( PhysMotionPlan );

protected:
    //set the duration of the motion plan to newDuration
    void duration( const PhysRelativeTime& newDuration);

    //True iff the times in times are in ascending order
    bool inAscendingOrder( const Times& times ) const;
    //PRE( times.size() > 0 )

private:
    // Operation deliberately revoked
    PhysMotionPlan( const PhysMotionPlan& );

    // Operation deliberately revoked
    PhysMotionPlan& operator =( const PhysMotionPlan& );

    // Operation deliberately revoked
    bool operator ==( const PhysMotionPlan& );

    // Data members
    PhysRelativeTime duration_; // The maximum time at which the mapping is defined
};

PER_DECLARE_PERSISTENT( PhysMotionPlan );

#ifdef _INLINE
    #include "phys/motplan.ipp"
#endif


#endif

/* End MOTPLAN.HPP **************************************************/
