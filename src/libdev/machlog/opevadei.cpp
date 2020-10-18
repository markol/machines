/*
 * O P E V A D E I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/opevadei.hpp"

#include "sim/manager.hpp"

#include "machlog/actor.hpp"
#include "machlog/machine.hpp"


#include "mathex/point2d.hpp"

PER_DEFINE_PERSISTENT( MachLogEvadeOperationImpl );

MachLogEvadeOperationImpl::MachLogEvadeOperationImpl(MachLogMachine * pActor )
:	pActor_( pActor ),
	finished_( false ),
	currentEvadeOpType_( MachLogEvadeOperation::SEEK_SAFER_GROUND ),
	pCachedOperation_( NULL )
{
	lastTimeIHadStrongThreat_ = SimManager::instance().currentTime();
    TEST_INVARIANT;
}

MachLogEvadeOperationImpl::~MachLogEvadeOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogEvadeOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogEvadeOperationImpl& t )
{

    o << "MachLogEvadeOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogEvadeOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogEvadeOperationImpl& evadeOpImpl )
{
	ostr << evadeOpImpl.pActor_;
	ostr << evadeOpImpl.finished_;
	ostr << evadeOpImpl.pCachedOperation_;
	ostr << evadeOpImpl.currentEvadeOpType_;
	ostr << evadeOpImpl.lastTimeIHadStrongThreat_;
														
}

void perRead( PerIstream& istr, MachLogEvadeOperationImpl& evadeOpImpl )
{
	istr >> evadeOpImpl.pActor_;
	istr >> evadeOpImpl.finished_;
	istr >> evadeOpImpl.pCachedOperation_;
	istr >> evadeOpImpl.currentEvadeOpType_;
	istr >> evadeOpImpl.lastTimeIHadStrongThreat_;													
}

MachLogEvadeOperationImpl::MachLogEvadeOperationImpl( PerConstructor )
{
}


/* End OPEVADEI.CPP ***************************************************/
