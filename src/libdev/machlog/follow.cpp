/*
 * F O L L O W . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */
 
//  Definitions of non-inline non-template methods and global functions

#include "machlog/follow.hpp"

#include "machlog/internal/followi.hpp"

#include "phys/motchunk.hpp"

#include "machphys/mcmovinf.hpp"

#include "machlog/mcmotseq.hpp"
#include "machlog/machine.hpp"
#include "machlog/races.hpp"
#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT( MachLogFollowOperation );

//dereferencing all these will cause a hit in the debug version - in the release version the compiler will remove
//any unreferenced variables.
#define CB_MachLogFollowOperation_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( MachLogMachine*, pActor_ ); \
    	CB_DEPIMPL( MachLogMachine*, pTarget_ ); \
		CB_DEPIMPL( MexPoint2d, offset_ ); \
		CB_DEPIMPL( CamouflagedOp, camoStatus_ ); \
		CB_DEPIMPL( TerminateFlag, terminateFlag_ ); 

MachLogFollowOperation::MachLogFollowOperation( MachLogMachine * pActor, MachLogMachine* pTarget,
                            const MexPoint2d& offset, CamouflagedOp camoStatus, TerminateFlag terminateFlag )
:	MachLogOperation( "FOLLOW_OPERATION", MachLogOperation::FOLLOW_OPERATION ),
	pImpl_( _NEW( MachLogFollowOperationImpl( pActor, pTarget, offset, camoStatus, terminateFlag ) ) )
{
	CB_MachLogFollowOperation_DEPIMPL();

	if( camoStatus_ == CAMOUFLAGED )
		pActor_->camouflaged( true );
    TEST_INVARIANT;
	
	pTarget_->attach( this );
}

MachLogFollowOperation::MachLogFollowOperation( MachLogMachine * pActor, MachLogMachine* pTarget,
                            const MexPoint2d& offset, PhysPathFindingPriority pathFindingPriority, CamouflagedOp camoStatus, TerminateFlag terminateFlag )
:	MachLogOperation( "FOLLOW_OPERATION", MachLogOperation::FOLLOW_OPERATION, pathFindingPriority ),
	pImpl_( _NEW( MachLogFollowOperationImpl( pActor, pTarget, offset, camoStatus, terminateFlag ) ) )
{
	CB_MachLogFollowOperation_DEPIMPL();

	if( camoStatus_ == CAMOUFLAGED )
		pActor_->camouflaged( true );
    TEST_INVARIANT;
	
	pTarget_->attach( this );
}

MachLogFollowOperation::~MachLogFollowOperation()
{
	CB_MachLogFollowOperation_DEPIMPL();

    TEST_INVARIANT;
	if( camoStatus_ == CAMOUFLAGED )
		pActor_->camouflaged( false );
	if( pTarget_ )
		pTarget_->detach( this );

	_DELETE( pImpl_ );
}

void MachLogFollowOperation::CLASS_INVARIANT
{
	INVARIANT( this );
}

//virtual
bool MachLogFollowOperation::doStart()
{
	CB_MachLogFollowOperation_DEPIMPL();

	if( checkNeedAndDoLeaveOperation( pActor_ ) )
		return false;

    //Check the machine still exists
    if( pTarget_ )
        pActor_->motionSeq().follow( pTarget_, offset_ );

	return true;
}

