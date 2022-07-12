/*
 * N U C L B O M I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogNuclearBombImpl

    Implementation class for a MachLogNuclearBomb
*/

#ifndef _MACHLOG_NUCLBOMI_HPP
#define _MACHLOG_NUCLBOMI_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "world4d/world4d.hpp"

class MachPhysWeaponData;
class MachPhysNuclearBomb;

class MachLogNuclearBombImpl
// Canonical form revoked
{
public:
    MachLogNuclearBombImpl( const MachPhysWeaponData* const );
    virtual ~MachLogNuclearBombImpl();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogNuclearBombImpl& t );

    MachLogNuclearBombImpl( const MachLogNuclearBombImpl& );
    MachLogNuclearBombImpl& operator =( const MachLogNuclearBombImpl& );

	friend class MachLogNuclearBomb; 

	MachPhysNuclearBomb*				pPhysNuclearBomb_;	
	const MachPhysWeaponData* const		pWeaponData_;	
	PhysAbsoluteTime					firstWaveStartTime_;
	PhysAbsoluteTime					firstWaveFinishTime_;
	PhysAbsoluteTime					secondWaveStartTime_;
	PhysAbsoluteTime					secondWaveFinishTime_;
	PhysAbsoluteTime					thirdWaveStartTime_;
	PhysAbsoluteTime					thirdWaveFinishTime_;
	PhysAbsoluteTime					destructionTime_;
	
	MATHEX_SCALAR 						shockwaveMainDamageRadius_;
		
};


#define CB_MachLogNuclearBomb_DEPIMPL() \
	CB_DEPIMPL( MachPhysNuclearBomb*					,pPhysNuclearBomb_ ); \
	CB_DEPIMPL( const MachPhysWeaponData* const		,pWeaponData_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,firstWaveStartTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,firstWaveFinishTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,secondWaveStartTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,secondWaveFinishTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,thirdWaveStartTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,thirdWaveFinishTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,shockwaveMainDamageRadius_ ); \	
	CB_DEPIMPL( PhysAbsoluteTime					,destructionTime_ ); 


#endif

/* End NUCLBOMI.HPP *************************************************/
