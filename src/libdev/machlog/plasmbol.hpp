/*
 * P U L S E B L B . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogPlasmaBolt

*/

#ifndef _MACHLOG_PLASMBOL_HPP
#define _MACHLOG_PLASMBOL_HPP

#include "base/base.hpp"
#include "machlog/lineproj.hpp"

class MachPhysPlasmaBolt;
class MachPhysLinearProjectile;
class MachActor;

class MachLogPlasmaBolt : public MachLogLinearProjectile
// Canonical form revoked
{
public:
    MachLogPlasmaBolt(
        MachLogRace* pRace,
		MachPhysLinearProjectile*,
        MachActor* pOwner,
        const MachPhysWeaponData& );

    ~MachLogPlasmaBolt();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogPlasmaBolt& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogPlasmaBolt );
	PER_FRIEND_READ_WRITE( MachLogPlasmaBolt );

private:
	void doBeDestroyed();

    MachLogPlasmaBolt( const MachLogPlasmaBolt& );
    MachLogPlasmaBolt& operator =( const MachLogPlasmaBolt& );
    bool operator ==( const MachLogPlasmaBolt& );

};

PER_DECLARE_PERSISTENT( MachLogPlasmaBolt );

#endif

/* End PULSEBLB.HPP *************************************************/
