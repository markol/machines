/*
 * C N T R L _ A I . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 * MachLogAIController represents the top of the
 * logical command heirarchy. I.e. it is the entity responsible
 * (for an NPC) of making the strategic desicions. This includes such things 
 * as creating administrator patrols.
 */

#ifndef _MACHLOG_CONTROLR_AI_HPP
#define _MACHLOG_CONTROLR_AI_HPP

//#include "ctl/pvector.hpp"
//#include "ctl/list.hpp"

//#include "utility/linetok.hpp"
#include "machphys/machphys.hpp"

#include "machlog/message.hpp"
//#include "machlog/pod.hpp"

#include "machlog/cntrl.hpp"
#include "stdlib/strfwd.hpp"

/* //////////////////////////////////////////////////////////////// */
class MachLogFactory;
class MachLogProductionUnit;
class MachLogMachine;
class MexPoint2d;
class MexPoint3d;
class W4dEntity;
class MachLogRace;
class MachLogSquadron;
struct DesiredMachineData;
class MachLogAIStrategicProductionUnit;
class UtlLineTokeniser;
class MachLogAIControllerImpl;
class SysPathName;
template < class X > class ctl_pvector;
template < class X > class ctl_list;

class MachLogAIController
: public MachLogController
{
public:
	typedef ctl_list< MachLogProductionUnit* >	ProductionList;

	MachLogAIController( MachLogRace* logRace, W4dEntity* pPhysObject, const string& AIStrategicRules );

	virtual ~MachLogAIController();

	// inherited from MachLogCanCommunicate...
	virtual void sendMessage( MachLogCommsId to, const MachLogMessage& ) const;
	virtual void receiveMessage( const MachLogMessage& );

	//inherited from MachLogCanAdminister
	virtual void handleIdleTechnician( MachLogCommsId );
	virtual void handleIdleAdministrator( MachLogCommsId );
	
	virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR );

//	void moveOutOfTheWay( MachLogMachine * pObj );

	void readRules( const SysPathName& );

	virtual void doOutputOperator( ostream& ) const;

	void addConstructionProductionUnit( MachLogProductionUnit* );
	//go through each of the existing construction production units.
	//if the production units are == and the global transforms are equal then do not post this
	//production unit but delete it instead.
	void addUniqueConstructionProductionUnit( MachLogProductionUnit* pProd );
	bool hasWaitingConstruction();
	bool getHighestPriorityConstruction( MachLogProductionUnit* );

	void addDesiredMachine( MachLogProductionUnit*, int desiredNumber );
	//these two functions allow the AI controller to match how many machines it has
	//against the desired number at all times.
	void machineCreated( const MachLogMachine& );
	void machineDestroyed( const MachLogMachine& );

	friend ostream& operator<<( ostream&, const MachLogAIController& );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogAIController );
	PER_FRIEND_READ_WRITE( MachLogAIController );

	enum ResearchInterest { HARDWARE, SOFTWARE };

	void checkForDynamicAllies( bool );
	bool checkForDynamicAllies() const;
	
	static PhysRelativeTime allyUpdateInterval();

private:
	friend class MachLogAIControllerImpl;
	void createCorrectSquadronOperation( MachLogSquadron*, const UtlLineTokeniser& );

	typedef ctl_pvector< DesiredMachineData >	DesiredMachineList;
	typedef ctl_pvector< MachLogAIStrategicProductionUnit > StrategicProductionList	;

	bool hasMineOnSite( const MexPoint2d& pos ) const;
	void handleEnemyContact( const MachLogMessage& );
	
	///////////////////////////////

	
	///////////////////////////////

	void deleteHolographAtPosition( const MexPoint3d& pos );

	///////////////////////////////
	
	PER_FRIEND_ENUM_PERSISTENT( ResearchInterest );
	MachLogAIControllerImpl*		pImpl_;

};

//private nested class for generating production list for race as a whole
struct DesiredMachineData
{
	MachLogProductionUnit*	pProdUnit_;
	int						desiredNumber_;
	int						actualNumber_;
	PER_MEMBER_PERSISTENT_DEFAULT( DesiredMachineData );
	PER_FRIEND_READ_WRITE( DesiredMachineData );
};


PER_DECLARE_PERSISTENT( MachLogAIController );
PER_DECLARE_PERSISTENT( DesiredMachineData );
/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHLOG_CONTROLR_AI_HPP	*/

/* End CONTROLR.HPP *************************************************/
