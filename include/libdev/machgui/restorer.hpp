
#ifndef _MACHLOG_RESTORER_HPP
#define _MACHLOG_RESTORER_HPP

#include "base/base.hpp"

class MachActor;
class MachLogConstruction;
class MachLogMachine;
class MachLogBeacon;
class MachLogFactory;
class MachLogGarrison;
class MachLogHardwareLab;
class MachLogPod;
class MachLogMine;
class MachLogMissileEmplacement;
class MachLogSmelter;

class MachLogAdministrator;
class MachLogAggressor;
class MachLogConstructor;
class MachLogGeoLocator;
class MachLogSpyLocator;
class MachLogTechnician;
class MachLogAPC;
class MachLogResourceCarrier;

/*
 * MachLogTypeRestorer takes a MachActor * as argument and maps it
 * to a virtual function which takes a type-casted subclass as an argument.
 * The flow of control for purposes of client responsibility is as follows:
 * If the actor is a subclass of MachLogMachine, then the virtual method
 * doProcessMachine is called. Further processing is done by subtypes of machine
 * if doProcessMachine returns PROCESS_AS_SUBTYPE. This applies recursively until 
 * a leaf in the MachLogType heirarchy is reached. If a given doProcessXXX method 
 * is not provided by the client, then the default implementation of the method,
 * which calls the virtual method doDefaultProcess, is used.
 * An identical flow of control applies for subclasses of MachLogConstruction.
 */

class MachLogTypeRestorer
// cannonical form revoked
{
public:

	MachLogTypeRestorer();

	virtual ~MachLogTypeRestorer();

protected:

	enum ProcessAsSubtype
	{
		PROCESS_AS_SUBTYPE = true, 
		DO_NOT_PROCESS_AS_SUBTYPE = false 
	};
	
	void process( MachActor * );
	
	///////////////////////////////
		
	virtual ProcessAsSubtype doProcessConstruction( MachLogConstruction * ) = 0;
	virtual ProcessAsSubtype doProcessMachine( MachLogMachine * ) = 0;
	virtual ProcessAsSubtype doDefaultProcess( MachActor * ) = 0;
	
	///////////////////////////////
							
	virtual ProcessAsSubtype doProcessBeacon( MachLogBeacon * );

	virtual ProcessAsSubtype doProcessFactory( MachLogFactory * );
	virtual ProcessAsSubtype doProcessCivilianFactory( MachLogFactory * );
	virtual ProcessAsSubtype doProcessMilitaryFactory( MachLogFactory * );

	virtual ProcessAsSubtype doProcessGarrison( MachLogGarrison * );
	virtual ProcessAsSubtype doProcessHardwareLab( MachLogHardwareLab * );
	virtual ProcessAsSubtype doProcessCivilianHardwareLab( MachLogHardwareLab * );
	virtual ProcessAsSubtype doProcessMilitaryHardwareLab( MachLogHardwareLab * );
	virtual ProcessAsSubtype doProcessPod( MachLogPod * );
	virtual ProcessAsSubtype doProcessMine( MachLogMine * );

	virtual ProcessAsSubtype doProcessMissileEmplacement( MachLogMissileEmplacement * );
	virtual ProcessAsSubtype doProcessTurret( MachLogMissileEmplacement * );
	virtual ProcessAsSubtype doProcessSentry( MachLogMissileEmplacement * );
	virtual ProcessAsSubtype doProcessLauncher( MachLogMissileEmplacement * );
	virtual ProcessAsSubtype doProcessICBM( MachLogMissileEmplacement * );

//	virtual ProcessAsSubtype doProcessSoftwareLab( MachLogSoftwareLab * );
	virtual ProcessAsSubtype doProcessSmelter( MachLogSmelter * );

	///////////////////////////////
	
	virtual ProcessAsSubtype doProcessAdministrator( MachLogAdministrator * );
	virtual ProcessAsSubtype doProcessBoss( MachLogAdministrator * );
	virtual ProcessAsSubtype doProcessOverseer( MachLogAdministrator * );
	virtual ProcessAsSubtype doProcessCommander( MachLogAdministrator * );

	virtual ProcessAsSubtype doProcessAggressor( MachLogAggressor * );
	virtual ProcessAsSubtype doProcessAssassin( MachLogAggressor * );
	virtual ProcessAsSubtype doProcessBallista( MachLogAggressor * );
	virtual ProcessAsSubtype doProcessGrunt( MachLogAggressor * );
	virtual ProcessAsSubtype doProcessKnight( MachLogAggressor * );
	virtual ProcessAsSubtype doProcessNinja( MachLogAggressor * );

	virtual ProcessAsSubtype doProcessConstructor( MachLogConstructor * );
	virtual ProcessAsSubtype doProcessDozer( MachLogConstructor * );
	virtual ProcessAsSubtype doProcessBuilder( MachLogConstructor * );
	virtual ProcessAsSubtype doProcessBehemoth( MachLogConstructor * );
	
	virtual ProcessAsSubtype doProcessLocator( MachLogMachine * );
	virtual ProcessAsSubtype doProcessGeoLocator( MachLogGeoLocator * );
	virtual ProcessAsSubtype doProcessSpyLocator( MachLogSpyLocator * );
	
	virtual ProcessAsSubtype doProcessTechnician( MachLogTechnician * );
	virtual ProcessAsSubtype doProcessLabTechTechnician( MachLogTechnician * );
	virtual ProcessAsSubtype doProcessTechBoyTechnician( MachLogTechnician * );
	virtual ProcessAsSubtype doProcessBrainBoxTechnician( MachLogTechnician * );

	virtual ProcessAsSubtype doProcessTransporter( MachLogMachine * );
	virtual ProcessAsSubtype doProcessPersonnelCarrier( MachLogAPC * );
	virtual ProcessAsSubtype doProcessResourceCarrier( MachLogResourceCarrier * );
		
	///////////////////////////////

private:
	MachLogTypeRestorer( const MachLogTypeRestorer& );
	MachLogTypeRestorer& operator =( const MachLogTypeRestorer& );
	bool operator ==( const MachLogTypeRestorer& );
	
	///////////////////////////////

	void processMachine( MachLogMachine * pMach );
	void processAdministrator( MachLogAdministrator * pActor );
	void processAggressor( MachLogAggressor * pActor );
	void processConstructor( MachLogConstructor * pActor );
	void processLocator( MachLogMachine * pActor );
	void processTechnician( MachLogTechnician * pActor );
	void processTransporter( MachLogMachine * pActor );
	
	///////////////////////////////

	void processConstruction( MachLogConstruction * pMach );
	void processFactory( MachLogFactory * pMach );
	void processHardwareLab( MachLogHardwareLab * pMach );
	void processMissileEmplacement( MachLogMissileEmplacement * pMach );

	///////////////////////////////
};

/* //////////////////////////////////////////////////////////////// */

#endif	// #ifndef _MACHLOG_RESTORER_HPP

/* End RESTORER.HPP *************************************************/