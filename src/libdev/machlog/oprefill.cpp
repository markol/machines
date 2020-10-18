/*
 * O P L M I N E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/oprefill.hpp"

#include "ctl/list.hpp"

#include "mathex/line2d.hpp"

#include "machphys/mphydata.hpp"
#include "machphys/slocdata.hpp"

#include "machlog/mcmotseq.hpp"

#include "machlog/garrison.hpp"
#include "machlog/move.hpp"
#include "machlog/inout.hpp"
#include "machlog/races.hpp"
#include "machlog/spy.hpp"

PER_DEFINE_PERSISTENT( MachLogRefillLandMinesOperation );
/* //////////////////////////////////////////////////////////////// */

MachLogRefillLandMinesOperation::MachLogRefillLandMinesOperation( MachLogSpyLocator * pActor )
:	MachLogOperation( "REFILLLANDMINES_OPERATION", MachLogOperation::REFILLLANDMINES_OPERATION ),
	pActor_( pActor ),
	dest_( pActor_->position() ),
	doneMove_( false )
{
}

MachLogRefillLandMinesOperation::~MachLogRefillLandMinesOperation( )
{
}

void MachLogRefillLandMinesOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogRefillLandMinesOperation Dest " << dest_ << " position " << pActor_->position() << std::endl;
	o << " Actor mines " << pActor_->nMines() << std::endl;
	o << " Garrions in race " << MachLogRaces::instance().garrisons( pActor_->race() ).size() << std::endl;
}

///////////////////////////////////

bool MachLogRefillLandMinesOperation::doStart()
{
	HAL_STREAM("(" << pActor_->id() << ") MLRefillLandMinesOperation::doStart\n" );
	MachLogConstruction* pConstruction;
	bool insideABuilding = checkNeedLeaveOperation( pActor_, &pConstruction );
	HAL_STREAM(" inside Building " << insideABuilding << std::endl );
	if( insideABuilding and pConstruction->objectType() != MachLog::GARRISON )
		return not checkNeedAndDoLeaveOperation( pActor_ );

	return true;
}


void MachLogRefillLandMinesOperation::doFinish()
{
}

bool MachLogRefillLandMinesOperation::doIsFinished() const
{
	HAL_STREAM("(" << pActor_->id() << ") MLRefilLLMinesOp::doIsFinished\n" );
	bool result = ( pActor_->nMines() == pActor_->data().mineCapacity() and
			 not pActor_->motionSeq().hasDestination() and
			 doneMove_ )
			 or MachLogRaces::instance().garrisons( pActor_->race() ).size() == 0;
	HAL_STREAM(" result " << result << std::endl );
	return result;
}

//virtual
bool MachLogRefillLandMinesOperation::doBeInterrupted()
{
	pActor_->motionSeq().stop();
	return not pActor_->motionSeq().hasDestination();
}

//virtual
PhysRelativeTime MachLogRefillLandMinesOperation::doUpdate( )
{
	if( pSubOperation() )
		return 1.0;
	//are we full of mines? If so then issue move operation
	HAL_STREAM("(" << pActor_->id() << ") MLRefillLMinesOp::doUpdate\n" );
	HAL_STREAM(" nMines " << pActor_->nMines() << " capacity " << pActor_->data().mineCapacity() << std::endl );
	if( pActor_->nMines() == pActor_->data().mineCapacity() )
	{
		HAL_STREAM(" spy is full issue to move to origin\n" );
		subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, dest_ ) ) );
		doneMove_ = true;
		return 5.0;
	}
	//get closest garrison and issue MLEnterBuildingOperation
	HAL_STREAM(" find closest garrison\n" );
	MATHEX_SCALAR sqrRange = 1000000000;

	MachLogGarrison* pGarrison = NULL;
	MachPhysStation* pStation = NULL;	//dummy value - we don't actually care about the station
	bool foundGarrison = MachLogRaces::instance().findFriendlyGarrisonClosestTo( *pActor_, &pGarrison, &pStation, MachLogRaces::FREE_BAYS_UNIMPORTANT );
	ASSERT( foundGarrison, "A spy should always be able to enter any garrison. Check your parmdata settings." );

	HAL_STREAM(" issue MLEnterBuildingOp\n" );
	subOperation( pActor_, _NEW( MachLogEnterBuildingOperation( pActor_, pGarrison, NULL ) ) );
	return 10.0;
}

void perWrite( PerOstream& ostr, const MachLogRefillLandMinesOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.pGarrison_;
	ostr << op.complete_;
	ostr << op.clientSuppliedGarrison_;
	ostr << op.dest_;
	ostr << op.doneMove_;
}

void perRead( PerIstream& istr, MachLogRefillLandMinesOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.pGarrison_;
	istr >> op.complete_;
	istr >> op.clientSuppliedGarrison_;
	istr >> op.dest_;
	istr >> op.doneMove_;
}

MachLogRefillLandMinesOperation::MachLogRefillLandMinesOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

/* //////////////////////////////////////////////////////////////// */
