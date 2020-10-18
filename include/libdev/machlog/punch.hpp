/*
 * V O R T B O M B . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogPunchBlast

    A blasting punch, involving punching.
*/

#ifndef _MACHLOG_PUNCH_HPP
#define _MACHLOG_PUNCH_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "machlog/expandbl.hpp"
#include "world4d/world4d.hpp"

// forward declarations
class MachPhysPunchBlast;
class MachLogPunchBlastImpl;
class MachPhysWeaponData;
class MachLogRace;
class MachActor;

class MachLogPunchBlast : public MachLogExpandingBlast
// Canonical form revoked
{
public:
    MachLogPunchBlast(
        MachLogRace* pRace,
        MachPhysPunchBlast* pPhysPunchBlast,
		const MachPhysWeaponData&,
        MachActor* pOwner );

    ~MachLogPunchBlast( void );									
								 									 
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogPunchBlast& t );
	
protected:	
	
	virtual PhysAbsoluteTime firstWaveStartTime() const;	
	virtual PhysAbsoluteTime firstWaveFinishTime() const;	
	
	virtual bool hitVictimFirstWave( const MachActor& victim ) const ;
	virtual void inflictDamageFirstWave( MachActor* pVictim );
	
	virtual PhysAbsoluteTime destructionTime() const;
	
	virtual MATHEX_SCALAR potentialKillRadiusMultiplier() const;
	
	virtual void ownerDestroyed();

private:
	// opeartions deliberately revoked
    MachLogPunchBlast( const MachLogPunchBlast& );
    MachLogPunchBlast& operator =( const MachLogPunchBlast& );
    bool operator ==( const MachLogPunchBlast& );

	void doBeDestroyed();	
	
	// data members
	MachLogPunchBlastImpl* pImpl_;
};

#endif

/* End PUNCH.HPP *************************************************/
