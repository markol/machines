/*
 * N U C L B O M B . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogNuclearBomb

    Triggers the physical ion beam animation and handles the logical explosion.
*/

#ifndef _MACHLOG_NUCLBOMB_HPP
#define _MACHLOG_NUCLBOMB_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "machlog/expandbl.hpp"
#include "world4d/world4d.hpp"

// forward declarations
class MachPhysNuclearBomb;
class MachLogNuclearBombImpl;
class MachPhysWeaponData;
class MachLogRace;
class MachActor;

class MachLogNuclearBomb : public MachLogExpandingBlast
// Canonical form revoked
{
public:
    MachLogNuclearBomb(	MachLogRace* pRace,
        				const MexPoint3d& startPosition,
						const MachPhysWeaponData&,
        				MachActor* pOwner );

    ~MachLogNuclearBomb();
									 								 									 
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogNuclearBomb& t );
	
	static MachPhysNuclearBomb* pNewPhysNuclearBomb( const MexPoint3d& startPosition, 
													MachPhysNuclearBomb** ppPhysNuclearBomb );

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
    MachLogNuclearBomb( const MachLogNuclearBomb& );
    MachLogNuclearBomb& operator =( const MachLogNuclearBomb& );
    bool operator ==( const MachLogNuclearBomb& );
	
	
	MachLogNuclearBombImpl* pImpl();
	MachPhysNuclearBomb* pNewPhysNuclearBomb( const MexPoint3d& startPosition, const MachPhysWeaponData& weaponData );

	void doBeDestroyed();		
	
	// data members
	MachLogNuclearBombImpl* pImpl_;
};

#endif

/* End NUCLBOMB.HPP *************************************************/
