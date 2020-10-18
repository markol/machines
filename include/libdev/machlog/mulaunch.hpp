/*
 * P U L S E W E P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogMultiLauncher

	Multi launcher classes
*/

#ifndef _MACHLOG_MULAUNCH_HPP
#define _MACHLOG_MULAUNCH_HPP

#include "base/base.hpp"
#include "machlog/wepline.hpp"

class MachPhysLinearWeapon;
class MachPhysMultiLauncher;

class MachLogMultiLauncher : public MachLogLinearWeapon
// Canonical form revoked
{
public:
    MachLogMultiLauncher( MachLogRace*, MachPhysMultiLauncher*, MachActor* pOwner );
    virtual ~MachLogMultiLauncher( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogMultiLauncher& t );

	//the persistence mechanism has to reset which weapons go where
	//so we have to allow it to remount the weapons.
	void setPhysicalMultiLauncher( MachPhysMultiLauncher* );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogMultiLauncher );
	PER_FRIEND_READ_WRITE( MachLogMultiLauncher );

protected:

	virtual	MachLogLinearProjectile* createLinearProjectile
	(
		const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
	    const W4dEntity& target, const MexPoint3d& targetOffset
	);

private:
    MachLogMultiLauncher( const MachLogMultiLauncher& );
    MachLogMultiLauncher& operator =( const MachLogMultiLauncher& );
    bool operator ==( const MachLogMultiLauncher& );

	MachPhysMultiLauncher*	pPhysMultiLauncher_;
};

PER_DECLARE_PERSISTENT( MachLogMultiLauncher );

#endif

/* End PULSEWEP.HPP *************************************************/
