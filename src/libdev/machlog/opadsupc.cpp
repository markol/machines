/*
 * O P A D S U P C . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/opadsupc.hpp"

//#include "ctl/algorith.hpp"
#include "sim/manager.hpp"
#include "phys/cspace2.hpp"

#include "machphys/mphydata.hpp"
#include "machphys/objdata.hpp"

#include "machlog/administ.hpp"
#include "machlog/animatio.hpp"
#include "machlog/constron.hpp"
#include "machlog/construc.hpp"
#include "machlog/opadcapt.hpp"
#include "machlog/opadcon.hpp"
#include "machlog/opaddeco.hpp"
#include "machlog/opadrepa.hpp"
#include "machlog/squad.hpp"
#include "machlog/strategy.hpp"

#include <algorithm>

/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT( MachLogAdminSuperConstructOperation );

MachLogAdminSuperConstructOperation::MachLogAdminSuperConstructOperation( MachLogAdministrator * pActor, const Constructions& constructionArray, MachLogOperation::OperationType opType )
:	MachLogOperation( "ADMIN_SUPERCONSTRUCT_OPERATION", MachLogOperation::ADMIN_SUPERCONSTRUCT_OPERATION ),
	pActor_( pActor ),
  	opType_( opType ),
  	lastUpdateTime_( SimManager::instance().currentTime() )
{
	PRE ( opType_ == MachLogOperation::CONSTRUCT_OPERATION or
			opType_ == MachLogOperation::DECONSTRUCT_OPERATION or
			opType_ == MachLogOperation::CAPTURE_OPERATION or
			opType_ == MachLogOperation::REPAIR_OPERATION );

	PRE ( constructionArray_.size() != 0 );


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

	if ( opType_ == MachLogOperation::CONSTRUCT_OPERATION )
		addReservationToAllConstructions();
}

/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */

MachLogAdminSuperConstructOperation::~MachLogAdminSuperConstructOperation()
{
	// if this superop is for construct operation subops, we must ensure that no
	// "reservations" in the form of pre-attached constructors are still attached to any
	// remaining constructions
	if ( opType_ == MachLogOperation::CONSTRUCT_OPERATION )
		cancelReservationFromAllConstructions();

	// and finally detach the op from any remaining constructions
	for( Constructions::iterator i = constructionArray_.begin(); i!=constructionArray_.end(); ++i )
    {
		(*i)->detach( this );
	}

}

