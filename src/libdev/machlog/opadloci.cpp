/*
 * O P A D L O C I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/opadloci.hpp"

#include "machlog/administ.hpp"

//#include "mathex/point2d.hpp"

PER_DEFINE_PERSISTENT( MachLogAdminLocateOperationImpl );

MachLogAdminLocateOperationImpl::MachLogAdminLocateOperationImpl( MachLogAdministrator * pActor, const MexPoint3d& dest )
:	pActor_( pActor ),
	dest_( dest ),
	hasExternalPath_( false ),
	currentElement_( 0 ),
	complete_( false )
{
    TEST_INVARIANT;
}

MachLogAdminLocateOperationImpl::MachLogAdminLocateOperationImpl( MachLogAdministrator * pActor )
:	pActor_( pActor ),
	hasExternalPath_( false ),
	currentElement_( 0 ),
	complete_( false )
{
	TEST_INVARIANT;
}

MachLogAdminLocateOperationImpl::MachLogAdminLocateOperationImpl( MachLogAdministrator * pActor,  const MachLogAdminLocateOperation::Path& externalPath )
:	pActor_( pActor ),
	externalPath_( externalPath ),
	hasExternalPath_( true ),
	currentElement_( 0 ),
	complete_( false )
{
	TEST_INVARIANT;
}


MachLogAdminLocateOperationImpl::~MachLogAdminLocateOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogAdminLocateOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogAdminLocateOperationImpl& t )
{

    o << "MachLogAdminLocateOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogAdminLocateOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogAdminLocateOperationImpl& adminLocateOpImpl )
{
	ostr << adminLocateOpImpl.pActor_;
    ostr << adminLocateOpImpl.complete_;
	ostr << adminLocateOpImpl.dest_;
	ostr << adminLocateOpImpl.path_;
	ostr << adminLocateOpImpl.externalPath_;
	ostr << adminLocateOpImpl.hasExternalPath_;
	ostr << adminLocateOpImpl.currentElement_;
}

void perRead( PerIstream& istr, MachLogAdminLocateOperationImpl& adminLocateOpImpl )
{
	istr >> adminLocateOpImpl.pActor_;
    istr >> adminLocateOpImpl.complete_;
	istr >> adminLocateOpImpl.dest_;
	istr >> adminLocateOpImpl.path_;
	istr >> adminLocateOpImpl.externalPath_;
	istr >> adminLocateOpImpl.hasExternalPath_;
	istr >> adminLocateOpImpl.currentElement_;
}

MachLogAdminLocateOperationImpl::MachLogAdminLocateOperationImpl( PerConstructor )
{
}



/* End OPADLOCI.CPP ***************************************************/
