/*
 * O P D E C O N S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/opdecons.hpp"

//#include "ctl/algorith.hpp"

#include "mathex/abox2d.hpp"

#include "phys/cspace2.hpp"

#include "sim/manager.hpp"

#include "machphys/ctordata.hpp"
#include "machphys/mphydata.hpp"

#include "machlog/animatio.hpp"
#include "machlog/constron.hpp"
#include "machlog/construc.hpp"
#include "machlog/machvman.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/move.hpp"
#include "machlog/races.hpp"
#include "machlog/stats.hpp"
#include "machlog/planet.hpp"

PER_DEFINE_PERSISTENT( MachLogDeconstructOperation );

MachLogDeconstructOperation::MachLogDeconstructOperation( MachLogConstructor * pActor, MachLogConstruction * pConstr )
:	MachLogLabourOperation( pActor, pConstr, "DECONSTRUCT_OPERATION", MachLogOperation::DECONSTRUCT_OPERATION )
{
	pConstr->addThreat( pActor->id() );
}

MachLogDeconstructOperation::~MachLogDeconstructOperation()
{
	if( pConstruction() )
		pConstruction()->removeThreat( pConstructor()->id() );
}


void MachLogDeconstructOperation::doOutputOperator( ostream& o ) const
{

	o << "CAPTURE OPERATION " << std::endl;

	MachLogLabourOperation::doOutputOperator( o );
	if( pConstruction() )
		o << " Deconstructing (" << pConstruction()->id() << ")[" << pConstruction()->objectType() << "]\n";
	else
		o << " Target destroyed or invalid. " << std::endl;
}

///////////////////////////////////

// virtual
PhysRelativeTime MachLogDeconstructOperation::interactWithBuilding()
{
	PhysRelativeTime interval = 2.0;

	MachLogConstruction* pConstron = pConstruction();
	MachLogConstructor* pConstructorMachine = pConstructor();
	HAL_STREAM("(" << pConstructorMachine->id() << ") MachLogDeconstructOperation::interactWithBuilding\n" );
	HAL_INDENT( 2 );

	MachPhys::BuildingMaterialUnits units = pConstructorMachine->data().constructionRate();

	// rate of deconstruction varies according to whether or not this is a fristd::endly or
	// enemy construction. Deconstructing an enemy location should be much slower as they're
	// hardly likely to be co-operating. These rates are defined in statimpl.cpp, in the
	// assignment list.

	if ( pConstron->race() == pConstructorMachine->race() )
		units *= MachLogRaces::instance().stats().friendlyDeconstructMultiplier();
	else
		units *= MachLogRaces::instance().stats().enemyDeconstructMultiplier();

	MachPhys::BuildingMaterialUnits BMUValueOfRemoved = ( units * ( SimManager::instance().currentTime() - lastUpdateTime() ) / 60 );

	MachPhys::HitPointUnits hpsRemoved = pConstron->hitPointValueOfBMUs( BMUValueOfRemoved );

	HAL_STREAM("BMUValueOfRemoved = " << BMUValueOfRemoved << " hpsRemoved " << hpsRemoved << std::endl );
   	// can't take what's not there

	MachPhys::HitPointUnits remainingConstructionhps = pConstron->hp();
	HAL_STREAM("remaining hp " << remainingConstructionhps << std::endl );

	if( remainingConstructionhps < hpsRemoved and hpsRemoved > 0 )
	{
		HAL_STREAM("rejigging BMUVlaueOfRemoved and hpsRemoved...\n" );
		BMUValueOfRemoved *= ( (float)remainingConstructionhps / (float)hpsRemoved );
		hpsRemoved = remainingConstructionhps;
		HAL_STREAM("BMUValueOfRemoved = " << BMUValueOfRemoved << " hpsRemoved " << hpsRemoved << std::endl );
	}

	// Only weld away happily if we actually removed something.
	if( hpsRemoved > 0 )
	{
		//subOperation( pConstructorMachine, _NEW( MachLogConstructAnimation( pConstructorMachine, pConstron, units ) ) );
		pConstructorMachine->constructing( true, pConstron->id() );
		HAL_STREAM("hpsRemoved is > 0 so set constructing animation going\n" );
		//REMEMBER - this call could delete the object pointed to by pConstruction_
		HAL_STREAM("calling beHit with damage " << hpsRemoved << std::endl );
		pConstron->beHit( hpsRemoved, MachPhys::N_WEAPON_TYPES, pConstructorMachine );
	}
	else
		pConstructorMachine->constructing( false );

	MachPhys::BuildingMaterialUnits amountToCashIn = (int)( (float)BMUValueOfRemoved *
			MachLogRaces::instance().stats().secondhandRefundablePercentage() );


	MachLogRaces::instance().smartAddBMUs( pConstructorMachine->race(), amountToCashIn );

	//now, if that caused the destruction of the thing, let's have a notification e-mail
	if( pConstron->hpRatio() <= 0 )
	{
	 	MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::CONSTRUCTOR,
																pConstructorMachine->subType(),
																MachLogMachineVoiceMailManager::MEV_BUILDING_DECONSTRUCTED,
																pConstructorMachine->id(),
																pConstructorMachine->race() );

		// post "awaiting new job" voicemail if no more operations on the queue

		if( not pConstructorMachine->isDoingSuperConstruct() )
    		MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::CONSTRUCTOR, pConstructorMachine->subType(), MachLogMachineVoiceMailManager::MEV_AWAITING_NEW_JOB, pConstructorMachine->id(),  pConstructorMachine->race() );

	}

	//only update time if removed something.
//			HAL_STREAM(" checking not( amountToRemove == 0 [ " << ( amountToRemove == 0 ) << "] and nBMU != 0 [" << ( nBMU != 0 ) << "] " << std::endl );
	if( BMUValueOfRemoved != 0 )
	{
		HAL_STREAM("BMUValueOfRemoved != 0 therefore setting lastUpdateTime \n" );
		lastUpdateTime( SimManager::instance().currentTime() );
	}
	else
	{
		//oops didn't manage to add anything on lets try again....
//				HAL_STREAM(" didn't buiold anything setting state to DISMANTLING again\n" );
		state( MachLogLabourOperation::INTERACTING );
	}

	HAL_INDENT( -2 );
	HAL_STREAM("(" << pConstructorMachine->id() << ") MachLogDeconstructOperation::interactWithBuilding\n" );
	return interval;
}

bool MachLogDeconstructOperation::doIsFinished() const
{
	bool finished = not currentlyAttached();

	return finished;
}

// virtual
bool MachLogDeconstructOperation::clientSpecificNotification( int /*clientData*/ )
{
	return true;
}


void perWrite( PerOstream& ostr, const MachLogDeconstructOperation& op )
{
	const MachLogLabourOperation& base1 = op;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogDeconstructOperation& op )
{
	MachLogLabourOperation& base1 = op;

	istr >> base1;
}

MachLogDeconstructOperation::MachLogDeconstructOperation( PerConstructor con )
:	MachLogLabourOperation( con )
{
}

/* End OPCAPTUR.CPP *************************************************/
