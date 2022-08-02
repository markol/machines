/*
 * F L A M E T H R . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysFlameThrower

    Models a flame thrower.
*/

#ifndef _MACHPHYS_FLAMETHR_HPP
#define _MACHPHYS_FLAMETHR_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/wepline.hpp"

class W4dEntity;
class MachPhysFlameBall;
class MachPhysArtefact;

class MachPhysFlameThrower : public MachPhysLinearWeapon
// Canonical form revoked
{
public:
    //Constructs appropriate
    MachPhysFlameThrower( W4dEntity* pParent, const MexTransform3d& localTransform,
                    MachPhys::WeaponType type, MachPhys::Mounting mounting );

    //dtor
    virtual ~MachPhysFlameThrower();

    //Return an exemplar flamethrower - ensures the mesh is loaded
    static const MachPhysFlameThrower& exemplar( MachPhys::WeaponType type );

    virtual MachPhysLinearProjectile* createProjectile
    (
        const PhysAbsoluteTime& burstStartTime, uint flameIndex, W4dEntity* pParent,
        const W4dEntity& target, const MexPoint3d& targetOffset
    );

    /////////////////////////////////////////////////////////
    // Inherited from MachPhysWeapon
	virtual PhysRelativeTime fire( const PhysAbsoluteTime& startTime, int numberInBurst );

    //Play the victim animation for a machine or construction at startTime.
    //fromDirection indicates the flight path of the hitting projectile.
    //Returns the duration of the animation.
    virtual PhysRelativeTime victimAnimation( const PhysAbsoluteTime& startTime,
                                              const MexLine3d& fromDirection,
                                              MachPhysMachine* pMachine ) const;
    virtual PhysRelativeTime victimAnimation( const PhysAbsoluteTime& startTime,
                                              const MexLine3d& fromDirection,
                                              MachPhysConstruction* pConstruction ) const;

    /////////////////////////////////////////////////////////

    //Do the work of applying a victim animation to machine/construction/artefact
    //Return animation duration.
    static PhysRelativeTime applyVictimAnimation( const PhysAbsoluteTime& startTime,
                                                  const MexLine3d& fromDirection,
                                                  MachPhysMachine* pMachine );
    static PhysRelativeTime applyVictimAnimation( const PhysAbsoluteTime& startTime,
                                                  const MexLine3d& fromDirection,
                                                  MachPhysConstruction* pConstruction );
    static PhysRelativeTime applyVictimAnimation( const PhysAbsoluteTime& startTime,
                                                  const MexLine3d& fromDirection,
                                                  MachPhysArtefact* pArtefact );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysFlameThrower& t );

    PER_MEMBER_PERSISTENT( MachPhysFlameThrower );

private:
    MachPhysFlameThrower( const MachPhysFlameThrower& );
    MachPhysFlameThrower& operator =( const MachPhysFlameThrower& );
    bool operator ==( const MachPhysFlameThrower& );

    //One-time constructor (per type) used to create the exemplars
    MachPhysFlameThrower( MachPhys::WeaponType type );

    //the composite file path for given type
    static const char* compositeFilePath( MachPhys::WeaponType type );
	MachPhysFlameBall* createFlameBall
	(
		const PhysAbsoluteTime& burstStartTime,
		uint flameIndex,
		W4dEntity* pParent,
		const W4dEntity& target,
		const MexPoint3d& targetOffset
	);

    friend class MachPhysWeaponPersistence;
};

PER_DECLARE_PERSISTENT( MachPhysFlameThrower );
PER_READ_WRITE( MachPhysFlameThrower );

#endif

/* End FLAMETHR.HPP ****************************************************/
