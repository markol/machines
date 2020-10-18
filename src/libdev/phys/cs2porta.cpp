/*
 * C S 2 P O R T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/internal/cs2porta.hpp"
#include "phys/internal/cs2dmgra.hpp"
#include "ctl/erase.hpp"

#ifndef _INLINE
    #include "phys/internal/cs2porta.ipp"
#endif

//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dPortal::PhysCS2dPortal( const PhysConfigSpace2d::DomainId& domainId1,
                    const PhysConfigSpace2d::DomainId& domainId2,
                    const MexPoint2d& endPoint1,
                    const MexPoint2d& endPoint2 )
:   domainId1_( domainId1 ),
    domainId2_( domainId2 ),
    endPoint1_( endPoint1 ),
    endPoint2_( endPoint2 )
{
    length_ = endPoint1.euclidianDistance( endPoint2 );
    domainVertexIds_.reserve( 2 );

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dPortal::~PhysCS2dPortal()
{
    TEST_INVARIANT;

}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dPortal::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const PhysCS2dPortal& t )
{

    o << "PhysCS2dPortal " << (void*)&t <<
         "  Domains " << t.domainId1_.asScalar() << " " << t.domainId2_.asScalar() <<
         "  points " << t.endPoint1_ << " to " << t.endPoint2_ << std::endl;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dPortal::addDomainVertex
(
    const PhysConfigSpace2d::DomainVertexId& domainVertexId,
    MATHEX_SCALAR distance,
    MATHEX_SCALAR clearance,
    const PhysConfigSpace2d::PortalId& portalId,
    PhysCS2dDomainGraph* pDomainGraph,
    ObstacleFlags flags
)
{
    PRE( not pDomainGraph->containsVertex( domainVertexId ) );
    PRE( distance >= 0.0 and distance <= length() );

    const MATHEX_SCALAR proportionalDistance = distance / length();
    const MATHEX_SCALAR x = endPoint1_.x() * (1 - proportionalDistance) +
                      endPoint2_.x() *  proportionalDistance;
    const MATHEX_SCALAR y = endPoint1_.y() * (1 - proportionalDistance) +
                      endPoint2_.y() *  proportionalDistance;

    const MexPoint2d position( x, y );

    //  If this matches an existing vertex that has a flags setting of
    //  zero - i.e. takes account of all obstacles - we don't need to
    //  add this vertex, we already have a suitable vertex and set of arcs.

    bool    addVertex = true;
    for( DomainVertexIds::iterator i = domainVertexIds_.begin();
      i != domainVertexIds_.end() and addVertex; ++i )
    {
        const PhysCS2dDomainVertex& domainVertex = pDomainGraph->vertex( *i );

        if( domainVertex.point() == position and
            domainVertex.clearance() == clearance and
            domainVertex.flags() == 0 )
        {
            addVertex = false;
        }
    }

    if( addVertex )
    {
        // Enter the vertex into the graph

        pDomainGraph->addVertex(
          domainVertexId,
          PhysCS2dDomainVertex( position,
          clearance, flags, portalId ) );

        domainVertexIds_.push_back( domainVertexId );

        const PhysCS2dDomainVertex& domainVertex = pDomainGraph->vertex( domainVertexId );

        CONFIG_SPACE_STREAM( "Add domain vertex " << domainVertex << std::endl );
    }

    return addVertex;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dPortal::removeDomainVertex( PhysConfigSpace2d::DomainVertexId domainVertexId,
                         PhysCS2dDomainGraph* pDomainGraph )
{
    //PRE( nDomainVertexs_ == 0  or  pDomainGraph->containsVertex( domainVertexId1_ ) );
    //PRE( nDomainVertexs_ == 1  or  pDomainGraph->containsVertex( domainVertexId2_ ) );
    PRE( containsDomainVertex( domainVertexId ) );
    PRE( pDomainGraph->containsVertex( domainVertexId ) );

    //Remove from the graph
    pDomainGraph->removeVertex( domainVertexId );

    ctl_erase_first_instance( &domainVertexIds_, domainVertexId );
}
//////////////////////////////////////////////////////////////////////////////////////////

MexPoint2d PhysCS2dPortal::proportionalPoint( MATHEX_SCALAR f ) const
{
    MATHEX_SCALAR g = 1.0 - f;
    return MexPoint2d( endPoint1_.x() * g + endPoint2_.x() * f,
                       endPoint1_.y() * g + endPoint2_.y() * f );
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End CS2PORTA.CPP *************************************************/
