/*
 * P U L S E B L B . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogPulseBlob

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_PULSEBLB_HPP
#define _MACHLOG_PULSEBLB_HPP

#include "base/base.hpp"
#include "machlog/lineproj.hpp"
class MachPhysPulseBlob;
class MachPhysLinearProjectile;
class MachActor;

class MachLogPulseBlob : public MachLogLinearProjectile
// Canonical form revoked
{
public:
    MachLogPulseBlob(
        MachLogRace* pRace,
		MachPhysLinearProjectile*,
        MachActor* pOwner,
        const MachPhysWeaponData& );

    ~MachLogPulseBlob();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogPulseBlob& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogPulseBlob );
	PER_FRIEND_READ_WRITE( MachLogPulseBlob );

private:
	//static MachPhysPulseBlob* pNewPhysPulseBlob( size_t level, const MexPoint3d& startPosition );

	void doBeDestroyed();

    MachLogPulseBlob( const MachLogPulseBlob& );
    MachLogPulseBlob& operator =( const MachLogPulseBlob& );
    bool operator ==( const MachLogPulseBlob& );

};

PER_DECLARE_PERSISTENT( MachLogPulseBlob );

#endif

/* End PULSEBLB.HPP *************************************************/
