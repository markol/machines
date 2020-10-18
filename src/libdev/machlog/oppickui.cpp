/*
 * O P P I C K U I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/oppickui.hpp"

#include "machlog/rescarr.hpp"

//#include "mathex/point2d.hpp"

PER_DEFINE_PERSISTENT( MachLogPickUpOperationImpl );

MachLogPickUpOperationImpl::MachLogPickUpOperationImpl( MachLogResourceCarrier * pActor )
:	pActor_( pActor ),
  	finished_( false ),
	destinationPadPosition_( pActor->position() )			// dummy assignment
{
    TEST_INVARIANT;
}

MachLogPickUpOperationImpl::~MachLogPickUpOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogPickUpOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogPickUpOperationImpl& t )
{

    o << "MachLogPickUpOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogPickUpOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogPickUpOperationImpl& pickUpOpImpl )
{
	ostr << pickUpOpImpl.pActor_;
	ostr << pickUpOpImpl.finished_;
	ostr << pickUpOpImpl.destinationPadPosition_;
}

void perRead( PerIstream& istr, MachLogPickUpOperationImpl& pickUpOpImpl )
{
	istr >> pickUpOpImpl.pActor_;
	istr >> pickUpOpImpl.finished_;
	istr >> pickUpOpImpl.destinationPadPosition_;
}

MachLogPickUpOperationImpl::MachLogPickUpOperationImpl( PerConstructor )
{
}



/* End OPPICKUI.CPP ***************************************************/
