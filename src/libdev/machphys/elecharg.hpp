/*
 * E L E C H A R G . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysElectroCharger

    Models a flame thrower.
*/

#ifndef _MACHPHYS_ELECHARG_HPP
#define _MACHPHYS_ELECHARG_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/wepline.hpp"

class W4dEntity;
class MachPhysElectro;
class MachPhysMachine;
class MachPhysConstruction;
class MachPhysArtefact;

class MachPhysElectroCharger : public MachPhysLinearWeapon
// Canonical form revoked
{
public:

    //Constructs appropriate
    MachPhysElectroCharger( W4dEntity* pParent, const MexTransform3d& localTransform,
                            MachPhys::Mounting mounting );

    //dtor
    virtual ~MachPhysElectroCharger();

    //Return an exemplar flamethrower - ensures the mesh is loaded
    static const MachPhysElectroCharger& exemplar( MachPhys::Mounting mounting );

    virtual MachPhysLinearProjectile* createProjectile
    (
        const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
        const W4dEntity& target, const MexPoint3d& targetOffset
    );

	virtual PhysRelativeTime fire( const PhysAbsoluteTime& startTime, int numberInBurst );

    virtual PhysRelativeTime victimAnimation( const PhysAbsoluteTime& startTime,
                                              const MexLine3d& fromDirection,
                                              MachPhysMachine* pMachine ) const;

    virtual PhysRelativeTime victimAnimation( const PhysAbsoluteTime& startTime,
                                              const MexLine3d& fromDirection,
                                              MachPhysConstruction* pConstruction ) const;

	//Apply the victim animation to machine/construction/artefact at startTime.
    //return animation duration.
	static PhysRelativeTime applyVictimAnimation
	(
    	const PhysAbsoluteTime& startTime,
        const MexLine3d& fromDirection,
    	MachPhysMachine* pMachine
	);

	static PhysRelativeTime applyVictimAnimation
	(
    	const PhysAbsoluteTime& startTime,
        const MexLine3d& fromDirection,
    	MachPhysConstruction* pConstruction
	);

	static PhysRelativeTime applyVictimAnimation
	(
    	const PhysAbsoluteTime& startTime,
        const MexLine3d& fromDirection,
    	MachPhysArtefact* pArtefact
	);

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysElectroCharger );
    PER_FRIEND_READ_WRITE( MachPhysElectroCharger );

    friend ostream& operator <<( ostream& o, const MachPhysElectroCharger& t );

private:
    MachPhysElectroCharger( const MachPhysElectroCharger& );
    MachPhysElectroCharger& operator =( const MachPhysElectroCharger& );
    bool operator ==( const MachPhysElectroCharger& );

    //One-time constructor (per type) used to create the exemplars
    MachPhysElectroCharger( MachPhys::Mounting mounting );

	MachPhysElectro* createElectro
	(
		const PhysAbsoluteTime& burstStartTime,
		const PhysRelativeTime& duration,
		W4dEntity* pParent,
		const W4dEntity& target,
		const MexPoint3d& targetOffset
	);

    //Do the halo lightning effect on a victim animation to entity pVictim.
    //Return animation duration
    static PhysRelativeTime startLightning( const PhysAbsoluteTime& startTime,
                                      const PhysRelativeTime& lightningDuration, W4dEntity* pVictim);

    //the composite file path for given type
    static const char* compositeFilePath();

    friend class MachPhysWeaponPersistence;
};

PER_DECLARE_PERSISTENT( MachPhysElectroCharger );

#endif

/* End ELECHARG.HPP ****************************************************/
