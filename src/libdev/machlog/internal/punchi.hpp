/*
 * P U N C H I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogPunchBlastImpl

    Punch Blast Implementation class - as seen on TV's "Tomorrow's World"
*/

#ifndef _MACHLOG_PUNCHI_HPP
#define _MACHLOG_PUNCHI_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "world4d/world4d.hpp"

class MachPhysWeaponData;
class MachPhysPunchBlast;

class MachLogPunchBlastImpl
// Canonical form revoked
{
public:
    MachLogPunchBlastImpl( const MachPhysWeaponData* const );
    virtual ~MachLogPunchBlastImpl();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogPunchBlastImpl& t );

    MachLogPunchBlastImpl( const MachLogPunchBlastImpl& );
    MachLogPunchBlastImpl& operator =( const MachLogPunchBlastImpl& );

	friend class MachLogPunchBlast; 

	MachPhysPunchBlast*					pPhysPunchBlast_;	
	const MachPhysWeaponData* const		pWeaponData_;	
	PhysAbsoluteTime					destructionWaveStartTime_;
	PhysAbsoluteTime					destructionWaveFinishTime_;
	PhysAbsoluteTime					destructionTime_;	
};


#define CB_MachLogPunchBlast_DEPIMPL() \
	CB_DEPIMPL( MachPhysPunchBlast*					,pPhysPunchBlast_ ); \
	CB_DEPIMPL( const MachPhysWeaponData* const		,pWeaponData_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,destructionWaveStartTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,destructionWaveFinishTime_ ); \
	CB_DEPIMPL( PhysAbsoluteTime					,destructionTime_ ); 


#endif

/* End PUNCHI.HPP *************************************************/
