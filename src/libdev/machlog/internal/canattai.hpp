/*
 * C A N A T T A I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogCanAttackImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_CANATTAI_HPP
#define _MACHLOG_CANATTAI_HPP

#include "base/base.hpp"
#include "ctl/pvector.hpp"
#include "machlog/canattac.hpp"

class MachLogCanAttackImpl
// Canonical form revoked
{
public:
    MachLogCanAttackImpl( MachActor*, MachPhysCanAttack*, MachPhys::WeaponCombo );
    ~MachLogCanAttackImpl();

	PER_MEMBER_PERSISTENT( MachLogCanAttackImpl );
	PER_FRIEND_READ_WRITE( MachLogCanAttackImpl );

    void CLASS_INVARIANT;

private:
	friend class MachLogCanAttack;
    friend ostream& operator <<( ostream& o, const MachLogCanAttackImpl& t );

    MachLogCanAttackImpl( const MachLogCanAttackImpl& );
    MachLogCanAttackImpl& operator =( const MachLogCanAttackImpl& );

    MachActor*              	pMe_; //The actor which is the canAttack
    MachPhysCanAttack*      	pPhysCanAttack_; //The physical rep for me
	MachActor*					pCurrentTarget_;
	bool						currentlyAttached_;
	MachPhys::WeaponCombo		weaponCombo_;
	MachLogCanAttack::Weapons	weapons_;
	ulong						lastFireFrame_;
	MATHEX_SCALAR				minimumInaccuracy_;		// ranges from 0 to 100.0
	MATHEX_SCALAR				currentInaccuracy_;		// as above, but has lower bound of above. Can vary above this due to damage etc.
	PhysAbsoluteTime			lastFireTime_;			// used as a reference to improve aim in successive shots
	MATHEX_SCALAR 				lastFireDiceRoll_;			// as above - the actual quality of the shot last time, as it were
	int							alertness_;				// determines how frequently environment is sampled for targets
	PhysAbsoluteTime			lastAlertnessDiminishTime_;		// last time your alertness was properly dimished (i.e. by at least 1)
	PhysAbsoluteTime			nextTimeAllowedToCheckAndAttack_;	// if current time is less than this, won't fo CAACT
	PhysAbsoluteTime			nextHitByTestTime_;		// time after which we will test CAACT on actor we've been hit by even IF we already have a current target.
	MATHEX_SCALAR				maxWeaponRange_;		// cached for efficiency. Updated whenever armed with a new weapon
};

PER_DECLARE_PERSISTENT( MachLogCanAttackImpl );

#endif

/* End CANATTAI.HPP *************************************************/
