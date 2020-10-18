/*
 * O P S T A N D G . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/opstandg.hpp"

#include "machlog/canattac.hpp"
#include "machlog/machine.hpp"

PER_DEFINE_PERSISTENT( MachLogStandGroundOperation );
/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachLogStandGroundOperation::MachLogStandGroundOperation( MachLogMachine * pActor )
:	MachLogOperation( "STAND_GROUND_OPERATION", MachLogOperation::STAND_GROUND_OPERATION ),
	pActor_( pActor )
{
	pActor_->isStandingGround( true );
	TEST_INVARIANT;
}


/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */

MachLogStandGroundOperation::~MachLogStandGroundOperation()
{
    TEST_INVARIANT;
	pActor_->isStandingGround( false );
}

/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogStandGroundOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogStandGroundOperation " << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogStandGroundOperation::doStart()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogStandGroundOperation::doFinish()
{
	/* Intentionally Empty	*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogStandGroundOperation::doIsFinished() const
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual 
PhysRelativeTime MachLogStandGroundOperation::doUpdate( )
{
	ASSERT( pActor_->isStandingGround(), "Doing standing ground op yet machine is flagged as NOT standing ground!" );
	// periodically refresh alertness
	pActor_->asCanAttack().setMinimumAlertness( 125 );
	return 10.0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
bool MachLogStandGroundOperation::doBeInterrupted()
{
	return true;
}

void perWrite( PerOstream& ostr, const MachLogStandGroundOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
}

void perRead( PerIstream& istr, MachLogStandGroundOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
}

MachLogStandGroundOperation::MachLogStandGroundOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* End OPSTANDG.CPP *************************************************/
