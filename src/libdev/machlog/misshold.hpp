/*
 * M I S S H O L D . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogMissileHolder

    The missiles require an update method to be called once per frame - this allows the smoke
	trail to work. In a networking environment we do not want to have full blown MLMissiles
	on the other nodes - we do not in fact want any linear projectile classes.
	To overcome this we have a simple holder class derived from SimProjectile.
*/

#ifndef _MACHLOG_MISSHOLD_HPP
#define _MACHLOG_MISSHOLD_HPP

#include "base/base.hpp"
#include "sim/projecti.hpp"

class MachPhysMissile;

class MachLogMissileHolder : public SimProjectile
// Canonical form revoked
{
public:
    MachLogMissileHolder( SimProcess* pProcess, W4dEntity* pPhysObject, const PhysAbsoluteTime& destroyAtTime );
    ~MachLogMissileHolder();

    void CLASS_INVARIANT;

    virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime,
                                     MATHEX_SCALAR clearanceFromDisplayedVolume );


private:
    friend ostream& operator <<( ostream& o, const MachLogMissileHolder& t );

    MachLogMissileHolder( const MachLogMissileHolder& );
    MachLogMissileHolder& operator =( const MachLogMissileHolder& );
	PhysAbsoluteTime 	destroyAtTime_;
	MachPhysMissile*	pPhysMissile_;

};


#endif

/* End MISSHOLD.HPP *************************************************/
