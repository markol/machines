/*
 * I O N B E A M I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogIonBeamImpl

    Implementation class for a MachLogIonBeam
*/

#ifndef _MACHLOG_IONBEAMI_HPP
#define _MACHLOG_IONBEAMI_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "world4d/world4d.hpp"

class MachPhysWeaponData;
class MachPhysIonBeam;

class MachLogIonBeamImpl
// Canonical form revoked
{
public:
    MachLogIonBeamImpl( const MachPhysWeaponData* const );
    virtual ~MachLogIonBeamImpl( void );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogIonBeamImpl& t );

    MachLogIonBeamImpl( const MachLogIonBeamImpl& );
    MachLogIonBeamImpl& operator =( const MachLogIonBeamImpl& );

	friend class MachLogIonBeam; 

	MachPhysIonBeam*					pPhysIonBeam_;	
	const MachPhysWeaponData* const		pWeaponData_;	
	PhysAbsoluteTime					firstWaveStartTime_;
	PhysAbsoluteTime					firstWaveFinishTime_;
	PhysAbsoluteTime					secondWaveStartTime_;
	PhysAbsoluteTime					secondWaveFinishTime_;
	PhysAbsoluteTime					thirdWaveStartTime_;
	PhysAbsoluteTime					thirdWaveFinishTime_;
	PhysAbsoluteTime					destructionTime_;	
};


#define CB_MachLogIonBeam_DEPIMPL() \
	CB_DEPIMPL( MachPhysIonBeam*					,pPhysIonBeam_ ); \
	CB_DEPIMPL( const MachPhysWeaponData* const		,pWeaponData_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,firstWaveStartTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,firstWaveFinishTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,secondWaveStartTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,secondWaveFinishTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,thirdWaveStartTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,thirdWaveFinishTime_ ); \	
	CB_DEPIMPL( PhysAbsoluteTime					,destructionTime_ ); 


#endif

/* End IONBEAMI.HPP *************************************************/
