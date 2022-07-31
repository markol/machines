/*
 * I O N B E A M . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogIonBeam

    Triggers the physical ion beam animation and handles the logical explosion.
*/

#ifndef _MACHLOG_IONBEAM_HPP
#define _MACHLOG_IONBEAM_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "machlog/expandbl.hpp"
#include "world4d/world4d.hpp"

// forward declarations
class MachPhysIonBeam;
class MachLogIonBeamImpl;
class MachPhysWeaponData;
class MachLogRace;
class MachActor;

class MachLogIonBeam : public MachLogExpandingBlast
// Canonical form revoked
{
public:
    MachLogIonBeam(
        MachLogRace* pRace,
        const MexPoint3d& startPosition,
		const MachPhysWeaponData&,
        MachActor* pOwner );

    ~MachLogIonBeam();
									 								 									 
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogIonBeam& t );
	
	static MachPhysIonBeam* pNewPhysIonBeam( const MexPoint3d& startPosition, MachPhysIonBeam** ppPhysIonBeam );

protected:

	virtual PhysAbsoluteTime firstWaveStartTime() const;	
	virtual PhysAbsoluteTime firstWaveFinishTime() const;	
	virtual PhysAbsoluteTime secondWaveStartTime() const;	
	virtual PhysAbsoluteTime secondWaveFinishTime() const;
	virtual PhysAbsoluteTime thirdWaveStartTime() const;	
	virtual PhysAbsoluteTime thirdWaveFinishTime() const;
	
	virtual bool hitVictimFirstWave( const MachActor& victim ) const ;
	virtual void inflictDamageFirstWave( MachActor* pVictim );
	virtual bool hitVictimSecondWave( const MachActor& victim ) const ;
	virtual void inflictDamageSecondWave( MachActor* pVictim );
	virtual bool hitVictimThirdWave( const MachActor& victim ) const ;
	virtual void inflictDamageThirdWave( MachActor* pVictim );
	
	virtual PhysAbsoluteTime destructionTime() const;
	
	virtual MATHEX_SCALAR potentialKillRadiusMultiplier() const;
	
private:
	// opeartions deliberately revoked
    MachLogIonBeam( const MachLogIonBeam& );
    MachLogIonBeam& operator =( const MachLogIonBeam& );
    bool operator ==( const MachLogIonBeam& );
	
	
	MachLogIonBeamImpl* pImpl();
	MachPhysIonBeam* pNewPhysIonBeam( const MexPoint3d& startPosition, const MachPhysWeaponData& weaponData );

	void doBeDestroyed();
	
	// data members
	MachLogIonBeamImpl* pImpl_;
};

#endif

/* End IONBEAM.HPP *************************************************/
