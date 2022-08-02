/*
 * O B J T R A C K . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dObjectTracker

    Generates a transform and distance attempting to keep an object's x axis
    pointing at a point of some other object.
*/

#ifndef _WORLD4D_OBJTRACK_HPP
#define _WORLD4D_OBJTRACK_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/countptr.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "phys/phys.hpp"

//forward refs
class W4dEntity;

class W4dObjectTracker
// Canonical form revoked
{
public:
    //ctor. Generates a transform for an object at offset localOrigin from owner parent.
    //The natural orientation of the object is aligned with that of parent.
    //However, attempts to point the x axis at targetOffset relative to target by
    //two successive rotations. These are a yaw (about the z axis) through an angle
    //with sine modulus no greater than maxAbsYawSine, followed by a pitch (about y axis)
    //through an angle with sine modulus no greater than maxAbsPitchSine.
    //In addition the object has a conceptual length along the x axis, and attempts to
    //maintain a length to reach the target point. The actual length must be between
    //minLength and maxLength, with unscaled length of the object being defaultLength.
    W4dObjectTracker( const W4dEntity& parent, const MexPoint3d& localOrigin,
                      const W4dEntity& target, const MexPoint3d& targetOffset,
                      MATHEX_SCALAR maxAbsYawSine, MATHEX_SCALAR maxAbsPitchSine,
                      MATHEX_SCALAR minLength, MATHEX_SCALAR maxLength,
                      MATHEX_SCALAR defaultLength );
    //PRE( maxAbsYawSine >= 0.0  and  maxAbsYawSine <= 1.0 ):
    //PRE( maxAbsPitchSine >= 0.0  and  maxAbsPitchSine <= 1.0 ):
    //PRE( minLength <= maxLength );
    //PRE( defaultLength >=  minLength and defaultLength <= maxLength );

    //dtor.
    ~W4dObjectTracker();

    //Selectors
    const W4dEntity& parent() const;
    const MexPoint3d& localOrigin() const;
    const W4dEntity& target() const;
    const MexPoint3d& targetOffset() const;
    MATHEX_SCALAR maxAbsYawSine() const;
    MATHEX_SCALAR maxAbsPitchSine() const;
    MATHEX_SCALAR minLength() const;
    MATHEX_SCALAR maxLength() const;
    MATHEX_SCALAR defaultLength() const;

    //Set/get a delta angle to be used to adjust the yaw angle for tracking.
    //Must supply the angle as a sine and cosine.
    void deltaAngle( MATHEX_SCALAR yawDeltaSine, MATHEX_SCALAR yawDeltaCosine );
    //PRE( fabs( 1.0 - (yawDeltaSine*yawDeltaSine + yawDeltaCosine*yawDeltaCosine) ) < 0.00001 );

    MATHEX_SCALAR yawDeltaSine() const;
    MATHEX_SCALAR yawDeltaCosine() const;

    //Computes the tracking object transform and returns it in pTrackTransform
    //according to rules described with ctor.
    //Returns the object's conceptual length.
    MATHEX_SCALAR track( MexTransform3d* pTrackTransform ) const;
    //PRE( pTrackTransform != NULL );

    //Returns a rotation only transform in pTrackTransform which would cause the x axis to
    //point at target relative to an axis frame.
    //maxAbsYawSine etc have the same meaning as the track method above.
    static MATHEX_SCALAR track( const MexPoint3d& target, MexTransform3d* pTrackTransform,
                                MATHEX_SCALAR maxAbsYawSine, MATHEX_SCALAR maxAbsPitchSine,
                                MATHEX_SCALAR yawDeltaSine = 0.0, MATHEX_SCALAR yawDeltaCosine = 1.0 );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const W4dObjectTracker& t );

    PER_MEMBER_PERSISTENT( W4dObjectTracker );
    PER_FRIEND_READ_WRITE( W4dObjectTracker );

private:
    W4dObjectTracker( const W4dObjectTracker& );
    W4dObjectTracker& operator =( const W4dObjectTracker& );
    bool operator ==( const W4dObjectTracker& );

    //data members
    const W4dEntity* pParent_; //Parent of the object which is doing the tracking
    MexPoint3d localOrigin_; //Offset from pParent to origin of the tracking object
    const W4dEntity* pTarget_; //The target being tracked
    MexPoint3d targetOffset_; //Actual point being aimed at relative to pTarget_;
    MATHEX_SCALAR maxAbsYawSine_;//sine of maximum angle can rotate about z axis (either direction)
    MATHEX_SCALAR maxAbsPitchSine_;//sine of maximum angle can rotate about y axis (either direction)
    MATHEX_SCALAR yawDeltaSine_; //sine of angel to modify computed yaw angle
    MATHEX_SCALAR yawDeltaCosine_; //sine of angel to modify computed yaw angle
    MATHEX_SCALAR minLength_; //Minimum length of tracking object (in x)
    MATHEX_SCALAR maxLength_; //Maximum length of tracking object (in x)
    MATHEX_SCALAR defaultLength_; //Unstretched length

    //Mutable cached data
    PhysAbsoluteTime lastUpdateTime_; //Time last updated data below
    MexTransform3d trackerTransform_; //The computed tracking object transform
    MATHEX_SCALAR trackerLength_; //Computed length in x of tracker
};

PER_DECLARE_PERSISTENT( W4dObjectTracker );

//Define counted ptr
typedef CtlCountedPtr< W4dObjectTracker > W4dObjectTrackerPtr;

#ifdef _INLINE
    #include "world4d/objtrack.ipp"
#endif


#endif

/* End OBJTRACK.HPP *************************************************/
