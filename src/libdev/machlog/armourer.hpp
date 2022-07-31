/*
 * A R M O U R E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogArmourer

    static methods only.
	This class will take the various types of MLCanAttack and arm them accordingly.
	The physical weapons will already have been applied to the model. This class
	will create the MLWeapon instances, tie them to the MPWeapons and then assign them to the MLCanAttack.

*/

#ifndef _MACHLOG_ARMOURER_HPP
#define _MACHLOG_ARMOURER_HPP

#include "base/base.hpp"

class MachLogAggressor;
class MachLogAdministrator;
class MachLogMissileEmplacement;
class MachLogPod;
class MachLogWeapon;
class MachPhysWeapon;
class MachLogRace;
class MachActor;

class MachLogArmourer
// Canonical form revoked
{
public:
    ~MachLogArmourer();

	static void arm( MachLogAggressor* );
	static void rearm( MachLogAggressor* );
	static void arm( MachLogAdministrator* );
	static void rearm( MachLogAdministrator* );
	static void arm( MachLogMissileEmplacement* );
	static void rearm( MachLogMissileEmplacement* );
	static void arm( MachLogPod* );
	static void rearm( MachLogPod* );

	static void checkAndSetSpecialWeapons( MachLogWeapon*, MachPhysWeapon* );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogArmourer& t );

private:

	static MachLogWeapon* createWeapon( MachLogRace*, MachPhysWeapon*, MachActor* );

    MachLogArmourer();
    MachLogArmourer( const MachLogArmourer& );
    MachLogArmourer& operator =( const MachLogArmourer& );
    bool operator ==( const MachLogArmourer& );

};


#endif

/* End ARMOURER.HPP *************************************************/
