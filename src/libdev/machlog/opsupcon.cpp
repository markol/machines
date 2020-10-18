/*
 * O P S U P C O N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/opsupcon.hpp"

#include "ctl/algorith.hpp"

#include "sim/manager.hpp"

#include "phys/cspace2.hpp"

#include "machphys/mphydata.hpp"
#include "machphys/objdata.hpp"


#include "machlog/animatio.hpp"
#include "machlog/constron.hpp"
#include "machlog/construc.hpp"
#include "machlog/machvman.hpp"
#include "machlog/opcaptur.hpp"
#include "machlog/opconstr.hpp"
#include "machlog/opdecons.hpp"
#include "machlog/oprepair.hpp"
#include "machlog/strategy.hpp"
//#include "machlog/races.hpp"

#include <algorithm>

PER_DEFINE_PERSISTENT( MachLogSuperConstructOperation );
/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachLogSuperConstructOperation::MachLogSuperConstructOperation( MachLogConstructor * pActor, const Constructions& constructionArray, MachLogOperation::OperationType opType )
:	MachLogOperation( "SUPERCONSTRUCT_OPERATION", MachLogOperation::SUPERCONSTRUCT_OPERATION ),
	pActor_( pActor ),
	doingFirstOp_( true ),
  	opType_( opType ),
  	lastUpdateTime_( SimManager::instance().currentTime() ),
	pTargetConstruction_( NULL )
{
	PRE ( opType_ == MachLogOperation::CONSTRUCT_OPERATION or
			opType_ == MachLogOperation::DECONSTRUCT_OPERATION or
			opType_ == MachLogOperation::CAPTURE_OPERATION or
			opType_ == MachLogOperation::REPAIR_OPERATION );

	PRE ( constructionArray.size() != 0 );

	// attach to all constructions in constructionArray and build constructionArray_ in reverse order
	// from constructionArray.

	constructionArray_.reserve( constructionArray.size() );

	for( Constructions::const_reverse_iterator i = constructionArray.rbegin(); i != constructionArray.rend(); ++i )
    {
		MachLogConstruction* pConstruction = *i;

		Constructions::iterator iDuplicate = find( constructionArray_.begin(), constructionArray_.end(), pConstruction );

		// only add this to the array (and attach to it) if it isn't a duplicate
		if( iDuplicate == constructionArray_.end() )
		{
			// not a duplicate - add it into the superOp's array
			pConstruction->attach( this );
			constructionArray_.push_back( pConstruction );
		}
	}

	// need to do "early attaches" to wireframes scheduled for construction to ensure
	// the logical constructions are not deleted in the event of other constructors
	// adding themselves to that construction's list (by targetting it for a construct op)
	// then removing themselves, which would usually leave the constructor list empty and
	// be grounds for the construction being deleted. When this superop's constructors are
	// themselves "really" added to the constructions later, the construction will ignore
	// the duplicate attach. The pActor_ constructor is removed from all remaining constructions
	// when this operation is deleted.

	// !! This now superceded by "reservation" system.

	if ( opType_ == MachLogOperation::CONSTRUCT_OPERATION )
		addReservationToAllConstructions();

	pActor_->isDoingSuperConstruct( true );

}

/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */

MachLogSuperConstructOperation::~MachLogSuperConstructOperation()
{
	// if this superop is for construct operation subops, we must ensure that no
	// "reservations" in the form of pre-attached constructors are still attached to any
	// remaining constructions
	if ( opType_ == MachLogOperation::CONSTRUCT_OPERATION )
		cancelReservationFromAllConstructions();

	// and finally detach the op from any remaining constructions
	for( Constructions::iterator i = constructionArray_.begin(); i!=constructionArray_.end(); ++i )
		(*i)->detach( this );

	pActor_->isDoingSuperConstruct( false );

}

