/*
 * C S P A C E 2 . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdio.h>

#include "mathex/point3d.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/cvexpgon.hpp"
#include "mathex/sausag2d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/vec2.hpp"
#include "mathex/circle2d.hpp"
#include "mathex/line2d.hpp"
#include "mathex/random.hpp"

#include "phys/cspace2.hpp"
#include "phys/internal/cs2impl.hpp"
#include "phys/internal/cs2domai.hpp"
#include "phys/internal/cs2findp.hpp"
#include "phys/internal/cs2dmfnd.hpp"
#include "phys/internal/cs2domai.hpp"
#include "phys/internal/cs2vigra.hpp"
#include "phys/internal/cs2mochu.hpp"
#include "phys/internal/cs2porta.hpp"

#include "utility/indent.hpp"
#include "utility/ascpict.hpp"

#include <algorithm>

#ifndef _INLINE
    #include "phys/cspace2.ipp"
#endif

//////////////////////////////////////////////////////////////////////////////////////////

PhysConfigSpace2d::PhysConfigSpace2d
(
    const MexPoint2d& minPoint, const MexPoint2d& maxPoint, Mode newMode,
    MATHEX_SCALAR domainClearance, MATHEX_SCALAR minPortalClearance,
    MATHEX_SCALAR maxPortalClearance, ObstacleFlags allObstacleFlags
)
:   impl_( *_NEW( PhysCS2dImpl( this, MexAlignedBox2d( minPoint, maxPoint ),
                                newMode, 10.0, 2, 1.0, 5.0, 4, 22.0, domainClearance,
                                minPortalClearance, maxPortalClearance, allObstacleFlags ) ) )
{

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysConfigSpace2d::~PhysConfigSpace2d()
{
    TEST_INVARIANT;

    //Free the data object
    _DELETE( &impl_ );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysConfigSpace2d::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const PhysConfigSpace2d& t )
{

    o << "PhysConfigSpace2d " << (void*)&t << " start" << std::endl;
    UtlIndentOstream    ostr( o, "  " );
    ostr << t.impl_;
    o << "PhysConfigSpace2d " << (void*)&t << " end" << std::endl;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysConfigSpace2d::DomainId PhysConfigSpace2d::addDomain( const MexAlignedBox2d& box )
{
    PRE( impl_.mode() == SUBTRACTIVE );

    //Allocate the id
    DomainId id =  impl_.domainIdGenerator().next();

    //Construct the new domain
    PhysCS2dDomain* pDomain = _NEW( PhysCS2dDomain( id, box ) );

    //Add it
    impl_.add( pDomain );

    return id;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysConfigSpace2d::DomainId PhysConfigSpace2d::addDomain
(
    const MexAlignedBox2d& box, std::auto_ptr< MexPolygon2d >& boundaryAPtr
)
{
    PRE( impl_.mode() == SUBTRACTIVE );

    //Allocate the id
    DomainId id =  impl_.domainIdGenerator().next();

    //Construct the new domain
    PhysCS2dDomain* pDomain = _NEW( PhysCS2dDomain( id, box, boundaryAPtr ) );

    //Add it to the serial map
    impl_.add( pDomain );

    return id;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::domainExists( const DomainId& id ) const
{
    PRE( mode() == SUBTRACTIVE );

    return impl_.domains().contains( id );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysConfigSpace2d::removeDomain( const DomainId& id )
{
    PRE( mode() == SUBTRACTIVE );
    PRE( domainExists( id ) );

    //Get the domain from the map
    PhysCS2dImpl::DomainMap& domains = impl_.domains();
    PhysCS2dDomain* pDomain = domains[ id ];
    POST( pDomain != NULL );

    //Remove all its portals
    const ctl_vector< PortalId >& portals = pDomain->portals();
    while( portals.size() != 0 )
    {
        POST_DATA( size_t j = portals.size() );
        removePortal( portals.back() );
        POST( portals.size() == j - 1 );
    }

    //Remove from map
    domains.remove( id );

    //Remove from the BSP Tree
    impl_.domainTree().remove( pDomain );

    //Delete the domain
    _DELETE( pDomain );

    POST( not domainExists( id ) );
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysConfigSpace2d::Mode PhysConfigSpace2d::mode() const
{
    return impl_.mode();
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysConfigSpace2d::PortalId PhysConfigSpace2d::addPortal
(
    const DomainId& id1, const DomainId& id2,
    const MexPoint2d& point1, const MexPoint2d& point2
)
{
    PRE( mode() == SUBTRACTIVE );
    PRE( domainExists( id1 ) );
    PRE( domainExists( id2 ) );

    //Construct a portal object
    PhysCS2dPortal* pPortal = _NEW( PhysCS2dPortal( id1, id2, point1, point2 ) );

    //Allocate an id
    PhysConfigSpace2d::PortalId id = impl_.portalIdGenerator().next();

    //Add the portals to the 2 domains
    impl_.domains()[ id1 ]->addPortal( id );
    impl_.domains()[ id2 ]->addPortal( id );

    //Enter in the portal map
    impl_.portals().add( id, pPortal );

    //Add the domain vertices to the portal
    impl_.addPortalDomainVertexs( id );

    return id;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysConfigSpace2d::removePortal( const PortalId& id )
{
    PRE( mode() == SUBTRACTIVE );
    PRE( portalExists( id ) );

    //Get the portal pointer
    PhysCS2dImpl::PortalMap& portals = impl_.portals();
    PhysCS2dPortal* pPortal = portals[ id ];
    POST( pPortal != NULL );

    //Remove any DomainVertexs on the portal
    PhysCS2dDomainGraph& domainGraph = *(impl_.pDomainGraph());

    while( pPortal->nDomainVertexs() > 0 )
        pPortal->removeDomainVertex( pPortal->domainVertexId( 0 ), &domainGraph );

    //Remove from the map
    portals.remove( id );

    //Remove the portal from the associated domains
    PhysCS2dImpl::DomainMap& domains = impl_.domains();
    domains[ pPortal->domainId1() ]->removePortal( id );
    domains[ pPortal->domainId2() ]->removePortal( id );

    //Delete the portal
    _DELETE( pPortal );

    POST( not portalExists( id ) );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::portalExists( const PortalId& id ) const
{
    PRE( mode() == SUBTRACTIVE )

    return impl_.portals().contains( id );
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysConfigSpace2d::PolygonId PhysConfigSpace2d::add
(
    std::auto_ptr< MexPolygon2d >& polygonAPtr,
    MATHEX_SCALAR height,
    ObstacleFlags flags,
    Longevity type
)
{
    //generate an id for the polygon
    PolygonId id = impl_.polygonIdGenerator().next();

    //Construct a new holder for the polygon
    PhysCS2dPolygon* pPolygonHolder = _NEW(
      PhysCS2dPolygon( id, polygonAPtr, height, flags, type) );
    POST( pPolygonHolder != NULL );

    //Store in the polygon map
    impl_.polygons().add( id, pPolygonHolder );

    //Increment the count
    ++impl_.nPolygons();

    //Add to the appropriate BSP tree
    if( type == PhysConfigSpace2d::PERMANENT )
        impl_.permanentPolygonTree().add( pPolygonHolder );
    else
        impl_.temporaryPolygonTree().add( pPolygonHolder );

    //If maintaining a visibility graph, add the polygon to it
    if( isMaintainingVisibilityGraph() )
        impl_.pVisibilityGraph()->addPolygon( id );


    //Mark the domain update region out of date, and add the polygon's boundary to
    //the region to be updated.
    if( type == PERMANENT )
    {
        if( impl_.domainRegionOutOfDate_ )
            impl_.domainUpdateRegion_.unionWith( pPolygonHolder->boundary() );
        else
        {
            impl_.domainRegionOutOfDate_ = true;
            impl_.domainUpdateRegion_ = pPolygonHolder->boundary();
        }
    }

    //Unless buffering polygon updates, update the portals as required
    if( not impl_.bufferPolygonPortalUpdates_ )
        impl_.updateDomainRegion();

    return id;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::exists( const PolygonId& id ) const
{
    return impl_.polygons().contains( id );
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysConfigSpace2d::Longevity PhysConfigSpace2d::longevity( const PolygonId& id ) const
{
    PRE( exists( id ) );

    return impl_.polygons()[ id ]->longevity();
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysConfigSpace2d::longevity( const PolygonId& id, Longevity type )
{
    PRE( exists( id ) );

    //Get the polygon and check its current setting
    PhysCS2dPolygon* pPolygonHolder = impl_.polygons()[ id ];
    if( pPolygonHolder->longevity() != type )
    {
        //Remove from current BSP tree
        if( pPolygonHolder->longevity() == PERMANENT )
            impl_.permanentPolygonTree().remove( pPolygonHolder );
        else
            impl_.temporaryPolygonTree().remove( pPolygonHolder );

        //Add to the other tree
        if( type == PERMANENT )
            impl_.permanentPolygonTree().add( pPolygonHolder );
        else
            impl_.temporaryPolygonTree().add( pPolygonHolder );

        //Reset the polygon type
        pPolygonHolder->longevity( type );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysConfigSpace2d::remove( PolygonId id )
{
    PRE( exists( id ) );

    //Get the pointer
    PhysCS2dImpl::PolygonMap& polygons = impl_.polygons();
    PhysCS2dPolygon* pPolygonHolder = polygons[ id ];

    //Mark the domain update region out of date, and add the polygon's boundary to
    //the region to be updated.
    if( pPolygonHolder->longevity() == PERMANENT )
    {
        if( impl_.domainRegionOutOfDate_ )
            impl_.domainUpdateRegion_.unionWith( pPolygonHolder->boundary() );
        else
        {
            impl_.domainRegionOutOfDate_ = true;
            impl_.domainUpdateRegion_ = pPolygonHolder->boundary();
        }
    }

    //Remove from its BSP tree
    if( pPolygonHolder->longevity() == PERMANENT )
        impl_.permanentPolygonTree().remove( pPolygonHolder );
    else
        impl_.temporaryPolygonTree().remove( pPolygonHolder );

    //Remove from the map
    polygons.remove( id );

    //Free the polygon id
    impl_.polygonIdGenerator().free( id );

    //Decrement the count
    --impl_.nPolygons();

    //If maintaining a visibility graph, remove the polygon from it
    if( isMaintainingVisibilityGraph() )
        impl_.pVisibilityGraph()->removePolygon( id );

    //Delete it
    _DELETE( pPolygonHolder );

    //Unless buffering polygon updates, update the portals as required
    if( not impl_.bufferPolygonPortalUpdates_ )
        impl_.updateDomainRegion();
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::contains( const MexPoint2d& testPoint, ObstacleFlags flags,
                                PolygonId* pId, UsePolygons whichPolygons ) const
{
    //Check inside the outer boundary
    bool inSet = impl_.boundary().contains( testPoint );

    //Check for containment with the appropriate BSP trees of polygons
    if( inSet )
    {
        //See if the point is inside any polygon
        bool inPolygons = false;

        if( whichPolygons == USE_PERMANENT_ONLY or whichPolygons == USE_ALL )
            inPolygons = impl_.contains( testPoint, pId, impl_.permanentPolygonTree(), flags );

        if( not inPolygons and
            (whichPolygons == USE_TEMPORARY_ONLY or whichPolygons == USE_ALL) )
            inPolygons = impl_.contains( testPoint, pId, impl_.temporaryPolygonTree(), flags );

        //Decide what this means based on space type
        inSet = ( impl_.mode() == SUBTRACTIVE ? not inPolygons: inPolygons );
    }

    return inSet;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::contains( const MexPoint3d& p1, const MexPoint3d& p2, ObstacleFlags flags, PolygonId* pId,
                                  UsePolygons whichPolygons ) const
{
    PRE( mode() == SUBTRACTIVE );

    //Construct 2d points from the 3d points supplied, and store the min z coord
    MexPoint2d end1( p1.x(), p1.y() );
    MexPoint2d end2( p2.x(), p2.y() );
    MATHEX_SCALAR height = ( p1.z() <= p2.z() ? p1.z() : p2.z() );

    //Check inside the outer boundary
    bool inSet = impl_.boundary().contains( end1 ) and impl_.boundary().contains( end2 );

    //Check for containment with the appropriate BSP trees of polygons
    if( inSet )
    {
        //See if the line intersects any polygon
        if( whichPolygons == USE_PERMANENT_ONLY or whichPolygons == USE_ALL )
            inSet = not impl_.intersects( end1, end2, height, pId, impl_.permanentPolygonTree(),
                                          Mathex::TOUCH_ISNT_INTERSECT, flags );

        if( inSet and
            (whichPolygons == USE_TEMPORARY_ONLY or whichPolygons == USE_ALL) )
            inSet = not impl_.intersects( end1, end2, height, pId, impl_.temporaryPolygonTree(),
                                          Mathex::TOUCH_ISNT_INTERSECT, flags );
    }

    return inSet;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::contains( const MexPolygon2d& polygon, ObstacleFlags flags, PolygonId* pId,
                                  UsePolygons whichPolygons ) const
{
    PRE( mode() == SUBTRACTIVE );

    //Construct a bounding box for the polygon
    MexAlignedBox2d polygonBoundary;
    polygon.boundary( &polygonBoundary );

    //Check inside the outer boundary
    bool inSet = impl_.boundary().contains( polygonBoundary );

    //Check for containment with the appropriate BSP trees of polygons
    if( inSet )
    {
        PolygonIds ids;
        ids.reserve( 2 );

        //See if the polygon intersects any polygon
        if( whichPolygons == USE_PERMANENT_ONLY or whichPolygons == USE_ALL )
        {
            inSet = not impl_.intersects( polygon, polygonBoundary, &ids, 1,
                                          impl_.permanentPolygonTree(),
                                          Mathex::TOUCH_ISNT_INTERSECT, flags );
        }

        if( inSet and
            (whichPolygons == USE_TEMPORARY_ONLY or whichPolygons == USE_ALL) )
        {
            inSet = not impl_.intersects( polygon, polygonBoundary, &ids, 1,
                                          impl_.temporaryPolygonTree(),
                                          Mathex::TOUCH_ISNT_INTERSECT, flags );
        }

        if( not inSet ) *pId = ids[0];
    }
    else *pId = 0;

    return inSet;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::contains( const MexPolygon2d& polygon, ObstacleFlags flags, PolygonIds* pIds,
                                  UsePolygons whichPolygons ) const
{
    PRE( mode() == SUBTRACTIVE );

    //Construct a bounding box for the polygon
    MexAlignedBox2d polygonBoundary;
    polygon.boundary( &polygonBoundary );

    //Check inside the outer boundary
    bool inSet = impl_.boundary().contains( polygonBoundary );

    //Check for containment with the appropriate BSP trees of polygons
    if( inSet )
    {
        PolygonIds ids;

        //See if the polygon intersects any polygon
        if( whichPolygons == USE_PERMANENT_ONLY or whichPolygons == USE_ALL )
            inSet = not impl_.intersects( polygon, polygonBoundary, pIds, 10000000L,
                                          impl_.permanentPolygonTree(),
                                          Mathex::TOUCH_ISNT_INTERSECT, flags );

        if( inSet and
            (whichPolygons == USE_TEMPORARY_ONLY or whichPolygons == USE_ALL) )
            inSet = not impl_.intersects( polygon, polygonBoundary, pIds, 10000000L,
                                          impl_.temporaryPolygonTree(),
                                          Mathex::TOUCH_ISNT_INTERSECT, flags );
    }
    else pIds->push_back( 0 );

    return inSet;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysConfigSpace2d::FindPathId PhysConfigSpace2d::addFindPath
(
    const MexPoint2d& startPoint, const MexPoint2d& endPoint, MATHEX_SCALAR clearance,
    ObstacleFlags flags, PhysPathFindingPriority priority
)
{
    PRE_DATA( PolygonId polygonId );
    PRE( mode() == SUBTRACTIVE );
    //PRE( contains( startPoint, &polygonId, USE_PERMANENT_ONLY ) );
    //PRE( contains( endPoint, &polygonId, USE_PERMANENT_ONLY  ) );
    PRE( clearance > 0 );

    //generate a new id
    FindPathId id = impl_.findPathIdGenerator().next();

    //Construct a new findPath object
    PhysCS2dFindPath* pFindPath =
        _NEW( PhysCS2dFindPath( this, startPoint, endPoint, clearance, flags, priority ) );

    //Add it to the map
    impl_.findPaths().add( id, pFindPath );

    //Add it to the queue if not already finished
    if( not pFindPath->isFinished() )
        impl_.findPathQueue().push_back( pFindPath );

    return id;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::findPathExists( const FindPathId& id ) const
{
    return impl_.findPaths().contains( id );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::updateFindPath( const FindPathId& id, const PhysRelativeTime& maxTime )
{
    CS2VGRA_STREAM("PhysConfigSpace2d::updateFindPath entry " << (void*)this << std::endl);
    CS2VGRA_INDENT( 2 );

    PRE( findPathExists( id ) );

    //Get the CS2dPathFind object pointer from the map
    PhysCS2dFindPath* pIdFindPath = impl_.findPaths()[ id ];

    PhysCS2dImpl::FindPaths& findPathQueue = impl_.findPathQueue();
    if( findPathQueue.size() != 0 )
    {
        //Get first in queue and update it.
        PhysCS2dFindPath* pFindPath = pCurrentFindPath();

        CS2VGRA_INSPECT( (void*)pFindPath );
        CS2VGRA_INSPECT( (void*)pIdFindPath );

        pFindPath->update( maxTime );

        //If complete, remove from queue
        if( pFindPath->isFinished() )
            removeCurrentFindPathFromQueue();
    }

    const bool result = pIdFindPath->isFinished();

    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM("PhysConfigSpace2d::updateFindPath exit " << (void*)this << std::endl);

    return result;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::isFindPathDone( const FindPathId& id ) const
{
    PRE( findPathExists( id ) );

    //Get the CS2dFindPath object pointer from the map, and see if finished
    const PhysCS2dFindPath* pFindPath = impl_.findPaths()[ id ];
    return pFindPath->isFinished();
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysConfigSpace2d::removeFindPath( const FindPathId& id )
{
    PRE( findPathExists( id ) );

    //Get the CS2dFindPath object pointer from the map, remove it from the map and
    //queue and delete it
    PhysCS2dFindPath* pFindPath = impl_.findPaths()[ id ];

    if( impl_.hasCurrentFindPath_ and
      ( impl_.pCurrentFindPath_ == pFindPath ) )
    {
        impl_.hasCurrentFindPath_ = false;
    }

    impl_.findPaths().remove( id );
    impl_.findPathIdGenerator().free( id );

    if( not pFindPath->isFinished() )
    {
        PhysCS2dImpl::FindPaths& findPathQueue = impl_.findPathQueue();
        PhysCS2dImpl::FindPaths::iterator it
            = std::find( findPathQueue.begin(), findPathQueue.end(), pFindPath );
        ASSERT( it != findPathQueue.end(), "" )
        findPathQueue.erase( it );
    }

    _DELETE( pFindPath );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::findPath( const FindPathId& id, Path* pPath )
{
    PRE( findPathExists( id ) );
    PRE( isFindPathDone( id ) );

    //Get the CS2dFindPath object pointer from the map
    PhysCS2dFindPath* pFindPath = impl_.findPaths()[ id ];

    //See if we have a path and get it
    bool havePath = pFindPath->output( pPath );

    //Delete the findPath object
    removeFindPath( id );

    return havePath;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::domain( const MexPoint2d& point, DomainId* pId ) const
{
    PRE( mode() == SUBTRACTIVE );

    //Check inside the outer boundary
    bool inDomain = impl_.boundary().contains( point );

    //See if in an actual domain
    if( inDomain )
    {
        //Construct a boundary for the point
        MexAlignedBox2d pointBoundary( point, point );

        //Get a vector of the domains overlapping this boundary using the BSP tree
        ctl_vector< PhysCS2dDomain* > possibleDomains;
        possibleDomains.reserve( 16 );
        impl_.domainTree().overlapping( pointBoundary, &possibleDomains );

        //Check each one to see if contains the point
        inDomain = false;
        size_t n = possibleDomains.size();
        for( size_t i = 0; i != n; ++i )
        {
            if( possibleDomains[i]->contains( point ) )
            {
                inDomain = true;
                *pId = possibleDomains[i]->id();
                break;
            }
        }
    }

    return inDomain;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysConfigSpace2d::isMaintainingVisibilityGraph
(
    bool doMaintain, PhysConfigSpace2d* pOriginalSpace, PhysCS2dExpansionSpace* pExpansionSpace,
    MATHEX_SCALAR clearance
)
{
    //Get existing graph pointer
    PhysCS2dVisibilityGraph* pGraph = impl_.pVisibilityGraph();

    if( doMaintain and pGraph == NULL )
    {
        //Set up a visibility graph
        impl_.createVisibilityGraph( pOriginalSpace, pExpansionSpace, clearance );
    }
    else if( not doMaintain and pGraph != NULL )
    {
        //Destroy the graph
        impl_.deleteVisibilityGraph();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::isMaintainingVisibilityGraph() const
{
    return impl_.pVisibilityGraph() != NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////

const MexPolygon2d& PhysConfigSpace2d::polygon( const PolygonId& polygonId ) const
{
    PRE( exists( polygonId ) )  ;
    return impl_.polygons()[ polygonId ]->polygon();
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::isPolygonEnabled( const PolygonId& polygonId ) const
{
    PRE( exists( polygonId ) )  ;
    return impl_.polygons()[ polygonId ]->isEnabled();
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysConfigSpace2d::isPolygonEnabled( const PolygonId& polygonId, bool enable )
{
    PRE( exists( polygonId ) )  ;
    impl_.polygons()[ polygonId ]->isEnabled( enable );
}
//////////////////////////////////////////////////////////////////////////////////////////

size_t PhysConfigSpace2d::nPolygons() const
{
    return impl_.nPolygons();
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysConfigSpace2d::traceFindPath( const FindPathId& id, ostream& o )
{
    PRE( findPathExists( id ) );
    o << *(impl_.findPaths()[ id ]);
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& PhysConfigSpace2d::traceObstacles
(
    ostream& o
)
{
    const MATHEX_SCALAR xStart = impl_.boundary().minCorner().x();
    const MATHEX_SCALAR xEnd = impl_.boundary().maxCorner().x();
    const MATHEX_SCALAR xIncr = 50.0;
    const MATHEX_SCALAR yStart = impl_.boundary().minCorner().y();
    const MATHEX_SCALAR yEnd = impl_.boundary().maxCorner().y();
    const MATHEX_SCALAR yIncr = 50.0;

    return traceObstacles( o, USE_ALL,
      xStart, xEnd, xIncr, yStart, yEnd, yIncr );
}

ostream& PhysConfigSpace2d::traceObstacles
(
    ostream& o, UsePolygons /* whichPolygons */,
    MATHEX_SCALAR xStart, MATHEX_SCALAR xEnd, MATHEX_SCALAR /* xIncr */,
    MATHEX_SCALAR yStart, MATHEX_SCALAR yEnd, MATHEX_SCALAR /* yIncr */
)
{
    o << std::endl << "ConfigSpace obstacles " << std::endl << std::endl;

    //  Make sure that each character represents a multiple of 5 metres

    uint    nColumns;

    adjustLimits( &xStart, &xEnd, &yStart, &yEnd, &nColumns );

    UtlAsciiPicture picture( xStart, yStart, xEnd, yEnd, nColumns );

    picture.grid();


    for( PolygonId i = impl_.polygonIdGenerator().begin();
      i != impl_.polygonIdGenerator().end(); ++i )
    {
        if( exists( i ) )
        {
            picture.fill( polygon( i ), '*' );
//            picture.outline( polygon( (PolygonId)(i) ), '.' );
        }
    }

    picture.underText( "*  obstacles\n" );

    //  Add the domains
    for( DomainId i = impl_.domainIdGenerator().begin();
      i != impl_.domainIdGenerator().end(); ++i )
    {
        if( impl_.domains().contains( i ) )
        {
            const PhysCS2dDomain& domain = *impl_.domains()[ i ];
            MexAlignedBox2d boundary;

            domain.boundary( &boundary );
            picture.outline( boundary, '.' );
        }
    }

    picture.underText( ".  domains\n" );

    for( PortalId i = impl_.portalIdGenerator().begin();
      i != impl_.portalIdGenerator().end(); ++i )
    {
        if( portalExists( i ) )
        {
            const PhysCS2dPortal& portal = *impl_.portals()[ i ];
            picture.line( MexLine2d( portal.endPoint1(), portal.endPoint2() ), 'o' );
        }
    }

    picture.underText( "o  portals\n" );

    PhysCS2dDomain* pCachedDomain = NULL;

    for( uint x = 0; x < picture.nColumns(); ++x )
    {
        for( uint y = 0; y < picture.nRows(); ++y )
        {
            const MexPoint2d location = picture.point( x, y );
            if( not pointValid( location, &pCachedDomain ) )
            {
                picture.pixel( x, y, '~' );
            }
        }
    }

    picture.underText( "~  points not in a domain or an obstacle\n" );

    o << picture;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::findSpace( const MexPoint2d& startPoint, const MexPoint2d& targetPoint,
                    MATHEX_SCALAR clearance, MATHEX_SCALAR radius, ObstacleFlags flags,
                    MexPoint2d* pResult )
{
    //Get the distance between the 2 points, and hence find the first test point
    MATHEX_SCALAR distance = startPoint.euclidianDistance( targetPoint );

    MexPoint2d linePoint;
    if( fabs( distance ) < 0.01 )
    {
        linePoint.x( targetPoint.x() + radius );
        linePoint.y( targetPoint.y() + radius );
    }
    else
    {
        MATHEX_SCALAR f = radius / distance;
        linePoint.x( f * startPoint.x() + (1 - f) * targetPoint.x() );
        linePoint.y( f * startPoint.y() + (1 - f) * targetPoint.y() );
    }

    //Try 4 points
    bool doneIt = false;
	DomainId domainId;
    MATHEX_SCALAR dx = linePoint.x() - targetPoint.x();
    MATHEX_SCALAR dy = linePoint.y() - targetPoint.y();

    for( int i = 0; i != 4; ++i )
    {
        MexPoint2d testPoint;

        switch( i )
        {
            case 0:
            {
                testPoint.x( targetPoint.x() + dx );
                testPoint.y( targetPoint.y() + dy );
                break;
            }
            case 1:
            {
                testPoint.x( targetPoint.x() + dy );
                testPoint.y( targetPoint.y() - dx );
                break;
            }
            case 2:
            {
                testPoint.x( targetPoint.x() - dy );
                testPoint.y( targetPoint.y() + dx );
                break;
            }
            case 3:
            {
                testPoint.x( targetPoint.x() - dx );
                testPoint.y( targetPoint.y() - dy );
                break;
            }
        }

        //Construct a circle using desired clearance, and see if intersects any obstacles
        //  TBD: The circle used here should match the circle used in
        //  MachLogMachineMotionSequencer::targetPositionContainedInSpace. I would
        //  prefer them to both run through the same bit of code. Bob

        MexCircle2d testCircle( testPoint, clearance * 1.01 );

        PolygonId id;
        if( contains( testCircle, flags, &id, USE_ALL ) and domain( testPoint, &domainId ) )
        {
            doneIt = true;
            *pResult = testPoint;
            break;
        }

        //Try another point rotated 90 degrees
    }

    POST( implies( doneIt, domain( *pResult, &domainId ) ) );

    return doneIt;
}

//////////////////////////////////////////////////////////////////////////////////////////

PhysConfigSpace2d::DomainFindPathId PhysConfigSpace2d::addDomainFindPath
(
    const MexPoint2d& startPoint, const MexPoint2d& endPoint,
    MATHEX_SCALAR clearance, ObstacleFlags flags, PhysPathFindingPriority priority
)
{
    PRE_DATA( PolygonId polygonId );
    PRE( mode() == SUBTRACTIVE );

    //generate a new id
    DomainFindPathId id = impl_.domainFindPathIdGenerator().next();

    //Construct a new domainFindPath object
    PhysCS2dDomainFindPath* pDomainFindPath =
        _NEW( PhysCS2dDomainFindPath( this, startPoint, endPoint, clearance, flags, priority ) );

    //Add it to the map
    impl_.domainFindPaths().add( id, pDomainFindPath );

    //Add it to the queue, unless already finished
    if( not pDomainFindPath->isFinished() )
        impl_.domainFindPathQueue().push_back( pDomainFindPath );

    return id;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::domainFindPathExists( const DomainFindPathId& id ) const
{
    return impl_.domainFindPaths().contains( id );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::updateDomainFindPath( const DomainFindPathId& id, const PhysRelativeTime& maxTime )
{
    PRE( domainFindPathExists( id ) );

    //Get the CS2dPathFind object pointer from the map
    PhysCS2dDomainFindPath* pIdDomainFindPath = impl_.domainFindPaths()[ id ];

    PhysCS2dImpl::DomainFindPaths& domainFindPathQueue = impl_.domainFindPathQueue();
    if( domainFindPathQueue.size() != 0 )
    {
        //Get first in queue and update it.
        PhysCS2dDomainFindPath* pDomainFindPath = pCurrentDomainFindPath();

//        domainFindPathQueue.front();

        pDomainFindPath->update( maxTime );

        //If complete, remove from queue
        if( pDomainFindPath->isFinished() )
            removeCurrentDomainFindPathFromQueue();
//            domainFindPathQueue.erase( domainFindPathQueue.begin() );
    }

    return pIdDomainFindPath->isFinished();
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::isDomainFindPathDone( const DomainFindPathId& id ) const
{
    PRE( domainFindPathExists( id ) );

    //Get the CS2dDomainFindPath object pointer from the map, and see if finished
    const PhysCS2dDomainFindPath* pDomainFindPath = impl_.domainFindPaths()[ id ];
    return pDomainFindPath->isFinished();
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysConfigSpace2d::removeDomainFindPath( const DomainFindPathId& id )
{
    PRE( domainFindPathExists( id ) );

    //Get the CS2dDomainFindPath object pointer from the map,
    //remove it from the map and queue and delete it
    PhysCS2dDomainFindPath* pDomainFindPath = impl_.domainFindPaths()[ id ];

    if( impl_.hasCurrentDomainFindPath_ and
      ( impl_.pCurrentDomainFindPath_ == pDomainFindPath ) )
    {
        impl_.hasCurrentDomainFindPath_ = false;
    }

    impl_.domainFindPaths().remove( id );
    impl_.domainFindPathIdGenerator().free( id );

    if( not pDomainFindPath->isFinished() )
    {
        PhysCS2dImpl::DomainFindPaths& domainFindPathQueue = impl_.domainFindPathQueue();
        PhysCS2dImpl::DomainFindPaths::iterator it
            = find( domainFindPathQueue.begin(), domainFindPathQueue.end(), pDomainFindPath );
        ASSERT( it != domainFindPathQueue.end(), "" )
        domainFindPathQueue.erase( it );
    }

    _DELETE( pDomainFindPath );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::domainFindPath( const DomainFindPathId& id, PortalPoints* pPortalPoints )
{
    PRE( domainFindPathExists( id ) );
    PRE( isDomainFindPathDone( id ) );

    //Get the CS2dDomainFindPath object pointer from the map
    PhysCS2dDomainFindPath* pDomainFindPath = impl_.domainFindPaths()[ id ];

    //See if we have a path and get it
    bool havePath = pDomainFindPath->output( pPortalPoints );

    //Delete the domainFindPath object
    removeDomainFindPath( id );

    return havePath;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysConfigSpace2d::traceDomainFindPath( const DomainFindPathId& id, ostream& o )
{
    PRE( domainFindPathExists( id ) );
    o << *(impl_.domainFindPaths()[ id ]);
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysConfigSpace2d::add
(
    const PhysMotionChunk& motionChunk, const ObjectId& objectId,
    const ObjectIds& ignoreIds,
    MotionChunkId *pId, ObjectId* pCollisionObjectId,
    PhysAbsoluteTime* pCollideTime
)
{
    PRE( pId != NULL );
    PRE( pCollisionObjectId != NULL );
    PRE( pCollideTime != NULL );

    //Get up a bounding box for the new chunk, and get a vector of all the chunks in
    //the tree which might intersect the new chunk
    MexAlignedBox2d inBoundary;
    motionChunk.boundary( &inBoundary );

    PhysCS2dImpl::MotionChunkTree& motionChunkTree = impl_.motionChunkTree();
    PhysCS2dImpl::MotionChunkTree::PItems motionChunkHolders;
    motionChunkHolders.reserve( 64 );

    size_t n = motionChunkTree.overlapping( inBoundary, &motionChunkHolders );

    //Check for intersections between the incoming chunk and each potentially
    //colliding existing chunk
    bool ok = true;
    for( size_t i = 0; i != n; ++i)
    {
        const PhysCS2dMotionChunk& holder = *(motionChunkHolders[i]);

        const bool ignore = find(
            ignoreIds.begin(), ignoreIds.end(),
            holder.objectId() ) != ignoreIds.end();

        if( not ignore )
        {
            PhysAbsoluteTime oldTime;
            const PhysMotionChunk& oldMotionChunk = holder.motionChunk();
            if( motionChunk.intersects( oldMotionChunk, &oldTime )  and
                (ok  or  oldTime < *pCollideTime ) )
            {
                ok = false;
                *pCollideTime = oldTime;
                *pCollisionObjectId = holder.objectId();
                //JON_STREAM("-------------> Motion chunk intersection" << std::endl << motionChunk << oldMotionChunk;)
            }
        }
    }

    //If no collisions add the chunk
    if( ok )
        *pId = add( motionChunk, objectId );

    return ok;
}

bool PhysConfigSpace2d::motionChunkExists( const MotionChunkId& id ) const
{
    return impl_.motionChunks().contains( id );
}

void PhysConfigSpace2d::removeMotionChunk( const MotionChunkId& id )
{
    PRE( motionChunkExists( id ) );

    //Get the holder pointer
    PhysCS2dImpl::MotionChunkMap& motionChunks = impl_.motionChunks();
    PhysCS2dMotionChunk* pHolder = motionChunks[ id ];

    //Remove from its BSP tree
    impl_.motionChunkTree().remove( pHolder );

    //Remove from the map
    motionChunks.remove( id );

    //Free the chunk id
    impl_.motionChunkIdGenerator().free( id );

    //Delete it
    _DELETE( pHolder );
}

const PhysMotionChunk& PhysConfigSpace2d::motionChunk( const MotionChunkId& id ) const
{
    PRE( motionChunkExists( id ) );

    return (impl_.motionChunks()[ id ])->motionChunk();
}

const PhysConfigSpace2d::ObjectId& PhysConfigSpace2d::motionChunkObjectId
(
    const MotionChunkId& id
) const
{
    PRE( motionChunkExists( id ) );

    return (impl_.motionChunks()[ id ])->objectId();
}

MATHEX_SCALAR PhysConfigSpace2d::portalLength( const PortalId& id ) const
{
    PRE( mode() == SUBTRACTIVE );
    PRE( portalExists( id ) );

    //Get the portal
    PhysCS2dPortal& portal = *(impl_.portals()[ id ]);

    //Return the length
    return portal.length();
}

MexPoint2d PhysConfigSpace2d::portalPoint( const PortalId& id, MATHEX_SCALAR distance ) const
{
    PRE( mode() == SUBTRACTIVE );
    PRE( portalExists( id ) );

    //Get the portal
    PhysCS2dPortal& portal = *(impl_.portals()[ id ]);

    //get the endpoints and length
    const MexPoint2d& p1 = portal.endPoint1();
    const MexPoint2d& p2 = portal.endPoint2();
    MATHEX_SCALAR length = portal.length();

    //Hence return a point the appropriate distance between
    return MexPoint2d( p1, p2, distance / length );
}

void PhysConfigSpace2d::findPathIgnorePolygons( const FindPathId& id, const PolygonIds& polygons )
{
    PRE( findPathExists( id ) );

    //Pass the info on to the findpath object
    PhysCS2dFindPath* pFindPath = impl_.findPaths()[ id ];
    pFindPath->ignorePolygons( polygons );
}

void PhysConfigSpace2d::findIntersections(
    const PhysMotionChunk& motionChunk,
    const ObjectId& objectId,
    Intersections* pIntersections ) const
{
    PRE( pIntersections != NULL );

	CONFIG_SPACE_STREAM( "Enter findIntersections" << std::endl );
	CONFIG_SPACE_INDENT( 2 );

	CONFIG_SPACE_INSPECT( objectId );
	CONFIG_SPACE_INSPECT( motionChunk );

    //Get up a bounding box for the new chunk, and get a vector of all the chunks in
    //the tree which might intersect the new chunk
    MexAlignedBox2d inBoundary;
    motionChunk.boundary( &inBoundary );

    PhysCS2dImpl::MotionChunkTree& motionChunkTree = impl_.motionChunkTree();
    PhysCS2dImpl::MotionChunkTree::PItems motionChunkHolders;
    motionChunkHolders.reserve( 64 );

    size_t n = motionChunkTree.overlapping( inBoundary, &motionChunkHolders );

	CONFIG_SPACE_INSPECT( n );

    //Check for intersections between the incoming chunk and each potentially
    //colliding existing chunk

    for( size_t i = 0; i != n; ++i)
    {
        const PhysCS2dMotionChunk& holder = *(motionChunkHolders[i]);

		CONFIG_SPACE_INSPECT( holder.objectId() );

        if( holder.objectId() != objectId )
        {
            PhysAbsoluteTime oldTime;
            const PhysMotionChunk& oldMotionChunk = holder.motionChunk();

			CONFIG_SPACE_INSPECT( oldMotionChunk );

            if( motionChunk.intersects( oldMotionChunk, &oldTime ) )
            {
				CONFIG_SPACE_STREAM( "Intersects at time " << oldTime << std::endl );

                pIntersections->push_back( IntersectionData( holder.objectId(), oldTime ) );
            }
        }
    }

	CONFIG_SPACE_INDENT( -2 );
	CONFIG_SPACE_STREAM( "Exit findIntersections" << std::endl );
}

bool PhysConfigSpace2d::contains( const MexSausage2d& sausage, ObstacleFlags flags, PolygonIds* pIds,
                                  UsePolygons whichPolygons ) const
{
    PRE( mode() == SUBTRACTIVE );

    //Construct a bounding box for the polygon
    MexAlignedBox2d sausageBoundary;
    sausage.boundary( &sausageBoundary );

    //Check inside the outer boundary
    bool inSet = impl_.boundary().contains( sausageBoundary );

    //Check for containment with the appropriate BSP trees of polygons
    if( inSet )
    {
        PolygonIds ids;

        //See if the polygon intersects any polygon
        if( whichPolygons == USE_PERMANENT_ONLY or whichPolygons == USE_ALL )
            inSet = not impl_.intersects( sausage, sausageBoundary, pIds, 10000000L,
                                          impl_.permanentPolygonTree(),
                                          Mathex::TOUCH_ISNT_INTERSECT, flags );

        if( inSet and
            (whichPolygons == USE_TEMPORARY_ONLY or whichPolygons == USE_ALL) )
            inSet = not impl_.intersects( sausage, sausageBoundary, pIds, 10000000L,
                                          impl_.temporaryPolygonTree(),
                                          Mathex::TOUCH_ISNT_INTERSECT, flags );
    }
    else pIds->push_back( 0 );

    return inSet;
}

bool PhysConfigSpace2d::contains( const MexCircle2d& circle, ObstacleFlags flags, PolygonId *pId,
                                  UsePolygons whichPolygons ) const
{
    PRE( mode() == SUBTRACTIVE );

    //Construct a bounding box for the polygon
    MexAlignedBox2d circleBoundary;
    circle.boundary( &circleBoundary );

    //Check inside the outer boundary
    bool inSet = impl_.boundary().contains( circleBoundary );

    ASSERT_INFO( circle );
    ASSERT_INFO( circleBoundary );
    ASSERT_INFO( inSet );

    //Check for containment with the appropriate BSP trees of polygons
    if( inSet )
    {
        PolygonIds ids;
        ids.reserve( 2 );

        //See if the polygon intersects any polygon
        if( whichPolygons == USE_PERMANENT_ONLY or whichPolygons == USE_ALL )
            inSet = not impl_.intersects( circle, circleBoundary, &ids, 1,
                                          impl_.permanentPolygonTree(),
                                          Mathex::TOUCH_ISNT_INTERSECT, flags );

        if( inSet and
            (whichPolygons == USE_TEMPORARY_ONLY or whichPolygons == USE_ALL) )
            inSet = not impl_.intersects( circle, circleBoundary, &ids, 1,
                                          impl_.temporaryPolygonTree(),
                                          Mathex::TOUCH_ISNT_INTERSECT, flags );

        if( not inSet ) *pId = ids[0];
    }
    else *pId = 0;

    return inSet;
}

void PhysConfigSpace2d::findIntersections(
    const PhysMotionChunk& motionChunk,
    const ObjectId& objectId,
    ChunkIntersections* pIntersections ) const
{
    PRE( pIntersections != NULL );

    //Get up a bounding box for the new chunk, and get a vector of all the chunks in
    //the tree which might intersect the new chunk
    MexAlignedBox2d inBoundary;
    motionChunk.boundary( &inBoundary );

    PhysCS2dImpl::MotionChunkTree& motionChunkTree = impl_.motionChunkTree();
    PhysCS2dImpl::MotionChunkTree::PItems motionChunkHolders;
    motionChunkHolders.reserve( 64 );

    size_t n = motionChunkTree.overlapping( inBoundary, &motionChunkHolders );

    //Check for intersections between the incoming chunk and each potentially
    //colliding existing chunk

    for( size_t i = 0; i != n; ++i)
    {
        const PhysCS2dMotionChunk& holder = *(motionChunkHolders[i]);

        if( holder.objectId() != objectId )
        {
            PhysAbsoluteTime oldTime;
            const PhysMotionChunk& oldMotionChunk = holder.motionChunk();
            if( motionChunk.intersects( oldMotionChunk, &oldTime ) )
            {
                pIntersections->push_back( ChunkIntersectionData( holder.objectId(), holder.id(), oldTime ) );
            }
        }
    }
}

PhysConfigSpace2d::MotionChunkId PhysConfigSpace2d::add
(
    const PhysMotionChunk& motionChunk, const ObjectId& objectId
)
{
	CONFIG_SPACE_STREAM( "Enter add" << std::endl );

    //generate an id for the chunk
    MotionChunkId id = impl_.motionChunkIdGenerator().next();

    //Construct a new holder for the chunk
    PhysCS2dMotionChunk* pHolder = _NEW( PhysCS2dMotionChunk( id, objectId, motionChunk) );

	CONFIG_SPACE_INSPECT( *pHolder );

    //Store in the motion chunk map
    impl_.motionChunks().add( id, pHolder );

    //Add to the BSP tree
    PhysCS2dImpl::MotionChunkTree& motionChunkTree = impl_.motionChunkTree();
    motionChunkTree.add( pHolder );

	CONFIG_SPACE_STREAM( "Exit add" << std::endl );

    return id;
}

const MexAlignedBox2d& PhysConfigSpace2d::boundary() const
{
    return impl_.boundary();
}

bool PhysConfigSpace2d::findSpace( const MexPoint2d& targetPoint,
                    MATHEX_SCALAR clearance, MATHEX_SCALAR radius, ObstacleFlags flags,
                    MexPoint2d* pResult )
{

    //  All we need to do is construct a random start point then we can use the
    //  normal findSpace method.
    MexBasicRandom r = MexBasicRandom::constructSeededFromTime();

    MexVec2 offset;
    do
    {
        offset.setVector( mexRandomScalar( &r, -1.0, 10.0 ),
          mexRandomScalar( &r, -1.0, 10.0 ) );
    } while( offset.isZeroVector() );

    MexPoint2d startPoint = targetPoint;
    startPoint += offset;

    return findSpace( startPoint, targetPoint, clearance, radius, flags, pResult );

}

PhysCS2dDomainFindPath* PhysConfigSpace2d::pCurrentDomainFindPath()
{
    PRE( impl_.domainFindPathQueue().size() > 0 );

    PhysCS2dDomainFindPath* result = NULL;

    PhysCS2dImpl::DomainFindPaths& domainFindPathQueue = impl_.domainFindPathQueue();

    if( impl_.hasCurrentDomainFindPath_ )
    {
        result = impl_.pCurrentDomainFindPath_;
    }
    else
    {
        //  Find the domain find path with the highest priority
        impl_.pCurrentDomainFindPath_ = domainFindPathQueue[ 0 ];
        PhysPathFindingPriority highestPriority = domainFindPathQueue[ 0 ]->priority();

        for( size_t i = 0; i < domainFindPathQueue.size(); ++i )
        {
            if( domainFindPathQueue[ i ]->priority() > highestPriority )
            {
                impl_.pCurrentDomainFindPath_ = domainFindPathQueue[ i ];
                highestPriority = domainFindPathQueue[ i ]->priority();
            }

            domainFindPathQueue[ i ]->increasePriority();
        }

        result = impl_.pCurrentDomainFindPath_;
        impl_.hasCurrentDomainFindPath_ = true;
    }

    POST( impl_.hasCurrentDomainFindPath_ );
    POST( result != NULL );

    return result;
}

void PhysConfigSpace2d::removeCurrentDomainFindPathFromQueue()
{
    PRE( impl_.hasCurrentDomainFindPath_ );

    PhysCS2dImpl::DomainFindPaths& domainFindPathQueue = impl_.domainFindPathQueue();

    PhysCS2dImpl::DomainFindPaths::iterator it
        = find( domainFindPathQueue.begin(), domainFindPathQueue.end(), impl_.pCurrentDomainFindPath_ );

    ASSERT( it != domainFindPathQueue.end(), "" )

    domainFindPathQueue.erase( it );

    impl_.hasCurrentDomainFindPath_ = false;

    POST( not impl_.hasCurrentDomainFindPath_ );
}

PhysCS2dFindPath* PhysConfigSpace2d::pCurrentFindPath()
{
    CS2VGRA_STREAM("PhysConfigSpace2d::pCurrentFindPath entry " << (void*)this << std::endl);
    CS2VGRA_INDENT( 2 );

    PRE( impl_.findPathQueue().size() > 0 );

    PhysCS2dFindPath* result = NULL;

    PhysCS2dImpl::FindPaths& findPathQueue = impl_.findPathQueue();

    if( impl_.hasCurrentFindPath_ )
    {
        result = impl_.pCurrentFindPath_;
    }
    else
    {
        //  Find the find path with the highest priority
        impl_.pCurrentFindPath_ = findPathQueue[ 0 ];
        PhysPathFindingPriority highestPriority = findPathQueue[ 0 ]->priority();

        for( size_t i = 0; i < findPathQueue.size(); ++i )
        {
            CS2VGRA_INSPECT( i );
            CS2VGRA_INSPECT( findPathQueue[ i ]->priority() );
            CS2VGRA_INSPECT( (void*)findPathQueue[ i ] );
            CS2VGRA_INSPECT( (void*)impl_.pCurrentFindPath_ );

            if( findPathQueue[ i ]->priority() > highestPriority )
            {
                impl_.pCurrentFindPath_ = findPathQueue[ i ];
                highestPriority = findPathQueue[ i ]->priority();
            }

            findPathQueue[ i ]->increasePriority();
        }

        result = impl_.pCurrentFindPath_;
        impl_.hasCurrentFindPath_ = true;
    }

    POST( impl_.hasCurrentFindPath_ );
    POST( result != NULL );

    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM("PhysConfigSpace2d::pCurrentFindPath entry " << (void*)this << std::endl);

    return result;
}

void PhysConfigSpace2d::removeCurrentFindPathFromQueue()
{
    PRE( impl_.hasCurrentFindPath_ );

    PhysCS2dImpl::FindPaths& findPathQueue = impl_.findPathQueue();

    PhysCS2dImpl::FindPaths::iterator it
        = find( findPathQueue.begin(), findPathQueue.end(), impl_.pCurrentFindPath_ );

    ASSERT( it != findPathQueue.end(), "" )
    findPathQueue.erase( it );

    impl_.hasCurrentFindPath_ = false;

    POST( not impl_.hasCurrentFindPath_ );
}

void PhysConfigSpace2d::bufferPolygonPortalUpdates( bool doBuffer )
{
    impl_.bufferPolygonPortalUpdates_ = doBuffer;

    //Clear any outstanding updates if not buffering any longer
    if( not doBuffer )
        impl_.updateDomainRegion();
}

bool PhysConfigSpace2d::bufferPolygonPortalUpdates()
{
    return impl_.bufferPolygonPortalUpdates_;
}

void PhysConfigSpace2d::findIntersections(
    const MexAlignedBox2d& box,
    ChunkIntersectionsNoTime* pIntersections ) const
{
    PRE( pIntersections != NULL );

    //Get a vector of all the chunks in
    //the tree which intersect the box

    PhysCS2dImpl::MotionChunkTree& motionChunkTree = impl_.motionChunkTree();
    PhysCS2dImpl::MotionChunkTree::PItems motionChunkHolders;
    motionChunkHolders.reserve( 64 );

    size_t n = motionChunkTree.overlapping( box, &motionChunkHolders );
    const MexConvexPolygon2d polygon( box );

    for( size_t i = 0; i != n; ++i)
    {
        const PhysCS2dMotionChunk& holder = *(motionChunkHolders[i]);
        const PhysMotionChunk& motionChunk = holder.motionChunk();
        const MexSausage2d sausage2d(
            motionChunk.startPoint(),
            motionChunk.endPoint(),
            motionChunk.clearance() );

        if( sausage2d.intersects( polygon ) )
            pIntersections->push_back( ChunkIntersectionDataNoTime( holder.objectId(), holder.id() ) );
    }
}

bool PhysConfigSpace2d::intersectsWithMotionChunk(
    const MexAlignedBox2d& box ) const
{
    //Get a vector of all the chunks in
    //the tree which intersect the box

    PhysCS2dImpl::MotionChunkTree& motionChunkTree = impl_.motionChunkTree();
    PhysCS2dImpl::MotionChunkTree::PItems motionChunkHolders;
    motionChunkHolders.reserve( 64 );

    size_t n = motionChunkTree.overlapping( box, &motionChunkHolders );

    const MexConvexPolygon2d polygon( box );

    bool intersects = false;

    for( size_t i = 0; i != n and not intersects; ++i)
    {
        const PhysCS2dMotionChunk& holder = *(motionChunkHolders[i]);

        const PhysMotionChunk& motionChunk = holder.motionChunk();
        const MexSausage2d sausage2d(
            motionChunk.startPoint(),
            motionChunk.endPoint(),
            motionChunk.clearance() );

        intersects = sausage2d.intersects( polygon );
    }

    return intersects;
}

bool PhysConfigSpace2d::add(
    const PhysMotionChunk& motionChunkToAdd,
    const ObjectId& objectId,
    const ObjectIds& ignoreIds,
    const ChunkIntersectionsNoTime& checkChunks,
    MotionChunkId *pId,
    ObjectId* pCollisionObjectId,
    PhysAbsoluteTime* pCollideTime )
{
    bool ok = true;

    for( size_t i = 0; i != checkChunks.size(); ++i)
    {
        const MotionChunkId testMotionChunkId = checkChunks[ i ].collisionChunkId();
        const ObjectId testObjectId = checkChunks[ i ].collisionObjectId();

        const bool ignore = find(
            ignoreIds.begin(), ignoreIds.end(),
            testObjectId ) != ignoreIds.end();

        if( not ignore )
        {
            PhysAbsoluteTime testTime;

            const PhysMotionChunk& testMotionChunk = motionChunk( testMotionChunkId );

            if( motionChunkToAdd.intersects( testMotionChunk, &testTime )  and
                (ok  or  testTime < *pCollideTime ) )
            {
                ok = false;
                *pCollideTime = testTime;
                *pCollisionObjectId = testObjectId;
            }
        }
    }

    //If no collisions add the chunk
    if( ok )
        *pId = add( motionChunkToAdd, objectId );

    return ok;

}

void PhysConfigSpace2d::findIntersections(
    const MexSausage2d& sausage,
    const ObjectIds& ignoreObjectIds,
    ChunkIntersectionsNoTime* pChunkIntersections ) const
{
    PRE( pChunkIntersections != NULL );

    //Get a vector of all the chunks in
    //the tree which intersect the bouding volume of the sausage

    MexAlignedBox2d sausageBoundary;
    sausage.boundary( &sausageBoundary );

    const PhysCS2dImpl::MotionChunkTree& motionChunkTree = impl_.motionChunkTree();

    PhysCS2dImpl::MotionChunkTree::PItems motionChunkHolders;
    motionChunkHolders.reserve( 64 );
    const size_t n = motionChunkTree.overlapping( sausageBoundary, &motionChunkHolders );

    for( size_t i = 0; i != n; ++i)
    {
        const PhysCS2dMotionChunk& holder = *(motionChunkHolders[i]);

        const bool ignore = find(
            ignoreObjectIds.begin(), ignoreObjectIds.end(),
            holder.objectId() ) != ignoreObjectIds.end();

        if( not ignore )
        {
            const PhysMotionChunk& motionChunk = holder.motionChunk();
            const MexSausage2d motionChunkSausage(
                motionChunk.startPoint(),
                motionChunk.endPoint(),
                motionChunk.clearance() );

            if( sausage.intersects( motionChunkSausage ) )
                pChunkIntersections->push_back( ChunkIntersectionDataNoTime( holder.objectId(), holder.id() ) );
        }
    }
}

void PhysConfigSpace2d::traceIllegalPoints( ostream& o ) const
{
    o << "Start tracing illegal points" << std::endl;

    //  By offsetting the start points by a random number we will get better coverage
    //  over multiple runs of the program
    MexBasicRandom r = MexBasicRandom::constructSeededFromTime();

    const MATHEX_SCALAR xIncr = 1.0;
    const MATHEX_SCALAR xStart = impl_.boundary().minCorner().x() +
      mexRandomScalar( &r, 0.0, xIncr );
    const MATHEX_SCALAR xEnd = impl_.boundary().maxCorner().x();
    const MATHEX_SCALAR yIncr = 1.0;
    const MATHEX_SCALAR yStart = impl_.boundary().minCorner().y() +
      mexRandomScalar( &r, 0.0, yIncr );
    const MATHEX_SCALAR yEnd = impl_.boundary().maxCorner().y();

    //  By keeping a record of where the invalid points have turned up we
    //  avoid printing multiple (i.e. thousands) of messages for the same
    //  area
    ctl_vector< bool > line;

    line.reserve( ( yEnd - yStart ) / yIncr );

    PhysCS2dDomain* pCachedDomain = NULL;

    for( MATHEX_SCALAR y = yStart; y < yEnd; y += yIncr )
    {
        uint xIndex = 0;
        for( MATHEX_SCALAR x = xStart; x < xEnd; x += xIncr, ++xIndex )
        {
            const MexPoint2d location( x, y );

            const bool valid = pointValid( location, &pCachedDomain );

            if( not valid )
            {
                bool printWarning = true;

                //  Check the previous point on this line
                if( xIndex > 0 and not line[ xIndex - 1 ] )
                    printWarning = false;

                //  Check the point immediately above this one
                if( line.size() > xIndex and not line[ xIndex ] )
                    printWarning = false;

                if( printWarning )
                    o << "Point " << location << " not in obstacle or domain" << std::endl;
            }

            if( line.size() > xIndex )
                line[ xIndex ] = valid;
            else
                line.push_back( valid );

        }
    }

    o << "Finish tracing illegal points" << std::endl;
}

bool PhysConfigSpace2d::pointValid(
    const MexPoint2d& location,
    PhysCS2dDomain** ppCachedDomain ) const
{
    bool result = false;

    //  Caching gives us a speed increase because of spatial coherence
    if( *ppCachedDomain != NULL )
    {
        if( (*ppCachedDomain)->contains( location ) )
            result = true;
    }

    if( not result )
    {
        DomainId domainId;
        const bool inDomain = domain( location, &domainId ) != 0;

        if( inDomain )
        {
            result = true;
            (*ppCachedDomain) = impl_.domains_[ domainId ];
        }
        else
        {
            const ObstacleFlags flags = 0;
            PolygonId id;
            const bool inObstacle = not contains( location, flags, &id );

            if( inObstacle )
                result = true;

            *ppCachedDomain = NULL;
        }
    }

    return result;
}

void PhysConfigSpace2d::adjustLimits(
    MATHEX_SCALAR* pXStart, MATHEX_SCALAR* pXEnd,
    MATHEX_SCALAR* pYStart, MATHEX_SCALAR* pYEnd,
    uint* pNColumns )
{
    const MATHEX_SCALAR xRange = *pXEnd - *pXStart;
    const uint maxColumns = 110;
    const MATHEX_SCALAR basicUnit = 5.0;

    MATHEX_SCALAR t1 = xRange / maxColumns;
    t1 /= basicUnit;
    t1 = ceil( t1 );
    const MATHEX_SCALAR width = t1 * basicUnit;
    *pNColumns = ceil( xRange / width );

    ASSERT_INFO( *pNColumns );
    ASSERT_INFO( width );
    ASSERT_INFO( *pNColumns * width );
    ASSERT_INFO( xRange );
    ASSERT( *pNColumns * width > xRange, "" );

    //  Adjust the max. x value to fit exactly in the columns we want
    *pXEnd = *pXStart + *pNColumns * width;

    //  Round up the y range to the next multiple of our basic unit

    MATHEX_SCALAR yRange = *pYEnd - *pYStart;

    yRange /= basicUnit;
    yRange = ceil( yRange );
    yRange *= basicUnit;

    *pYEnd = *pYStart + yRange;

    ++(*pNColumns);
}

void PhysConfigSpace2d::findIntersections(
    const MexSausage2d& sausage,
    const MexDouble& minHeight,
    const MexDouble& maxHeight,
    PhysAbsoluteTime startTime,
    PhysAbsoluteTime endTime,
    const ObjectIds& ignoreObjectIds,
    ObjectIds* pIntersectionIds ) const
{
    PRE( pIntersectionIds != NULL );

    CONFIG_SPACE_STREAM( "Find intersections" << std::endl );
    CONFIG_SPACE_INSPECT( sausage );
    CONFIG_SPACE_INSPECT( minHeight );
    CONFIG_SPACE_INSPECT( maxHeight );
    CONFIG_SPACE_INSPECT( startTime );
    CONFIG_SPACE_INSPECT( endTime );

    //Get a vector of all the chunks in
    //the tree which intersect the bouding volume of the sausage

    MexAlignedBox2d sausageBoundary;
    sausage.boundary( &sausageBoundary );

    const PhysCS2dImpl::MotionChunkTree& motionChunkTree = impl_.motionChunkTree();

    PhysCS2dImpl::MotionChunkTree::PItems motionChunkHolders;
    motionChunkHolders.reserve( 64 );
    const size_t n = motionChunkTree.overlapping( sausageBoundary, &motionChunkHolders );

	CONFIG_SPACE_INSPECT( n );

    for( size_t i = 0; i != n; ++i)
    {
        const PhysCS2dMotionChunk& holder = *(motionChunkHolders[i]);
        const PhysMotionChunk& motionChunk = holder.motionChunk();

		CONFIG_SPACE_INSPECT( motionChunk );

        bool ignore = not motionChunk.timeIntersects( startTime, endTime );

        if( not ignore )
        {
			CONFIG_SPACE_WHERE;

            if( maxHeight < motionChunk.minHeight() or
                minHeight > motionChunk.maxHeight() )
            {
                ignore = true;
            }
        }

        if( not ignore )
        {
			CONFIG_SPACE_WHERE;
            ignore = find(
                ignoreObjectIds.begin(), ignoreObjectIds.end(),
                holder.objectId() ) != ignoreObjectIds.end();
        }

        if( not ignore )
        {
			CONFIG_SPACE_WHERE;
            const MexSausage2d motionChunkSausage(
                motionChunk.startPoint(),
                motionChunk.endPoint(),
                motionChunk.clearance() );

			CONFIG_SPACE_INSPECT( motionChunkSausage );

            if( sausage.intersects( motionChunkSausage ) )
            {
                pIntersectionIds->push_back( holder.objectId() );

                CONFIG_SPACE_STREAM( "Intersected with motion chunk:" << std::endl << motionChunk );
            }
        }
    }

	CONFIG_SPACE_INSPECT( *pIntersectionIds );
}

bool PhysConfigSpace2d::add(
    const PhysMotionChunk& motionChunk,
    const ObjectId& objectId,
    MotionChunkId *pId,
    ObjectId* pCollisionObjectId,
    PhysAbsoluteTime* pCollideTime )
{
    ObjectIds ignoreIds;

    ignoreIds.push_back( objectId );

    return add(
        motionChunk,
        objectId,
        ignoreIds,
        pId,
        pCollisionObjectId,
        pCollideTime );
}

/* End CSPACE2.CPP **************************************************/
