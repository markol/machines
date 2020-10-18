/*
 * O P A U T S C I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/opautsci.hpp"

#include "machlog/debris.hpp"
#include "machlog/operatio.hpp"
#include "machlog/rescarr.hpp"

#include "mathex/point2d.hpp"


PER_DEFINE_PERSISTENT( MachLogAutoScavengeOperationImpl );

MachLogAutoScavengeOperationImpl::MachLogAutoScavengeOperationImpl( MachLogResourceCarrier* pScavenger, MachLogDebris* pDebris )
:	pScavenger_( pScavenger ),
	pDebris_( pDebris ),
	finished_( false ),
	initiatedScavengeOp_( false ),
	pCachedOperation_( NULL )
{
	PRE( pScavenger->isScavenger() );

    TEST_INVARIANT;
}

MachLogAutoScavengeOperationImpl::~MachLogAutoScavengeOperationImpl()
{
    TEST_INVARIANT;
}

void MachLogAutoScavengeOperationImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogAutoScavengeOperationImpl& t )
{

    o << "MachLogAutoScavengeOperationImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogAutoScavengeOperationImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogAutoScavengeOperationImpl& AutoScavengeOpImpl )
{
	ostr << AutoScavengeOpImpl.pScavenger_;
	ostr << AutoScavengeOpImpl.pDebris_;
	ostr << AutoScavengeOpImpl.finished_;
	ostr << AutoScavengeOpImpl.initiatedScavengeOp_;	
	ostr << AutoScavengeOpImpl.pCachedOperation_;		
}

void perRead( PerIstream& istr, MachLogAutoScavengeOperationImpl& AutoScavengeOpImpl )
{
	istr >> AutoScavengeOpImpl.pScavenger_;
	istr >> AutoScavengeOpImpl.pDebris_;
	istr >> AutoScavengeOpImpl.finished_;
	istr >> AutoScavengeOpImpl.initiatedScavengeOp_;	
	istr >> AutoScavengeOpImpl.pCachedOperation_;													
}

MachLogAutoScavengeOperationImpl::MachLogAutoScavengeOperationImpl( PerConstructor )
{
}


/* End OPAUTSCI.CPP ***************************************************/