/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogAdminSuperConstructOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogAdminSuperConstructOperation\n";
   	switch( opType_ )
	{
		case MachLogOperation::CONSTRUCT_OPERATION:
			o << "ADMIN_CONSTRUCT_OPERATION" << std::endl;
			break;
		case MachLogOperation::DECONSTRUCT_OPERATION:
			o << "ADMIN_DECONSTRUCT_OPERATION" << std::endl;
			break;
		case MachLogOperation::CAPTURE_OPERATION:
			o << "ADMIN_CAPTURE_OPERATION" << std::endl;
			break;
		case MachLogOperation::REPAIR_OPERATION:
			o << "ADMIN_REPAIR_OPERATION" << std::endl;
			break;
		default:
			o << "UNKNOWN STATE, DADDY-O...SOMETHING'S WRONG " << (int)opType_ << std::endl;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogAdminSuperConstructOperation::doStart()
{
	bool result = not checkNeedAndDoLeaveOperation( pActor_ );
   	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime
MachLogAdminSuperConstructOperation::doUpdate( )
{

	ASSERT ( constructionArray_.size() != 0, "MachLogAdminSuperConstructOperation::doUpdate : we shouldn't have an empty construction vector here." );
	ASSERT ( nConstructorsRemaining() != 0, "MachLogAdminSuperConstructOperation::doUpdate : we shouldn't have an empty constructors vector here." );

	if ( pSubOperation() )
		return 2.0;		// (or whatever...)

	// otherwise, we're free to proceed with assigning the op.

	// time to acquire us the next construction and execute the op on it.
	// incidentally, note that because we (deliberately) constructed the member
	// vector constructionArray_ in the reverse order from the vector passed as a
	// parameter, we take the last entry from the vector to preserve the order of
	// operations.

	MachLogConstruction* pTargetConstruction = NULL;

	ASSERT( constructionArray_.size() > 0, "Should have at least one construction in doUpdate()." );

	// safe to do this as if doIsFinished() hasn't returned true, we know the constructionArray_ still
	//	has at least one member
	//Constructions::iterator i = constructionArray_.end()-1;
	pTargetConstruction = constructionArray_.back();

	ASSERT( pTargetConstruction, "pTargetConstruction should not be NULL" );

	// let's assign a new subop to the administrator

  	switch( opType_ )
	{
	case MachLogOperation::CONSTRUCT_OPERATION:
		subOperation( pActor_, _NEW( MachLogAdminConstructOperation( pActor_, pTargetConstruction ) ) );
		break;

	case MachLogOperation::DECONSTRUCT_OPERATION:
		subOperation( pActor_, _NEW( MachLogAdminDeconstructOperation( pActor_, pTargetConstruction ) ) );
		break;

	case MachLogOperation::CAPTURE_OPERATION:
		subOperation( pActor_, _NEW( MachLogAdminCaptureOperation( pActor_, pTargetConstruction ) ) );
		break;

	case MachLogOperation::REPAIR_OPERATION:
		subOperation( pActor_, _NEW( MachLogAdminRepairOperation( pActor_, pTargetConstruction ) ) );
		break;

	default:
		ASSERT ( false, "MachLogAdminSuperConstructOperation::doUpdate has an invalid opType_ in the switch." );
	}

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

	return 10.0;	//or whatever

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogAdminSuperConstructOperation::doFinish()
{
	/* Intentionally Empty	*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogAdminSuperConstructOperation::doIsFinished() const
{
	return ( constructionArray_.size() == 0 or nConstructorsRemaining() == 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
bool MachLogAdminSuperConstructOperation::doBeInterrupted()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogAdminSuperConstructOperation::addReservationToAllConstructions()
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

void MachLogAdminSuperConstructOperation::cancelReservationFromAllConstructions()
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MachLogAdminSuperConstructOperation::nConstructorsRemaining()	const
{
	int nConstructors = 0;
	for( MachLogSquadron::Machines::iterator i = pActor_->squadron()->machines().begin();
	     i!=pActor_->squadron()->machines().end(); ++i )
    {
        if( (*i)->objectType() == MachLog::CONSTRUCTOR )
        {
			++nConstructors;
        }
    }

	return nConstructors;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
bool MachLogAdminSuperConstructOperation::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData )
{
	// precondition should REALLY be included to check if it's a construction....but it's a bit messy.

	// find match in our group of constructors

	bool stayAsObserver = true;
	MachLogConstruction* pConstruction = NULL;

	Constructions::iterator iTarget;
	Constructions::iterator i = find( constructionArray_.begin(), constructionArray_.end(), pSubject );

	ASSERT ( i != constructionArray_.end(), "MachLogAdminSuperConstructOperation::beNotified : notified by an unexpected subject." );

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
			if ( opType_ == MachLogOperation::CONSTRUCT_OPERATION and ( pConstruction->race() != pActor_->race() ) )
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


	default:
		;
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

void perWrite( PerOstream& ostr, const MachLogAdminSuperConstructOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.constructionArray_;
	ostr << op.opType_;
	ostr << op.pTargetConstruction_;
	ostr << op.lastUpdateTime_;
}

void perRead( PerIstream& istr, MachLogAdminSuperConstructOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.constructionArray_;
	istr >> op.opType_;
	istr >> op.pTargetConstruction_;
	istr >> op.lastUpdateTime_;
	for( MachLogAdminSuperConstructOperation::Constructions::iterator i = op.constructionArray_.begin();
		i != op.constructionArray_.end();
		++i )
		(*i)->attach( &op );
}

MachLogAdminSuperConstructOperation::MachLogAdminSuperConstructOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

/* End OPADSUPC.CPP *************************************************/
