/*
 * M C D M G D A T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysMachineDamageData

    Data implementation for W4dEntity
*/

#ifndef _WORLD4D_MCDMGDAT_HPP
#define _WORLD4D_MCDMGDAT_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"

class MachPhysMachine;
class MachPhysMachineBurning;
class MachPhysMachineDamage;

class MachPhysMachineDamageData
// Canonical form revoked
{
public:

    //Persistence
    PER_MEMBER_PERSISTENT( MachPhysMachineDamageData );
    PER_FRIEND_READ_WRITE( MachPhysMachineDamageData );

private:
	MachPhysMachineDamageData( MachPhysMachine* pTargetMachine );
    MachPhysMachineDamageData( const MachPhysMachineDamageData& );

    ~MachPhysMachineDamageData( void );

	MachPhysMachineBurning& machineBurning();
	void finishBurning();
	bool isBurning() const;
	bool isBurningFinished() const;

	bool isDamaged() const;
	void damageLevel( const double& percent );
	const double& damageLevel() const;
	void updateDamageLevel(); //to be called every 60 seconds

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysMachineDamageData& t );
    MachPhysMachineDamageData& operator =( const MachPhysMachineDamageData& );

	friend class MachPhysMachine;

	//data members
	MachPhysMachine*			pTargetMachine_;
    MachPhysMachineBurning* 	pMachineBurning_;          //Data related to being on fire
	MachPhysMachineDamage*		pMachineDamage_;
};

PER_DECLARE_PERSISTENT( MachPhysMachineDamageData );

#endif

/* End MCDMGDAT.HPP *************************************************/
