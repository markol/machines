/*
 * O P P I C K U I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/opputdwi.hpp"

#include "machlog/rescarr.hpp"

//#include "mathex/point2d.hpp"

PER_DEFINE_PERSISTENT( MachLogPutDownOperationImpl );

MachLogPutDownOperationImpl::MachLogPutDownOperationImpl( MachLogResourceCarrier * pActor )
:	pActor_( pActor ),
  	finished_( false )
{
    TEST_INVARIANT;
}

MachLogPutDownOperationImpl::~MachLogPutDownOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogPutDownOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogPutDownOperationImpl& t )
{

    o << "MachLogPutDownOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogPutDownOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogPutDownOperationImpl& putDownOpImpl )
{
	ostr << putDownOpImpl.pActor_;
	ostr << putDownOpImpl.destinationPadPosition_;
	ostr << putDownOpImpl.finished_;
}

void perRead( PerIstream& istr, MachLogPutDownOperationImpl& putDownOpImpl )
{
	istr >> putDownOpImpl.pActor_;
	istr >> putDownOpImpl.destinationPadPosition_;
	istr >> putDownOpImpl.finished_;
}

MachLogPutDownOperationImpl::MachLogPutDownOperationImpl( PerConstructor )
{
}



/* End OPPUTDWI.CPP ***************************************************/
