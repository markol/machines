/*
 * V O R T B O M I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogVortexBombImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_VORTBOMI_HPP
#define _MACHLOG_VORTBOMI_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "world4d/world4d.hpp"

class MachPhysWeaponData;
class MachPhysVortexBomb;

class MachLogVortexBombImpl
// Canonical form revoked
{
public:
    MachLogVortexBombImpl( const MachPhysWeaponData* const );
    virtual ~MachLogVortexBombImpl();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogVortexBombImpl& t );

    MachLogVortexBombImpl( const MachLogVortexBombImpl& );
    MachLogVortexBombImpl& operator =( const MachLogVortexBombImpl& );

	friend class MachLogVortexBomb; 

	MachPhysVortexBomb*					pPhysVortexBomb_;	
	const MachPhysWeaponData* const		pWeaponData_;	
	PhysAbsoluteTime					destructionWaveStartTime_;
	PhysAbsoluteTime					destructionWaveFinishTime_;
	PhysAbsoluteTime					destructionTime_;	
};


#define CB_MachLogVortexBomb_DEPIMPL() \
	CB_DEPIMPL( MachPhysVortexBomb*					,pPhysVortexBomb_ ); \
	CB_DEPIMPL( const MachPhysWeaponData* const		,pWeaponData_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,destructionWaveStartTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,destructionWaveFinishTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,destructionTime_ ); 


#endif

/* End VORTBOMI.HPP *************************************************/
