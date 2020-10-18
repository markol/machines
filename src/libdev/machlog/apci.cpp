/*
 * A P C I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/apci.hpp"

PER_DEFINE_PERSISTENT( MachLogAPCImpl );

MachLogAPCImpl::MachLogAPCImpl()
:   amountCarried_( 0 ),
	nMachinesCarried_( 0 ),
	nMachinesIncoming_( 0 ),
	turningTowardMachine_( false ),
	nDeployOps_( 0 ),
	dropzonePosition_( MexPoint3d( 0,0,0 ) )		// dummy value until set
{
	carriedMachines_.reserve( 20 );
	incomingMachines_.reserve( 20 );
    TEST_INVARIANT;
}

MachLogAPCImpl::~MachLogAPCImpl()
{
    TEST_INVARIANT;

}

void MachLogAPCImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogAPCImpl& t )
{

    o << "MachLogAPCImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogAPCImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogAPCImpl& actorImpl )
{
	ostr << actorImpl.nMachinesCarried_;
	ostr << actorImpl.nMachinesIncoming_;
	ostr << (int&)actorImpl.amountCarried_;
	ostr << actorImpl.carriedMachines_;
	ostr << actorImpl.incomingMachines_;
	ostr << actorImpl.turningTowardMachine_;	
	ostr << actorImpl.nDeployOps_;	
	ostr << actorImpl.dropzonePosition_;		
}

void perRead( PerIstream& istr, MachLogAPCImpl& actorImpl )
{
	istr >> actorImpl.nMachinesCarried_;
	istr >> actorImpl.nMachinesIncoming_;
	istr >> (int&)actorImpl.amountCarried_;
	istr >> actorImpl.carriedMachines_;
	istr >> actorImpl.incomingMachines_;
	istr >> actorImpl.turningTowardMachine_;
	istr >> actorImpl.nDeployOps_;
	istr >> actorImpl.dropzonePosition_;
		
}



/* End APCI.CPP *****************************************************/
