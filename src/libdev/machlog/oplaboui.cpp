/*
 * O P L A B O U I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/oplaboui.hpp"

#include "sim/manager.hpp"

#include "machlog/construc.hpp"
#include "machlog/constron.hpp"
#include "machlog/buildpnt.hpp"

//#include "mathex/point2d.hpp"

PER_DEFINE_PERSISTENT( MachLogLabourOperationImpl );


MachLogLabourOperationImpl::MachLogLabourOperationImpl( MachLogConstructor * pActor, MachLogConstruction * pConstr )
:	pActor_( pActor ),
  pConstruction_( pConstr ),
  state_( MachLogLabourOperation::MOVING ),
  lastUpdateTime_( SimManager::instance().currentTime() ),
  hasBuildPoint_( false ),
  currentlyAttached_( true ),
  pBuildPoint_( NULL )
{
    TEST_INVARIANT;
}

MachLogLabourOperationImpl::~MachLogLabourOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogLabourOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogLabourOperationImpl& t )
{

    o << "MachLogLabourOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogLabourOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogLabourOperationImpl& labourOpImpl )
{
	ostr << labourOpImpl.pActor_;
	ostr << labourOpImpl.pConstruction_;
	ostr << labourOpImpl.state_;
	ostr << labourOpImpl.lastUpdateTime_;
	ostr << labourOpImpl.currentlyAttached_;	
	ostr << labourOpImpl.pBuildPoint_;	
	ostr << labourOpImpl.hasBuildPoint_;															
}

void perRead( PerIstream& istr, MachLogLabourOperationImpl& labourOpImpl )
{
	istr >> labourOpImpl.pActor_;
	istr >> labourOpImpl.pConstruction_;
	istr >> labourOpImpl.state_;
	istr >> labourOpImpl.lastUpdateTime_;
	istr >> labourOpImpl.currentlyAttached_;	
	istr >> labourOpImpl.pBuildPoint_;
	istr >> labourOpImpl.hasBuildPoint_;																													
}

MachLogLabourOperationImpl::MachLogLabourOperationImpl( PerConstructor )
{
}


/* End OPLABOUI.CPP ***************************************************/
