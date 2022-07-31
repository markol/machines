/*
 * T R E C H O R B . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogTreacheryOrb

    The logical trechary orbs
*/

#ifndef _MACHLOG_TRECHORB_HPP
#define _MACHLOG_TRECHORB_HPP

#include "base/base.hpp"
#include "machlog/lineproj.hpp"
class MachPhysTreacheryOrb;
class MachPhysLinearProjectile;
class MachActor;

class MachLogTreacheryOrb : public MachLogLinearProjectile
// Canonical form revoked
{
public:
    MachLogTreacheryOrb(
        MachLogRace* pRace,
		MachPhysLinearProjectile*,
        MachActor* pOwner,
        const MachPhysWeaponData& );

    ~MachLogTreacheryOrb();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogTreacheryOrb& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogTreacheryOrb );
	PER_FRIEND_READ_WRITE( MachLogTreacheryOrb );

private:
	void doBeDestroyed();

    MachLogTreacheryOrb( const MachLogTreacheryOrb& );
    MachLogTreacheryOrb& operator =( const MachLogTreacheryOrb& );
    bool operator ==( const MachLogTreacheryOrb& );
	MachPhys::Race		race_;
	MachLogRace*		pLogRace_;

};

PER_DECLARE_PERSISTENT( MachLogTreacheryOrb );

#endif

/* End TRECHORB.HPP *************************************************/
