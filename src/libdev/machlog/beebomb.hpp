/*
 * B E E B O M B . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogBeeBomb

*/

#ifndef _MACHLOG_BEEBOMB_HPP
#define _MACHLOG_BEEBOMB_HPP

#include "base/base.hpp"
#include "machlog/lineproj.hpp"

class MachPhysBeeBomb;
class MachPhysLinearProjectile;
class MachActor;

class MachLogBeeBomb : public MachLogLinearProjectile
// Canonical form revoked
{
public:
    MachLogBeeBomb(
        MachLogRace* pRace,
		MachPhysLinearProjectile*,
        MachActor* pOwner,
        const MachPhysWeaponData& );

    ~MachLogBeeBomb();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogBeeBomb& t );

	//PER_MEMBER_PERSISTENT_VIRTUAL( MachLogBeeBomb );
	//PER_FRIEND_READ_WRITE( MachLogBeeBomb );

private:
	void doBeDestroyed();

    MachLogBeeBomb( const MachLogBeeBomb& );
    MachLogBeeBomb& operator =( const MachLogBeeBomb& );
    bool operator ==( const MachLogBeeBomb& );
	
	// data members
	
	MachLogRace*						pLogRace_;
	const MachPhysWeaponData& 			weaponData_;

};

//PER_DECLARE_PERSISTENT( MachLogBeeBomb );

#endif

/* End BEEBOMB.HPP *************************************************/
