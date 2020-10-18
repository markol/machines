/*
 * S T G M W E P . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysMetalStingWeapon

    the wast metal sting linear weapon
*/

#ifndef _MACHPHYS_STGMWEP_HPP
#define _MACHPHYS_STGMWEP_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/wepline.hpp"

class W4dEntity;
class MachPhysMetalSting;
class MachPhysMachine;
class MachPhysConstruction;
class MachPhysArtefact;


class MachPhysMetalStingWeapon : public MachPhysLinearWeapon
// Canonical form revoked
{
public:
	//public constructor
    MachPhysMetalStingWeapon( W4dEntity* pParent, const MexTransform3d& localTransform );

    virtual ~MachPhysMetalStingWeapon( void );

    static const MachPhysMetalStingWeapon& exemplar();

    virtual MachPhysLinearProjectile* createProjectile
    (
        const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
        const W4dEntity& target, const MexPoint3d& targetOffset
    );

	MachPhysMetalSting* createMetalSting
	(
		const PhysAbsoluteTime& burstStartTime,
		uint index,
		W4dEntity* pParent,
		const W4dEntity& target,
		const MexPoint3d& targetOffset
	);


	virtual PhysRelativeTime fire( const PhysAbsoluteTime& startTime, int numberInBurst );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysMetalStingWeapon );
    PER_FRIEND_READ_WRITE( MachPhysMetalStingWeapon );

private:
    friend ostream& operator <<( ostream& o, const MachPhysMetalStingWeapon& t );

    MachPhysMetalStingWeapon( const MachPhysMetalStingWeapon& );
    MachPhysMetalStingWeapon& operator =( const MachPhysMetalStingWeapon& );

	//one tiem constructor
    MachPhysMetalStingWeapon( void );

    friend class MachPhysWeaponPersistence;

    //the composite file path for given type
    static const char* compositeFilePath();

};

PER_DECLARE_PERSISTENT( MachPhysMetalStingWeapon );

#endif

/* End STGMWEP.HPP **************************************************/
