/*
 * B E E B O M X I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogBeeBombExplosionImpl

    Implementation class for a MachLogBeeBombExplosion
*/

#ifndef _MACHLOG_BEEBOMXI_HPP
#define _MACHLOG_BEEBOMXI_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "world4d/world4d.hpp"

class MachPhysWeaponData;
class MachPhysBeeBombExplosion;

class MachLogBeeBombExplosionImpl
// Canonical form revoked
{
public:
    MachLogBeeBombExplosionImpl( const MachPhysWeaponData* const );
    virtual ~MachLogBeeBombExplosionImpl();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogBeeBombExplosionImpl& t );

    MachLogBeeBombExplosionImpl( const MachLogBeeBombExplosionImpl& );
    MachLogBeeBombExplosionImpl& operator =( const MachLogBeeBombExplosionImpl& );

	friend class MachLogBeeBombExplosion; 

	MachPhysBeeBombExplosion* 			pPhysBeeBombExplosion_;	
	const MachPhysWeaponData* const		pWeaponData_;	
	PhysAbsoluteTime					firstWaveStartTime_;
	PhysAbsoluteTime					firstWaveFinishTime_;
	PhysAbsoluteTime					destructionTime_;	
};


#define CB_MachLogBeeBombExplosion_DEPIMPL() \
	CB_DEPIMPL( MachPhysBeeBombExplosion*			,pPhysBeeBombExplosion_ ); \
	CB_DEPIMPL( const MachPhysWeaponData* const		,pWeaponData_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,firstWaveStartTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,firstWaveFinishTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,destructionTime_ ); 


#endif

/* End BEEBOMXI.HPP *************************************************/