/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogSuperConstructOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogSuperConstructOperation\n";
   	switch( opType_ )
	{
		case MachLogOperation::CONSTRUCT_OPERATION:
			o << "CONSTRUCT_OPERATION" << std::endl;
			break;
		case MachLogOperation::DECONSTRUCT_OPERATION:
			o << "DECONSTRUCT_OPERATION" << std::endl;
			break;
		case MachLogOperation::CAPTURE_OPERATION:
			o << "CAPTURE_OPERATION" << std::endl;
			break;
		case MachLogOperation::REPAIR_OPERATION:
			o << "REPAIR_OPERATION" << std::endl;
			break;
		default:
			o << "UNKNOWN STATE " << (int)opType_ << std::endl;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogSuperConstructOperation::doStart()
{
	bool result = not checkNeedAndDoLeaveOperation( pActor_ );
   	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime
MachLogSuperConstructOperation::doUpdate()
{
	HAL_STREAM("(" << pActor_->id() << ") MachLogSuperConstructOperation::doUpdate() " << std::endl );
	if ( pSubOperation() )
		return 5.0;

  	// otherwise, we're free to proceed with assigning the op.

	// time to acquire us the next construction and execute the op on it.
	// incidentally, note that because we (deliberately) constructed the member
	// vector constructionArray_ in the reverse order from the vector passed as a
	// parameter, we take the last entry from the vector to preserve the order of
	// operations.

	HAL_STREAM(" there are " << constructionArray_.size() << " constructions left to do work on\n" );
	ASSERT( constructionArray_.size() > 0, "Should have at least one construction in doUpdate()." );

	MachLogConstruction* pTargetConstruction = NULL;

	// safe to do this as if doIsFinished() hasn't returned true, we know the constructionArray_ still
	//	has at least one member
	pTargetConstruction = constructionArray_.back();

	ASSERT( pTargetConstruction, "pTargetConstruction should not be NULL" );

	// let's assign a new subop to the constructor

//	HAL_STREAM(" assigning a subOperation of the appropriate type: *this is " << *this << std::endl );
  	switch( opType_ )
	{
	case MachLogOperation::CONSTRUCT_OPERATION:
		subOperation( pActor_, _NEW( MachLogConstructOperation( pActor_, pTargetConstruction ) ) );
		break;

	case MachLogOperation::DECONSTRUCT_OPERATION:
		subOperation( pActor_, _NEW( MachLogDeconstructOperation( pActor_, pTargetConstruction ) ) );
		break;

	case MachLogOperation::CAPTURE_OPERATION:
		subOperation( pActor_, _NEW( MachLogCaptureOperation( pActor_, pTargetConstruction ) ) );
		break;

	case MachLogOperation::REPAIR_OPERATION:
		subOperation( pActor_, _NEW( MachLogRepairOperation( pActor_, pTargetConstruction ) ) );
		break;

	default:
		ASSERT_INFO( (int)opType_ );
		ASSERT( false, "MachLogSuperConstructOperation::doUpdate has an invalid opType_ in the switch." );
	}

	// give a "moving to next site" mail ONLY if we've dealt with at least one site before in this superop.
	if( doingFirstOp_ )
	{
	        doingFirstOp_ = false;
	}
	else
	        MachLogMachineVoiceMailManager::instance().postNewMail( MachLog::CONSTRUCTOR, pActor_->subType(), MachLogMachineVoiceMailManager::MEV_MOVING_TO_NEXT, pActor_->id(),  pActor_->race() );

	// "fire and forget": we don't care about observing that construction anymore
	pTargetConstruction->detach( this );


	if ( opType_ == MachLogOperation::CONSTRUCT_OPERATION )
	{
		// now we actually have a constructor added to the construction (via the assigned operation),
		// the superop can release its reservation.
		if( pTargetConstruction->race() == pActor_->race() )
		{
			pTargetConstruction->cancelReservation();
		}
	}

	// delete that construction from the end of constructionArray_;
	constructionArray_.pop_back();

	HAL_STREAM("MLSuperConstructOp is complete\n" );

	return 10.0;	//or whatever
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogSuperConstructOperation::doFinish()
{
	/* Intentionally Empty	*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogSuperConstructOperation::doIsFinished() const
{
	return ( constructionArray_.size() == 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
bool MachLogSuperConstructOperation::doBeInterrupted()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogSuperConstructOperation::addReservationToAllConstructions()
{
	for( Constructions::iterator i = constructionArray_.begin(); i!=constructionArray_.end(); ++i )
    {
		MachLogConstruction& construction = (**i);
		if( construction.race() == pActor_->race() )
		{
			construction.addReservation();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogSuperConstructOperation::cancelReservationFromAllConstructions()
{
	for( Constructions::iterator i = constructionArray_.begin(); i!=constructionArray_.end(); ++i )
    {
		MachLogConstruction& construction = (**i);
		if( construction.race() == pActor_->race() )
		{
			construction.cancelReservation();
		}
	}
}

/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogSuperConstructOperation::addConstructorToAllConstructions()
{
	for( Constructions::iterator i = constructionArray_.begin(); i!=constructionArray_.end(); ++i )
    {
		(*i)->addConstructor( pActor_ );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogSuperConstructOperation::removeConstructorFromAllConstructions()
{
	for( Constructions::iterator i = constructionArray_.begin(); i!=constructionArray_.end(); ++i )
    {
		(*i)->removeConstructor( pActor_ );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

//virtual
bool MachLogSuperConstructOperation::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData )
{
	// precondition should REALLY be included to check if it's a construction....but it's a bit messy.

	// find match in our group of constructors

	bool stayAsObserver = true;
	MachLogConstruction* pConstruction = NULL;

	Constructions::iterator iTarget;// = constructionArray_.begin()
	Constructions::iterator i = find( constructionArray_.begin(), constructionArray_.end(), pSubject );

	ASSERT( i != constructionArray_.end(), "Construction was not found in list!" );

	iTarget = i;
	pConstruction = (*i);

	switch( event )
	{
		case W4dSubject::DELETED:
			stayAsObserver = false;
			break;
		case W4dSubject::CLIENT_SPECIFIC:
		{
			switch( clientData )
			{
				case MachLog::RACE_CHANGED:
					if ( opType_ == MachLogOperation::CAPTURE_OPERATION and ( pConstruction->race() == pActor_->race() )	)
						stayAsObserver = false;
					if ( opType_ == MachLogOperation::REPAIR_OPERATION and ( pConstruction->race() != pActor_->race() )	)
						stayAsObserver = false;
					if ( opType_ == MachLogOperation::CONSTRUCT_OPERATION )
						stayAsObserver = false;
					break;

				case MachLog::HEALTH_STATUS_CHANGED:
					if ( opType_ == MachLogOperation::REPAIR_OPERATION )
					{
						const MachPhysObjectData& objData = pConstruction->objectData();
						if ( pConstruction->hp() == objData.hitPoints() )
							stayAsObserver = false;
					}
					break;

				case MachLog::PERCENTAGE_COMPLETE_CHANGED:
					if ( opType_ == MachLogOperation::CONSTRUCT_OPERATION and pConstruction->isComplete() )
						stayAsObserver = false;
					break;
			}
		}
		default: ;
	}

	if ( stayAsObserver == false )
	{
		constructionArray_.erase( iTarget );
		if ( opType_ == MachLogOperation::CONSTRUCT_OPERATION )
		{
			if( pConstruction->race() == pActor_->race() )
			{
				pConstruction->cancelReservation();
			}
		}
	}

  	return stayAsObserver;
}

void perWrite( PerOstream& ostr, const MachLogSuperConstructOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.constructionArray_;
	ostr << op.opType_;
	ostr << op.pTargetConstruction_;
	ostr << op.lastUpdateTime_;
	ostr << op.doingFirstOp_;
}

void perRead( PerIstream& istr, MachLogSuperConstructOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.constructionArray_;
	istr >> op.opType_;
	istr >> op.pTargetConstruction_;
	istr >> op.lastUpdateTime_;
	istr >> op.doingFirstOp_;

	//reattach to all the constructions
	for( MachLogSuperConstructOperation::Constructions::iterator i = op.constructionArray_.end();
		i != op.constructionArray_.begin(); --i )
    {
		(*(i-1) )->attach( &op );
	}
}

MachLogSuperConstructOperation::MachLogSuperConstructOperation( PerConstructor con )
:	MachLogOperation( con )
{
}
