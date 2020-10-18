/*
 * O P D A P C I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/opdapci.hpp"

#include "machlog/apc.hpp"

//#include "mathex/point2d.hpp"

PER_DEFINE_PERSISTENT( MachLogDeployAPCOperationImpl );

MachLogDeployAPCOperationImpl::MachLogDeployAPCOperationImpl( MachLogAPC * pAPC )
:	pAPC_( pAPC ),
	finished_( false ),
	status_( MachLogDeployAPCOperation::GOTO_DESTINATION )
{
    TEST_INVARIANT;
}

MachLogDeployAPCOperationImpl::~MachLogDeployAPCOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogDeployAPCOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogDeployAPCOperationImpl& t )
{

    o << "MachLogDeployAPCOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogDeployAPCOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogDeployAPCOperationImpl& deployAPCOpImpl )
{
	ostr << deployAPCOpImpl.pAPC_;
	ostr << deployAPCOpImpl.finished_;
	ostr << deployAPCOpImpl.status_;
}

void perRead( PerIstream& istr, MachLogDeployAPCOperationImpl& deployAPCOpImpl )
{
	istr >> deployAPCOpImpl.pAPC_;
	istr >> deployAPCOpImpl.finished_;
	istr >> deployAPCOpImpl.status_;
}

MachLogDeployAPCOperationImpl::MachLogDeployAPCOperationImpl( PerConstructor )
{
}



/* End OPDAPCI.CPP ***************************************************/
