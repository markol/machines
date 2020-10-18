/*
 * W E A P O N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogWeapon

    Logical descriptor for a weapon. Each one has MachPhysWeapon
*/

#ifndef _MACHLOG_WEAPON_HPP
#define _MACHLOG_WEAPON_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "world4d/observer.hpp"
#include "machphys/machphys.hpp"

class MachPhysWeapon;
class MachActor;
class MachLogRace;
class Point3d;
class MachLogFireData;
class MexVec3;

class MachLogWeapon : public W4dObserver
// Canonical form revoked
{
public:
	enum DummySeed{ DUMMY_SEED = -1 };
	
    MachLogWeapon( MachLogRace*, MachPhysWeapon*, MachActor* pOwner );
    virtual ~MachLogWeapon( void );

	PhysRelativeTime fire( MachActor* pTarget, const MachLogFireData& fireData );
	PhysRelativeTime fire( MachActor* pTarget );
	PhysRelativeTime fire( const MexPoint3d& position );
    
	bool recharged() const;

	// instant discharge and instant recharge methods
	void setChargeToZero();
	void setChargeToMaximum();

	int	percentageRecharge() const;
	const MachPhysWeapon& physWeapon() const;
	MATHEX_SCALAR range() const;
	const MachActor& owner() const;


    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogWeapon& t );

	virtual bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData = 0 );

    virtual void domainDeleted( W4dDomain* pDomain );

    MachPhys::Mounting mounting() const;
    MachPhys::WeaponType type() const;

	void fireWithoutEcho( int numberInBurst );

	//the persistence mechanism has to reset which weapons go where
	//so we have to allow it to remount the weapons.
	void setPhysicalWeapon( MachPhysWeapon* );
	
	PhysAbsoluteTime lastFireTime() const;

	PER_MEMBER_PERSISTENT_ABSTRACT( MachLogWeapon );
	PER_FRIEND_READ_WRITE( MachLogWeapon );

protected:

	MachLogRace&	logRace();
	MachPhysWeapon& physWeapon();
	virtual void doFire( MachActor* pTarget, const MachLogFireData& fireData )=0;
	virtual void doFire( const MexPoint3d& position );
	MachActor& owner();
	MexVec3 getDirection( const MexPoint3d& start, MachActor* pTarget );
	
	//Attaching methods
	bool currentlyAttached();
	void currentlyAttached( bool );

	MachActor& target();
	void target( MachActor* );

private:
	void dt( const char* text, bool start );
    MachLogWeapon( const MachLogWeapon& );
    MachLogWeapon& operator =( const MachLogWeapon& );
    bool operator ==( const MachLogWeapon& );
	MachPhysWeapon*		pPhysWeapon_;
	PhysAbsoluteTime	lastFireTime_;
	MachLogRace*		pLogRace_;
	MachActor*			pOwner_;

	//only certain guns types will attach to targets
	bool				currentlyAttached_;
	MachActor*			pTarget_;


};

PER_DECLARE_PERSISTENT( MachLogWeapon );

#endif

/* End WEAPON.HPP ***************************************************/
