/*
 * W E P D A T A . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
int MachPhysWeaponData::damagePoints() const
{
    return damagePoints_;
}

_CODE_INLINE
MATHEX_SCALAR MachPhysWeaponData::range() const
{
    return range_;
}

_CODE_INLINE
MATHEX_SCALAR MachPhysWeaponData::projectileSpeed() const
{
    return projectileSpeed_;
}

_CODE_INLINE
uint MachPhysWeaponData::nRoundsPerBurst() const
{
    return nRoundsPerBurst_;
}

_CODE_INLINE
PhysRelativeTime MachPhysWeaponData::burstDuration() const
{
    return burstDuration_;
}

_CODE_INLINE
PhysRelativeTime MachPhysWeaponData::reloadTime() const
{
    return reloadTime_;
}

_CODE_INLINE
void MachPhysWeaponData::damagePoints( int newDamagePoint )
{
    damagePoints_ = newDamagePoint;
}

_CODE_INLINE
void MachPhysWeaponData::range( MATHEX_SCALAR newRange )
{
    PRE( newRange > 0.0 );
    range_ = newRange;
}

_CODE_INLINE
void MachPhysWeaponData::projectileSpeed( MATHEX_SCALAR newProjectileSpeed )
{
    PRE( newProjectileSpeed > 0.0 );
    projectileSpeed_ = newProjectileSpeed;
}

_CODE_INLINE
void MachPhysWeaponData::nRoundsPerBurst( uint newNRoundsPerBurst )
{
    PRE( newNRoundsPerBurst != 0 );
    nRoundsPerBurst_ = newNRoundsPerBurst;
}

_CODE_INLINE
void MachPhysWeaponData::burstDuration( PhysRelativeTime newBurstDuration )
{
    PRE( newBurstDuration > 0.0 );
    burstDuration_ = newBurstDuration;
}

_CODE_INLINE
void MachPhysWeaponData::reloadTime( PhysRelativeTime newReloadTime )
{
    PRE( newReloadTime > 0.0 );
    reloadTime_ = newReloadTime;
}

_CODE_INLINE
const MachPhysWeaponData::LaunchOffsets& MachPhysWeaponData::launchOffsets() const
{
    return launchOffsets_;
}

_CODE_INLINE
void MachPhysWeaponData::launchOffsets( const LaunchOffsets& offsets )
{
    launchOffsets_ = offsets;
}

_CODE_INLINE
MATHEX_SCALAR MachPhysWeaponData::recoilDistance() const
{
    return recoilDistance_;
}

_CODE_INLINE
PhysRelativeTime MachPhysWeaponData::recoilBackTime() const
{
    return recoilBackTime_;
}

_CODE_INLINE
PhysRelativeTime MachPhysWeaponData::recoilForeTime() const
{
    return recoilForeTime_;
}

_CODE_INLINE
const MachPhysWeaponData::Extras& MachPhysWeaponData::extras() const
{
    return extras_;
}

_CODE_INLINE
void MachPhysWeaponData::recoilDistance( MATHEX_SCALAR distance )
{
    recoilDistance_ = distance;
}

_CODE_INLINE
void MachPhysWeaponData::recoilBackTime( PhysRelativeTime time )
{
    PRE( time >= 0.0 );
    recoilBackTime_ = time;
}

_CODE_INLINE
void MachPhysWeaponData::recoilForeTime( PhysRelativeTime time )
{
    PRE( time >= 0.0 );
    recoilForeTime_ = time;
}

_CODE_INLINE
void MachPhysWeaponData::extras( const Extras& values )
{
    extras_ = values;
}

_CODE_INLINE
bool MachPhysWeaponData::canTilt() const
{
    return minTiltAngle_.asScalar() != maxTiltAngle_.asScalar();
}

_CODE_INLINE
const MexRadians& MachPhysWeaponData::minTiltAngle() const
{
    return minTiltAngle_;
}

_CODE_INLINE
const MexRadians& MachPhysWeaponData::maxTiltAngle() const
{
    return maxTiltAngle_;
}

_CODE_INLINE
void MachPhysWeaponData::tiltAngles( const MexRadians& minAngle, const MexRadians& maxAngle )
{
    PRE( minAngle.asScalar() <= maxAngle.asScalar() );
    minTiltAngle_ = minAngle;
    maxTiltAngle_ = maxAngle;
}

_CODE_INLINE
const MexRadians& MachPhysWeaponData::verticalDeviationAngle() const
{
    return verticalDeviationAngle_;
}

_CODE_INLINE
const MexRadians& MachPhysWeaponData::horizontalDeviationAngle() const
{
    return horizontalDeviationAngle_;
}

_CODE_INLINE
void MachPhysWeaponData::verticalDeviationAngle( const MexRadians& angle )
{
    PRE( angle.asScalar() > 0.0 );
    verticalDeviationAngle_ = angle;
}

_CODE_INLINE
void MachPhysWeaponData::horizontalDeviationAngle( const MexRadians& angle )
{
    PRE( angle.asScalar() > 0.0 );
    horizontalDeviationAngle_ = angle;
}
/* End WEPDATA.IPP **************************************************/
