/*
 * A G G R E S S R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 * MachLogAggressor
 * Implements the logical behaviour of Aggressors.
 */

#ifndef _MACHLOG_AGGRESSR_HPP
#define _MACHLOG_AGGRESSR_HPP

#include "mathex/radians.hpp"

#include "machlog/canattac.hpp"
#include "machlog/machine.hpp"

/* //////////////////////////////////////////////////////////////// */

class MachPhysAggressor;
class MachPhysAggressorData;

class MachLogAggressor
: public MachLogMachine,
  public MachLogCanAttack
// canonical form revoked
{
public:

    MachLogAggressor( const MachPhys::AggressorSubType& subType, Level hwLevel, Level swLevel,
    					MachLogRace * pRace, const MexPoint3d& location,
    					MachPhys::WeaponCombo );

    MachLogAggressor( const MachPhys::AggressorSubType& subType, Level hwLevel, Level swLevel,
    					MachLogRace * pRace, const MexPoint3d& location,
    					MachPhys::WeaponCombo, UtlId withId );

    virtual ~MachLogAggressor() {};

	///////////////////////////////

	//view of MachPhys data objects
	virtual const MachPhysMachineData& machineData() const;
	const MachPhysAggressorData& data() const;

	///////////////////////////////

    void CLASS_INVARIANT;
	

	// inherited from SimActor and MachActor
	virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR clearanceFromDisplayedVolume );

	///////////////////////////////
	
	const MachPhysAggressor& physAggressor() const;
	MachPhysAggressor& physAggressor();

	const MachPhys::AggressorSubType& subType() const;
	
	// inherited from MachLogMachine
    virtual bool fearsThisActor( const MachActor& otherActor ) const;
    // inherited from MachActor
    virtual int militaryStrength() const;

	//Amount of damage is passed in beHit()
	virtual void beHit( const int&, MachPhys::WeaponType byType = MachPhys::N_WEAPON_TYPES,
						MachActor* pByActor = NULL, MexLine3d* pByDirection = NULL, MachActor::EchoBeHit = MachActor::ECHO );

	virtual void beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay = 0, MachActor* pByActor = NULL, EchoBeHit = ECHO );

	// useful for eradicator-specific logic
	bool isEradicator() const;

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogAggressor );
	PER_FRIEND_READ_WRITE( MachLogAggressor );


private:


    static MachPhysAggressor* pNewPhysAggressor( const MachPhys::AggressorSubType& subType, Level hwLevel, Level swLevel, MachLogRace * pRace,
                                                 const MexPoint3d& location,
                                                 MachPhys::WeaponCombo );

    MachLogAggressor( const MachLogAggressor& );
    MachLogAggressor& operator =( const MachLogAggressor& );

	///////////////////////////////

    MachPhys::AggressorSubType 		subType_;

	///////////////////////////////
};

PER_DECLARE_PERSISTENT( MachLogAggressor );
/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHLOG_AGGRESSR_HPP	*/

/* End AGGRESSR.HPP *************************************************/
