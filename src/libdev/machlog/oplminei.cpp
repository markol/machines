/*
 * O P L M I N E I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/oplminei.hpp"

#include "machlog/spy.hpp"

//#include "mathex/point2d.hpp"

PER_DEFINE_PERSISTENT( MachLogDropLandMineOperationImpl );

MachLogDropLandMineOperationImpl::MachLogDropLandMineOperationImpl( MachLogSpyLocator * pActor, const MexPoint3d& dest )
:	pActor_( pActor ),
	dest_( dest ),
	failedAttempts_( 0 ),
	moved_( false )
{
    TEST_INVARIANT;
}

MachLogDropLandMineOperationImpl::MachLogDropLandMineOperationImpl( MachLogSpyLocator * pActor,  const MachLogDropLandMineOperation::Path& externalPath )
:	pActor_( pActor ),
	failedAttempts_( 0 ),
	path_( externalPath ),
	moved_( false )
{
    TEST_INVARIANT;
}

MachLogDropLandMineOperationImpl::~MachLogDropLandMineOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogDropLandMineOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogDropLandMineOperationImpl& t )
{

    o << "MachLogDropLandMineOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogDropLandMineOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogDropLandMineOperationImpl& landMineOpImpl )
{
	ostr << landMineOpImpl.pActor_;
	ostr << landMineOpImpl.dest_;
	ostr << landMineOpImpl.path_;
	ostr << landMineOpImpl.failedAttempts_;
	ostr << landMineOpImpl.moved_;
}

void perRead( PerIstream& istr, MachLogDropLandMineOperationImpl& landMineOpImpl )
{
	istr >> landMineOpImpl.pActor_;
	istr >> landMineOpImpl.dest_;
	istr >> landMineOpImpl.path_;
	istr >> landMineOpImpl.failedAttempts_;
	istr >> landMineOpImpl.moved_;
}

MachLogDropLandMineOperationImpl::MachLogDropLandMineOperationImpl( PerConstructor )
{
}



/* End OPLMINEI.CPP ***************************************************/
