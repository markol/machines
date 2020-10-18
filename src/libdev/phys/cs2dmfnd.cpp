/*
 * C S 2 D M F N D . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

//#include "mathex/epsilon.hpp"
#include "mathex/vec2.hpp"
#include "mathex/poly2d.hpp"

#include "phys/trace.hpp"
#include "phys/phys.hpp"
#include "phys/internal/cs2impl.hpp"
#include "phys/internal/cs2dmfnd.hpp"
#include "phys/internal/cs2dmcac.hpp"
#include "profiler/profiler.hpp"

#ifndef _INLINE
    #include "phys/internal/cs2dmfnd.ipp"
#endif

#ifdef PRODUCTION
    #define CS2PATH_ENTER( fn )
    #define CS2PATH_EXIT( fn )
#else
    #define CS2PATH_ENTER( fn ) CS2PATH_STREAM( "Enter PhysCS2dDomainFindPath::" << fn << " " << (void*)this << std::endl ),CS2PATH_INDENT( 2 )
    #define CS2PATH_EXIT( fn )  CS2PATH_INDENT( -2 ),CS2PATH_STREAM( "Exit PhysCS2dDomainFindPath::" << fn << " " << (void*)this << std::endl )
#endif

//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dDomainFindPath::PhysCS2dDomainFindPath()
:   pConfigSpace_( NULL ),
    state_( UNDEFINED ),
    pDomainAlg_( NULL ),
    foundPath_( false ),
    flags_( 0 ),
    priority_(-1)
{
    //Should never be used

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dDomainFindPath::PhysCS2dDomainFindPath
(
    PhysConfigSpace2d* pSpace, const MexPoint2d& startPoint, const MexPoint2d& endPoint,
    MATHEX_SCALAR clearance,
    ObstacleFlags flags,
    PhysPathFindingPriority priority
)
:   pConfigSpace_( pSpace ),
    startPoint_( startPoint ),
    endPoint_( endPoint ),
    state_( NOT_STARTED ),
    pDomainAlg_( NULL ),
    clearance_( clearance ),
    flags_( flags ),
    priority_( priority )
{
    CS2PATH_ENTER( "PhysCS2dDomainFindPath" );

    CS2PATH_INSPECT( startPoint );
    CS2PATH_INSPECT( endPoint );
    CS2PATH_INSPECT( clearance );
    CS2PATH_INSPECT( flags );

    PRE( pSpace != NULL );
    PRE( startPoint.euclidianDistance( endPoint ) > MexEpsilon::instance() );

    //Ensure suitable collection sizes
    domainPath_.reserve( 32 );

    //Do initial processing
    start();

    TEST_INVARIANT;

    CS2PATH_EXIT( "PhysCS2dDomainFindPath" );
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dDomainFindPath::~PhysCS2dDomainFindPath()
{
    CS2PATH_ENTER( "~PhysCS2dDomainFindPath" );

    TEST_INVARIANT;

    //Extra work required in some states
    switch( state_ )
    {
        case MACRO: { endMacroSearch(); break; }
    }

    //Delete owned entities
    _DELETE( pDomainAlg_ );

    CS2PATH_EXIT( "~PhysCS2dDomainFindPath" );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dDomainFindPath::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dDomainFindPath::start( void )
{
    CS2PATH_ENTER( "start" );

    PRE( state_ == NOT_STARTED );

    //See if start/end points lie in different domains
    //If not, the state will be set to finished, otherwise PENDING_MACRO.
    //DONT call startMacroSearch here, because client may have a queue, and that would
    //lock up.
    needMacroSearch();
    CS2PATH_EXIT( "end" );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dDomainFindPath::needMacroSearch( void )
{
    CS2PATH_ENTER( "needMacroSearch" );
    PRE( state_ == NOT_STARTED );

    //Store the domainGraph version
    const PhysCS2dDomainGraph& domainGraph = pConfigSpace_->impl().domainGraph();
    domainGraphVersion_ = domainGraph.version();

    //Get the domain ids in which the start and end points lie
    bool startInADomain = pConfigSpace_->domain( startPoint_, &startDomainId_ );
    bool endInADomain = pConfigSpace_->domain( endPoint_, &endDomainId_ );

    //Both points must be in a domain to be able to find a path
    bool needed = false;
    if( startInADomain and endInADomain )
    {
        //If in same domain then the empty path is the one we want
        if( startDomainId_ == endDomainId_ )
        {
            state_ = FINISHED;
            foundPath_ = true;
        }
        else
        {
            needed = true;
            state_ = PENDING_MACRO;
        }
    }
    else
        state_ = FINISHED;

    CS2PATH_EXIT( "needMacroSearch" );
    return needed;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dDomainFindPath::startMacroSearch( void )
{
    CS2PATH_ENTER( "startMacroSearch" );

    PRE( state_ == PENDING_MACRO );

    if( PhysCS2dDomainFindPathCache::instance().findPath(
      startDomainId_, startPoint_,
      endDomainId_, endPoint_,
      clearance_, flags(), &domainPath_ ) )
    {
        state_ = FINISHED;
        foundPath_ = true;
    }
    else
    {
        //If the domain graph can be locked, lock it
        PhysCS2dImpl& impl = *(pConfigSpace_->pImpl());
        if( not impl.domainGraphInUse() )
        {
            //Enter the macro search phase
            state_ = MACRO;

            //Ensure the domain vertex graph is up to date with respect to any permanent
            //polygons that have been added
            impl.updateDomainRegion();

            //Lock the graph and set this as the active macro findPath
            impl.domainGraphInUse( true );
            impl.activeDomainFindPath( this );

            //Add domain vertices for the start and end points in their respective domains
            startDomainVertexId_ = impl.addDomainVertex( startPoint_, clearance_, flags_ );
            endDomainVertexId_ = impl.addDomainVertex( endPoint_, clearance_, flags_ );

            //Join these vertices to all other domain vertices in the same domain
            impl.addDomainArcs( startDomainVertexId_, startDomainId_ );
            impl.addDomainArcs( endDomainVertexId_, endDomainId_ );

            //Store the domainGraph version
            domainGraphVersion_ = impl.domainGraph().version();

            //Construct an A* algorithm for the search
            pDomainAlg_ = _NEW( DomainAStarAlg( clearance(), flags() ) );
            pDomainAlg_->start( impl.domainGraph(), startDomainVertexId_, endDomainVertexId_ );
        }
    }

    CS2PATH_EXIT( "startMacroSearch" );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dDomainFindPath::updateMacroSearch( const PhysRelativeTime& maxTime )
{
    CS2PATH_ENTER( "updateMacroSearch" );

    PRE( state_ == MACRO );
    PRE( pDomainAlg_ != NULL );

    //Update the algorithm
    PhysAbsoluteTime entryTime = Phys::time();

    //If the domain graph needs to be updated, or has been updated, we must cancel the current search
    PhysCS2dImpl* pSpaceImpl = pConfigSpace_->pImpl();
    bool graphChanged = domainGraphVersion_ != pSpaceImpl->domainGraph().version();
    if( graphChanged or pSpaceImpl->domainUpdateRegionExists() )
    {
        //Cancel the search
        endMacroSearch();
        state_ = NOT_STARTED;

        //Update the graph
        pConfigSpace_->pImpl()->updateDomainRegion();
    }
    else
    {
        do
        {
            if( pDomainAlg_->update() == DomainAStarAlg::FINISHED ) break;
        }
        while( Phys::time() - entryTime < maxTime );
    }

    CS2PATH_EXIT( "updateMacroSearch" );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dDomainFindPath::isMacroSearchFinished( void ) const
{
    PRE( state_ == MACRO );
    PRE( pDomainAlg_ != NULL );

    return pDomainAlg_->isFinished();
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dDomainFindPath::endMacroSearch( void )
{
    CS2PATH_ENTER( "endMacroSearch" );

    PRE( state_ == MACRO );
    PRE( pDomainAlg_ != NULL );
    PRE( domainPath_.size() == 0 );

    //If the A* algorithm finished, we can get the results
    PhysCS2dImpl& impl = *(pConfigSpace_->pImpl());

    if( pDomainAlg_->isFinished() and domainGraphVersion_ == impl.domainGraph().version() )
    {
        //Get the results from the A* algorithm
        DomainAStarAlg::Vertices domainVertexIds = pDomainAlg_->output();
        foundPath_ = domainVertexIds.size() != 0;

        //Stuff the coordinates into the output buffer
        for( DomainAStarAlg::Vertices::iterator it = domainVertexIds.begin();
             it != domainVertexIds.end(); ++it )
        {
            //Access the path point
            PhysConfigSpace2d::DomainVertexId domainVertexId = *it;

            //Don't output the start and end points
            if( domainVertexId != startDomainVertexId_ and
                domainVertexId != endDomainVertexId_ )
            {
                //Get the domain vertex
                const PhysCS2dDomainVertex& domainVertex =
                    impl.domainGraph().vertex( domainVertexId );

                //hence get the portal
                PortalId portalId = domainVertex.portalId();
                const PhysCS2dPortal& portal = *(impl.portals()[ portalId ]);

                //Compute the distance of the vertex along the portal
                MexVec2 end1ToVtx( portal.endPoint1(), domainVertex.point() );
                MexVec2 portalDirection( portal.endPoint1(), portal.endPoint2(), portal.length() );
                MATHEX_SCALAR distance = end1ToVtx.dotProduct( portalDirection );

                //Add this point to the result path
                domainPath_.push_back( PortalPoint( portalId, distance ) );
            }
        }

        //  Cache the path for possible later use
        PhysCS2dDomainFindPathCache::instance().addPath(
            startDomainId_, startPoint_, endDomainId_,
            endPoint_, clearance_, flags(), domainPath_ );
    }

    //Delete the algorithm
    _DELETE( pDomainAlg_ );
    pDomainAlg_ = NULL;

    //Remove the extra domain vertices from the domain graph that we added for our
    //start and end points.
    impl.removeDomainVertex( startDomainVertexId_ );
    impl.removeDomainVertex( endDomainVertexId_ );

    //Unlock the graph, and clear this as the active domain findPath
    impl.domainGraphInUse( false );
    impl.activeDomainFindPath( NULL );

    //Set state to finished.
    state_ = FINISHED;

    CS2PATH_EXIT( "endMacroSearch" );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dDomainFindPath::output( PortalPoints* pPortalPoints ) const
{
    CS2PATH_ENTER( "output" );

    PRE( state_ == FINISHED );
    PRE( pPortalPoints != NULL );

    //If we have a path, return it
    if( foundPath_ )
    {
        if( domainPath_.size() != 0 )
            pPortalPoints->reserve( domainPath_.size() );

        *pPortalPoints = domainPath_;
    }


    if( CS2PATH_STREAM_ENABLED )
    {
        CS2PATH_STREAM( "Portal points found" << std::endl );

        for( PortalPoints::const_iterator i = domainPath_.begin(); i != domainPath_.end(); ++i )
        {
            const PortalPoint& portalPoint = (*i);
            PhysConfigSpace2d::PortalId portalId = portalPoint.first;
            MATHEX_SCALAR distance = portalPoint.second;

            MexPoint2d point = pConfigSpace_->portalPoint( portalId, distance );

            CS2PATH_STREAM( point << std::endl );
        }
    }

    CS2PATH_EXIT( "output" );

    return foundPath_;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dDomainFindPath::update( const PhysRelativeTime& maxTime )
{
    CS2PATH_ENTER( "update" );

    PRE( state_ != UNDEFINED );

    //Take action depending on state, until finished or run out of time
    PhysAbsoluteTime entryTime = Phys::time();
    PhysRelativeTime timeLeft = maxTime;
    bool canDoMore = true;

    PHYSTRACE(">>>>>>>>>>>>>>>> PhysCS2dDomainFindPath::update at " << entryTime << std::endl << *this );

    do
    {
        switch( state_ )
        {
            case NOT_STARTED:
            {
                //Kick it off
                start();
                break;
            }

            case PENDING_MACRO:
            {
                //Try to start the macro search
                startMacroSearch();

                //If still in pending state, there's probably another findPath using
                //the domain graph.
                if( state_ == PENDING_MACRO )
                    canDoMore = false;
                break;
            }

            case MACRO:
            {
                //Doing macro search
                if( isMacroSearchFinished() ) endMacroSearch();
                else updateMacroSearch( timeLeft );
                break;
            }

            case FINISHED:
            {
                canDoMore = false;
                break;
            }

            default:
                ASSERT( false, "Invalid state in CS2dDomainFindPath" );
        }

        //How much time do we have left?
        timeLeft = maxTime - (Phys::time() - entryTime);
    }
    while( canDoMore  and  timeLeft > 0 );

    CS2PATH_EXIT( "update" );
}

//////////////////////////////////////////////////////////////////////////////////////////
PhysCS2dDomainFindPath::DomainAStarAlg::DomainAStarAlg( MATHEX_SCALAR minClearance, ObstacleFlags flags )
: minClearance_( minClearance ),
  flags_( flags )
{
}
//////////////////////////////////////////////////////////////////////////////////////////
//virtual
MATHEX_SCALAR PhysCS2dDomainFindPath::DomainAStarAlg::estimatedCost
(
    const PhysCS2dDomainVertex& from, const PhysCS2dDomainVertex& to
) const
{
    CS2PATH_ENTER( "estimatedCost" );

    MATHEX_SCALAR result = from.point().euclidianDistance( to.point() );

    A_STAR_STREAM( "Estimated cost from " << from.point() << " to " << to.point() << " is " <<
        result << std::endl );

    CS2PATH_EXIT( "estimatedCost" );

    return result;
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
bool PhysCS2dDomainFindPath::DomainAStarAlg::vertexAvailable
(
    const PhysCS2dDomainVertex& vertex
) const
{
    CS2PATH_ENTER( "vertexAvailable" );

    //  TBD: Examine flags at this point as well
    bool result = vertex.clearance() >= minClearance_;

    A_STAR_INSPECT( vertex.clearance() );
    A_STAR_INSPECT( minClearance_ );

    if( result )
    {
        //  See if the flags mean that the vertex is unavailable

        A_STAR_INSPECT( vertex.flags() );
        A_STAR_INSPECT( flags_ );

        if( vertex.flags() and ( ( vertex.flags() & flags_ ) == 0 ) )
            result = false;
    }

    A_STAR_STREAM( "Vertex at " << vertex.point() << " is " << ( result ? "available" : "not available" ) << std::endl );

    CS2PATH_EXIT( "vertexAvailable" );

    return result;
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const PhysCS2dDomainFindPath& t )
{
    o << "PhysCS2dDomainFindPath " << (void*)&t << " ";

    switch( t.state_ )
    {
        case PhysCS2dDomainFindPath::UNDEFINED:
        {
            o << "UNDEFINED";
            break;
        }

        case PhysCS2dDomainFindPath::NOT_STARTED:
        {
            o << "NOT_STARTED";
            break;
        }

        case PhysCS2dDomainFindPath::PENDING_MACRO:
        {
            o << "PENDING_MACRO";
            break;
        }

        case PhysCS2dDomainFindPath::MACRO:
        {
            o << "MACRO";
            break;
        }

        case PhysCS2dDomainFindPath::FINISHED:
        {
            o << "FINISHED";
            break;
        }
    }

    o << "  ";
    o << t.startPoint_ << " -> " << t.endPoint_ << " ";
    o << "priority " << t.priority();

    return o;
}

PhysPathFindingPriority PhysCS2dDomainFindPath::priority() const
{
    return priority_;
}

void PhysCS2dDomainFindPath::increasePriority()
{
    ++priority_;
}

//////////////////////////////////////////////////////////////////////////////////////////
/* End CS2DMFND.CPP *************************************************/
