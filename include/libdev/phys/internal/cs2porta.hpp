/*
 * C S 2 P O R T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysCS2dPortal

    Represents a Portal added to a PhysConfigSpace2d.
*/

#ifndef _PHYS_CS2PORTA_HPP
#define _PHYS_CS2PORTA_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point2d.hpp"

#include "phys/cspace2.hpp"
#include "ctl/vector.hpp"

//Forward refs
class PhysCS2dDomainGraph;

//orthodox canonical (revoked )
class PhysCS2dPortal
{
public:
    //ctor. Links domains withd ids domainId1/2 via the line
    //joining endPoint1/2.
    PhysCS2dPortal( const PhysConfigSpace2d::DomainId& domainId1,
                    const PhysConfigSpace2d::DomainId& domainId2,
                    const MexPoint2d& endPoint1,
                    const MexPoint2d& endPoint2 );

    //dtor.
    ~PhysCS2dPortal( void );

    //Data access
    PhysConfigSpace2d::DomainId domainId1( void ) const;
    PhysConfigSpace2d::DomainId domainId2( void ) const;
    const MexPoint2d& endPoint1( void ) const;
    const MexPoint2d& endPoint2( void ) const;
    MATHEX_SCALAR length( void ) const;

    //The point proportionalDistance of the way from endPoint1() to endPoint2()
    MexPoint2d proportionalPoint( MATHEX_SCALAR proportionalDistance ) const;

    //////////////////////////////////////////////
    //DomainVertexs on the portal

    typedef PhysConfigSpace2d::ObstacleFlags ObstacleFlags;

    //Add a DomainVertex on this portal distance from the first endpoint.
    //The vertex is to have id domainVertexId.
    //This portal's id is portalId.
    //The vertex is to be added to pDomainGraph.
    //The vertex will not be added if this portal
    //already has a vertex in the same position, with the same
    //clearance and with the flags set to zero (i.e. takes 
    //account of all obstacles)
    //Returns true iff the vertex was added
    bool addDomainVertex( const PhysConfigSpace2d::DomainVertexId& domainVertexId,
                          MATHEX_SCALAR distance,
                          MATHEX_SCALAR clearance,
                          const PhysConfigSpace2d::PortalId& portalId,
                          PhysCS2dDomainGraph* pDomainGraph,
                          ObstacleFlags flags );
    //PRE( nDomainVertexs() < 2 );
    //PRE( not pDomainGraph->containsVertex( domainVertexId ) );
    //PRE( distance >= 0.0 and distance <= length() );

    //Number registered
    size_t nDomainVertexs( void ) const;

    //Id of ith
    const PhysConfigSpace2d::DomainVertexId& domainVertexId( size_t i ) const;
    //PRE( i < nDomainVertexs() );

    //True if the vertex with id domainVertexId is registered with this portal
    bool containsDomainVertex( const PhysConfigSpace2d::DomainVertexId& domainVertexId ) const;

    //Deletes the vertex with id domainVertexId. Also removes from pDomainGraph.
    void removeDomainVertex( PhysConfigSpace2d::DomainVertexId domainVertexId,
                             PhysCS2dDomainGraph* pDomainGraph );
    //PRE( containsDomainVertex( domainVertexId ) );
    //PRE( pDomainGraph->containsVertex( domainVertexId ) );
    //////////////////////////////////////////////

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysCS2dPortal& t );

private:
    // Operation deliberately revoked
    PhysCS2dPortal( const PhysCS2dPortal& );
    PhysCS2dPortal& operator =( const PhysCS2dPortal& );
    bool operator ==( const PhysCS2dPortal& );

    //data members
    PhysConfigSpace2d::DomainId domainId1_;//Domains linked by the portal
    PhysConfigSpace2d::DomainId domainId2_;
    MexPoint2d endPoint1_;//The end points of the portal line
    MexPoint2d endPoint2_;

    typedef ctl_vector< PhysConfigSpace2d::DomainVertexId > DomainVertexIds;

    //  On portal domain graph nodes    
    DomainVertexIds domainVertexIds_;
    
    //Cached data members
    MATHEX_SCALAR length_; //Distance between endpoints

};

#ifdef _INLINE
    #include "internal/cs2porta.ipp"
#endif


#endif

/* End CS2PORTA.HPP *************************************************/
