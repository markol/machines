/*
 * O P A D C O N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "machlog/opadcon.hpp"

#include "ctl/list.hpp"
#include "ctl/pvector.hpp"

#include "mathex/point2d.hpp"

#include "phys/cspace2.hpp"
#include "phys/motchunk.hpp"

#include "machphys/machdata.hpp"
#include "machphys/mcmovinf.hpp"
#include "machphys/random.hpp"

#include "machlog/administ.hpp"
#include "machlog/cntrl.hpp"
#include "machlog/construc.hpp"
#include "machlog/constron.hpp"
#include "machlog/follow.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/mine.hpp"
#include "machlog/move.hpp"
#include "machlog/opconstr.hpp"
#include "machlog/planet.hpp"
#include "machlog/races.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/squad.hpp"
#include "machlog/strategy.hpp"

PER_DEFINE_PERSISTENT( MachLogAdminConstructOperation );

MachLogAdminConstructOperation::MachLogAdminConstructOperation( MachLogAdministrator * pActor )
:	MachLogOperation( "ADMIN_CONSTRUCT_OPERATION", MachLogOperation::ADMIN_CONSTRUCT_OPERATION ),
	pActor_( pActor ),
    complete_( false ),
	pConstruction_( NULL ),
	currentlyAttached_( false )
{
}

MachLogAdminConstructOperation::MachLogAdminConstructOperation( MachLogAdministrator * pActor, MachLogConstruction* pConstruction )
:	MachLogOperation( "ADMIN_CONSTRUCT_OPERATION", MachLogOperation::ADMIN_CONSTRUCT_OPERATION ),
	pActor_( pActor ),
    complete_( false ),
	pConstruction_( pConstruction ),
	currentlyAttached_( true )
{
	ASSERT( pConstruction_, "Pointer to construction passed into MLAdminConstructOp is NULL\n" );
	pConstruction_->attach( this );

	if( pConstruction_->race() == pActor_->race() )
	{
		pConstruction_->addReservation();	
	}
}

MachLogAdminConstructOperation::~MachLogAdminConstructOperation()
{
	if( currentlyAttached_ )
	{
		pConstruction_->detach( this );

		if( pConstruction_->race() == pActor_->race() )
		{
			pConstruction_->cancelReservation();	
		}		
	}
		
}
void MachLogAdminConstructOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogAdminConstructOperation\n"; 
}

///////////////////////////////////

bool MachLogAdminConstructOperation::doStart()
{
	return not checkNeedAndDoLeaveOperation( pActor_ );
}

PhysRelativeTime MachLogAdminConstructOperation::doUpdate( )
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
			/*
			// for the future......?
			if ( ( not (*i)->isIdle() ) 
			and not ( (*i)->strategy().currentOperationType() == MachLogOperation::FOLLOW_OPERATION )
			and not (  (*i)->strategy().currentOperationType() == MachLogOperation::MOVE_TO_OPERATION ) )	
			*/
			
			//!! Note that allConstructorsIdle is currently unused.
			//!! Perhaps we should have a "noConstructorsAvailable" instead. If this is the case,
			//!! I guess we should wait for at least one to become available.......?
			//!! Actually, maybe this is taken care of anyway by the way the thing cycles round
			//!! assigning free constructors and only terminates when the thign is built.
        }
    }

    //If we have no constructors, dissovle the squadron
    if( nConstructors == 0 )
    {
        if( MachLogRaces::instance().controller( pActor_->race() ).type() == MachLogController::AI_CONTROLLER )
        	pActor_->squadron()->releaseAllMachines();
        complete_ = true;
        return 0.0;
    }

	bool haveIncompleteConstruction = false;
	MachLogConstruction* pConstruction = NULL;
	if( not pConstruction_ )
	{
	    //Find an incomplete mine
	    MachLogMine* pMine;
	    haveIncompleteConstruction = findIncompleteMine( &pMine );			
		pConstruction = pMine;
	}
	else
	{
		haveIncompleteConstruction = not pConstruction_->isComplete();
		pConstruction = pConstruction_;
		complete_ = pConstruction_->isComplete();
	}
    //Task the transporter/constructors as required
	////HAL_STREAM("(" << pActor_->id() << ") MLAdminConstructOp::doUpdate checking all machines in squadron\n" );
	////HAL_STREAM(" (void*)pConstruction " << (void*)pConstruction << std::endl );
	////HAL_STREAM(" haveIncompleteConstruction " << haveIncompleteConstruction << std::endl );
	for( MachLogSquadron::Machines::iterator i = pActor_->squadron()->machines().begin();
	     i!=pActor_->squadron()->machines().end(); ++i )
    {
		// don't interrupt actors who are currently evading aggressors
		if( ( not (*i)->evading() ) and
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
                if( haveIncompleteConstruction )
                {
                    ////HAL_STREAM("  Assigning construct operation to " << (*i)->id() << std::endl;)
    				pConstructor->newOperation( _NEW( MachLogConstructOperation( pConstructor, pConstruction ) ) );
                    taskedConstructor = true;
                }
                else
                {
                    ////HAL_STREAM("  Move out of way " << (*i)->id() << std::endl;)
            		pActor_->moveOutOfTheWay( pConstructor );
                }
			}
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

    //Tell the administrator where to go/*, but only if all constructors were idle, */
    //to ensure he gets on with telling them what to do, rather than scooting off
    //for miles and ignoring them on the way.
//    if( allConstructorsIdle )
//    {
/*        if( haveIncompleteMine )
        {
            //Get the mine and administrator locations
        	MexPoint2d mineLocation( pMine->position() );
            MexPoint2d administratorLocation( pActor_->position() );

            //Check for not in scanner range of the mine
            MATHEX_SCALAR scannerRange = pActor_->machineData().scannerRange();
            if( mineLocation.sqrEuclidianDistance( administratorLocation ) > scannerRange * scannerRange )
            {
                //Find a position near the mine
            	PhysConfigSpace2d& configSpace = MachLogPlanet::instance().configSpace();
                MexPoint2d targetPoint;
                if( not pActor_->motionSeq().hasDestination() and configSpace.findSpace( administratorLocation, mineLocation,
                                           pActor_->highClearence(),
                                           scannerRange * 0.75, &targetPoint ) )
                {
                    subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, MexPoint3d( targetPoint ) ) ) );
                    ////HAL_STREAM( "  moving administrator to mine at " << targetPoint << std::endl );
                }
            }
        }  
        else
    	{
              //Go to a garrison
    	    	////HAL_STREAM( " moving administrator out of the way\n" );
			if( not pActor_->motionSeq().hasDestination() )
    		pActor_->moveOutOfTheWay( pActor_ );
	    }
//    }*/
  	//HAL_STREAM( "MADConOp::doChooseSubOperation exiting\n" );
	return 15.0;		
}

