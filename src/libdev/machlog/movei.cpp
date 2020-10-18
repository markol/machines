/*
 * M O V E I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/movei.hpp"

#include "machlog/machine.hpp"

//#include "mathex/point2d.hpp"

///////////////////////////////////////////// MOVE impl ////////////////////////////////////////////////////

PER_DEFINE_PERSISTENT( MachLogMoveToOperationImpl );

MachLogMoveToOperationImpl::MachLogMoveToOperationImpl( MachLogMachine * pActor, const MexPoint3d& dest, bool checkForLeave )
:	pActor_( pActor ),
  	dest_( dest ),
  	checkForLeave_( checkForLeave ),
    commandId_( MachLogMoveToOperation::NO_ID ),
	tolerance_( -1 )
{
    TEST_INVARIANT;
}

MachLogMoveToOperationImpl::MachLogMoveToOperationImpl( MachLogMachine * pActor, const MachLogMoveToOperation::Path& path, bool checkForLeave )
:	pActor_( pActor ),
	path_( path ),
	checkForLeave_( checkForLeave ),
    commandId_( MachLogMoveToOperation::NO_ID ),
	tolerance_( -1 )
{
    TEST_INVARIANT;
}

MachLogMoveToOperationImpl::MachLogMoveToOperationImpl( MachLogMachine * pActor, const MexPoint3d& dest, size_t commandId, bool checkForLeave )
:	pActor_( pActor ),
  	dest_( dest ),
  	checkForLeave_( checkForLeave ),
    commandId_( commandId ),
	tolerance_( -1 )
{
    TEST_INVARIANT;
}

MachLogMoveToOperationImpl::MachLogMoveToOperationImpl( MachLogMachine * pActor, const MachLogMoveToOperation::Path& path, size_t commandId, bool checkForLeave )
:	pActor_( pActor ),
	path_( path ),
	checkForLeave_( checkForLeave ),
    commandId_( commandId ),
	tolerance_( -1 )
{
    TEST_INVARIANT;
}

MachLogMoveToOperationImpl::~MachLogMoveToOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogMoveToOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogMoveToOperationImpl& t )
{

    o << "MachLogMoveToOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogMoveToOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogMoveToOperationImpl& moveOpImpl )
{
	ostr << moveOpImpl.pActor_;
	ostr << moveOpImpl.dest_;
	ostr << moveOpImpl.checkForLeave_;
	ostr << moveOpImpl.path_;
	ostr << moveOpImpl.commandId_;
	ostr << moveOpImpl.tolerance_;
	
}

void perRead( PerIstream& istr, MachLogMoveToOperationImpl& moveOpImpl )
{
	istr >> moveOpImpl.pActor_;
	istr >> moveOpImpl.dest_;
	istr >> moveOpImpl.checkForLeave_;
	istr >> moveOpImpl.path_;
	istr >> moveOpImpl.commandId_;
	istr >> moveOpImpl.tolerance_;
}

MachLogMoveToOperationImpl::MachLogMoveToOperationImpl( PerConstructor )
{
}

///////////////////////////////////////////// MOVE AND TURN impl ////////////////////////////////////////////////////

PER_DEFINE_PERSISTENT( MachLogMoveAndTurnOperationImpl );

MachLogMoveAndTurnOperationImpl::MachLogMoveAndTurnOperationImpl( MachLogMachine * pActor, 
									const MexPoint3d& dest,
									const MexPoint3d& endFacing,
									bool checkForLeave,
									PhysRelativeTime initialDelay )
:	pActor_( pActor ),
	dest_( dest ),
	state_( MachLogMoveAndTurnOperation::WAITING ),
	endFacing_( endFacing ),
	checkForLeave_( checkForLeave ),
	initialDelay_( initialDelay ),
	dontMoveUntil_( 0.0 )	// safe (dummy) value - will be set later
{
    TEST_INVARIANT;
}
	
MachLogMoveAndTurnOperationImpl::~MachLogMoveAndTurnOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogMoveAndTurnOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogMoveAndTurnOperationImpl& t )
{

    o << "MachLogMoveAndTurnOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogMoveAndTurnOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogMoveAndTurnOperationImpl& moveTurnOpImpl )
{
	ostr << moveTurnOpImpl.state_;
	ostr << moveTurnOpImpl.pActor_;
	ostr << moveTurnOpImpl.dest_;
	ostr << moveTurnOpImpl.endFacing_;
	ostr << moveTurnOpImpl.checkForLeave_;
	ostr << moveTurnOpImpl.dontMoveUntil_;
	ostr << moveTurnOpImpl.initialDelay_;
}

void perRead( PerIstream& istr, MachLogMoveAndTurnOperationImpl& moveTurnOpImpl )
{
	istr >> moveTurnOpImpl.state_;
	istr >> moveTurnOpImpl.pActor_;
	istr >> moveTurnOpImpl.dest_;
	istr >> moveTurnOpImpl.endFacing_;
	istr >> moveTurnOpImpl.checkForLeave_;
	istr >> moveTurnOpImpl.dontMoveUntil_;
	istr >> moveTurnOpImpl.initialDelay_;
}

MachLogMoveAndTurnOperationImpl::MachLogMoveAndTurnOperationImpl( PerConstructor )
{
}


/* End MOVEI.CPP ***************************************************/
