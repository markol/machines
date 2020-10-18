/*
 * C S 2 F I N D P . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/cvexpgon.hpp"
#include "mathex/epsilon.hpp"
#include "mathex/sausag2d.hpp"

#include "phys/trace.hpp"
#include "phys/phys.hpp"
#include "phys/internal/cs2impl.hpp"
#include "phys/internal/cs2findp.hpp"
#include "phys/internal/cs2vigra.hpp"

#ifndef _INLINE
    #include "phys/internal/cs2findp.ipp"
#endif

//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dFindPath::PhysCS2dFindPath()
:   pConfigSpace_( NULL ),
    state_( UNDEFINED ),
    pIgnorePolygons_( NULL ),
    flags_( 0 )
{
    //Should never be used
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dFindPath::PhysCS2dFindPath( PhysConfigSpace2d* pSpace, const MexPoint2d& startPoint,
                      const MexPoint2d& endPoint, MATHEX_SCALAR clearance, ObstacleFlags flags,
                      PhysPathFindingPriority priority )

:   pConfigSpace_( pSpace ),
    startPoint_( startPoint ),
    endPoint_( endPoint ),
    clearance_( clearance ),
    state_( NOT_STARTED ),
    pIgnorePolygons_( NULL ),
    flags_( flags ),
    priority_( priority )
{
    PRE( pSpace != NULL );
    PRE( clearance > 0 );
    PRE( startPoint.euclidianDistance( endPoint ) > MexEpsilon::instance() );

    CS2VGRA_STREAM("PhysCS2dFindPath::PhysCS2dFindPath " << (void*)this << std::endl << "  from " << startPoint << " to " << endPoint << " clearance " << clearance << std::endl);
    CS2VGRA_INSPECT( startPoint_ );
    CS2VGRA_INSPECT( endPoint_ );
    CS2VGRA_INSPECT( clearance_ );
    CS2VGRA_INSPECT( priority_ );
    CS2VGRA_INSPECT( flags_ );

    fullPath_.reserve( 12 );

    //Do initial processing
    start();

    CS2VGRA_STREAM( *this << std::endl );

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dFindPath::~PhysCS2dFindPath()
{
    CS2VGRA_STREAM("PhysCS2dFindPath::~PhysCS2dFindPath " << (void*)this << std::endl);
    CS2VGRA_INDENT( 2 );
    CS2VGRA_STREAM( *this << std::endl );

    TEST_INVARIANT;

    //Extra work required in some states
    switch( state_ )
    {
        case PATHFIND: { endPathSearch( FORCE_ABORT ); break; }
    }

    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM("PhysCS2dFindPath::~PhysCS2dFindPath " << (void*)this << std::endl);
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dFindPath::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dFindPath::spaceIntersections( const MexPoint2d& p1, const MexPoint2d& p2,
                               MATHEX_SCALAR clearance, PolygonIds* pPolygonIds ) const
{
    CS2VGRA_STREAM( *this << std::endl );

    PRE( clearance > 0 );
    PRE( pPolygonIds != NULL );

    //Construct a sausage for the path
    MexSausage2d sausage( p1, p2, clearance );

    CS2VGRA_STREAM( *this << std::endl );

    //See how many intersections we have
    return not pConfigSpace_->contains( sausage, flags_, pPolygonIds,
                                        PhysConfigSpace2d::USE_PERMANENT_ONLY );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dFindPath::start( void )
{
    CS2VGRA_STREAM("PhysCS2dFindPath::start entry " << (void*)this << std::endl);
    CS2VGRA_INDENT( 2 );
    CS2VGRA_STREAM( *this << std::endl );

    PRE( state_ == NOT_STARTED );

    //JON_STREAM("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl << "Starting FindPath from " << startPoint_ << " to " << endPoint_ << std::endl );

    //Check to see if the direct path is ok
    PolygonIds intersectingPolygons;
    intersectingPolygons.reserve( 64 );

    //Set the current path to be the direct route from start to end
    fullPath_.push_back( startPoint_ );
    fullPath_.push_back( endPoint_ );

    //See if the direct route is ok
    if( not spaceIntersections( startPoint_, endPoint_, clearance_,
                                &intersectingPolygons ) )
    {
        //The direct path is ok
        state_ = FINISHED;
    }
    else
    {
        //Enter state waiting to do the pathfind search
        state_ = PENDING_PATHFIND;
    }

    CS2VGRA_STREAM( *this << std::endl );
    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM("PhysCS2dFindPath::start exit " << (void*)this << std::endl);
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dFindPath::output( Path* pPath ) const
{
    CS2VGRA_STREAM("PhysCS2dFindPath::output entry " << (void*)this << std::endl);
    CS2VGRA_INDENT( 2 );
    CS2VGRA_STREAM( *this << std::endl );

    PRE( state_ == FINISHED );
    PRE( pPath != NULL );

    //If we have a path, return it
    bool havePath = fullPath_.size() != 0;
    if( havePath )
    {
        pPath->reserve( fullPath_.size() );
        *pPath = fullPath_;
    }

    CS2VGRA_STREAM( *this << std::endl );
    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM("PhysCS2dFindPath::output exit " << (void*)this << std::endl);

    return havePath;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dFindPath::update( const PhysRelativeTime& maxTime )
{
    CS2VGRA_STREAM("PhysCS2dFindPath::update entry " << (void*)this << std::endl);
    CS2VGRA_INDENT( 2 );

    CS2VGRA_STREAM( *this << std::endl );

    PRE( state_ != UNDEFINED );

    //Take action depending on state, until finished or run out of time
    PhysAbsoluteTime entryTime = Phys::time();
    PhysRelativeTime timeLeft = maxTime;
    bool canDoMore = true;

    //Disable any polygons we want to ignore
    ignorePolygons( true );

    //JON_STREAM(">>>>>>>>>>>>>>>> PhysCS2dFindPath::update at " << entryTime << std::endl << *this );

    do
    {
        switch( state_ )
        {
            case NOT_STARTED:
            {
                CS2VGRA_STREAM("  NOT_STARTED:" << std::endl );
                //Kick it off
                start();
                break;
            }

            case PENDING_PATHFIND:
            {
                CS2VGRA_STREAM("  PENDING_PATHFIND:" << std::endl );
                //try to start the sub-path search
                startPathSearch();

                //If still in pending state, there's probably another findPath using
                //the expansion space, so advance it. That way we'll get a look-in sooner!
                if( state_ == PENDING_PATHFIND )
                {
                    canDoMore = false;
                }
                break;
            }

            case PATHFIND:
            {
                CS2VGRA_STREAM("  PATHFIND:" << std::endl );
                //try to complete the sub-path search
                bool done = isPathSearchFinished();
                if( not done )
                {
                    updatePathSearch( timeLeft );
                    done = isPathSearchFinished();
                }

                if( done )
                    endPathSearch( DO_NOT_FORCE_ABORT );

                break;
            }

            case FINISHED:
            {
                CS2VGRA_STREAM("  FINISHED:" << std::endl );
                canDoMore = false;
                break;
            }

            default:
                ASSERT( false, "Invalid state in CS2dFindPath" );
        }

        //How much time do we have left?
        timeLeft = maxTime - (Phys::time() - entryTime);
    }
    while( canDoMore  and  timeLeft > 0 );

    //Reenable any polygons we wanted to ignore
    ignorePolygons( false );

    CS2VGRA_STREAM( *this << std::endl );
    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM("PhysCS2dFindPath::update exit " << (void*)this << " state " << (int)state_ << std::endl);
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dFindPath::startPathSearch( void )
{
    CS2VGRA_STREAM( "PhysCS2dFindPath::startPathSearch enter " << (void*)this << std::endl );
    CS2VGRA_INDENT( 2 );
    CS2VGRA_STREAM( *this << std::endl );

    PRE( state_ == PENDING_PATHFIND );

    //We must be able to open an expansion space. Only one allowed at a time
    PhysCS2dImpl& impl = *pConfigSpace_->pImpl();
    if( not impl.expansionSpaceIsOpen() )
    {
        //The clearance we use for the expansion space has to be greater than the clearance
        //asked for. This is because we use swept rectangles for each path segment. These
        //rectangles bound the actual swept path, which would be rounded at the ends, and therefore
        //include some space not strictly necessary. We also add 1% clearance so we don't get
        //spurious collisions where the path touches the original rectangles.
        MATHEX_SCALAR root2 = 1.414213562;
        expansionDistance_ = clearance_ * root2 * 1.01;

        //Open the expansion space
        impl.expansionSpaceOpen( expansionDistance_ );
        POST( impl.expansionSpaceIsOpen( expansionDistance_ ) );

        //Set this as the active findpath
        impl.activeFindPath( this );

        //Ensure the space maintains a visibility graph
        PhysConfigSpace2d* pVisibilitySpace = impl.expansionSpace( expansionDistance_ );
        pVisibilitySpace->isMaintainingVisibilityGraph( true, pConfigSpace_, &impl.expansionSpace(), clearance_ );

        //Add all the polygons the initial subpath intersects to the expansion space
        size_t nExtraPolygons;
        checkPath( &nExtraPolygons );

        //Set up the findPath search in the visibility graph of the expansion space
        PhysCS2dVisibilityGraph* pVisibilityGraph =
            pVisibilitySpace->pImpl()->pVisibilityGraph();
        pVisibilityGraph->setFindPath( startPoint_, endPoint_ );

        //Enter the state indicating running a subPath search
        state_ = PATHFIND;
    }

    CS2VGRA_STREAM( *this << std::endl );
    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM("PhysCS2dFindPath::startPathSearch exit " << (void*)this << std::endl );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dFindPath::checkPath( size_t* nExtraPolygons )
{
    CS2VGRA_STREAM("PhysCS2dFindPath::checkPath entry " << (void*)this << std::endl);
    CS2VGRA_INDENT( 2 );
    CS2VGRA_STREAM( *this << std::endl );

    PRE( fullPath_.size() > 1 );

    //Deal with each section of the current path
    bool contained = true;
    *nExtraPolygons = 0;
    PhysCS2dImpl& impl = *pConfigSpace_->pImpl();
    size_t nSections = fullPath_.size() - 1;

    MATHEX_SCALAR sqrLimit = MexEpsilon::instance() * 10;
    sqrLimit *= sqrLimit;

    for( size_t i = 0; i != nSections; ++i )
    {
        //JON_STREAM( "Checking fullPath section from " << fullPath_[ i ] << " to " << fullPath_[ i+1 ] << std::endl);
        //Check for almost coincident points. This can happen with very short arcs.
        //Just ignore the section.
        if( fullPath_[ i ].sqrEuclidianDistance( fullPath_[ i + 1 ] ) < sqrLimit )
            continue;

        //generate a sausage for this section
        MexSausage2d sausage( fullPath_[ i ], fullPath_[ i + 1 ], clearance_ );

        //Find all the polygons it intersects in the space
        PhysConfigSpace2d::PolygonIds polygonIds;
        polygonIds.reserve( 64 );
        if( not pConfigSpace_->contains( sausage, flags_, &polygonIds ) )
        {
            //Set flag indicating any section not contained
            contained = false;

            //Add each polygon to the expansion space not already in it
            for( PhysConfigSpace2d::PolygonIds::iterator it = polygonIds.begin();
                 it != polygonIds.end(); ++it )
            {
                PhysConfigSpace2d::PolygonId id = *it;
                if( not impl.expansionSpacePolygonExists( expansionDistance_, id ) )
                {
                    CS2VGRA_STREAM( "  Path clash with unexpanded polygon id " << id << std::endl );
                    CS2VGRA_STREAM("  Adding polygon to expansion space id " << id.asScalar() << std::endl);
                    impl.expansionSpaceAddPolygon( expansionDistance_, id );
                    ++(*nExtraPolygons);
                }
                else
                {
                    CS2VGRA_STREAM( "  Path clash with polygon id " << id << std::endl );
                }
            }
        }
        else
        {
            CS2VGRA_STREAM( "  Path ok!" << std::endl );
        }
    }

    CS2VGRA_INSPECT( fullPath_ );
    CS2VGRA_STREAM( *this << std::endl );

    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM("PhysCS2dFindPath::checkPath exit " << contained << std::endl);
    return contained;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dFindPath::isPathSearchFinished( void ) const
{
    CS2VGRA_STREAM( "PhysCS2dFindPath::isPathSearchFinished enter " << (void*)this << std::endl );
    CS2VGRA_INDENT( 2 );
    CS2VGRA_STREAM( *this << std::endl );

    PRE( state_ == PATHFIND );

    //Get the expansion space and its visibility graph
    PhysCS2dImpl& impl = *pConfigSpace_->pImpl();
    PhysConfigSpace2d* pVisibilitySpace = impl.expansionSpace( expansionDistance_ );
    PhysCS2dVisibilityGraph* pVisibilityGraph =
        pVisibilitySpace->pImpl()->pVisibilityGraph();

    CS2VGRA_STREAM( *this << std::endl );
    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM( "PhysCS2dFindPath::isPathSearchFinished exit " << (void*)this << std::endl );

    return pVisibilityGraph->isFindPathFinished();
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dFindPath::updatePathSearch( const PhysRelativeTime& maxTime )
{
    CS2VGRA_STREAM( "PhysCS2dFindPath::updatePathSearch enter " << (void*)this << std::endl );
    CS2VGRA_INDENT( 2 );
    CS2VGRA_STREAM( *this << std::endl );

    PRE( state_ == PATHFIND );

    //Get the expansion space and its visibility graph
    PhysCS2dImpl& impl = *pConfigSpace_->pImpl();
    PhysConfigSpace2d* pVisibilitySpace = impl.expansionSpace( expansionDistance_ );
    PhysCS2dVisibilityGraph* pVisibilityGraph =
        pVisibilitySpace->pImpl()->pVisibilityGraph();

    //Advance the search
    pVisibilityGraph->updateFindPath( maxTime, flags_ );

    CS2VGRA_STREAM( *this << std::endl );
    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM( "PhysCS2dFindPath::updatePathSearch exit " << (void*)this << std::endl );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dFindPath::endPathSearch( Abort forceAbort )
{
    CS2VGRA_STREAM("PhysCS2dFindPath::endPathSearch entry " << (void*)this << std::endl);
    CS2VGRA_INDENT( 2 );
    CS2VGRA_STREAM( *this << std::endl );

    PRE( state_ == PATHFIND );

    //Get the expansion space and its visibility graph
    PhysCS2dImpl& impl = *pConfigSpace_->pImpl();
    PhysConfigSpace2d* pVisibilitySpace = impl.expansionSpace( expansionDistance_ );
    PhysCS2dVisibilityGraph* pVisibilityGraph =
        pVisibilitySpace->pImpl()->pVisibilityGraph();

    //get the results, unless aborting the search
    bool donePath = false;
    bool failed = false;
    bool aborting = ( not isPathSearchFinished() ) or ( forceAbort == FORCE_ABORT );

    CS2VGRA_WHERE;

    size_t nExtraPolygons;
    size_t nPathPoints;

    CS2VGRA_WHERE;

    if( not aborting )
    {
        CS2VGRA_WHERE;
        fullPath_ = pVisibilityGraph->findPath();
        nPathPoints = fullPath_.size();
    }

    //If no path, we have failed
    if( aborting )
    {
        CS2VGRA_WHERE;
        donePath = true;
    }
    else if( nPathPoints == 0 )
    {
        failed = true;
        CS2VGRA_STREAM( "No path found in visibility graph" << std::endl << *pVisibilityGraph );
    }
    else if( checkPath( &nExtraPolygons ) )
    {
        //Path is complete
        donePath = true;
        CS2VGRA_STREAM( "Path found ok" << std::endl << *pVisibilityGraph );

        //Set state to finished
        state_ = FINISHED;
    }
    else if( nExtraPolygons == 0 )
    {
        //We have failed to find a path which avoids the obstacles.
        failed = true;
        CS2VGRA_STREAM( "Path found, but collides with space objects" << std::endl << *pVisibilityGraph );
    }
    else
    {
        //We got a path, but found it intersects more obstacles, so try again
        pVisibilityGraph->setFindPath( startPoint_, endPoint_ );
        CS2VGRA_STREAM( "Restarting path search avoiding new obstacles" << std::endl );
    }

    //Check for failure to find a clear path
    if( failed )
    {
        CS2VGRA_WHERE;
        //Wipe any existing full path results and set state to finished
        fullPath_.erase( fullPath_.begin(), fullPath_.end() );
        state_ = FINISHED;
        donePath = true;
    }

    //Check for finished processing this path
    if( donePath )
    {
        CS2VGRA_WHERE;
        //Close the expansion space
        impl.expansionSpaceClose( expansionDistance_ );

        //Clear this as the active subpath findpath
        impl.activeFindPath( NULL );
    }

    CS2VGRA_STREAM( *this << std::endl );
    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM("PhysCS2dFindPath::endPathSearch exit " << (void*)this << std::endl);
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const PhysCS2dFindPath& t )
{
    o << "PhysCS2dFindPath " << (void*)&t << "  ";

    switch( t.state_ )
    {
        case PhysCS2dFindPath::UNDEFINED:
        {
            o << "UNDEFINED";
            break;
        }

        case PhysCS2dFindPath::NOT_STARTED:
        {
            o << "NOT_STARTED";
            break;
        }
        case PhysCS2dFindPath::PENDING_PATHFIND:
        {
            o << "PENDING_PATHFIND";
            break;
        }

        case PhysCS2dFindPath::PATHFIND:
        {
            o << "PATHFIND";
            break;
        }

        case PhysCS2dFindPath::FINISHED:
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
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dFindPath::ignorePolygons( const PolygonIds& polygons )
{
    //Store pointer to the collection
    pIgnorePolygons_ = &polygons;
}

void PhysCS2dFindPath::ignorePolygons( bool doIgnore )
{
    if( pIgnorePolygons_ != NULL )
    {
        bool enabled = not doIgnore;

        PolygonIds::const_iterator citEnd = pIgnorePolygons_->end();
        for( PolygonIds::const_iterator cit = pIgnorePolygons_->begin(); cit != citEnd; ++cit )
        {
            PhysConfigSpace2d::PolygonId id = *cit;
            CS2VGRA_STREAM("  Setting status for polygon " << id.asScalar() << " " << enabled << std::endl);
            pConfigSpace_->isPolygonEnabled( id, enabled );
        }
    }
}

PhysPathFindingPriority PhysCS2dFindPath::priority() const
{
    return priority_;
}

void PhysCS2dFindPath::increasePriority()
{
    ++priority_;
}

//////////////////////////////////////////////////////////////////////////////////////////
/* End CS2FINDP.CPP *************************************************/
