/*
 * A T T A C K I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/followi.hpp"

#include "machlog/actor.hpp"
#include "machlog/machine.hpp"


#include "mathex/point2d.hpp"

PER_DEFINE_PERSISTENT( MachLogFollowOperationImpl );

MachLogFollowOperationImpl::MachLogFollowOperationImpl( MachLogMachine* pActor, 
							MachLogMachine* pTarget,
                            const MexPoint2d& offset, 
							MachLogFollowOperation::CamouflagedOp camoStatus,
                            MachLogFollowOperation::TerminateFlag terminateFlag )
:	pActor_( pActor ),
  	pTarget_( pTarget ),
  	offset_( offset ),
	camoStatus_( camoStatus ),
	terminateFlag_( terminateFlag )
{
    TEST_INVARIANT;
}

MachLogFollowOperationImpl::~MachLogFollowOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogFollowOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogFollowOperationImpl& t )
{

    o << "MachLogFollowOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogFollowOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogFollowOperationImpl& followOpImpl )
{
	ostr << followOpImpl.pActor_;
	ostr << followOpImpl.pTarget_;
	ostr << followOpImpl.offset_;
	ostr << followOpImpl.camoStatus_;
	ostr << followOpImpl.terminateFlag_;}

void perRead( PerIstream& istr, MachLogFollowOperationImpl& followOpImpl )
{
	istr >> followOpImpl.pActor_;
	istr >> followOpImpl.pTarget_;
	istr >> followOpImpl.offset_;
	istr >> followOpImpl.camoStatus_;
	istr >> followOpImpl.terminateFlag_;
}

MachLogFollowOperationImpl::MachLogFollowOperationImpl( PerConstructor )
{
}


/* End FOLLOWI.CPP ***************************************************/
