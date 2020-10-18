#include "machlog/administ.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/apc.hpp"
#include "machlog/construc.hpp"
#include "machlog/geolocat.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/spy.hpp"
#include "machlog/technici.hpp"

#include "machphys/administ.hpp"
#include "machphys/aggressr.hpp"
#include "machphys/apc.hpp"
#include "machphys/construc.hpp"
#include "machphys/geolocat.hpp"
#include "machphys/rescarr.hpp"
#include "machphys/spylocat.hpp"
#include "machphys/technici.hpp"

#include "machlog/beacon.hpp"
#include "machlog/constron.hpp"
#include "machlog/factory.hpp"
#include "machlog/garrison.hpp"
#include "machlog/hwlab.hpp"
#include "machlog/machine.hpp"
#include "machlog/mine.hpp"
#include "machlog/missilem.hpp"
#include "machlog/pod.hpp"
#include "machlog/smelter.hpp"
//#include "machlog/swlab.hpp"

#include "machgui/restorer.hpp"

/* ////////////////////////////////////////////////////// */
	
MachLogTypeRestorer::MachLogTypeRestorer()
{
	// Intentionally Empty
}

MachLogTypeRestorer::~MachLogTypeRestorer()
{
	// Intentionally Empty
}

void MachLogTypeRestorer::process( MachActor * pActor )
{
	switch( pActor->objectType() )
	{
		case MachLog::ADMINISTRATOR :
		case MachLog::AGGRESSOR :
		case MachLog::CONSTRUCTOR :
		case MachLog::GEO_LOCATOR :
		case MachLog::SPY_LOCATOR :
		case MachLog::TECHNICIAN :
		case MachLog::APC :
		case MachLog::RESOURCE_CARRIER :
			processMachine( &pActor->asMachine() );
		break;
		case MachLog::BEACON :
		case MachLog::FACTORY :
		case MachLog::GARRISON :
		case MachLog::HARDWARE_LAB :
		case MachLog::POD :
		case MachLog::MINE :
		case MachLog::MISSILE_EMPLACEMENT :
		case MachLog::SOFTWARE_LAB :
		case MachLog::SMELTER :
			processConstruction( &pActor->asConstruction() );
		break;
		DEFAULT_ASSERT_BAD_CASE( pActor->objectType() );
	}
}

/* ////////////////////////////////////////////////////// */

void MachLogTypeRestorer::processConstruction( MachLogConstruction * pConstruction )
{
	if( doProcessConstruction( pConstruction ) == PROCESS_AS_SUBTYPE )
	{
		switch( pConstruction->objectType() )
		{
			case MachLog::BEACON :
				doProcessBeacon( (MachLogBeacon*)pConstruction );
			break;
			case MachLog::FACTORY :
				processFactory( (MachLogFactory*)pConstruction );
			break;
			case MachLog::GARRISON :
				doProcessGarrison( (MachLogGarrison*)pConstruction );
			break;
			case MachLog::HARDWARE_LAB :
				processHardwareLab( (MachLogHardwareLab*)pConstruction );
			break;
			case MachLog::POD :
				doProcessPod( (MachLogPod*)pConstruction );
			break;
			case MachLog::MINE :
				doProcessMine( (MachLogMine*)pConstruction );
			break;
			case MachLog::MISSILE_EMPLACEMENT :
				processMissileEmplacement( (MachLogMissileEmplacement*)pConstruction );
			break;
			case MachLog::SMELTER :
				doProcessSmelter( (MachLogSmelter*)pConstruction );
			break;
			DEFAULT_ASSERT_BAD_CASE( pConstruction->objectType() );
		}
	}
}

/* ////////////////////////////////////////////////////// */

void MachLogTypeRestorer::processAdministrator( MachLogAdministrator * pActor )
{
	if( doProcessAdministrator( pActor ) == PROCESS_AS_SUBTYPE )
	{
		switch( pActor->subType() )
		{
			case MachPhys::BOSS :
				doProcessBoss( pActor );
			break;
			case MachPhys::COMMANDER :
				doProcessCommander( pActor );
			break;
			case MachPhys::OVERSEER :
				doProcessOverseer( pActor );
			break;
			DEFAULT_ASSERT_BAD_CASE( pActor->subType() );
		}
	}
}

