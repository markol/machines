/*
 * O P A D R E P A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved.
 */

#include "ctl/pvector.hpp"

#include "mathex/point2d.hpp"

#include "phys/cspace2.hpp"
#include "phys/motchunk.hpp"

#include "machphys/machdata.hpp"
#include "machphys/mcmovinf.hpp"
#include "machphys/random.hpp"


#include "machlog/opadrepa.hpp"
#include "machlog/oprepair.hpp"
#include "machlog/move.hpp"
#include "machlog/administ.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/construc.hpp"
#include "machlog/constron.hpp"
#include "machlog/races.hpp"
#include "machlog/planet.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/squad.hpp"
#include "machlog/strategy.hpp"
#include "machlog/follow.hpp"
#include "machlog/cntrl.hpp"

PER_DEFINE_PERSISTENT( MachLogAdminRepairOperation );

/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachLogAdminRepairOperation::MachLogAdminRepairOperation( MachLogAdministrator * pActor, MachLogConstruction* pConstruction_ )
:	MachLogOperation( "ADMIN_DECONSTRUCT_OPERATION", MachLogOperation::ADMIN_DECONSTRUCT_OPERATION ),
	pActor_( pActor ),
    complete_( false ),
	pConstruction_( pConstruction_ ),
	currentlyAttached_( true )
{
	ASSERT( pConstruction_, "Pointer to construction passed into MLAdminDeconstructOp is NULL\n" );
	pConstruction_->attach( this );
}

/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */

MachLogAdminRepairOperation::~MachLogAdminRepairOperation()
{
	if( currentlyAttached_ )
		pConstruction_->detach( this );
}

/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogAdminRepairOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogAdminRepairOperation\n"; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogAdminRepairOperation::doStart()
{
	return not checkNeedAndDoLeaveOperation( pActor_ );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime MachLogAdminRepairOperation::doUpdate( )
{
  	//HAL_STREAM( "MADConOp::doChooseSubOperation entry for " << pActor_->id() << std::endl);
    ////HAL_STREAM( "  Incomplete mine? " << haveIncompleteMine << std::endl;)

    //See if all our constructors are idle
    bool taskedConstructor = false;
    bool allConstructorsIdle = true;
	ctl_pvector< MachLogMachine > constructors;
    int nConstructors = 0;
	for( MachLogSquadron::Machines::iterator i = pActor_->squadron()->machines().begin();
	     i!=pActor_->squadron()->machines().end(); ++i )
    {
        if( (*i)->objectType() == MachLog::CONSTRUCTOR )
        {
			constructors.push_back( (*i) );
            ++nConstructors;
            if( not (*i)->isIdle() )
                allConstructorsIdle = false;
        }
    }

    
	// can't continue the op with no constructors - why, it would be madness!
    if( nConstructors == 0 )
    {
		complete_ = true;
	
		//If we are the AI race and we have no constructors, dissolve the squadron as it was only intended for
		// constructor-related ops. Free up the other machines for reassignment elsewhere.
        if( MachLogRaces::instance().controller( pActor_->race() ).type() == MachLogController::AI_CONTROLLER )
        	pActor_->squadron()->releaseAllMachines();
        
        return 0.0;
    }

	
	complete_ = ( pConstruction_->hp() == pConstruction_->maximumhp() );
	
	if (complete_ )
		return 0.0;
	
	
	// okay, we still have our target
	
    //Task the transporter/constructors as required
   
  	for( MachLogSquadron::Machines::iterator i = pActor_->squadron()->machines().begin();
	     i!=pActor_->squadron()->machines().end(); ++i )
    {

		// don't interrupt actors who are currently evading aggressors
		if( not (*i)->evading() and
			( (*i)->isIdle() or 
			  (*i)->strategy().currentOperationType() == MachLogOperation::ENTER_OPERATION or 
			  (*i)->strategy().currentOperationType() == MachLogOperation::FOLLOW_OPERATION ) )
		{
			if( (*i)->objectType() == MachLog::RESOURCE_CARRIER )
				MachLogResourceCarrier::assignResourceCarrierTask( &(*i )->asResourceCarrier() );
			if( (*i)->objectType() == MachLog::AGGRESSOR or (*i)->objectType() == MachLog::ADMINISTRATOR )
				(*i)->newOperation( _NEW( 
						MachLogFollowOperation( (*i), 
						constructors[ MachPhysRandom::randomInt(0, (int)constructors.size() ) ], 
						MexPoint2d( MachPhysRandom::randomDouble( -20, 20 ), MachPhysRandom::randomDouble( -20, 20 ) ) ) ) );
			if( (*i)->objectType() == MachLog::CONSTRUCTOR /* and  allConstructorsIdle*/ )
			{
                MachLogConstructor* pConstructor = &(*i )->asConstructor();
				pConstructor->newOperation( _NEW( MachLogRepairOperation( pConstructor, pConstruction_ ) ) );
				taskedConstructor = true;
			}
		}
        else
        {
            ////HAL_STREAM("  subordinate " << (*i)->id() << " is not idle " << std::endl;)
        }
    }
	//give the controlling administrator something to do.
	if( !pSubOperation() )
		subOperation( 	pActor_,
					_NEW( MachLogFollowOperation( 
						pActor_, 
						constructors[ MachPhysRandom::randomInt(0, (int)constructors.size() ) ], 
						MexPoint2d( MachPhysRandom::randomDouble( -20, 20 ), MachPhysRandom::randomDouble( -20, 20 ) ) ) ) );
	while( constructors.size() )
		constructors.erase( constructors.begin() );

	return 15.0;		
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogAdminRepairOperation::doFinish()
{
	/* Intentionally Empty	*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
bool MachLogAdminRepairOperation::doIsFinished() const
{
	return complete_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogAdminRepairOperation::doBeInterrupted()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
bool MachLogAdminRepairOperation::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData )
{
	ASSERT( pSubject == (W4dSubject*)pConstruction_, "Notified by unexpected subject!" ); 

	bool stayAttached = true;

	switch( event )
	{
	case W4dSubject::DELETED:
		//construction has been deleted
		pConstruction_ = NULL;
		currentlyAttached_ = false;
		complete_ = true;
		stayAttached = false;

		break;
	case W4dSubject::CLIENT_SPECIFIC:
		switch( clientData )
		{
			case MachLog::RACE_CHANGED:
				if( pConstruction_->race() != pActor_->race() )
				{
					pConstruction_ = NULL;
					currentlyAttached_ = false;
					complete_ = true;
					stayAttached = false;	
				}
				break;
			default:
				;
		}
		break;
	default:
		;
	}

	return stayAttached;
}

void perWrite( PerOstream& ostr, const MachLogAdminRepairOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.pConstruction_;
    ostr << op.complete_;
	ostr << op.currentlyAttached_;
}

void perRead( PerIstream& istr, MachLogAdminRepairOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.pConstruction_;
    istr >> op.complete_;
	istr >> op.currentlyAttached_;
	if( op.currentlyAttached_ )
		op.pConstruction_->attach( &op );	
}

MachLogAdminRepairOperation::MachLogAdminRepairOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* End OPADCAPT.CPP **************************************************/
