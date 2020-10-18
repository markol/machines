/*
 * O P A D M O V I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/opadmovi.hpp"

#include "machlog/administ.hpp"

//#include "mathex/point2d.hpp"

PER_DEFINE_PERSISTENT( MachLogAdminMoveToOperationImpl );

MachLogAdminMoveToOperationImpl::MachLogAdminMoveToOperationImpl( MachLogAdministrator * pActor, const MexPoint3d& dest, bool checkForLeave, MachLogFollowOperation::CamouflagedOp camoStatus )
: 	pActor_( pActor ),
  	dest_( dest ),
  	checkForLeave_( checkForLeave ),
	complete_( false ),
	camoStatus_( camoStatus )
{
    TEST_INVARIANT;
}

MachLogAdminMoveToOperationImpl::MachLogAdminMoveToOperationImpl( MachLogAdministrator* pActor, const MachLogAdminMoveToOperation::Path& path, bool checkForLeave, MachLogFollowOperation::CamouflagedOp camoStatus )
: 	pActor_( pActor ),
	path_( path ),
	checkForLeave_( checkForLeave ),
	complete_( false ),
	camoStatus_( camoStatus )
{
    TEST_INVARIANT;
}


MachLogAdminMoveToOperationImpl::~MachLogAdminMoveToOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogAdminMoveToOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogAdminMoveToOperationImpl& t )
{

    o << "MachLogAdminMoveToOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogAdminMoveToOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogAdminMoveToOperationImpl& adminMoveToOpImpl )
{
	ostr << adminMoveToOpImpl.pActor_;
    ostr << adminMoveToOpImpl.complete_;
	ostr << adminMoveToOpImpl.dest_;
	ostr << adminMoveToOpImpl.checkForLeave_;
	ostr << adminMoveToOpImpl.path_;

}

void perRead( PerIstream& istr, MachLogAdminMoveToOperationImpl& adminMoveToOpImpl )
{
	istr >> adminMoveToOpImpl.pActor_;
    istr >> adminMoveToOpImpl.complete_;
	istr >> adminMoveToOpImpl.dest_;
	istr >> adminMoveToOpImpl.checkForLeave_;
	istr >> adminMoveToOpImpl.path_;
}

MachLogAdminMoveToOperationImpl::MachLogAdminMoveToOperationImpl( PerConstructor )
{
}



/* End OPADMOVI.CPP ***************************************************/
