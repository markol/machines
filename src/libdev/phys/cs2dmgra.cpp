/*
 * C S 2 D M G R A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/internal/cs2dmgra.hpp"

//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dDomainGraph::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const PhysCS2dDomainGraph& t )
{

    o << "PhysCS2dDomainGraph " << (void*)&t << " start" << std::endl;
    o << "PhysCS2dDomain graph end" << std::endl;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
void PhysCS2dDomainGraph::doRemoveVertex( const VertexId& id )
{
    //Do base processing
    GraGraph::doRemoveVertex( id );

    //free the id
    domainVertexIdGenerator_.free( id );
}

//virtual
void PhysCS2dDomainGraph::doRemoveArc( const ArcId& id )
{
    //Do base processing
    GraGraph::doRemoveArc( id );

    //free the id
    domainArcIdGenerator_.free( id );
}

/* End CS2DMGRA.CPP *************************************************/
