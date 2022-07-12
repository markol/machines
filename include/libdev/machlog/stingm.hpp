/*
 * STINGM . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogMetalSting

*/

#ifndef _MACHLOG_STINGM_HPP
#define _MACHLOG_STINGM_HPP

#include "base/base.hpp"
#include "machlog/lineproj.hpp"

class MachPhysMetalSting;
class MachPhysLinearProjectile;
class MachActor;

class MachLogMetalSting : public MachLogLinearProjectile
// Canonical form revoked
{
public:
    MachLogMetalSting(
        MachLogRace* pRace,
		MachPhysLinearProjectile*,
        MachActor* pOwner,
        const MachPhysWeaponData& );

    ~MachLogMetalSting();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogMetalSting& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogMetalSting );
	PER_FRIEND_READ_WRITE( MachLogMetalSting );

private:
	void doBeDestroyed();

    MachLogMetalSting( const MachLogMetalSting& );
    MachLogMetalSting& operator =( const MachLogMetalSting& );
    bool operator ==( const MachLogMetalSting& );

};

PER_DECLARE_PERSISTENT( MachLogMetalSting );

#endif

/* End STINGM.HPP *************************************************/
