/*
 * E X P A N D B L . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogExpandingBlast

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_EXPANDBL_HPP
#define _MACHLOG_EXPANDBL_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "sim/projecti.hpp"
#include "world4d/world4d.hpp"

// forward declarations
class MachLogExpandingBlastImpl;
class MachLogRace;
class MachActor;
class MachLogConstruction;
template < class T > class ctl_pvector;

class MachLogExpandingBlast : public SimProjectile		
// Canonical form revoked
{
public:

	enum SuppressDetach{ SUPPRESS_DETACH, DONT_SUPPRESS_DETACH };
	enum CanHitAirUnits{ CAN_HIT_AIR_UNITS, CANT_HIT_AIR_UNITS };

    MachLogExpandingBlast(  MachLogRace* pRace,
							W4dEntity* pPhysObject,
					        const MexPoint3d& startPosition,
					        MachActor* pOwner,
							MATHEX_SCALAR blastRadius,
							CanHitAirUnits canHitAirUnits = CAN_HIT_AIR_UNITS );

    ~MachLogExpandingBlast( void );
	
	typedef ctl_pvector< MachActor > Victims;
	
    virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime,
                                     MATHEX_SCALAR clearanceFromDisplayedVolume );
									 
	virtual bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData = 0 );
						
	const MachLogRace* pRace() const;	
	const MachActor* pOwner() const;
	const MexPoint3d& startPosition() const;
	MATHEX_SCALAR blastRadius() const;
								 									 
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogExpandingBlast& t );
	
protected:

	MachLogRace* pRace();	
	
	void finished( bool status );
	bool finished() const;
		
	MachActor* pOwner();
	
	Victims& 			victims();	
	int nVictims() const;		// do we even need this?
	bool addVictim( MachActor* pVictim );	// returns false if actor was duplicate and thus not added
	void finishedWithVictim( MachActor* pVictim, SuppressDetach suppressDetach = DONT_SUPPRESS_DETACH );
	virtual void ownerDestroyed();
	
	// an unfortunate necessity due to the possibility of concentric waves hitting in the same frame,
	// the second hitting an actor whose DELETED message has not yet got back to us via the observer pattern.
	// This call ENFORCES an observer delete if applicable.
	void victimMayHaveBeenKilled( MachActor* pVictim, int damageTaken );
	
	// note that non-abstract derived classes MUST provide an implementation for testing a hit
	// and implementing damage for the first wave. Second and third waves are not compulsory,
	// and so have a (dummy) default implementation.
	
	virtual bool hitVictimFirstWave( const MachActor& pVictim ) const = 0;	
	virtual bool hitVictimSecondWave( const MachActor& pVictim ) const;
	virtual bool hitVictimThirdWave( const MachActor& pVictim ) const;
	
	virtual void inflictDamageFirstWave( MachActor* pVictim ) = 0;
	virtual void inflictDamageSecondWave( MachActor* pVictim );
	virtual void inflictDamageThirdWave( MachActor* pVictim );
	
	// default times for second and third wave begin and end times is the destruction time of the object,
	// and will automatically mean no damage checking for those waves unless overridden.
	
	virtual PhysAbsoluteTime firstWaveStartTime() const = 0;	
	virtual PhysAbsoluteTime secondWaveStartTime() const { return destructionTime(); }	
	virtual PhysAbsoluteTime thirdWaveStartTime() const { return destructionTime(); }
	
	virtual PhysAbsoluteTime firstWaveFinishTime() const = 0;	
	virtual PhysAbsoluteTime secondWaveFinishTime() const { return destructionTime(); }
	virtual PhysAbsoluteTime thirdWaveFinishTime() const { return destructionTime(); }

	// ratio of blast radius which we should check for victims at the time of the
	// blast - as those outside blast range now may wander inside.
	virtual MATHEX_SCALAR potentialKillRadiusMultiplier() const = 0;
	
	PhysAbsoluteTime creationTime() const;
	virtual PhysAbsoluteTime destructionTime() const = 0;		
	
	bool actorWithinRadius( const MachActor& victim, MATHEX_SCALAR distanceFromEpicentre ) const;	
	
	// this returns NULL if the explosion is not inside a construction
	MachLogConstruction* pConstruction();

private:

    MachLogExpandingBlast( const MachLogExpandingBlast& );
    MachLogExpandingBlast& operator =( const MachLogExpandingBlast& );
    bool operator ==( const MachLogExpandingBlast& );
	MachLogExpandingBlastImpl* pImpl();

	void doBeDestroyed();	
	
	void buildVictimList();
	void buildVictimMachinesList();
	void buildVictimNonMachinesList();
	
	void detachFromFirstWave();
	
	void damageCheckFirstWave();
	void damageCheckSecondWave();
	void damageCheckThirdWave();
	
	bool centreOfActorWithinRadius( const MachActor& victim, MATHEX_SCALAR distanceFromEpicentre ) const;
	bool constructionWithinRadius( const MachLogConstruction& victim, MATHEX_SCALAR distanceFromEpicentre ) const;
		
	// data members
	MachLogExpandingBlastImpl* pImpl_;
};

#endif

/* End EXPANDBL.HPP *************************************************/
