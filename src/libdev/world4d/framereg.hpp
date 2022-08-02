/*
 * F R A M E R E G . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dFrameRegulator

    This class is used in conjunction with W4dEntityPlan.
    The concept is that an animation is defined which needs to step
    frame by frame, rather than being simply on a time basis.
    For example, a fireball animation may consist of a sequence of
    bitmaps which should be displayed on consecutive frames, and the
    effect of skipping or duplicating frames would be aesthetically
    unsatisfactory.
    This class specifies the number of frames that the animation is defined
    over, the ideal time the animation should take, and returns an effective
    time slightly different from the current simulation time. This time will
    cause related plans (especially material plans) to select the appropriate frame.
*/

#ifndef _WORLD4D_FRAMEREG_HPP
#define _WORLD4D_FRAMEREG_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"

//bitwise canonical
class W4dFrameRegulator
{
public:
    //ctor. Animation is to start at startTime. Ideally should run for
    //idealDuration, but no longer than maxDuration.
    //The actual number of animation frames is nFrames.
    W4dFrameRegulator( const PhysAbsoluteTime& startTime, const PhysRelativeTime& idealDuration,
                       const PhysRelativeTime& maxDuration, size_t nFrames );

    //dtor
    ~W4dFrameRegulator();

    //Accessors
    const PhysAbsoluteTime& startTime() const;
    const PhysRelativeTime& idealDuration() const;
    const PhysRelativeTime& maxDuration() const;
    size_t startFrame() const;
    size_t nFrames() const;

    void startTime( const PhysAbsoluteTime& startTime );

    //The effective time to use to get the correct frame displayed in the animation
    PhysAbsoluteTime effectiveTime() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dFrameRegulator& t );

    PER_MEMBER_PERSISTENT( W4dFrameRegulator );
    PER_FRIEND_READ_WRITE( W4dFrameRegulator );
private:
    //data members
    PhysAbsoluteTime startTime_; //The actual time at which the animation starts
    PhysRelativeTime idealDuration_; //The approximate time we'd like the animation to run
    PhysRelativeTime maxDuration_; //The longest time we want the animation to run
    size_t startFrame_; //The simulation frame number when the animation is activated
    size_t nFrames_; //The number of frames in the animation
};

PER_DECLARE_PERSISTENT( W4dFrameRegulator );

#ifdef _INLINE
    #include "world4d/framereg.ipp"
#endif


#endif

/* End FRAMEREG.HPP *************************************************/
