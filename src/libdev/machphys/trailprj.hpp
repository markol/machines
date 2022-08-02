/*
 * T R A I L P R J . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysTrailedProjectile

    Models an abstract type of linear projectile with vapour and flame	trails, and end explosion.
*/

#ifndef _MACHPHYS_TRAILPRJ_HPP
#define _MACHPHYS_TRAILPRJ_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "machphys/machphys.hpp"

#include "machphys/lineproj.hpp"
#include "mathex/vec3.hpp"

//class MachPhysTrailedProjectileImpl;
class MachPhysVapourTrail;
class W4dLink;
class W4dEntity;
class MachPhysWeaponData;
class MexPoint3d;

class MachPhysTrailedProjectile : public MachPhysLinearProjectile
{
public:

    MachPhysTrailedProjectile( W4dEntity* pParent, const W4dTransform3d& localTransform, size_t level );

    MachPhysTrailedProjectile(const MachPhysTrailedProjectile& copyMe, W4dEntity* pParent,
							  const W4dTransform3d& localTransform);
    MachPhysTrailedProjectile(const MachPhysTrailedProjectile& copyMe, W4dEntity* pParent,
							  const W4dTransform3d& localTransform, CopyLights);

    //dtor
    virtual ~MachPhysTrailedProjectile();

	enum ExplosionColour : unsigned char {RED, GREEN};
	//simulates an explosion of a projectile at explosionTransform: a light and 3 fire balls,
	//the brightness of the light and the size of the fire balls are related to the missileStrength
	static void explosion(W4dEntity* pParent,
				   const MexTransform3d& explosionTransform,
				   const PhysAbsoluteTime& startTime,
                   const PhysRelativeTime& duration,
                   const RenColour& lightCol,
                   const MATHEX_SCALAR& missileStrength,
                   ExplosionColour colour = RED
                  );

	static void explosionWithoutLights(W4dEntity* pParent,
				   const MexTransform3d& explosionTransform,
				   const PhysAbsoluteTime& startTime,
                   const PhysRelativeTime& duration,
                   const MATHEX_SCALAR& missileStrength,
                   ExplosionColour colour = RED
                  );

	enum { MISSILE1=1,
	       MISSILE2,
	       MISSILE3,
	       MISSILE4,
	       MISSILE5,
	       MISSILE6,
	       MISSILE7,
	       MISSILE_LARGE,
	       NUCLEAR_MISSILE,
	       BEE_BOMB,
	       METAL_STING,
	       NMISSILES};

    //  Do specific updates - things like the vapour trail.
    void update();

	//make plans for the flame
	void startFlame( const PhysAbsoluteTime& startTime, const MachPhysWeaponData& data);

	//default launching activities: spin, move, vapour trail, flame
	virtual PhysRelativeTime beLaunched
	(
	    const PhysAbsoluteTime& startTime,
	    const MachPhysWeaponData& data,
	    const MexPoint3d& targetOffsetGlobal
	);

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysTrailedProjectile& t );

    PER_MEMBER_PERSISTENT_VIRTUAL( MachPhysTrailedProjectile );
    PER_FRIEND_READ_WRITE( MachPhysTrailedProjectile );

protected:

	//data members
    MachPhysVapourTrail* pVapourTrail_;
	W4dLink* pFlame_;

    MexVec3         vapourTrailOffset_;
    PhysAbsoluteTime destructionTime_;
	size_t level_;

	//MachPhysTrailedProjectileImpl* pImpl_;

private:
	void sharedCopyCtor();

    // Operation deliberately revoked
    MachPhysTrailedProjectile& operator =( const MachPhysTrailedProjectile& );
    bool operator ==( const MachPhysTrailedProjectile& );
};

PER_DECLARE_PERSISTENT( MachPhysTrailedProjectile );
PER_ENUM_PERSISTENT( MachPhysTrailedProjectile::ExplosionColour );


#endif

/* End TRAILPRJ.HPP **************************************************/