void MachLogTypeRestorer::processAggressor( MachLogAggressor * pActor )
{
	if( doProcessAggressor( pActor ) == PROCESS_AS_SUBTYPE )
	{
		switch( pActor->subType() )
		{
			case MachPhys::ASSASSIN :
				doProcessAssassin( pActor );
			break;
			case MachPhys::BALLISTA :
				doProcessBallista( pActor );
			break;
			case MachPhys::GRUNT :
				doProcessGrunt( pActor );
			break;
			case MachPhys::KNIGHT :
				doProcessKnight( pActor );
			break;
			case MachPhys::NINJA :
				doProcessNinja( pActor );
			break;
			DEFAULT_ASSERT_BAD_CASE( pActor->subType() );
		}
	}
}

void MachLogTypeRestorer::processConstructor( MachLogConstructor * pActor )
{
	if( doProcessConstructor( pActor ) == PROCESS_AS_SUBTYPE )
	{
		switch( pActor->subType() )
		{
			case MachPhys::BEHEMOTH :
				doProcessBehemoth( pActor );
			break;
			case MachPhys::BUILDER :
				doProcessBuilder( pActor );
			break;
			case MachPhys::DOZER :
				doProcessDozer( pActor );
			break;
			DEFAULT_ASSERT_BAD_CASE( pActor->subType() );
		}
	}
}

void MachLogTypeRestorer::processLocator( MachLogMachine * pActor )
{
	if( doProcessLocator( pActor ) == PROCESS_AS_SUBTYPE )
	{
		switch( pActor->objectType() )
		{
			case MachLog::GEO_LOCATOR :
				doProcessGeoLocator( &pActor->asGeoLocator() );
			break;
			case MachLog::SPY_LOCATOR :
				doProcessSpyLocator( &pActor->asSpyLocator() );
			break;
			DEFAULT_ASSERT_BAD_CASE( pActor->objectType() );
		}
	}
}

void MachLogTypeRestorer::processTechnician( MachLogTechnician * pActor )
{
	if( doProcessTechnician( pActor ) == PROCESS_AS_SUBTYPE )
	{
		switch( pActor->subType() )
		{
			case MachPhys::BRAIN_BOX :
				doProcessBrainBoxTechnician( pActor );
			break;
			case MachPhys::LAB_TECH :
				doProcessLabTechTechnician( pActor );
			break;
			case MachPhys::TECH_BOY :
				doProcessTechBoyTechnician( pActor );
			break;
			DEFAULT_ASSERT_BAD_CASE( pActor->subType() );
		}
	}
}

void MachLogTypeRestorer::processTransporter( MachLogMachine * pActor )
{
	if( doProcessTransporter( pActor ) == PROCESS_AS_SUBTYPE )
	{
		switch( pActor->objectType() )
		{
			case MachLog::APC :
				doProcessPersonnelCarrier( &pActor->asAPC() );
			break;
			case MachLog::RESOURCE_CARRIER :
				doProcessResourceCarrier( &pActor->asResourceCarrier() );
			break;
			DEFAULT_ASSERT_BAD_CASE( pActor->objectType() );
		}
	}
}

/* ////////////////////////////////////////////////////// */

void MachLogTypeRestorer::processMachine( MachLogMachine * pMachine )
{
	if( doProcessMachine( pMachine ) == PROCESS_AS_SUBTYPE )
	{
		switch( pMachine->objectType() )
		{
			case MachLog::ADMINISTRATOR :
				processAdministrator( &pMachine->asAdministrator() );
			break;
			case MachLog::AGGRESSOR :	
				processAggressor(  &pMachine->asAggressor() );
			break;
			case MachLog::CONSTRUCTOR :
				processConstructor( &pMachine->asConstructor() );
			break;
			case MachLog::GEO_LOCATOR :
			case MachLog::SPY_LOCATOR :
				processLocator( pMachine );
			break;
			case MachLog::TECHNICIAN :
				processTechnician( &pMachine->asTechnician() );
			break;
			case MachLog::APC :
			case MachLog::RESOURCE_CARRIER :
				processTransporter( pMachine );
			break;
			DEFAULT_ASSERT_BAD_CASE( pMachine->objectType() );
		}
	}
}