void MachLogAdminConstructOperation::doFinish()
{
	/* Intentionally Empty	*/
}
	
bool MachLogAdminConstructOperation::doIsFinished() const
{
	return complete_;
//	return pConstruction_->isComplete();
}

bool MachLogAdminConstructOperation::doBeInterrupted()
{
	return true;
}

bool MachLogAdminConstructOperation::findIncompleteMine( MachLogMine** ppMine )
{
	bool found = false;
	MachLogRaces::Mines& mines = MachLogRaces::instance().mines( pActor_->MachLogMachine::race() );
	if( mines.size() != 0 )
	{
  		////HAL_STREAM( " at least one mine : " << mines.size() );
		MachLogRaces::Mines::iterator i = mines.begin();
		for( ; i != mines.end(); ++i )
		{
            if( not (*i)->isComplete() )
            {
                found = true;
                *ppMine = (*i);
				//Do them in the order that they were discovered!!!!
				break;
            }
		}
	}

    return found;
}


//virtual
bool MachLogAdminConstructOperation::beNotified( W4dSubject* pSubject,
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


void perWrite( PerOstream& ostr, const MachLogAdminConstructOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.pConstruction_;
    ostr << op.complete_;
	ostr << op.currentlyAttached_;
}

void perRead( PerIstream& istr, MachLogAdminConstructOperation& op )
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

MachLogAdminConstructOperation::MachLogAdminConstructOperation( PerConstructor con )
:	MachLogOperation( con )
{
}


