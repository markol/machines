/*
 * A D M I N I S T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _MACHLOG_ADMINIST_HPP
#define _MACHLOG_ADMINIST_HPP

#include "machlog/machine.hpp"

#include "machlog/canattac.hpp"
#include "machlog/canadmin.hpp"
//#include "machlog/cntrl.hpp"
/* //////////////////////////////////////////////////////////////// */

//class ostream;
class MachPhysAdministrator;
class MachPhysAdministratorData;

class MachLogAdministrator
: public MachLogMachine,
//  public MachLogCanPickUp,
//  public MachLogCanPutDown,
  public MachLogCanAttack,
  public MachLogCanAdminister
// cannonical form revoked
{
public:

	MachLogAdministrator( 	const MachPhys::AdministratorSubType& subType, 
							MachLogMachine::Level hwLevel,  MachLogMachine::Level swLevel, 
    						MachLogRace * pRace, const MexPoint3d& location,
    						MachPhys::WeaponCombo wc );

	MachLogAdministrator( 	const MachPhys::AdministratorSubType& subType, 
							MachLogMachine::Level hwLevel,  MachLogMachine::Level swLevel, 
    						MachLogRace * pRace, const MexPoint3d& location,
    						MachPhys::WeaponCombo wc, UtlId );

	virtual ~MachLogAdministrator();
	
	///////////////////////////////

	//view of MachPhys data objects
	virtual const MachPhysMachineData& machineData() const;
	const MachPhysAdministratorData& data() const;
	
	//inherited from MachLogCanCommunicate
	virtual void receiveMessage( const MachLogMessage& message );

	void handleIdleTechnician( MachLogCommsId obj );

	const MachPhysAdministrator& physAdministrator() const;
	MachPhysAdministrator& physAdministrator();

	const MachPhys::AdministratorSubType& subType() const;

	// inherited from SimActor and MachActor
	virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR clearanceFromDisplayedVolume );
	
	// inherited from MachLogMachine
    virtual bool fearsThisActor( const MachActor& otherActor ) const;
    // inherited from MachActor
    virtual int militaryStrength() const;
	
	//Amount of damage is passed in beHit()
	virtual void beHit( const int&, MachPhys::WeaponType byType = MachPhys::N_WEAPON_TYPES,
						MachActor* pByActor = NULL, MexLine3d* pByDirection = NULL, MachActor::EchoBeHit = MachActor::ECHO );

	virtual void beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay = 0, MachActor* pByActor = NULL, EchoBeHit = ECHO );

    

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogAdministrator );
	PER_FRIEND_READ_WRITE( MachLogAdministrator );

private:


	MachLogAdministrator( const MachLogAdministrator& );
	MachLogAdministrator& operator =( const MachLogAdministrator& );
	bool operator ==( const MachLogAdministrator& ) const;

    static MachPhysAdministrator* pNewPhysAdministrator( const MachPhys::AdministratorSubType& subType, 
    											Level hwLevel, Level swLevel, MachLogRace * pRace,
                                                const MexPoint3d& location,
                                                MachPhys::WeaponCombo );

	virtual void doOutputOperator( ostream& o ) const;
	MachPhys::AdministratorSubType		subType_;

};

PER_DECLARE_PERSISTENT( MachLogAdministrator );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHLOG_ADMINIST_HPP	*/

/* End ADMINIST.HPP *************************************************/