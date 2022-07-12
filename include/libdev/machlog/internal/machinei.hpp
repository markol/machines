/*
 * M A C H I N E I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogMachineImpl

    machine implementation class
*/

#ifndef _MACHLOG_MACHINEI_HPP
#define _MACHLOG_MACHINEI_HPP

#include "base/base.hpp"
#include "machlog/machine.hpp"

class MachLogMachineImpl
// Canonical form revoked
{
public:
    MachLogMachineImpl( MachPhys::MachineType, MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel, MachPhys::Race race );
    ~MachLogMachineImpl();

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT( MachLogMachineImpl );
	PER_FRIEND_READ_WRITE( MachLogMachineImpl );

	//public methods to allow persistence mechanism to do its business.
    MachLogConstruction* 			pTempConstruction() const;
    void							pTempConstruction( MachLogConstruction* );
    bool 							needToIgnoreEntrancePolygon() const;
    void							needToIgnoreEntrancePolygon( bool );
    bool 							needToIgnoreConstructionPolygon() const;
    void							needToIgnoreConstructionPolygon( bool );
    bool 							restoreInsideBuilding() const;
    void							restoreInsideBuilding( bool );
   	MachLogMissileEmplacement*      pSafestMissileEmplacement() const;
   	MachLogMachine*					pSafestMachine() const;
	MachLogConstruction*			pInOrOnPadConstruction() const;
	MachLogEntrance*				pLockedEntrance() const;

private:
	friend class MachLogMachine;
    friend ostream& operator <<( ostream& o, const MachLogMachineImpl& t );

    MachLogMachineImpl( const MachLogMachineImpl& );
    MachLogMachineImpl& operator =( const MachLogMachineImpl& );

    MachPhys::MachineType			type_;
    MachLogMachine::State			state_;
	MachLogEntrance*				pLockedEntrance_;
	MachPhysStation*				pLockedStation_;	
	MachLogMachine::Level			swLevel_;
	MachLogMachine::Level			hwLevel_;
	MachLogSquadron*				pSquadron_;
	MachLogSquadron*				pOriginalSquadron_;
	PhysAbsoluteTime				lastUpdateTime_;
	W4dEntity*              		pMarker_; //The entity used to mark the machine
	MachLog::DefCon					defCon_;
	MachPhysHealAura*				pHealAura_;
	int								healAuraReferences_;
	bool							insideAPC_;
	bool							standGround_;	
	bool							evading_; 		// am I performing evasive manoeuvre operations?
	bool							camouflaged_;			
	
	// used to determine interval during which subsequent hits from gunfire will not cause more verbal e-mails 
   	PhysAbsoluteTime                lastHitTime_;
	
	// virtual defcon will ALWAYS be treated as HIGH for virtualDefConInterval_ seconds after a direct command is issued.
   	PhysAbsoluteTime                lastStrengthEstimateTime_;	
	
	// used in lazy evaluation of local strength 
   	PhysAbsoluteTime                nextTrueDefConTime_;
   	
	// used in lazy evaluation of safest nearby machine and turret to run to 
   	PhysAbsoluteTime                lastSafestMachineEstimateTime_;
   	PhysAbsoluteTime                lastSafestMissileEmplacementEstimateTime_;
   	MachLogMissileEmplacement*      pSafestMissileEmplacement_;
   	MachLogMachine*					pSafestMachine_;
	MachLogAPC*						pAPCImInside_;
	MachLogConstruction*			pInOrOnPadConstruction_; //Set when machine is inside or on an entrance pad
    MachLogConstruction* 			pTempConstruction_; //Used by the persistence mechanism
    bool 							needToIgnoreEntrancePolygon_; //used by persistence to restore ignoring in machinecreated()
    bool 							needToIgnoreConstructionPolygon_; //used by persistence to restore ignoring in machinecreated()
    bool 							restoreInsideBuilding_; //used by persistence to restore inside building
	bool							usingNVG_;
	bool 							isTeleporting_;
	PhysAbsoluteTime				teleportFinishTime_;	
	MachLog1stPersonMachineHandler*	pFirstPersonMachineHandler_;
	bool							rampaging_;				// used by task attack to keep track of which squadron machines have actually set off intoo the world from their assembly points

};

PER_DECLARE_PERSISTENT( MachLogMachineImpl );
#endif

/* End MACHINEI.HPP *************************************************/
