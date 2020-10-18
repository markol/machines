/*
 * B E E W A V E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogBeeBombExplosion

    Triggers the physical bee bomb explosion and handles the logical explosion.
*/

#ifndef _MACHLOG_BEEBOMEX_HPP
#define _MACHLOG_BEEBOMEX_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "machlog/expandbl.hpp"
#include "world4d/world4d.hpp"

// forward declarations
class MachPhysBeeBombExplosion;
class MachLogBeeBombExplosionImpl;
class MachPhysWeaponData;
class MachLogRace;
class MachActor;

class MachLogBeeBombExplosion : public MachLogExpandingBlast
// Canonical form revoked
{
public:
    MachLogBeeBombExplosion(
        MachLogRace* pRace,
        const MexPoint3d& startPosition,
		const MachPhysWeaponData&,
        MachActor* pOwner );

    ~MachLogBeeBombExplosion( void );
									 								 									 
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogBeeBombExplosion& t );
	
	static MachPhysBeeBombExplosion* pNewPhysBeeBombExplosion( const MexPoint3d& startPosition, MachPhysBeeBombExplosion** ppPhysBeeBombExplosion );

protected:

	virtual PhysAbsoluteTime firstWaveStartTime() const;	
	virtual PhysAbsoluteTime firstWaveFinishTime() const;	
	
	virtual bool hitVictimFirstWave( const MachActor& victim ) const ;
	virtual void inflictDamageFirstWave( MachActor* pVictim );
	
	virtual PhysAbsoluteTime destructionTime() const;		
	virtual MATHEX_SCALAR potentialKillRadiusMultiplier() const;
	
private:
	// opeartions deliberately revoked
    MachLogBeeBombExplosion( const MachLogBeeBombExplosion& );
    MachLogBeeBombExplosion& operator =( const MachLogBeeBombExplosion& );
    bool operator ==( const MachLogBeeBombExplosion& );
	
	
	MachLogBeeBombExplosionImpl* pImpl();
	MachPhysBeeBombExplosion* pNewPhysBeeBombExplosion( const MexPoint3d& startPosition, const MachPhysWeaponData& weaponData );

	void doBeDestroyed();
	
	// data members
	MachLogBeeBombExplosionImpl* pImpl_;
};

#endif

/* End BEEBOMEX.HPP *************************************************/
