/*
 * C O N T R O L R . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 * MachLogPCController represents one of the PC UI/ML interface
 * in the logical command heirarchy. 
 */

#ifndef _MACHLOG_CNTRL_PC_HPP
#define _MACHLOG_CNTRL_PC_HPP

#include "sim/actor.hpp"

#include "machphys/machphys.hpp"

#include "machlog/cntrl.hpp"
#include "machlog/message.hpp"

/* //////////////////////////////////////////////////////////////// */

class MachLogPCController
: public MachLogController
{
public:

	enum SquadronType
	{
		NO_SQUAD,
		LOCATOR_SQUAD,
		CONSTRUCTION_SQUAD,
		PATROL_SQUAD,
		KILLER_SQUAD,
		N_SQUAD_TYPES
	};

	MachLogPCController( MachLogRace* logRace, W4dEntity* pPhysObject );

	virtual ~MachLogPCController();

	// inherited from MachLogCanCommunicate...
	virtual void sendMessage( MachLogCommsId to, const MachLogMessage& ) const;
	virtual void receiveMessage( const MachLogMessage& );

	//inherited from MachLogCanAdminister
	virtual void handleIdleTechnician( MachLogCommsId );
	virtual void handleIdleAdministrator( MachLogCommsId );
	virtual void handleIdleAggressor( MachLogCommsId );
	virtual void handleIdleAPC( MachLogCommsId );
	virtual void handleIdleResourceCarrier( MachLogCommsId );
	virtual void handleIdleConstructor( MachLogCommsId );
	virtual void handleIdleGeoLocator( MachLogCommsId );
	virtual void handleIdleSpyLocator( MachLogCommsId );
	
	virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR );

	virtual void doOutputOperator( ostream& o) const;

	friend ostream& operator<<( ostream&, const MachLogPCController& );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogPCController );
	PER_FRIEND_READ_WRITE( MachLogPCController );

private:

};

PER_DECLARE_PERSISTENT( MachLogPCController );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHLOG_CONTRL_PC_HPP	*/

/* End CONTROLR.HPP *************************************************/
