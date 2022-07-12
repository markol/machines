/*
 * L I N E P R O J . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogLinearProjectile

    Base class for linear projectile weapons.
*/

#ifndef _MACHLOG_LINEAR_PROJECTILE_HPP
#define _MACHLOG_LINEAR_PROJECTILE_HPP

#include "base/base.hpp"

#include "sim/projecti.hpp"

#include "mathex/point3d.hpp"

#include "phys/phys.hpp"
#include "machphys/machphys.hpp"

class MexPoint3d;
class MachPhysLinearProjectile;
class MachLogRace;
class MachActor;
class MachPhysWeaponData;

class MachLogLinearProjectile : public SimProjectile
{
public:
	enum DamageCurveShape {
		CONSTANT_DAMAGE,
		R_RECIPROCAL_DAMAGE,
		R_SQUARED_DAMAGE,
		LINEAR_DAMAGE,
	};
	
	enum DetectIntersections {
		DO_DETECT_INTERSECTIONS,
		DO_NOT_DETECT_INTERSECTIONS
	};

    MachLogLinearProjectile(
        MachLogRace* pRace,
        MachPhysLinearProjectile*,
        MachActor* pOwner,
        const MachPhysWeaponData&,
		DetectIntersections detectIntersections = DO_DETECT_INTERSECTIONS );

    virtual ~MachLogLinearProjectile();

    // Override updates the logical machine's Ai process.
    // Clearance ignored for now.
    virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR clearanceFromDisplayedVolume );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogLinearProjectile& t );

    const MexPoint3d&   globalDestructionPosition() const;

	int damage() const;

	static void genericCheckForDamage( const MexPoint3d&, MATHEX_SCALAR range,
										DamageCurveShape, int damage, 
										MachPhys::WeaponType wt = MachPhys::N_WEAPON_TYPES,
										MexLine3d* line = NULL,
										MachActor* pActor = NULL );	

    //projectiles will be observers of their owning actor. This will allow checks
	//such as if owner do not cause damage etc.
	//but the owner can of course be destroyed before the projectile hence observer owner.
	virtual bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );

	//export when and how linear projectile will die
	const PhysAbsoluteTime& destructionTime() const;
	MachPhys::StrikeType 	strikeType() const;

	PER_MEMBER_PERSISTENT_ABSTRACT( MachLogLinearProjectile );
	PER_FRIEND_READ_WRITE( MachLogLinearProjectile );

protected:
    MATHEX_SCALAR   rangeToStationaryObject(
        const MexPoint3d& globalStartPosition,
        const MexPoint3d& globalEndPosition,
        MATHEX_SCALAR range );

    MATHEX_SCALAR   rangeToMovingObject(
        const MexPoint3d& globalStartPosition,
        const MexPoint3d& globalEndPosition,
        MATHEX_SCALAR range );

    MachPhysLinearProjectile& physLinearProjectile();
    const MachPhysLinearProjectile& physLinearProjectile() const;

	virtual void doBeDestroyed() = 0;
    
	bool hasOwner() const;
   	MachActor& owner();
	
	// occasionally, we may want to return the pointer directly when it may (legitimately) be NULL.
	// case in point - nuclear missile, which may need to create a bomb after the owner has died.
	MachActor* pOwner();
	
	void checkForDamage( MATHEX_SCALAR range, DamageCurveShape, MachPhys::WeaponType );     

private:
    // Operation deliberately revoked
    MachLogLinearProjectile( const MachLogLinearProjectile& );

    // Operation deliberately revoked
    MachLogLinearProjectile& operator =( const MachLogLinearProjectile& );

    // Operation deliberately revoked
    bool operator ==( const MachLogLinearProjectile& );

//    static MachPhysLinearProjectile* pNewPhysLinearProjectile(
//        size_t level, const MexPoint3d& startPosition );
        
    void destroyAt( const PhysAbsoluteTime& time );


    PhysAbsoluteTime    destructionTime_;
    MexPoint3d          globalDestructionPosition_;
	MATHEX_SCALAR		speed_;
	MATHEX_SCALAR		maxRange_;
	MachActor*			pOwner_;
	MachPhys::StrikeType	strikeType_;
	int					damage_;
	bool				currentlyAttached_;
};

PER_DECLARE_PERSISTENT( MachLogLinearProjectile );

#endif

/* End LINEAR_PROJECTILE.HPP **************************************************/
