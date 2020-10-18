/*
 * C S 2 Q W R I T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/cs2qwrit.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/point2d.hpp"
#include "phys/cspace2.hpp"
#include "phys/internal/cs2impl.hpp"
#include "phys/internal/cs2dmfnd.hpp"
#include "phys/internal/cs2findp.hpp"

PhysPathFindingQueueWriter::PhysPathFindingQueueWriter( const PhysConfigSpace2d& configSpace )
: configSpace_( configSpace )
{

    TEST_INVARIANT;
}

PhysPathFindingQueueWriter::~PhysPathFindingQueueWriter()
{
    TEST_INVARIANT;

}

void PhysPathFindingQueueWriter::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PhysPathFindingQueueWriter& t )
{
    const PhysCS2dImpl::DomainFindPaths& domainFindPathQueue = t.configSpace_.impl().domainFindPathQueue_;

    o << "Domain find path queue length " << domainFindPathQueue.size() << std::endl;

    for( PhysCS2dImpl::DomainFindPaths::const_iterator i = domainFindPathQueue.begin();
      i != domainFindPathQueue.end(); ++i )
    {
        o << "  " << (*(*i)) << std::endl;
    }

    const PhysCS2dImpl::FindPaths& findPathQueue = t.configSpace_.impl().findPathQueue_;

    o << "Find path queue length " << findPathQueue.size() << std::endl;

    for( PhysCS2dImpl::FindPaths::const_iterator i = findPathQueue.begin();
      i != findPathQueue.end(); ++i )
    {
        o << "  " << (*(*i)) << std::endl;
    }

    return o;
}

/* End QWRITE.CPP ***************************************************/
