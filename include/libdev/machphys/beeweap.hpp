/*
 * B E E W E A P . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysBeeWeapon

    the BEE weapon mounted on FLYER (BOSS )	 and its animations
*/

#ifndef _MACHPHYS_BEEWEAP_HPP
#define _MACHPHYS_BEEWEAP_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/wepline.hpp"

class W4dEntity;
class MachPhysBeeBomb;
class MachPhysMachine;
class MachPhysConstruction;
class MachPhysArtefact;


class MachPhysBeeBomber : public MachPhysLinearWeapon
// Canonical form revoked
{
public:
	//public constructor
    MachPhysBeeBomber( W4dEntity* pParent, const MexTransform3d& localTransform );

    virtual ~MachPhysBeeBomber( void );

    static const MachPhysBeeBomber& exemplar();

    virtual MachPhysLinearProjectile* createProjectile
    (
        const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
        const W4dEntity& target, const MexPoint3d& targetOffset
    );

	MachPhysBeeBomb* createBeeBomb
	(
		const PhysAbsoluteTime& burstStartTime,
		uint index,
		W4dEntity* pParent,
		const W4dEntity& target,
		const MexPoint3d& targetOffset
	);

	virtual PhysRelativeTime fire( const PhysAbsoluteTime& startTime, int numberInBurst );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysBeeBomber );
    PER_FRIEND_READ_WRITE( MachPhysBeeBomber );

private:
    friend ostream& operator <<( ostream& o, const MachPhysBeeBomber& t );

    MachPhysBeeBomber( const MachPhysBeeBomber& );
    MachPhysBeeBomber& operator =( const MachPhysBeeBomber& );

	//one tiem constructor
    MachPhysBeeBomber( void );

    friend class MachPhysWeaponPersistence;

    //the composite file path for given type
    static const char* compositeFilePath();

	//always carrys a MachPhysBeeBomb
	MachPhysBeeBomb* pBeeBomb_;
};

PER_DECLARE_PERSISTENT( MachPhysBeeBomber );

#endif

/* End BEEWEAP.HPP **************************************************/
