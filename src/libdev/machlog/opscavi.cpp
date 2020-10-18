/*
 * O P S C A V I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/opscavi.hpp"

#include "machlog/rescarr.hpp"

#include "machlog/debris.hpp"

//#include "mathex/point2d.hpp"

PER_DEFINE_PERSISTENT( MachLogScavengeOperationImpl );

MachLogScavengeOperationImpl::MachLogScavengeOperationImpl( MachLogResourceCarrier * pActor )
:	pActor_( pActor ),
  	finished_( false ),
	currentElement_( 0 ),
	dest_( pActor->position() )
{
    TEST_INVARIANT;
}

MachLogScavengeOperationImpl::~MachLogScavengeOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogScavengeOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogScavengeOperationImpl& t )
{

    o << "MachLogScavengeOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogScavengeOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogScavengeOperationImpl& scavengeOpImpl )
{
	ostr << scavengeOpImpl.pActor_;
	ostr << scavengeOpImpl.finished_;
	ostr << scavengeOpImpl.suppliers_;
	ostr << scavengeOpImpl.currentElement_;
	ostr << scavengeOpImpl.dest_;
}

void perRead( PerIstream& istr, MachLogScavengeOperationImpl& scavengeOpImpl )
{
	istr >> scavengeOpImpl.pActor_;
	istr >> scavengeOpImpl.finished_;
	istr >> scavengeOpImpl.suppliers_;
	istr >> scavengeOpImpl.currentElement_;
	istr >> scavengeOpImpl.dest_;
}

MachLogScavengeOperationImpl::MachLogScavengeOperationImpl( PerConstructor )
{
}



/* End OPSCAVI.CPP ***************************************************/
