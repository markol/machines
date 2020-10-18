/*
 * O P C O N S T R . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/opconstr.hpp"

#include "ctl/algorith.hpp"

#include "mathex/abox2d.hpp"

#include "phys/cspace2.hpp"

#include "sim/manager.hpp"

#include "machphys/mphydata.hpp"
#include "machphys/ctordata.hpp"

#include "machlog/animatio.hpp"
#include "machlog/buildpnt.hpp"
#include "machlog/constron.hpp"
#include "machlog/construc.hpp"
#include "machlog/machvman.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/move.hpp"
#include "machlog/races.hpp"
#include "machlog/planet.hpp"
#include "machlog/spacial.hpp"

PER_DEFINE_PERSISTENT( MachLogConstructOperation );

MachLogConstructOperation::MachLogConstructOperation( MachLogConstructor * pActor, MachLogConstruction * pConstr )
:	MachLogLabourOperation( pActor, pConstr, "CONSTRUCT_OPERATION", MachLogOperation::CONSTRUCT_OPERATION )		
{   
	// deliberately left blank
}


void MachLogConstructOperation::doOutputOperator( ostream& o ) const
{
	MachLogLabourOperation::doOutputOperator( o );
	
	if( pConstruction() )
		o << " Constructing (" << pConstruction()->id() << ")[" << pConstruction()->objectType() << "] Progress:" << pConstruction()->constructedUnits() << std::endl;
	else
		o << " Construction deleted!" << std::endl;
}

///////////////////////////////////


// virtual
PhysRelativeTime MachLogConstructOperation::interactWithBuilding()
{
	PhysRelativeTime interval = 2.0;

	MachLogConstructor* pConstructorGuy = pConstructor();
	MachLogConstruction* pUnfinishedConstruction = pConstruction();
	
	MachPhys::BuildingMaterialUnits units = pConstructor()->data().constructionRate();

	MachPhys::BuildingMaterialUnits amountToAdd = ( units * ( SimManager::instance().currentTime() - lastUpdateTime() ) / 60 );
	if( amountToAdd > 0 )
	{
		if( amountToAdd + pUnfinishedConstruction->constructedUnits() > pUnfinishedConstruction->objectData().cost() )
			amountToAdd = pUnfinishedConstruction->objectData().cost() - pUnfinishedConstruction->constructedUnits();
			
		amountToAdd = MachLogRaces::instance().smartSubtractBMUs( pConstructorGuy->race(), amountToAdd );

		pUnfinishedConstruction->advanceConstructionState( amountToAdd );
		
	   	lastUpdateTime( SimManager::instance().currentTime() );
		
		// if we added nothing, it's cos we're out of money, so don't animate. The workers are on strike.
		if( amountToAdd > 0 ) 
			//subOperation( pConstructorGuy, _NEW( MachLogConstructAnimation( pConstructorGuy, pUnfinishedConstruction, units ) ) );
			pConstructorGuy->constructing( true, pUnfinishedConstruction->id() );	
		else
			pConstructorGuy->constructing( false );
		
		// if the building is now complete, give a voicemail
		if( pUnfinishedConstruction->isComplete() )
		{
			MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::CONSTRUCTOR, pConstructorGuy->subType(), MachLogMachineVoiceMailManager::MEV_BUILDING_COMPLETE, pConstructorGuy->id(),  pConstructorGuy->race() );            		

			// post voicemail if no more operations on the queue
			if( not pConstructorGuy->isDoingSuperConstruct() )
			        MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::CONSTRUCTOR, pConstructorGuy->subType(), MachLogMachineVoiceMailManager::MEV_AWAITING_NEW_JOB, pConstructorGuy->id(),  pConstructorGuy->race() );

			interval = 0.0;
		}					
	}
	else
	{
		//oops didn't manage to add anything on lets try again....
		state( MachLogLabourOperation::INTERACTING );
		
		pConstructorGuy->constructing( true, pUnfinishedConstruction->id() );
	}
	
	return interval;
}
	
bool MachLogConstructOperation::doIsFinished() const
{
	bool finished = not pConstruction() 
					or pConstruction()->isComplete();
			
	return finished;        
}

// virtual
bool MachLogConstructOperation::clientSpecificNotification( int clientData )
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
				// not going to help construct an opponent's base. No way.
				stayAttached = false;
			}	
		}
		break;

		default:
			;
	}

	return stayAttached;
}
	

void perWrite( PerOstream& ostr, const MachLogConstructOperation& op )
{
	const MachLogLabourOperation& base1 = op;

	ostr << base1;   
}

void perRead( PerIstream& istr, MachLogConstructOperation& op )
{
	MachLogLabourOperation& base1 = op;

	istr >> base1;
}

MachLogConstructOperation::MachLogConstructOperation( PerConstructor con )
:	MachLogLabourOperation( con )
{
}

bool MachLogConstructOperation::constructingMissileEmplacement() const
{
	return pConstruction() and pConstruction()->objectIsMissileEmplacement() and pConstructor()->constructing();
}	

/* End OPCONSTR.CPP *************************************************/
