/*
 * L I N E P R O J . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLinearProjectile

    ABC for projectiles (usually fired from a weapon) which move in a straight
    line and explode at a given time.
*/

#ifndef _MACHPHYS_LINEPROJ_HPP
#define _MACHPHYS_LINEPROJ_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "world4d/composit.hpp"
#include "machphys/machphys.hpp"
#include "machphys/machine.hpp"

//Forward refs
class MexPoint3d;
class MexVec3;
class MexTransform3d;
class MachPhysWeaponData;
class MachPhysLinearProjectileImpl;
class MachPhysPlanetSurface;

//orthodox canonical (revoked)
class MachPhysLinearProjectile : public W4dComposite
{
public:
    //dtor
    virtual ~MachPhysLinearProjectile();

    //Set the surface to be used for any effects
    void planetSurface( MachPhysPlanetSurface* pSurface );

    //Query existence of defined surface/return it
    bool hasPlanetSurface() const;
    MachPhysPlanetSurface& planetSurface() const;
    //PRE( hasPlanetSurface() );

    //Set up standard motion for the projectile from startTime.
    //Flies in a straight line along it's x axis at the speed defined by data, and
    //for the distance defined by the range of data.
    //Also sets up visibility plan so only visible while in flight.
    //Returns the duration of the flight.
    PhysRelativeTime move( const PhysAbsoluteTime& startTime, const MachPhysWeaponData& data );

    //Destruct at time. Returns the duration of any destruction animation.
    //Adds visibility plan to set the projectile invisible at time.
    //Calls doBeDestroyedAt to implement any animation, and uses its result as the
    //return value.
    //strikeType indicates what kind of animation to perform from what has been hit
    PhysRelativeTime beDestroyedAt( const PhysAbsoluteTime& time, MachPhys::StrikeType strikeType );

    //Inherited from W4dEntity.
    //Default implementation returns false. NB this class created as not_solid.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    //Get flight data
    const PhysAbsoluteTime& flightStartTime() const;
    const PhysRelativeTime& flightDuration() const;
    const MexLine3d& flightPath() const;
    MexPoint3d flightPosition( const PhysAbsoluteTime& time ) const;

    //Utility function to compute the impact point with target for a projectile travelling along fromDirection
    //(defined in global coords).
    //True iff an impact is detected. If so the impact point is returned in pImpactPoint, with a unit direction vector
    //pointing back along the projectile's path returned in pUnitDirection. Both of these are defined in the
    //local coordinate system of target.
    static bool impactData( W4dEntity& target, const MexLine3d& fromDirection,
                            MexPoint3d* pImpactPoint, MexVec3* pUnitDirection );

	//The launcher control is default as outwith first person ie OTHER_CONTROL
	void setLauncherControl(MachPhysMachine::ControlType);
	MachPhysMachine::ControlType getLauncherControl() const;

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const MachPhysLinearProjectile& t );

    PER_MEMBER_PERSISTENT_VIRTUAL( MachPhysLinearProjectile );
    PER_FRIEND_READ_WRITE( MachPhysLinearProjectile );

protected:
    //ctor. Attaches to pParent at relative position localTransform
    MachPhysLinearProjectile( W4dEntity* pParent, const MexTransform3d& localTransform );

    //Similar copy ctor.
    MachPhysLinearProjectile( const MachPhysLinearProjectile& copyMe,
                              W4dEntity* pParent, const W4dTransform3d& localTransform );

    MachPhysLinearProjectile( const MachPhysLinearProjectile& copyMe,
                              W4dEntity* pParent, const W4dTransform3d& localTransform,
							  CopyLights);

    //Set flight data
    void flightStartTime( const PhysAbsoluteTime& startTime );
    void flightDuration( const PhysRelativeTime& duration );
    void flightPath( const MexLine3d& path );

    //Update any flight data.
    //Default implementation does nothing.
    virtual void updateFlightData();

	// Not all projectiles have lights attached.  For those that do, the derived
	// class can optionally suppress the lights, in which case this will return
	// true.  Beware: if this is false, it does not necessarilly imply that the
	// projectile has lights.
	bool lightsSuppressed() const;

private:
	void sharedCopyCtor();

    // Operation deliberately revoked
    MachPhysLinearProjectile( const MachPhysLinearProjectile& );
    MachPhysLinearProjectile& operator =( const MachPhysLinearProjectile& );
    bool operator ==( const MachPhysLinearProjectile& );

    //Override to add animation to the flying projectile.
    //It is moving from startPoint to endPoint(local coords), which takes time duration.
    //Default implementation does nothing.
    virtual void doMove( const MexPoint3d& startPoint, const MexPoint3d& endPoint,
                         const PhysRelativeTime& duration );

    //Override to add specific animations for the projectile destruction at time.
    //Return duration of such animation.
    //Default implementation does nothing and returns zero.
    virtual PhysRelativeTime doBeDestroyedAt( const PhysAbsoluteTime& time,
                                              MachPhys::StrikeType strikeType );


	MachPhysLinearProjectileImpl* pImpl_;

};

PER_DECLARE_PERSISTENT( MachPhysLinearProjectile );

#endif

/* End LINEPROJ.HPP *************************************************/
