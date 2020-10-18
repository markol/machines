/*
 * O P R E P A I R . C P P
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
#include "machlog/oprepair.hpp"
#include "machlog/constron.hpp"
#include "machlog/construc.hpp"
#include "machlog/machvman.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/move.hpp"
#include "machlog/races.hpp"
#include "machlog/planet.hpp"

PER_DEFINE_PERSISTENT( MachLogRepairOperation );

MachLogRepairOperation::MachLogRepairOperation( MachLogConstructor * pActor, MachLogConstruction * pConstr )
:	MachLogLabourOperation( pActor, pConstr, "REPAIR_OPERATION", MachLogOperation::REPAIR_OPERATION )
{
	// deliberately left blank
}

void MachLogRepairOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogRepairOperation\n";
	MachLogLabourOperation::doOutputOperator( o );

	if( pConstruction() )
		o << " Repairing (" << pConstruction()->id() << ")[" << pConstruction()->objectType() << "]\n";
	else
		o << " Target destroyed or invalid. " << std::endl;
}

///////////////////////////////////

// virtual
PhysRelativeTime MachLogRepairOperation::interactWithBuilding()
{
	PhysRelativeTime interval = 2.0;

	MachLogConstructor* pConstructorGuy = pConstructor();
	MachLogConstruction* pDamagedConstruction = pConstruction();

	MachLogRaces& races = MachLogRaces::instance();
	MachPhys::Race constructorRace = pConstructorGuy->race();

	MachPhys::BuildingMaterialUnits constructRate = pConstructorGuy->data().constructionRate();

	MachPhys::BuildingMaterialUnits BMUValueAdded = ( constructRate * ( SimManager::instance().currentTime() - lastUpdateTime() ) / 60 );
	MachPhys::BuildingMaterialUnits nBMUsAvailable = races.nBuildingMaterialUnits( constructorRace );

	if( BMUValueAdded > nBMUsAvailable )
		BMUValueAdded = nBMUsAvailable;

	MachPhys::HitPointUnits hpsAdded = pDamagedConstruction->hitPointValueOfBMUs( BMUValueAdded );

	if( hpsAdded > 0 )
	{
		//subOperation( pConstructorGuy, _NEW( MachLogConstructAnimation( pConstructorGuy, pDamagedConstruction, constructRate ) ) );
		pConstructorGuy->constructing( true, pDamagedConstruction->id() );

		const MachPhysObjectData& objData = pDamagedConstruction->objectData();
		if( hpsAdded + pDamagedConstruction->hp() > pDamagedConstruction->objectData().hitPoints() )
			hpsAdded = pDamagedConstruction->objectData().hitPoints() - pDamagedConstruction->hp();

		pDamagedConstruction->addRepairPoints( hpsAdded );

		// if the building is now repaired, give a voicemail
		if( pDamagedConstruction->hpRatio() >= 1.0 )
		{
			MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::CONSTRUCTOR, pConstructorGuy->subType(), MachLogMachineVoiceMailManager::MEV_BUILDING_REPAIRED, pConstructorGuy->id(), constructorRace );

			// post voicemail if no more operations on the queue
			if( not pConstructorGuy->isDoingSuperConstruct() )
				MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::CONSTRUCTOR, pConstructorGuy->subType(), MachLogMachineVoiceMailManager::MEV_AWAITING_NEW_JOB, pConstructorGuy->id(), constructorRace );
		}

		//only update time and pay the cash if hps were added on.
		races.smartSubtractBMUs( constructorRace, BMUValueAdded );
		lastUpdateTime( SimManager::instance().currentTime() );
	}
	else
	{
		// if we added nothing, it's cos we're out of money, so don't animate. The workers are on strike.
		pConstructorGuy->constructing( false );

		state( MachLogLabourOperation::INTERACTING );
	}

	return interval;
}

bool MachLogRepairOperation::doIsFinished() const
{
	bool finished = ( not pConstruction() )
					or pConstruction()->hpRatio() == 1.0;

	return finished;
}

// virtual
bool MachLogRepairOperation::clientSpecificNotification( int clientData )
{
	PRE( pConstruction() );
	PRE( pConstructor() );

	bool stayAttached = true;

	switch( clientData )
	{
		case MachLog::RACE_CHANGED:
		{
			MachLogRaces::DispositionToRace disposition = MachLogRaces::instance().dispositionToRace( pConstruction()->race(), pConstructor()->race() );

			if( disposition == MachLogRaces::ENEMY or disposition == MachLogRaces::NEUTRAL )
			{
				// not going to repair an opponent's base. No way.
				stayAttached = false;
			}
		}
		break;

		default:
			;
	}

	return stayAttached;
}


void perWrite( PerOstream& ostr, const MachLogRepairOperation& op )
{
	const MachLogLabourOperation& base1 = op;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogRepairOperation& op )
{
	MachLogLabourOperation& base1 = op;

	istr >> base1;

}

MachLogRepairOperation::MachLogRepairOperation( PerConstructor con )
:	MachLogLabourOperation( con )
{
}

// Forced recompile 19/2/99 CPS
/* End OPREPAIR.CPP *************************************************/
