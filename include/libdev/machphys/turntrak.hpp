/*
 * T U R N T R A K . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysTurnerTracker

    Controls a turning entity's turning motion. Can be given commands to
    turn to an absolute angle, or track another object.
*/

#ifndef _MACHPHYS_TURNTRAK_HPP
#define _MACHPHYS_TURNTRAK_HPP

#include "base/base.hpp"
#include "world4d/world4d.hpp"

class ITurnerTracker; //Implementation data object
class MexRadians;

class MachPhysTurnerTracker
// Canonical form revoked
{
public:
    //The object to rotate is pTurningObject, and it is turningAxis that rotates.
    //The transform of the object at time of construction is taken as the zero position.
    //When tracking, pointingAxis attempts to point at a target object.
    //The rate of rotation is turnRate.
    MachPhysTurnerTracker( W4dEntity* pTurningObject, W4d::Axis turningAxis, W4d::Axis pointingAxis,
                           MexRadians turnRate );

    ~MachPhysTurnerTracker();

    //Initiate a move to absolute angle, returning duration of the move
    PhysRelativeTime turnToAngle( MexRadians angle );

    //Initiate a move through relative angle, returning duration of the move
    PhysRelativeTime turnByAngle( MexRadians angle );

    //Do instantaneous move to angle, recording specified time
    void snapToAngle( const PhysAbsoluteTime& atTime, MexRadians angle );

    //Start tracking targetObject, attempting to get the pointing axis aiming in its direction.
    void trackTarget( const W4dEntity& targetObject );

    //Cancel any current tracking
    void stopTracking();

    //true if tracking a target
    bool isTrackingTarget() const;

    //The object being tracked
    const W4dEntity& targetObject() const;
    //PRE( isTrackingTarget() );

    //The current angle
    MexRadians currentAngle() const;

    //The current angle trying to turn to.
    MexRadians targetAngle() const;
    //PRE( not isTrackingTarget() );

    //Set limits on the motion as specified.
    //Default is no limits.
    void limits( MexRadians minAngle, MexRadians maxAngle );
    //PRE( minAngle.asScalar() <= maxAngle.asScalar() );

    //true if angular limits on the motion have been set.
    bool hasLimits() const;

    //true if the current angle is within tolerance of any limits
    bool nearLimits( MexRadians tolerance ) const;

    //Accessors
    MexRadians minAngle() const;
    MexRadians maxAngle() const;

    //Reset the base transform's location to that of the moving entity's current location
    void updateBaseLocation();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachPhysTurnerTracker& t );

    //revoked
    MachPhysTurnerTracker( const MachPhysTurnerTracker& );
    MachPhysTurnerTracker& operator =( const MachPhysTurnerTracker& );

    //data members
    ITurnerTracker* pImpl_; //Implementation data object
};

#ifdef _INLINE
    #include "machphys/turntrak.ipp"
#endif


#endif

/* End TURNTRAK.HPP *************************************************/
