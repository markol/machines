/*
 * C S 2 D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/poly2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/cvexpgon.hpp"
#include "mathex/sausag2d.hpp"
#include "mathex/circle2d.hpp"
#include "phys/internal/cs2impl.hpp"
#include "phys/internal/cs2vigra.hpp"
#include "phys/internal/cs2expsp.hpp"
#include "phys/internal/cs2dmgra.hpp"
#include "phys/internal/cs2dmfnd.hpp"

#include "utility/indent.hpp"

#ifndef _INLINE
    #include "phys/internal/cs2impl.ipp"
#endif

//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dImpl::PhysCS2dImpl( PhysConfigSpace2d* pConfigSpace,
                            const MexAlignedBox2d& boundary,
                            const PhysConfigSpace2d::Mode& newMode,
                            MATHEX_SCALAR domainMinResolution,
                            size_t nDomainMaxItems,
                            MATHEX_SCALAR domainLipSize,
                            MATHEX_SCALAR polygonMinResolution,
                            size_t nPolygonMaxItems,
                            MATHEX_SCALAR polygonLipSize,
                            MATHEX_SCALAR domainClearance,
                            MATHEX_SCALAR minPortalClearance,
                            MATHEX_SCALAR maxPortalClearance,
                            ObstacleFlags allObstacleFlags )
:   configSpace_( *pConfigSpace ),
    boundary_( boundary ),
    mode_( newMode ),
    domains_( 16 ),
    portals_( 16 ),
    polygons_( 16 ),
    findPaths_( 16 ),
    domainFindPaths_( 16 ),
    nDomainGraphClients_( 0 ),
    nPolygons_( 0 ),
    pVisibilityGraph_( NULL ),
    pExpansionSpace_( NULL ),
    pActiveDomainFindPath_( NULL ),
    pActiveFindPath_( NULL ),
    domainClearance_( domainClearance ),
    domainTree_( boundary.minCorner(), boundary.maxCorner(),
                 domainMinResolution, nDomainMaxItems, domainLipSize ),
    permanentPolygonTree_( boundary.minCorner(), boundary.maxCorner(),
                 polygonMinResolution, nPolygonMaxItems, polygonLipSize ),
    temporaryPolygonTree_( boundary.minCorner(), boundary.maxCorner(),
                 polygonMinResolution, nPolygonMaxItems, polygonLipSize ),
    motionChunkTree_( boundary.minCorner(), boundary.maxCorner(),
                 polygonMinResolution, nPolygonMaxItems, polygonLipSize ),
    motionChunks_( 16 ),
    domainRegionOutOfDate_( true ),
    domainUpdateRegion_( boundary ),
    minPortalClearance_( minPortalClearance ),
    maxPortalClearance_( maxPortalClearance ),
    allFlags_( allObstacleFlags ),
    hasCurrentDomainFindPath_( false ),
    hasCurrentFindPath_( false ),
    pCurrentDomainFindPath_( NULL ),
    pCurrentFindPath_( NULL ),
    bufferPolygonPortalUpdates_( false )
{
    findPathQueue_.reserve( 32 );
    domainFindPathQueue_.reserve( 32 );

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dImpl::~PhysCS2dImpl()
{
    TEST_INVARIANT;

    //Delete all the domains
    for( PhysConfigSpace2d::DomainId id = domainIdGenerator_.begin();
         id != domainIdGenerator_.end(); ++id )
    {
        if( domains_.contains( id ) )
            _DELETE( domains_[ id ] );
    }

    //Delete all the portals
    for( PhysConfigSpace2d::PortalId id = portalIdGenerator_.begin();
         id != portalIdGenerator_.end(); ++id )
    {
        if( portals_.contains( id ) )
            _DELETE( portals_[ id ] );
    }

    //Delete all the polygons
    for( PhysConfigSpace2d::PolygonId id = polygonIdGenerator_.begin();
         id != polygonIdGenerator_.end(); ++id )
    {
        if( polygons_.contains( id ) )
            _DELETE( polygons_[ id ] );
    }

    //Delete all the motion chunks
    for( PhysConfigSpace2d::MotionChunkId id = motionChunkIdGenerator_.begin();
         id != motionChunkIdGenerator_.end(); ++id )
    {
        if( motionChunks_.contains( id ) )
            _DELETE( motionChunks_[ id ] );
    }

    //Delete other items
    if( pVisibilityGraph_ != NULL ) _DELETE( pVisibilityGraph_ );
    if( pExpansionSpace_ != NULL ) _DELETE( pExpansionSpace_ );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dImpl::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dImpl::add( PhysCS2dDomain* pDomain )
{
    //Add the domain to the collection
    domains_.add( DomainMap::KeyValueAssoc( pDomain->id(), pDomain ) );

    //Add to the BSP tree of domains
    domainTree_.add( pDomain );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const PhysCS2dImpl& t )
{
    UtlIndentOstream    indentOstr( o, "  " );

    uint nDomains = 0;

    //The domains
    for( PhysConfigSpace2d::DomainId id = t.domainIdGenerator_.begin();
         id != t.domainIdGenerator_.end(); ++id )
    {
        if( t.domains_.contains( id ) )
        {
            PhysCS2dDomain* pDomain = t.domains_[ id ];
            o << "Domain id " << id.asScalar() << std::endl;
            indentOstr << *pDomain;
            o << "End domain" << std::endl;

            ++nDomains;
        }
    }

    o << "# domains " << nDomains << std::endl;

    o << "Domain tree start" << std::endl;
    indentOstr << t.domainTree_;
    o << "Domain tree end" << std::endl;

    uint nPortals = 0;

    //The portals
    for( PhysConfigSpace2d::PortalId id = t.portalIdGenerator_.begin();
         id != t.portalIdGenerator_.end(); ++id )
    {
        if( t.portals_.contains( id ) )
        {
            PhysCS2dPortal* pPortal = t.portals_[ id ];
            o << "Portal id " << id.asScalar() << std::endl;
            indentOstr << *pPortal;
            o << "End portal" << std::endl;

            ++nPortals;
        }
    }

    o << "# portals " << nPortals << std::endl;

    //The domain graph
    t.trace( o, t.domainGraph_ );

    //The polygon BSP trees
    o << "Permanent Polygon BSP tree start" << std::endl;
    indentOstr << t.permanentPolygonTree_;
    o << "Polygon BSP tree end" << std::endl;

    o << "Temporary Polygon BSP tree start" << std::endl;
    indentOstr << t.temporaryPolygonTree_;
    o << "Polygon BSP tree end" << std::endl;

    //The polygons
    for( PhysConfigSpace2d::PolygonId id = t.polygonIdGenerator_.begin();
         id != t.polygonIdGenerator_.end(); ++id )
    {
        if( t.polygons_.contains( id ) )
        {
            PhysCS2dPolygon* pPolygonHolder = t.polygons_[ id ];
            o << *pPolygonHolder;
        }
    }

    o << "Motion chunks start" << std::endl;

    //  TBD: Improve the way this works - we should be able to get
    //  at the motion chunks directly.
    const PhysCS2dImpl::MotionChunkTree& motionChunkTree = t.motionChunkTree_;
    PhysCS2dImpl::MotionChunkTree::PItems motionChunkHolders;
    motionChunkHolders.reserve( 64 );

    size_t n = motionChunkTree.overlapping( t.boundary_, &motionChunkHolders );

    //Check for intersections between the incoming chunk and each potentially
    //colliding existing chunk

    for( size_t i = 0; i != n; ++i)
    {
        const PhysCS2dMotionChunk& holder = *(motionChunkHolders[i]);

        o << holder.objectId() << "  " << holder.motionChunk() << std::endl;
    }

    o << "Motion chunks end" << std::endl;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dImpl::trace( ostream& o, const PhysCS2dDomainGraph& domainGraph ) const
{
    o << "PhysCS2dDomainGraph start" << std::endl;

    UtlIndentOstream    indentOstr( o, "  " );

    uint nDomainVertices = 0;

    for( PhysConfigSpace2d::DomainVertexId id = _CONST_CAST(PhysCS2dImpl*, this)->domainVertexIdGenerator().begin();
         id != _CONST_CAST(PhysCS2dImpl*, this)->domainVertexIdGenerator().end(); ++id )
    {
        if( domainGraph.containsVertex( id ) )
        {
            const PhysCS2dDomainVertex& domainVertex = domainGraph.vertex( id );
            const ctl_vector< PhysConfigSpace2d::DomainArcId >& arcs = domainGraph.arcs( id );

            indentOstr << "DomainVertex id " << id.asScalar() << " " << domainVertex << " ";

            size_t nArcs = arcs.size();
            if( nArcs != 0 )
            {
                indentOstr << " arcs";
                for( size_t i = 0; i < nArcs; ++i )
                    indentOstr << " " << arcs[i].asScalar();
            }

            ++nDomainVertices;

            indentOstr << std::endl;
        }
    }

    indentOstr << "# domain vertices ( portal points ) " << nDomainVertices << std::endl;

    uint nDomainArcs = 0;

    for( PhysConfigSpace2d::DomainArcId id = _CONST_CAST(PhysCS2dImpl*, this)->domainArcIdGenerator().begin();
         id != _CONST_CAST(PhysCS2dImpl*, this)->domainArcIdGenerator().end(); ++id )
    {
        if( domainGraph.containsArc( id ) )
        {
            const PhysCS2dDomainArc& domainArc = domainGraph.arc( id );

            indentOstr << "DomainArc id " << id.asScalar() <<
                 " Vertexs " << domainGraph.fromVertex( id ).asScalar() <<
                 " " << domainGraph.toVertex( id ).asScalar() <<
                 " Domain " << domainArc.domainId().asScalar() << std::endl;

            ++nDomainArcs;
        }
    }

    indentOstr << "# domain arcs " << nDomainArcs << std::endl;

    o << "PhysCS2dDomain graph end" << std::endl;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dImpl::contains( const MexPoint2d& testPoint, PhysConfigSpace2d::PolygonId* pId,
                             const PolygonTree& polygonTree, ObstacleFlags flags ) const
{
    //Set up a bounding box for the point, and get a vector of all the polygons in
    //the tree which might contain the point
    MexAlignedBox2d pointBoundary( testPoint, testPoint );
    PolygonTree::PItems polygonHolders;
    polygonHolders.reserve( 512 );
    size_t n = polygonTree.overlapping( pointBoundary, &polygonHolders );

    //Check for the point inside each polygon
    bool in = false;
    for( size_t i = 0; not in  and  i != n; ++i)
    {
        const PhysCS2dPolygon& holder = *(polygonHolders[i]);

        if( holder.isEnabled( flags ) and holder.polygon().contains( testPoint ) )
        {
            in = true;
            *pId = holder.id();
        }
    }

    return in;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dImpl::intersects( const MexPoint2d& p1, const MexPoint2d& p2,
                               MATHEX_SCALAR lineHeight,
                               PhysConfigSpace2d::PolygonId* pId,
                               const PolygonTree& polygonTree,
                               Mathex::TouchingRule rule,
                               ObstacleFlags flags ) const
{
    //Set up a bounding box for the line, and get a vector of all the polygons in
    //the tree which might intersect the line
    MexAlignedBox2d lineBoundary( p1, p2 );
    PolygonTree::PItems polygonHolders;
    polygonHolders.reserve( 512 );
    size_t n = polygonTree.overlapping( lineBoundary, &polygonHolders );

    //Decide if touching is ok, and if so compute the line length
    bool touchingOk = rule == Mathex::TOUCH_ISNT_INTERSECT;
    MATHEX_SCALAR lineLength = 0;
    if( touchingOk )
        lineLength = p1.euclidianDistance( p2 );

    //Check for the line intersects each polygon
    bool hit = false;
    for( size_t i = 0; not hit  and  i != n; ++i)
    {
        const PhysCS2dPolygon& holder = *(polygonHolders[i]);
        if( holder.isEnabled( flags ) and
            holder.height() >= lineHeight and
            holder.polygon().intersects( p1, p2 ) )
        {
            if( touchingOk )
                hit = holder.polygon().isPenetratedBy( p1, p2, lineLength );
            else
                hit = holder.polygon().intersects( p1, p2, rule );

            *pId = holder.id();
        }
    }

    return hit;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dImpl::intersects( const MexPolygon2d& polygon,
                               const MexAlignedBox2d& polygonBoundary,
                               PhysConfigSpace2d::PolygonIds* pIds,
                               size_t maxHits,
                               const PolygonTree& polygonTree,
                               Mathex::TouchingRule rule,
                               ObstacleFlags flags ) const
{
    PRE( maxHits > 0 );

    //Get a vector of all the polygons in the tree which might intersect the polygon
    PolygonTree::PItems polygonHolders;
    polygonHolders.reserve( 512 );
    size_t n = polygonTree.overlapping( polygonBoundary, &polygonHolders );

    //Check for the polygon intersects each polygon
    bool hit = false;
    for( size_t i = 0; maxHits != 0  and  i != n; ++i)
    {
        const PhysCS2dPolygon& holder = *(polygonHolders[i]);
        if( holder.isEnabled( flags ) and
            holder.polygon().intersects( polygon, rule ) )
        {
            hit = true;
            --maxHits;
            pIds->push_back( holder.id() );
        }
    }

    return hit;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dImpl::addPortalDomainVertexs( const PortalId& portalId )
{
    const PhysCS2dPortal* pPortal = portals_[ portalId ];

    const ObstacleFlags useAllPolygons = 0;

    // By getting a list of those polygons which might intersect this portal
    // we can speed up the intersection tests
    PolygonTree::PItems polygons;

    intersectingPolygons(
      MexSausage2d( pPortal->endPoint1(), pPortal->endPoint2(), maxPortalClearance_ ),
      permanentPolygonTree(), &polygons );

    bool    foundAtEnd1;
    bool    foundAtEnd2;

    calculatePortalPoints( portalId, CHECK_END, CHECK_END,
      &foundAtEnd1, &foundAtEnd2, useAllPolygons, polygons );

    if( foundAtEnd1 and foundAtEnd2 )
    {
        //  We don't have to look for any more portal ponts because we found some that
        //  don't intersect with _any_ obstacles, are the maximum allowed size and are
        //  right at the end of the portal.
    }
    else
    {
        const CheckAtEnd checkAtEnd1 = foundAtEnd1 ? DONT_CHECK_END : CHECK_END;
        const CheckAtEnd checkAtEnd2 = foundAtEnd2 ? DONT_CHECK_END : CHECK_END;

        //  Try and add extra portal points which ignore certain classes of polygons

        ObstacleFlags allFlags = allFlags_;
        for( ObstacleFlags flags = 1; allFlags != 0; flags <<= 1, allFlags >>= 1 )
        {
            calculatePortalPoints( portalId, checkAtEnd1, checkAtEnd2,
              &foundAtEnd1, &foundAtEnd2, flags, polygons );
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dImpl::calculatePortalPoints( const PortalId& portalId,
  CheckAtEnd checkEnd1, CheckAtEnd checkEnd2,
  bool* pFoundAtEnd1, bool* pFoundAtEnd2,
  ObstacleFlags flags, const PolygonTree::PItems& polygons )
{
    CONFIG_SPACE_STREAM( "Calculate portal points for portal " << portalId.asScalar() << std::endl );
    CONFIG_SPACE_INSPECT( checkEnd1 );
    CONFIG_SPACE_INSPECT( checkEnd2 );
    CONFIG_SPACE_INSPECT( flags );

    //Get the portal pointer
    PhysCS2dPortal* pPortal = portals_[ portalId ];
    ASSERT( pPortal != NULL, "" );

    const MATHEX_SCALAR length = pPortal->endPoint1().euclidianDistance( pPortal->endPoint2() );

    CONFIG_SPACE_INSPECT( length );

    MATHEX_SCALAR   distance1   = 0;
    MATHEX_SCALAR   clearance1  = 0;
    MATHEX_SCALAR   distance2   = 0;
    MATHEX_SCALAR   clearance2  = 0;

    bool    addPoint1 = false;
    bool    addPoint2 = false;

    *pFoundAtEnd1 = false;
    *pFoundAtEnd2 = false;

    bool    end1PointFound = false;
    bool    end2PointFound = false;

    if( checkEnd1 == CHECK_END )
    {
        if( calculatePortalPoint( pPortal->endPoint1(), pPortal->endPoint2(),
          0, length, &distance1, &clearance1, pFoundAtEnd1, flags, polygons ) )
        {
            addPoint1 = true;
        }
        else
        {
            //  If we don't find a point searching from end1 we definitely won't find
            //  one searching from end2
            checkEnd2 = DONT_CHECK_END;
        }
    }

    if( checkEnd2 == CHECK_END )
    {

        if( calculatePortalPoint( pPortal->endPoint2(), pPortal->endPoint1(),
          0, length - ( distance1 + clearance1 ), &distance2, &clearance2, pFoundAtEnd2, flags, polygons ) )
        {
            //  Because we search for the second portal point from end2 we
            //  need to flip the distance so it is correct from end1

            distance2 = length - distance2;

            addPoint2 = true;
        }
    }

    //  If these portal points are quite close together we will only add one
    //  of them, since the other point isn't really gaining us much of an
    //  advantage.
    if( addPoint1 and addPoint2 )
    {
        const MATHEX_SCALAR close = 20.0;

        if( fabs( distance1 - distance2 ) <= close )
        {
            //  See if we can generate a portal point in between the two

            const MATHEX_SCALAR startDistance = ( distance1 + distance2 ) / 2.0 - maxPortalClearance_ - MexEpsilon::instance();
            const MATHEX_SCALAR endDistance = ( distance1 + distance2 ) / 2.0 + maxPortalClearance_ + MexEpsilon::instance();

            MATHEX_SCALAR distance;
            MATHEX_SCALAR clearance;
            bool    foundAtEnd;

            if( ( calculatePortalPoint( pPortal->endPoint1(), pPortal->endPoint2(),
              startDistance, endDistance, &distance, &clearance, &foundAtEnd, flags, polygons ) ) and
              ( clearance >= clearance1 ) and ( clearance >= clearance2 ) )
            {
                addPoint1 = false;
                addPoint2 = false;

                addDomainVertex( portalId, pPortal, distance, clearance, flags );
            }
            else
            {
                //  Drop the point with the smaller clearance

                if( clearance1 < clearance2 )
                    addPoint1 = false;
                else
                    addPoint2 = false;
            }
        }
    }

    if( addPoint1 )
        addDomainVertex( portalId, pPortal, distance1, clearance1, flags );

    if( addPoint2 )
        addDomainVertex( portalId, pPortal, distance2, clearance2, flags );

    CONFIG_SPACE_STREAM( std::endl );
}

//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dImpl::calculatePortalPoint(
  const MexPoint2d& end1, const MexPoint2d& end2,
  MATHEX_SCALAR startDistance, MATHEX_SCALAR endDistance,
  MATHEX_SCALAR* pPointDistance, MATHEX_SCALAR* pClearance,
  bool* pFoundAtEnd, ObstacleFlags flags, const PolygonTree::PItems& polygons )
{
    CONFIG_SPACE_INSPECT( startDistance );
    CONFIG_SPACE_INSPECT( endDistance );
    CONFIG_SPACE_INSPECT( end1 );
    CONFIG_SPACE_INSPECT( end2 );

    PRE_INFO( startDistance );
    PRE_INFO( endDistance );
    PRE( startDistance <= endDistance );

    const MATHEX_SCALAR length = end1.euclidianDistance( end2 );

    const MATHEX_SCALAR clearanceStep = 1.0;

    MATHEX_SCALAR clearance = std::min( ( endDistance - startDistance ) / 2.0, maxPortalClearance_ );
    MATHEX_SCALAR distance;

    bool    foundPoint = false;

    bool    first = true;

    *pFoundAtEnd = false;

    while( clearance >= minPortalClearance_ and not foundPoint )
    {
        for( distance = startDistance + clearance; distance < ( endDistance - clearance ) and not foundPoint; )
        {
            CONFIG_SPACE_INSPECT( clearance );
            CONFIG_SPACE_INSPECT( distance );

            MexPoint2d point(
              mexInterpolate( distance, 0.0, length, end1.x(), end2.x() ),
              mexInterpolate( distance, 0.0, length, end1.y(), end2.y() ) );

            CONFIG_SPACE_INSPECT( point );

            const MexCircle2d testCircle( point, clearance );

            PhysConfigSpace2d::PolygonIds dummyIds;
            dummyIds.reserve( 2 );

            if( not intersects( testCircle, polygons, &dummyIds, 1, Mathex::TOUCH_ISNT_INTERSECT, flags ) )
            {
                CONFIG_SPACE_STREAM( "Found point" << std::endl );

                ASSERT_DATA( PolygonId badId );
                ASSERT( configSpace_.contains( testCircle, flags, &badId ), "" );

                *pPointDistance = distance;
                *pClearance = clearance;

                foundPoint = true;

                //  It is useful for us to know if we found a portal point of the
                //  maximum radius at the very end position. It might save us from
                //  having to look for other portal points

                if( first )
                    *pFoundAtEnd = true;
            }
            else
            {
                ASSERT_DATA( PolygonId badId );
                ASSERT( not configSpace_.contains( testCircle, flags, &badId ), "" );

                CONFIG_SPACE_STREAM( "Intersected with polygon " << dummyIds[ 0 ].asScalar() << std::endl );
            }

            first = false;

            if( not foundPoint )
                distance += clearance;
        }

        if( not foundPoint )
        {
            // Try a point half way along - this helps with some easy cases that
            // fall through the gaps of the above code.

            distance = ( startDistance + endDistance ) / 2.0;
            const MexPoint2d point(
              mexInterpolate( distance, 0.0, length, end1.x(), end2.x() ),
              mexInterpolate( distance, 0.0, length, end1.y(), end2.y() ) );

            const MexCircle2d testCircle( point, clearance );

            PhysConfigSpace2d::PolygonIds dummyIds;
            dummyIds.reserve( 2 );

            if( not intersects( testCircle, polygons, &dummyIds, 1, Mathex::TOUCH_ISNT_INTERSECT, flags ) )
            {
                CONFIG_SPACE_STREAM( "Found half way point" << std::endl );

                ASSERT_DATA( PolygonId badId );
                ASSERT( configSpace_.contains( testCircle, flags, &badId ), "" );

                *pPointDistance = distance;
                *pClearance = clearance;

                foundPoint = true;
            }
            else
            {
                ASSERT_DATA( PolygonId badId );
                ASSERT( not configSpace_.contains( testCircle, flags, &badId ), "" );

                CONFIG_SPACE_STREAM( "Trying to find half way point but intersected with polygon " << dummyIds[ 0 ].asScalar() << std::endl );
            }
        }

        if( not foundPoint )
        {
            if( clearance > minPortalClearance_ and clearance - clearanceStep < minPortalClearance_ )
                clearance = minPortalClearance_;
            else
                clearance -= clearanceStep;
        }
    }

    //  The found at end test is really a found near end test,. This
    //  reduces the number of portal points we have to consider.
    if( ( *pPointDistance < 2.5 * *pClearance ) or
        ( *pPointDistance < ( endDistance - startDistance ) / 4.0 ) )
    {
        *pFoundAtEnd = true;
    }

    POST_INFO( foundPoint );
    POST_INFO( startDistance );
    POST_INFO( endDistance );
    POST_INFO( *pPointDistance );
    POST_INFO( *pClearance );

    CONFIG_SPACE_INSPECT( foundPoint );
    CONFIG_SPACE_INSPECT( startDistance );
    CONFIG_SPACE_INSPECT( endDistance );
    CONFIG_SPACE_INSPECT( *pPointDistance );
    CONFIG_SPACE_INSPECT( *pClearance );

    POST( implies( foundPoint, startDistance - MexEpsilon::instance() <= *pPointDistance - *pClearance ) );
    POST( implies( foundPoint, *pPointDistance + *pClearance <= endDistance + MexEpsilon::instance() ) );

    return foundPoint;
}

//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dImpl::addDomainVertex( const PortalId& portalId, PhysCS2dPortal* pPortal,
  MATHEX_SCALAR distance, MATHEX_SCALAR clearance, ObstacleFlags flags )
{
    PRE( pPortal == portals_[ portalId ] );

    //Add the new domain vertex to the graph
    DomainVertexId domainVertexId = domainVertexIdGenerator().next();
    if( pPortal->addDomainVertex( domainVertexId, distance, clearance, portalId, &domainGraph_, flags ) )
    {
        //Add arcs joining the new vertex to every other domain vertex on each
        //of the 2 domains
        addDomainArcs( domainVertexId, pPortal->domainId1() );
        addDomainArcs( domainVertexId, pPortal->domainId2() );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////

PhysConfigSpace2d::DomainVertexId PhysCS2dImpl::addDomainVertex( const MexPoint2d& point, MATHEX_SCALAR clearance, ObstacleFlags flags )
{
    //Generate the id
    DomainVertexId domainVertexId = domainVertexIdGenerator().next();

    //Construct the vertex and add to the graph
    domainGraph_.addVertex( domainVertexId, PhysCS2dDomainVertex( point, clearance, flags ) );

    return domainVertexId;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dImpl::removeDomainVertex( const DomainVertexId& id )
{
    PRE( domainGraph().containsVertex( id ) );

    //Remove from the graph
    domainGraph_.removeVertex( id );
}
//////////////////////////////////////////////////////////////////////////////////////////

static uint nUnnecessaryArcs = 0;

void PhysCS2dImpl::addDomainArcs( const DomainVertexId& inVertexId,
                                       const DomainId& domainId )
{
    PRE( domainGraph().containsVertex( inVertexId ) );
    PRE( domains_.contains( domainId ) );

    //Get graph etc
    FtlReusingIdGenerator& arcIdGenerator = domainArcIdGenerator();

    //Get the argument domainVertex data
    const PhysCS2dDomainVertex& inVertex = domainGraph_.vertex( inVertexId );
    PortalId inVertexPortalId = inVertex.portalId();
    //const PhysCS2dPortal& inVertexPortal = *(portals_[ inVertexPortalId ]);
    const MexPoint2d& inVertexPoint = inVertex.point();

    //get the domain's portal list
    PhysCS2dDomain& domain = *(domains_[ domainId ]);
    const ctl_vector< PortalId >& domainPortals = domain.portals();
    size_t nPortals = domainPortals.size();

    //Visit each portal of this domain
    for( size_t i = 0; i != nPortals; ++i )
    {
        //Get the domain portal, and ignore if same as vertex portal
        PortalId domainPortalId = domainPortals[i];
        if( domainPortalId != inVertexPortalId )
        {
            //Get number of domainVertexs on this portal
            PhysCS2dPortal& domainPortal = *(portals_[ domainPortalId ]);
            size_t nPortalDomainVertexs = domainPortal.nDomainVertexs();

            for( size_t j = 0; j != nPortalDomainVertexs; ++j )
            {
                //Get the domain's vertex
                DomainVertexId domainVertexId = domainPortal.domainVertexId( j );
                PhysCS2dDomainVertex& domainVertex = domainGraph_.vertex( domainVertexId );

const ObstacleFlags flags1 = inVertex.flags();
const ObstacleFlags flags2 = domainVertex.flags();

if( flags1 != 0 and flags2 != 0 and flags1 != flags2 )
{
    ++nUnnecessaryArcs;
    CONFIG_SPACE_INSPECT( nUnnecessaryArcs );
    CONFIG_SPACE_INSPECT( flags1 );
    CONFIG_SPACE_INSPECT( flags2 );
    CONFIG_SPACE_INSPECT( domainVertexId );
    CONFIG_SPACE_INSPECT( inVertexId );
}

                //Construct an arc between the 2 vertices
                DomainArcId arcId = arcIdGenerator.next();

                domainGraph_.addArc( inVertexId, domainVertexId, arcId,
                                    PhysCS2dDomainArc( domainId ) );

                //Get coordinates of the portal vertex
                const MexPoint2d& domainPoint = domainVertex.point();

                //Use the linear distance between the 2 vertices as the weight
                MATHEX_SCALAR distance = inVertexPoint.euclidianDistance( domainPoint );
                domainGraph_.weight( arcId, distance );
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dImpl::createVisibilityGraph
(
    PhysConfigSpace2d* pOriginalSpace, PhysCS2dExpansionSpace* pExpansionSpace,
    MATHEX_SCALAR clearance
)
{
    PRE( pVisibilityGraph_ == NULL )

    //Construct the graph
    pVisibilityGraph_ = _NEW( PhysCS2dVisibilityGraph( &configSpace_, pOriginalSpace, pExpansionSpace, clearance ) );

    //Register all permanent polygons currently defined
    for( PolygonId id = polygonIdGenerator_.begin(); id != polygonIdGenerator_.end(); ++id )
    {
        if( polygons_.contains( id ) )
            pVisibilityGraph_->addPolygon( id );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dImpl::deleteVisibilityGraph( void )
{
    PRE( pVisibilityGraph_ != NULL )

    //Delete the graph
    _DELETE( pVisibilityGraph_ );
    pVisibilityGraph_ = NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dImpl::expansionSpaceOpen( MATHEX_SCALAR expansionDistance )
{
    CS2VGRA_STREAM( "Enter PhysCS2dImpl::expansionSpaceOpen" << std::endl );
    CS2VGRA_INDENT( 2 );

    PRE( not expansionSpaceIsOpen() );

    //See if one already open with different distance, or same distance but too may
    //polygons, and delete if so.
    if( pExpansionSpace_ != NULL )
    {
        if( pExpansionSpace_->expansionDistance() != expansionDistance or
            ( pExpansionSpace_->configSpace().nPolygons() > EXPANSION_SPACE_THRESHOLD  and
              pExpansionSpace_->nClients() == 0 ) )
        {
            PRE( pExpansionSpace_->nClients() == 0 );
            _DELETE( pExpansionSpace_ );
            pExpansionSpace_ = NULL;
        }
    }

    //If we don't have one, create one
    if( pExpansionSpace_ == NULL )
    {
        pExpansionSpace_ = _NEW( PhysCS2dExpansionSpace( boundary(), expansionDistance ) );
    }

    //Increment the client count
    pExpansionSpace_->addClient();

    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM( "Exit PhysCS2dImpl::expansionSpaceOpen" << std::endl );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dImpl::expansionSpaceClose( MATHEX_SCALAR expansionDistance )
{
    CS2VGRA_STREAM( "Enter PhysCS2dImpl::expansionSpaceClose" << std::endl );
    CS2VGRA_INDENT( 2 );

    PRE( expansionSpaceIsOpen( expansionDistance ) );

    //decrement the client count
    pExpansionSpace_->removeClient();
    POST( pExpansionSpace_->nClients() == 0 );

    //Delete it. This is in case any polygons have been expanded which we won't want
    _DELETE( pExpansionSpace_ );
    pExpansionSpace_ = NULL;

    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM( "Exit PhysCS2dImpl::expansionSpaceClose" << std::endl );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dImpl::expansionSpaceIsOpen( MATHEX_SCALAR expansionDistance ) const
{
    return pExpansionSpace_ != NULL and
           pExpansionSpace_->expansionDistance() == expansionDistance and
           pExpansionSpace_->nClients() != 0;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dImpl::expansionSpaceIsOpen( void ) const
{
    return pExpansionSpace_ != NULL and
           pExpansionSpace_->nClients() != 0;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysConfigSpace2d* PhysCS2dImpl::expansionSpace( MATHEX_SCALAR expansionDistance )
{
    PRE( expansionSpaceIsOpen( expansionDistance ) );

    return pExpansionSpace_->pConfigSpace();
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dImpl::expansionSpaceAddPolygon( MATHEX_SCALAR expansionDistance, PolygonId id )
{
    PRE( expansionSpaceIsOpen( expansionDistance ) );
    PRE( not expansionSpacePolygonExists( expansionDistance, id ) );
    PRE( polygons_.contains( id ) );

    pExpansionSpace_->addPolygon( polygons_[ id ]->polygon(), id, polygons_[ id ]->flags() );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dImpl::expansionSpacePolygonExists
(
    MATHEX_SCALAR expansionDistance, PolygonId id
) const
{
    PRE( expansionSpaceIsOpen( expansionDistance ) );
    PRE( polygons_.contains( id ) );

    return pExpansionSpace_->polygonExists( id );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dImpl::updateDomainRegion()
{
    //Check there is an out of date region
    if( domainRegionOutOfDate_ )
    {
        //Create a collection of portal ids
        typedef ctl_vector< PhysConfigSpace2d::PortalId > PortalIds;
        PortalIds updatePortals;
        updatePortals.reserve( 128 );

        //trace( Diag::jonStream(), domainGraph_ );

        //Expand the domain update region by the domain clearance to be on the safe side
        MexPoint2d minCorner = domainUpdateRegion_.minCorner();
        MexPoint2d maxCorner = domainUpdateRegion_.maxCorner();
        domainUpdateRegion_.corners( MexPoint2d( minCorner.x() - domainClearance_,
                                                 minCorner.y() - domainClearance_ ),
                                     MexPoint2d( maxCorner.x() + domainClearance_,
                                                 maxCorner.y() + domainClearance_ ) );

        //Find all the portals we need to update
        for( PhysConfigSpace2d::PortalId id = portalIdGenerator_.begin();
             id != portalIdGenerator_.end(); ++id )
        {
            if( portals_.contains( id ) )
            {
                //Check the portal overlaps the update region
                PhysCS2dPortal* pPortal = portals_[ id ];
                MexAlignedBox2d portalBoundary( pPortal->endPoint1(), pPortal->endPoint2() );
                if( portalBoundary.intersects( domainUpdateRegion_ ) )
                {
                    //Add the id to our collection
                    updatePortals.push_back( id );

                    //Remove existing vertices
                    while( pPortal->nDomainVertexs() != 0 )
                    {
                        pPortal->removeDomainVertex( pPortal->domainVertexId( 0 ),
                                                     &domainGraph_ );
                    }
                }
            }
        }

        //Now add the vertices for each portal in the list
        for( PortalIds::iterator it = updatePortals.begin(); it != updatePortals.end(); ++it )
            addPortalDomainVertexs( *it );

        //trace( Diag::jonStream(), domainGraph_ );

        //Now it's up to date
        domainRegionOutOfDate_ = false;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dImpl::intersects( const MexSausage2d& sausage,
                               const MexAlignedBox2d& sausageBoundary,
                               PhysConfigSpace2d::PolygonIds* pIds,
                               size_t maxHits,
                               const PolygonTree& polygonTree,
                               Mathex::TouchingRule rule,
                               ObstacleFlags flags ) const
{
    PRE( maxHits > 0 );

    //Get a vector of all the polygons in the tree which might intersect the polygon
    PolygonTree::PItems polygonHolders;
    polygonHolders.reserve( 512 );
    size_t n = polygonTree.overlapping( sausageBoundary, &polygonHolders );

    //Check for the polygon intersects each polygon
    bool hit = false;
    for( size_t i = 0; maxHits != 0  and  i != n; ++i)
    {
        const PhysCS2dPolygon& holder = *(polygonHolders[i]);
        if( holder.isEnabled( flags )  and  sausage.intersects( holder.polygon(), rule ) )
        {
            hit = true;
            --maxHits;
            pIds->push_back( holder.id() );
        }
    }

    return hit;
}

//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dImpl::intersectingPolygons(
    const MexAlignedBox2d& box,
    const PolygonTree& polygonTree,
    PolygonTree::PItems *pPolygons ) const
{
    PRE( pPolygons != NULL );
    PRE( pPolygons->size() == 0 );

    pPolygons->reserve( 512 );
    polygonTree.overlapping( box, pPolygons );
}

//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dImpl::intersectingPolygons(
    const MexSausage2d& sausage,
    const PolygonTree& polygonTree,
    PolygonTree::PItems *pPolygons ) const
{
    //Construct a bounding box for the sausage
    MexAlignedBox2d sausageBoundary;
    sausage.boundary( &sausageBoundary );

    intersectingPolygons( sausageBoundary, polygonTree, pPolygons );
}

//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dImpl::intersects(
    const MexCircle2d& circle,
    const PolygonTree::PItems& polygons,
    PhysConfigSpace2d::PolygonIds* pIds,
    size_t maxHits,
    Mathex::TouchingRule rule,
    ObstacleFlags flags ) const
{
    PRE( maxHits > 0 );

    const size_t n = polygons.size();

    //Check for the polygon intersects each polygon
    bool hit = false;
    for( size_t i = 0; maxHits != 0  and  i != n; ++i)
    {
        const PhysCS2dPolygon& holder = *(polygons[i]);
        if( holder.isEnabled( flags )  and  circle.intersects( holder.polygon(), rule ) )
        {
            hit = true;
            --maxHits;
            pIds->push_back( holder.id() );

            ASSERT_INFO( circle );
            ASSERT_INFO( holder.polygon() );
        }
    }

    return hit;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dImpl::intersects(
    const MexCircle2d& circle,
    const MexAlignedBox2d& circleBoundary,
    PhysConfigSpace2d::PolygonIds* pIds,
    size_t maxHits,
    const PolygonTree& polygonTree,
    Mathex::TouchingRule rule,
    ObstacleFlags flags ) const
{
    PRE( maxHits > 0 );

    PolygonTree::PItems polygonHolders;

    intersectingPolygons( circleBoundary, polygonTree, &polygonHolders );

    const bool result = intersects( circle, polygonHolders, pIds, maxHits, rule, flags );

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dImpl::activeDomainFindPath( PhysCS2dDomainFindPath* pDomainFindPath )
{
    PRE( pDomainFindPath == NULL or pActiveDomainFindPath_ == NULL );
    pActiveDomainFindPath_ = pDomainFindPath;
}

void PhysCS2dImpl::listArcs( ostream& ostr ) const
{
    for( PhysConfigSpace2d::DomainArcId arcId = _CONST_CAST(PhysCS2dImpl*, this)->domainArcIdGenerator().begin();
         arcId != _CONST_CAST(PhysCS2dImpl*, this)->domainArcIdGenerator().end(); ++arcId )
    {
        if( domainGraph().containsArc( arcId ) )
        {
            PhysConfigSpace2d::DomainVertexId vertexId1 = domainGraph().fromVertex( arcId );
            PhysConfigSpace2d::DomainVertexId vertexId2 = domainGraph().toVertex( arcId );

            const PhysCS2dDomainVertex& vertex1 = domainGraph().vertex( vertexId1 );
            const PhysCS2dDomainVertex& vertex2 = domainGraph().vertex( vertexId2 );

            ostr << "Arc " << arcId << " domain " << domainGraph().arc( arcId ).domainId() << " from ( " << vertex1 << " ) to ( " << vertex2 << " )" << std::endl;
        }
    }
}

bool PhysCS2dImpl::domainUpdateRegionExists() const
{
    return domainRegionOutOfDate_;
}

bool PhysCS2dImpl::intersectsAnyExpanded( const MexSausage2d& sausage,
                               const MexAlignedBox2d& sausageBoundary,
                               const PolygonTree& polygonTree,
                               Mathex::TouchingRule rule,
                               ObstacleFlags flags,
                               MATHEX_SCALAR expansionDistance ) const
{
    PRE( expansionSpaceIsOpen( expansionDistance ) );

    //Get a vector of all the polygons in the tree which might intersect the polygon
    PolygonTree::PItems polygonHolders;
    polygonHolders.reserve( 512 );
    size_t n = polygonTree.overlapping( sausageBoundary, &polygonHolders );

    //Check for the polygon intersects each enabled polygon, which has been expanded
    bool hit = false;
    for( size_t i = 0; not hit  and  i != n; ++i)
    {
        const PhysCS2dPolygon& holder = *(polygonHolders[i]);
        if( holder.isEnabled( flags )  and
            expansionSpacePolygonExists( expansionDistance, holder.id() )  and
            sausage.intersects( holder.polygon(), rule ) )
        {
            CS2VGRA_STREAM("Clash with polygon id " << holder.id().asScalar() << std::endl);
            CS2VGRA_INSPECT( holder.polygon() );
            hit = true;
        }
    }

    return hit;
}
/* End CS2DATA.CPP **************************************************/
