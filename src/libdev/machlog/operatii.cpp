/*
 * O P E R A T I I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/operatii.hpp"

#include "sim/manager.hpp"


PER_DEFINE_PERSISTENT( MachLogOperationImpl );

MachLogOperationImpl::MachLogOperationImpl(
   const char* typeOp,
   MachLogOperation::OperationType typeOpEnum,
   PhysPathFindingPriority pathFindingPriority )

:	progress_( MachLogOperation::START ),
  	priority_( MachLogOperation::NORMAL_PRIORITY ),
  	pSubOperation_( NULL ),
  	nextCallBackTime_( SimManager::instance().currentTime() ),
  	operationTypeAsString_( typeOp ),
  	operationType_( typeOpEnum ),
    pathFindingPriority_( pathFindingPriority )
{
    TEST_INVARIANT;
}

MachLogOperationImpl::~MachLogOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogOperationImpl& t )
{

    o << "MachLogOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogOperationImpl& opImpl )
{
	ostr << opImpl.nextCallBackTime_;
	ostr << opImpl.pSubOperation_;
	ostr << opImpl.operationTypeAsString_;
	ostr << opImpl.progress_;
	ostr << opImpl.priority_;
	ostr << opImpl.operationType_;
	ostr << opImpl.groupMoveInfo_;
}

void perRead( PerIstream& istr, MachLogOperationImpl& opImpl )
{
	istr >> opImpl.nextCallBackTime_;
	istr >> opImpl.pSubOperation_;
	istr >> opImpl.operationTypeAsString_;
	istr >> opImpl.progress_;
	istr >> opImpl.priority_;
	istr >> opImpl.operationType_;
	istr >> opImpl.groupMoveInfo_;
}

MachLogOperationImpl::MachLogOperationImpl( PerConstructor )
{
}


/* End OPERATII.CPP ***************************************************/
