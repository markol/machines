/*
 * M A C H V M A N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogMachineVoiceMailManager

	A singleton handler class cross-referencing machine type and message type
	and passing on machine-specific mail id to voicemail manager
*/

#ifndef _MACHLOG_MACHVMAN_HPP
#define _MACHLOG_MACHVMAN_HPP

#include "utility/id.hpp"

#include "mathex/point3d.hpp"

#include "machlog/vmman.hpp"
#include "machlog/vmdata.hpp"
#include "machlog/machlog.hpp"


class MachLogVoiceMail;
class MachLogVoiceMailInfo;

class MachLogMachineVoiceMailManager
// Canonical form revoked
{
public:

	enum MachineVoiceMailEventID
	{
		MEV_SELF_DESTRUCT,
		MEV_BUILT,
		MEV_DESTROYED,
		MEV_NEAR_DEATH,
		MEV_CHANGED_RACE,
		MEV_VIRUS_INFECTED,
		MEV_DAMAGED,
		MEV_TASKED,
		MEV_MOVING,
		MEV_SELECTED,
		MEV_TARGET_ENEMY,
		MEV_RECYCLE,
		
		MEV_HEAL_TARGET,
		MEV_HEALING_COMPLETE,
		
		MEV_LAUNCH_VIRUS,
		MEV_VIRUS_LAUNCHED,
		MEV_TREACHERY_TARGET,
		
		MEV_BUILDING_COMPLETE,
		MEV_AWAITING_NEW_JOB,
		MEV_MOVING_TO_NEXT,
		MEV_MOVE_TO_SITE,
		MEV_BUILDING_CAPTURED,
		MEV_BUILDING_DECONSTRUCTED,
		MEV_BUILDING_REPAIRED
	};	

    //  Singleton class
    static MachLogMachineVoiceMailManager& instance( void );
    ~MachLogMachineVoiceMailManager( void );
	
	//Post verbal mail onto queue
	void postNewMail( MachLog::ObjectType, int subType, MachineVoiceMailEventID id, MachPhys::Race targetRace );
	void postNewMail( MachLog::ObjectType, int subType, MachineVoiceMailEventID id, UtlId actorId, MachPhys::Race targetRace );
	void postNewMail( MachLog::ObjectType, int subType, MachineVoiceMailEventID id, MexPoint3d position, MachPhys::Race targetRace );	

    void CLASS_INVARIANT;

private:

	// methods deliberately revoked
	
	MachLogMachineVoiceMailManager( const MachLogMachineVoiceMailManager& );
    MachLogMachineVoiceMailManager& operator =( const MachLogMachineVoiceMailManager& );
    MachLogMachineVoiceMailManager( void );
	
	//-------------------------------
	
	
	VoiceMailID getGlobalFromMachineEvent( MachLog::ObjectType ot, int subType, MachineVoiceMailEventID id, UtlId actorId = 0 );

    friend ostream& operator <<( ostream& o, const MachLogMachineVoiceMailManager& t );

};

ostream& operator <<( ostream& o, MachLogMachineVoiceMailManager::MachineVoiceMailEventID );


#endif

/* End MACHVMAN.HPP ****************************************************/
