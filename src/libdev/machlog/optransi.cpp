/*
 * O P T R A N S I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/optransi.hpp"

//#include "machlog/actor.hpp"
#include "machlog/rescarr.hpp"


//#include "mathex/point2d.hpp"

PER_DEFINE_PERSISTENT( MachLogTransportOperationImpl );

MachLogTransportOperationImpl::MachLogTransportOperationImpl( MachLogResourceCarrier * pActor )
:	pActor_( pActor ),
  	finished_( false )
{
    TEST_INVARIANT;
}

MachLogTransportOperationImpl::~MachLogTransportOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogTransportOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogTransportOperationImpl& t )
{

    o << "MachLogTransportOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogTransportOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogTransportOperationImpl& transportOpImpl )
{
	ostr << transportOpImpl.pActor_;
	ostr << transportOpImpl.finished_;
	ostr << transportOpImpl.donePickUp_;
}

void perRead( PerIstream& istr, MachLogTransportOperationImpl& transportOpImpl )
{
	istr >> transportOpImpl.pActor_;
	istr >> transportOpImpl.finished_;
	istr >> transportOpImpl.donePickUp_;
}

MachLogTransportOperationImpl::MachLogTransportOperationImpl( PerConstructor )
{
}


/* End OPTRANSI.CPP ***************************************************/
