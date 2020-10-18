/*
 * O P T R A N S P . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/optransp.hpp"

#include "machlog/internal/optransi.hpp"

#include "phys/motchunk.hpp"

#include "machphys/mcmovinf.hpp"
#include "machphys/random.hpp"

#include "machlog/smelter.hpp"
#include "machlog/races.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/oppickup.hpp"
#include "machlog/opputdwn.hpp"

PER_DEFINE_PERSISTENT( MachLogTransportOperation );

#define CB_MachLogTransportOperation_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( MachLogResourceCarrier*, pActor_ ); \
		CB_DEPIMPL( bool, finished_ ); \
		CB_DEPIMPL( bool, donePickUp_ );

MachLogTransportOperation::~MachLogTransportOperation()
{
	CB_MachLogTransportOperation_DEPIMPL();

	pActor_->removedTransportOp();

    TEST_INVARIANT;

	_DELETE( pImpl_ );
}

void MachLogTransportOperation::CLASS_INVARIANT
{
}

/* //////////////////////////////////////////////////////////////// */

MachLogTransportOperation::MachLogTransportOperation( MachLogResourceCarrier * pActor, MachLogResourceCarrier::StartWithPickUpOrPutDown startType )
:	MachLogOperation( "TRANSPORT_OPERATION", MachLogOperation::TRANSPORT_OPERATION ),
 	pImpl_( _NEW( MachLogTransportOperationImpl( pActor ) ) )	
{
	CB_MachLogTransportOperation_DEPIMPL();

	PRE( pActor_->isNormalResourceCarrier() );	

	pActor->addedTransportOp();

	donePickUp_ = ( startType == MachLogResourceCarrier::START_WITH_PUTDOWN );
}


void MachLogTransportOperation::doOutputOperator( ostream& o ) const
{
	CB_MachLogTransportOperation_DEPIMPL();

	o << "MachLogTransportOperation " << std::endl;		
}

bool MachLogTransportOperation::doStart()
{
	CB_MachLogTransportOperation_DEPIMPL();

	return not checkNeedAndDoLeaveOperation( pActor_ );
}

///////////////////////////////////

void MachLogTransportOperation::doFinish()
{
	/* Intentionally Empty	*/
}

bool MachLogTransportOperation::doIsFinished() const
{
	CB_MachLogTransportOperation_DEPIMPL();

	return finished_;
}

PhysRelativeTime
MachLogTransportOperation::doUpdate( )
{
	CB_MachLogTransportOperation_DEPIMPL();

	//have we got an active sub operation at the moment?
	if( pSubOperation() )
		return 2.0;

	//have we done the pickUp operation yet?
	if( not donePickUp_ )
	{
		if( not pActor_->hasSuppliers() )
		{
			//HAL_STREAM("(" << pActor_->id() << ") MLTransportOp...need to do pickup but run out of suppliers. Finish\n" );
			donePickUp_ = true;
		}
		else
		{
			//HAL_STREAM("(" << pActor_->id() << ") MLTransportOp...issuing PickUpOperation\n" );
			subOperation( pActor_, _NEW( MachLogPickUpOperation( pActor_ ) ) );
			donePickUp_ = true;
			return 10.0;
		}
	}

	//do the putdown part of the operation.
	//HAL_STREAM("(" << pActor_->id() << ") MLTransportOp...checking to see if pSmelter_ is not NULL\n" );
	MexPoint2d actorPos( pActor_->position() );
	if( pActor_->hasSmeltingBuilding() )
	{
		
		if(	pActor_->amountCarried() == 0 )
			donePickUp_ = false;
		else
		{			
  			//HAL_STREAM(" using point " << whichPoint << std::endl );
			subOperation( pActor_, _NEW( MachLogPutDownOperation( pActor_ ) ) );

			int iterations = pActor_->iterations();

			if( iterations != MachLogResourceCarrier::FOREVER )
			{
				--iterations;
				pActor_->iterations( iterations );
				if( iterations == 0 )
					finished_ = true;
			}
			else
				donePickUp_ = false;
		}
	}

	/*
	else
	{
		////HAL_STREAM(" smelter must have been destroyed setting finished flag\n" );
		finished_ = true;
	}
	*/

	return 1.0;
}

//virtual
bool MachLogTransportOperation::doBeInterrupted()
{
	return true;
}

/* //////////////////////////////////////////////////////////////// */

ostream& operator <<( ostream& o, const MachLogTransportOperation& t )
{
    o << "MachLogTransportOperation " << (void*)&t << " start" << std::endl;
    o << "MachLogTransportOperation " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogTransportOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;
}

void perRead( PerIstream& istr, MachLogTransportOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;

}

MachLogTransportOperation::MachLogTransportOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

/* End OPTRANSP.CPP *************************************************/
