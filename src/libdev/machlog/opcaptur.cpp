/*
 * O P C A P T U R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

//#include "machphys/plansurf.hpp"
//#include "ctl/algorith.hpp"
#include "sim/manager.hpp"

#include "mathex/abox2d.hpp"

#include "phys/cspace2.hpp"

#include "machphys/mphydata.hpp"
#include "machphys/ctordata.hpp"

#include "machlog/animatio.hpp"
#include "machlog/opcaptur.hpp"
#include "machlog/constron.hpp"
#include "machlog/construc.hpp"
#include "machlog/factory.hpp"
#include "machlog/hwlab.hpp"
#include "machlog/machvman.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/move.hpp"
#include "machlog/races.hpp"
#include "machlog/stats.hpp"
#include "machlog/planet.hpp"

PER_DEFINE_PERSISTENT( MachLogCaptureOperation );

MachLogCaptureOperation::MachLogCaptureOperation( MachLogConstructor * pActor, MachLogConstruction * pConstr )
:	MachLogLabourOperation( pActor, pConstr, "CAPTURE_OPERATION", MachLogOperation::CAPTURE_OPERATION )
{
	pConstr->addThreat( pActor->id() );
}

MachLogCaptureOperation::~MachLogCaptureOperation()
{
	if( pConstruction() )
		pConstruction()->removeThreat( pConstructor()->id() );
}

void MachLogCaptureOperation::doOutputOperator( ostream& o ) const
{
	o << "CAPTURE OPERATION " << std::endl;

	MachLogLabourOperation::doOutputOperator( o );

	if( pConstruction() )
	{
		o << " Capturing (" << pConstruction()->id() << ")[" << pConstruction()->objectType() << "]\n";
		const MachPhysObjectData& objData = pConstruction()->objectData();
		const static float captureRatio = 1.0 / 3.0;
		o << " hp " << pConstruction()->hp() << " maxHp " << objData.hitPoints() << " ratio " << ( (float)pConstruction()->hp() / (float)objData.hitPoints() ) << " check less than " << captureRatio << std::endl;
		o << " ";
	}
	else
	{
		o << " Target now destroyed or invalid." << std::endl;
	}

}

///////////////////////////////////

// virtual
PhysRelativeTime MachLogCaptureOperation::interactWithBuilding()
{
	ASSERT( pConstruction(), "Hey! The construction has gone! It shouldn't have!" );

	MachLogConstruction* pConstron = pConstruction();
	MachLogConstructor* pConstructorMachine = pConstructor();

	PhysRelativeTime interval = 2.0;

	pConstructorMachine->constructing( true, pConstron->id() );

	MachPhys::BuildingMaterialUnits units = pConstructorMachine->data().constructionRate() * MachLogRaces::instance().stats().enemyCaptureMultiplier();
	MachPhys::BuildingMaterialUnits criticalUnits = 0.3 * pConstron->maximumhp();

	MachPhys::BuildingMaterialUnits BMUValueOfDamage = ( units * ( SimManager::instance().currentTime() - lastUpdateTime() ) / 60 );

	MachPhys::HitPointUnits hpsRemoved = pConstron->hitPointValueOfBMUs( BMUValueOfDamage );

	// don't want to damage building far beyond what is necessary to capture it

	if( pConstron->hp() - hpsRemoved < criticalUnits )
		hpsRemoved = pConstron->hp() - criticalUnits;


	pConstron->beHit( hpsRemoved, MachPhys::N_WEAPON_TYPES, pConstructorMachine );
	//only update time if added something on.

	ASSERT( pConstron, "Hey! The construction has gone! It shouldn't have!" );

	if( not ( hpsRemoved == 0 ) )
	{
		lastUpdateTime( SimManager::instance().currentTime() );
	}
	else
	{
		//oops didn't manage to add anything on lets try again....

		state( MachLogLabourOperation::INTERACTING );
	}
	//End of TBD
	const MachPhysObjectData& objData = pConstron->objectData();
	const static float captureRatio = 1.0 / 3.0;
	if( (float)pConstron->hp() / (float)objData.hitPoints() < captureRatio )
	{
		//We have captured the building....we need to reassign it to our race.
		//new method of SimActor -> assignToDifferentProcess( SimProcess );
		//use machlograces to get MachLogRace and not actor->logRace dur to const
		pConstron->assignToDifferentRace( MachLogRaces::instance().race( pConstructorMachine->race() ) );

		// if this is a factory then halt all its production (we get the dosh back from
		// any machine under production)
		if ( pConstron->objectType() == MachLog::FACTORY )
			pConstron->asFactory().cancelAllProduction();

		// if this is a hw lab then halt all its research (we don't steal any of it)
		if ( pConstron->objectType() == MachLog::HARDWARE_LAB )
			pConstron->asHardwareLab().cancelAllResearch();

		// if this is a pod then set the podCaptured flag (for actor race) in MLRaces
		if ( pConstron->objectType() == MachLog::POD )
			MachLogRaces::instance().podCaptured( pConstructorMachine->race(), true );

		// give voicemail informing player of capture
		MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::CONSTRUCTOR, pConstructorMachine->subType(), MachLogMachineVoiceMailManager::MEV_BUILDING_CAPTURED, pConstructorMachine->id(),  pConstructorMachine->race() );

		// post voicemail if no more operations on the queue
	   	if( not pConstructorMachine->isDoingSuperConstruct() )
	   		MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::CONSTRUCTOR, pConstructorMachine->subType(), MachLogMachineVoiceMailManager::MEV_AWAITING_NEW_JOB, pConstructorMachine->id(),  pConstructorMachine->race() );

		interval = 0.0;
	}

	return interval;
}

bool MachLogCaptureOperation::doIsFinished() const
{
	bool finished = ( not pConstruction() )
					or pConstruction()->race() == pConstructor()->race();

 	return finished;
}

// virtual
bool MachLogCaptureOperation::clientSpecificNotification( int /*clientData*/ )
{
	return true;
}



void perWrite( PerOstream& ostr, const MachLogCaptureOperation& op )
{
	const MachLogLabourOperation& base1 = op;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogCaptureOperation& op )
{
	MachLogLabourOperation& base1 = op;

	istr >> base1;
}

MachLogCaptureOperation::MachLogCaptureOperation( PerConstructor con )
:	MachLogLabourOperation( con )
{
}

/* End OPCAPTUR.CPP *************************************************/