/* ///////////////////////////////////////////////////////////////// */

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessBeacon( MachLogBeacon * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessFactory( MachLogFactory * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessGarrison( MachLogGarrison * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessHardwareLab( MachLogHardwareLab * pActor )
{
	return doDefaultProcess( pActor );
}


MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessPod( MachLogPod * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessMine( MachLogMine * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessMissileEmplacement( MachLogMissileEmplacement * pActor )
{
	return doDefaultProcess( pActor );
}


MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessSmelter( MachLogSmelter * pActor )
{
	return doDefaultProcess( pActor );
}

/* //////////////////////////////////////////////////////////////// */

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessAdministrator( MachLogAdministrator * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessBoss( MachLogAdministrator * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessOverseer( MachLogAdministrator * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessCommander( MachLogAdministrator * pActor )
{
	return doDefaultProcess( pActor );
}

/* //////////////////////////////////////////////////////////////// */

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessAggressor( MachLogAggressor * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessAssassin( MachLogAggressor * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessBallista( MachLogAggressor * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessGrunt( MachLogAggressor * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessKnight( MachLogAggressor * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessNinja( MachLogAggressor * pActor )
{
	return doDefaultProcess( pActor );
}

/* //////////////////////////////////////////////////////////////// */

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessConstructor( MachLogConstructor * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessDozer( MachLogConstructor * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessBuilder( MachLogConstructor * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessBehemoth( MachLogConstructor * pActor )
{
	return doDefaultProcess( pActor );
}

/* //////////////////////////////////////////////////////////////// */

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessLocator( MachLogMachine * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessGeoLocator( MachLogGeoLocator * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessSpyLocator( MachLogSpyLocator * pActor )
{
	return doDefaultProcess( pActor );
}

/* //////////////////////////////////////////////////////////////// */

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessTechnician( MachLogTechnician * pActor )
{
	return doDefaultProcess( pActor );
}
MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessLabTechTechnician( MachLogTechnician * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessTechBoyTechnician( MachLogTechnician * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessBrainBoxTechnician( MachLogTechnician * pActor )
{
	return doDefaultProcess( pActor );
}

/* //////////////////////////////////////////////////////////////// */

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessTransporter( MachLogMachine * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessResourceCarrier( MachLogResourceCarrier * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessPersonnelCarrier( MachLogAPC * pActor )
{
	return doDefaultProcess( pActor );
}

/* //////////////////////////////////////////////////////////////// */
MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessCivilianHardwareLab( MachLogHardwareLab * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessMilitaryHardwareLab( MachLogHardwareLab * pActor )
{
	return doDefaultProcess( pActor );
}

void MachLogTypeRestorer::processHardwareLab( MachLogHardwareLab * pActor )
{
	if( doProcessHardwareLab( pActor ) == PROCESS_AS_SUBTYPE )
	{
		switch( pActor->subType() )
		{
			case MachPhys::LAB_CIVILIAN :
				doProcessCivilianHardwareLab( pActor );
			break;
			case MachPhys::LAB_MILITARY :
				doProcessMilitaryHardwareLab( pActor );
			break;
			DEFAULT_ASSERT_BAD_CASE( pActor->subType() );
		}
	}
}


void MachLogTypeRestorer::processFactory( MachLogFactory * pActor )
{
	if( doProcessFactory( pActor ) == PROCESS_AS_SUBTYPE )
	{
		switch( pActor->subType() )
		{
			case MachPhys::CIVILIAN :
				doProcessCivilianFactory( pActor );
			break;
			case MachPhys::MILITARY :
				doProcessMilitaryFactory( pActor );
			break;
			DEFAULT_ASSERT_BAD_CASE( pActor->subType() );
		}
	}
}

void MachLogTypeRestorer::processMissileEmplacement( MachLogMissileEmplacement * pActor )
{
	if( doProcessMissileEmplacement( pActor ) == PROCESS_AS_SUBTYPE )
	{
		switch( pActor->subType() )
		{
			case MachPhys::TURRET :
				doProcessTurret( pActor );
			break;
			case MachPhys::SENTRY :
				doProcessSentry( pActor );
			break;
			case MachPhys::LAUNCHER :
				doProcessLauncher( pActor );
			break;
			case MachPhys::ICBM :
				doProcessICBM( pActor );
			break;
			DEFAULT_ASSERT_BAD_CASE( pActor->subType() );
		}
	}
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessTurret( MachLogMissileEmplacement * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessSentry( MachLogMissileEmplacement * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessLauncher( MachLogMissileEmplacement * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessICBM( MachLogMissileEmplacement * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessMilitaryFactory( MachLogFactory * pActor )
{
	return doDefaultProcess( pActor );
}

MachLogTypeRestorer::ProcessAsSubtype 
MachLogTypeRestorer::doProcessCivilianFactory( MachLogFactory * pActor )
{
	return doDefaultProcess( pActor );
}

/* End RESTORER.CPP *************************************************/
