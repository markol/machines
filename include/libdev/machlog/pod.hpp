/*
 * POD . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogPod

    Handle logical operations for a Pod construction
*/

#ifndef _MACHLOG_POD_HPP
#define _MACHLOG_POD_HPP

#include "base/base.hpp"

#include "machlog/constron.hpp"
#include "machlog/canattac.hpp"

//Forward declarations
class MachPhysPod;
class MachLogRace;
class MexPoint3d;
class MexRadians;
class MachPhysPodData;
class MachLogPodImpl;

//Orthodox canonical (revoked)
class MachLogPod : public MachLogConstruction, public MachLogCanAttack
{
public:
    //Construct smelter of deisgnated race and level at location, rotated thru angle about
    //z axis.
    MachLogPod( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle, MachPhys::WeaponCombo wc );

    MachLogPod( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle, MachPhys::WeaponCombo wc, UtlId );

    ~MachLogPod();

    //Inherited from SimActor
    virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime,
                                     MATHEX_SCALAR clearanceFromDisplayedVolume );

	virtual const MachPhysConstructionData& constructionData() const;
	const MachPhysPodData& data() const;

	virtual PhysRelativeTime attack( MachActor* pTarget );
	virtual PhysRelativeTime attack( const MexPoint3d& position );
    bool hasIonCannon() const;
    bool ionCannonReady() const;

	void activateIonCannon();
	///////////////////////////////
	
	bool canAttack( const MachActor& other ) const;
	bool canTurnToAttack( const MachActor& other ) const;

	///////////////////////////////
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogPod& t );

    const MachPhysPod* pPhysPod() const;
	
	virtual void assignToDifferentRace( MachLogRace& newRace );
	
	virtual void beHit( const int&, MachPhys::WeaponType byType = MachPhys::N_WEAPON_TYPES,
						MachActor* pByActor = NULL, MexLine3d* pByDirection = NULL, EchoBeHit = MachActor::ECHO );
						
	virtual void beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay = 0, 
						MachActor* pByActor = NULL, EchoBeHit = MachActor::ECHO );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogPod );
	PER_FRIEND_READ_WRITE( MachLogPod );

private:
    // Operations deliberately revoked
    MachLogPod( const MachLogPod& );
    MachLogPod& operator =( const MachLogPod& );
    bool operator ==( const MachLogPod& );

    //Construct a new physical smelter. Used in ctor initializer list
    static MachPhysPod* pNewPhysPod( MachLogRace* pRace, uint level,
                     const MexPoint3d& location, const MexRadians& angle );

	void ionCannonAcquiredForFirstTime();
					 
    //The pod emplacement
    MachPhysPod* pPhysPod();
	
	//inherited from MachActor
	virtual int militaryStrength() const;

	bool inWeaponRange( const MachActor& other ) const;
    bool inAngleRange( const W4dEntity& entity ) const;
	static MexRadians angleRange();
	static MATHEX_SCALAR sinAngleRange();

    //Data members
	
	MachLogPodImpl* pImpl_;

};

PER_DECLARE_PERSISTENT( MachLogPod );

#ifdef _INLINE
    #include "machlog/pod.ipp"
#endif


#endif

/* End POD.HPP **************************************************/