//virtual
PhysRelativeTime MachLogFollowOperation::doUpdate()
{
	CB_MachLogFollowOperation_DEPIMPL();

	if( pSubOperation() )
	{
		// cannot have camouflage active if we are doing a subop
		if( camoStatus_ == CAMOUFLAGED )
			pActor_->camouflaged( false );
		return 2.0;	
	}
	else
	{
		// ensures camo is reactivated in case we were previously doing a subop
		if( camoStatus_ == CAMOUFLAGED )
			pActor_->camouflaged( true );	
	}
		

	//HAL_STREAM("(" << pActor_->id() << ") MLFollowOp::doUpdate\n" );
    if( pTarget_ != NULL )
    {
        MachLogMachineMotionSequencer& sequencer = pActor_->motionSeq();
        //First check that still following
        if( not sequencer.isFollowing() )
        {
            //Check target still exists
            if( not sequencer.hasDestination() )
            {
                //Resume following it
                sequencer.follow( pTarget_, offset_ );
            }
            else
            {
				pTarget_->detach( this );
                pTarget_ = NULL;
            }
        }

        //If we have a target update motion sequencer
        if( pTarget_ != NULL )
		{
			//HAL_STREAM("(" << pActor_->id() << ") MLFollowOp::doUpdate Exit MotionSeq::\n" );
			//HAL_STREAM( sequencer );
			//HAL_STREAM( "\n returning " );
			PhysRelativeTime result = pActor_->motionSeq().update( 0.001 );
			//HAL_STREAM( result << std::endl );
        	return 2.0;
		}
    }

	//HAL_STREAM("(" << pActor_->id() << ") MLFollowOp::doUpdate Exit return 1.0\n" );
	return 1.0;
}

//virtual
void MachLogFollowOperation::doFinish()
{
	CB_MachLogFollowOperation_DEPIMPL();

    pActor_->motionSeq().stop();
}

//virtual
bool MachLogFollowOperation::doIsFinished() const
{
	CB_MachLogFollowOperation_DEPIMPL();

	if( terminateFlag_ == TERMINATE_WHEN_CLOSE 
	    and pTarget_ 
		and pActor_->position().sqrEuclidianDistance( pTarget_->position() ) < 801 ) // root(800) metres away deemed sufficiently close
																					 // as randomDouble( -20, 20 ), randomDouble( -20, 20 )
																					 // is standard random follow offset
		return true;
	else	
    	return ( pTarget_ == NULL and not pActor_->motionSeq().hasDestination() );
}

//virtual
void MachLogFollowOperation::doOutputOperator( ostream& o ) const
{
	CB_MachLogFollowOperation_DEPIMPL();

	o << "MachLogFollowOperation, " << (void*)pTarget_ << "  " << offset_ << std::endl; 
	o << " finished " << doIsFinished() << std::endl;
	o << "  position = " << pActor_->position() << std::endl;
    if( pTarget_ != NULL )
    	o << "  pTarget_->id() = " << pTarget_->id() << std::endl;
//	o << pActor_->motionSeq() << std::endl;
}

//virtual
bool MachLogFollowOperation::doBeInterrupted()
{
	CB_MachLogFollowOperation_DEPIMPL();

	pActor_->motionSeq().stop();
	nextCallBackTime( SimManager::instance().currentTime() + pActor_->motionSeq().update( 0.001 ) );
	return not pActor_->motionSeq().hasDestination();
}

//virtual
bool MachLogFollowOperation::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData )
{
	CB_MachLogFollowOperation_DEPIMPL();

	ASSERT( pSubject == (W4dSubject*)pTarget_, "Received a message from an unexpected subject." );

	bool stayAttached = true;
	
	switch( event )
	{
	case W4dSubject::DELETED:
		pTarget_ = NULL;
		// stop in our tracks through sheer shock and disbelief........
		pActor_->motionSeq().stop();
		nextCallBackTime( SimManager::instance().currentTime() );
		stayAttached = false;
		break;
	case W4dSubject::CLIENT_SPECIFIC:
		switch( clientData )
		{
		case MachLog::TELEPORTED_OUT_OF_WORLD:
		case MachLog::RACE_CHANGED:
			// stop in our tracks through sheer shock and disbelief........
			pActor_->motionSeq().stop();
			pTarget_ = NULL;
			nextCallBackTime( SimManager::instance().currentTime() );
			stayAttached = false;
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


/////////////////////////////////////////////////// persistence /////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogFollowOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;


}

void perRead( PerIstream& istr, MachLogFollowOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;
	
	if( op.pTarget() )
		op.pTarget()->attach( &op );
}

MachLogMachine*	MachLogFollowOperation::pTarget()
{
	CB_MachLogFollowOperation_DEPIMPL();

	return pTarget_;
}

MachLogFollowOperation::MachLogFollowOperation( PerConstructor con )
:	MachLogOperation( con )
{
}


/* End FOLLOW.CPP ***************************************************/


