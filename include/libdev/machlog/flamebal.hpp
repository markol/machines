/*
 * F L A M E B A L . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogFlameBall

*/

#ifndef _MACHLOG_FLAMEBAL_HPP
#define _MACHLOG_FLAMEBAL_HPP

#include "base/base.hpp"
#include "machlog/lineproj.hpp"

class MachPhysFlameBall;
class MachPhysLinearProjectile;
class MachActor;

class MachLogFlameBall : public MachLogLinearProjectile
// Canonical form revoked
{
public:
    MachLogFlameBall(
        MachLogRace* pRace,
		MachPhysLinearProjectile*,
        MachActor* pOwner,
        const MachPhysWeaponData& );

    ~MachLogFlameBall( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogFlameBall& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogFlameBall );
	PER_FRIEND_READ_WRITE( MachLogFlameBall );

private:
	void doBeDestroyed();

    MachLogFlameBall( const MachLogFlameBall& );
    MachLogFlameBall& operator =( const MachLogFlameBall& );
    bool operator ==( const MachLogFlameBall& );

};

PER_DECLARE_PERSISTENT( MachLogFlameBall );

#endif

/* End PULSEBLB.HPP *************************************************/
