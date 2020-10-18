
#ifndef _MACHLOG_ACTNAMID_HPP
#define _MACHLOG_ACTNAMID_HPP

#include "machlog/machlog.hpp"
#include "machphys/machphys.hpp"
#include "stdlib/string.hpp"

class MachActor;

class MachLogActorStringIdRestorer
// cannonical form revoked
{
public:
	MachLogActorStringIdRestorer();
	virtual ~MachLogActorStringIdRestorer();

	// Returns string id used to display actors sub-type ( e.g. Lab-Tech, Commanded, Grunt ). Add hardware
	// level to get stringId that describes specific machine type ( e.g. Judas Warlord, Shark )
	static uint stringId( MachActor* );
	static uint stringId( MachLog::ObjectType objType, size_t subType );

	// Returns string id used to display name of weapon.
	static uint weaponStringId( MachPhys::WeaponCombo wc );

	// To display full id text about a machine the weaponStringId and stringId+hardware level 
	// are used ( stringId + hardware level + "WITH" + weaponStringId ). Sometimes this is not 
	// correct therefore isExceptionToRule should be called first which will return true if
	// the standard way of displaying full info is not sufficient. isExceptionToRule will return
	// the correct stringId which should not be modified in any way ( i.e. do not add hardware level ).
	static bool isExceptionToRule( MachLog::ObjectType objType, size_t subType, MachPhys::WeaponCombo wc, int level, uint* pStringId );
	static bool isExceptionToRule( MachActor*, uint* pStringId );

	static string getActorPromptText( MachActor*, uint actorStrId, uint actorWithWeaponStrId );
	static string getActorPromptText( MachLog::ObjectType objType, size_t subType, MachPhys::WeaponCombo wc, int level, uint actorStrId, uint actorWithWeaponStrId );
	
private:
	MachLogActorStringIdRestorer( const MachLogActorStringIdRestorer& );
	MachLogActorStringIdRestorer& operator =( const MachLogActorStringIdRestorer& );
	bool operator ==( const MachLogActorStringIdRestorer& );

	// Data members...	
	uint stringId_;
};

/* //////////////////////////////////////////////////////////////// */

#endif	// #ifndef _MACHLOG_RESTORER_HPP

/* End RESTORER.HPP *************************************************/