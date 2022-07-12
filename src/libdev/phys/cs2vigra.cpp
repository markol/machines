/*
 * C S 2 V I G R A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdlib.h>

#include "ctl/nbvector.hpp"
#include "device/time.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/epsilon.hpp"
#include "mathex/point3d.hpp"
#include "mathex/point2d.hpp"
#include "mathex/sausag2d.hpp"
#include "mathex/abox2d.hpp"

#include "phys/phys.hpp"
#include "phys/cspace2.hpp"
#include "phys/trace.hpp"
#include "phys/internal/cs2vigra.hpp"
#include "phys/internal/cs2expsp.hpp"
#include "phys/internal/cs2impl.hpp"

#ifndef _INLINE
    #include "phys/internal/cs2vigra.ipp"
#endif

#define FNTIME CS2VGRA_WHERE;bobTimer.currentTime()

class BobTimer
{
public:
    BobTimer()
    {
        entryTime_ = Phys::time();
    }
    ~BobTimer()
    {
        exitTime_ = Phys::time();

        CS2VGRA_STREAM( "Time in function " << ( exitTime_ - entryTime_ ) * 1000.0 << " ms" << std::endl );
    }
    void currentTime() const
    {
        MATHEX_SCALAR t = Phys::time();
        CS2VGRA_STREAM( "Time  " << ( t - entryTime_ ) * 1000.0 << " ms" << std::endl );
    }
private:
    MATHEX_SCALAR   entryTime_;
    MATHEX_SCALAR   exitTime_;
};

//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dVisibilityGraph::PhysCS2dVisibilityGraph
(
    PhysConfigSpace2d* pConfigSpace, PhysConfigSpace2d* pOriginalConfigSpace,
    PhysCS2dExpansionSpace* pExpansionSpace, MATHEX_SCALAR clearance
)
:   pConfigSpace_( pConfigSpace ),
    pOriginalConfigSpace_( pOriginalConfigSpace ),
    pExpansionSpace_( pExpansionSpace ),
    clearance_( clearance ),
    processingState_( NONE ),
    upToDate_( true ),
    findPathIsDefined_( false ),
    pFindPathAlg_( NULL ),
    findPathCompleted_( false ),
    startPointIndex_( 0 )
{
    unprocessedPolygons_.reserve( 64 );
    processingPolygonVertexIds_.reserve( 16 );

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dVisibilityGraph::~PhysCS2dVisibilityGraph()
{
    TEST_INVARIANT;

    invalidateFindPath();
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dVisibilityGraph::CLASS_INVARIANT
{
    INVARIANT( pConfigSpace_ != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const PhysCS2dVisibilityGraph& t )
{

    o <<"..................................................." << std::endl;
    o << "PhysCS2dVisibilityGraph " << (void*)&t << " start" << std::endl;

    //Write the vertices
    o << "Vertices" << std::endl;
    for( PhysCS2dVisibilityGraph::VertexId vertexId = t.vertexIdGenerator_.begin();
         vertexId != t.vertexIdGenerator_.end(); ++vertexId )
    {
        if( t.graph_.containsVertex( vertexId ) )
        {
            const PhysCS2dVisibilityGraph::VertexData& vertex = t.graph_.vertex( vertexId );
            o << "(" << vertexId << ") " << *(vertex.pPoint_) <<
                 " from Polygon id " << vertex.polygonId_ << std::endl;
        }
    }

    o << "Arcs" << std::endl;
    for( PhysCS2dVisibilityGraph::ArcId arcId = t.arcIdGenerator_.begin();
         arcId != t.arcIdGenerator_.end(); ++arcId )
    {
        if( t.graph_.containsArc( arcId ) )
        {
            o << "(" << arcId << ") from " << t.graph_.fromVertex( arcId ) << " to " <<
                 t.graph_.toVertex( arcId ) << std::endl;
        }
    }

    o << "PhysCS2dVisibilityGraph " << (void*)&t << " end" << std::endl;
    o <<"..................................................." << std::endl;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dVisibilityGraph::addPolygon( const PolygonId& polygonId )
{
    CS2VGRA_STREAM( "Enter addPolygon" << std::endl );
    CS2VGRA_INDENT( 2 );

    CS2VGRA_INSPECT( polygonId );

    PRE( pConfigSpace_->exists( polygonId ) );

    //Push onto the waiting vector, and set flag for not up to date
    unprocessedPolygons_.push_back( polygonId );
    upToDate_ = false;

    //Also, we'll need to trawl through the start/end vertices again, checking for arcs
    //with the new polygon vertices.
    endVertexProcessed_ = false;
    startVertexProcessed_ = false;

    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM( "Exit addPolygon" << std::endl );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dVisibilityGraph::updateGraph( const PhysRelativeTime& maxTime, ObstacleFlags flags )
{
    CS2VGRA_STREAM( "Enter updateGraph" << std::endl );
    CS2VGRA_INDENT( 2 );

    CS2VGRA_INSPECT( maxTime );

    //Loop until the graph is up to date, or alloted time has elapsed
    PhysAbsoluteTime entryTime;
    if( not upToDate_ ) entryTime = Phys::time();

    while( not upToDate_ )
    {
        switch( processingState_ )
        {
            case NONE:
            {
                //Decide what to do next. Always try polygons first.
                startProcessingPolygon( flags );
                if( processingState_ == NONE  and  findPathIsDefined_ )
                    startProcessingFindPathVertex();

                upToDate_ = processingState_ == NONE;
                break;
            }

            case POLYGON:
            {
                if( processingOldArcs_ )
                {
                    processOldArcs();
                }
                else if( processingPolygonVertex_ < processingPolygonVertexIds_.size() )
                {
                    if( processingGraphVertexId_ < vertexIdGenerator_.end() )
                        processNewVertexWithOldVertex( flags );
                    else
                        processNewVertexWithNewVertex( flags );
                }
                else
                {
                    endProcessingPolygon();
                }
                break;
            }

            case START_VERTEX:
            case END_VERTEX:
            {
                processFindPathVertexWithOldVertex( flags );
                break;
            }

            default:
                ASSERT( false, "Illegal state in PhysCS2dVisibilityGraph::updateGraph" );
        }

        //Check for time elapsed
        if( Phys::time() - entryTime > maxTime )
        {
            CS2VGRA_STREAM( "Run out of time" << std::endl );
            break;
        }
    }

    if( upToDate_ )
    {
        CS2VGRA_STREAM( "Graph now up to date" << std::endl );
    }

    CS2VGRA_STREAM( *this );
    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM( "Exit updateGraph" << std::endl );

    return upToDate_;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dVisibilityGraph::startProcessingPolygon( ObstacleFlags flags )
{
    CS2VGRA_STREAM( "Enter startProcessingPolygon" << std::endl );
    CS2VGRA_INDENT( 2 );

    PRE( processingState_ == NONE );

    //Find an unprocessed polygon
    while( unprocessedPolygons_.size() != 0 )
    {
        //Pop an id
        processingPolygonId_ = unprocessedPolygons_.back();
        unprocessedPolygons_.pop_back();

        //Check it still exists in the config space
        if( pConfigSpace_->exists( processingPolygonId_ ) )
        {
            PRE( processingPolygonVertexIds_.size() == 0 );

            //Get the polygon from the config space, and allocate a vertex id for
            //each vertex
            const MexPolygon2d& polygon = pConfigSpace_->polygon( processingPolygonId_ );

            CS2VGRA_STREAM( "Polygon " << polygon );

            // Remove any vertices that this polygon overlaps

            for( VertexId vertexId = vertexIdGenerator_.begin();
                 vertexId != vertexIdGenerator_.end(); ++vertexId )
            {
                if( graph_.containsVertex( vertexId ) and vertexId != startVertexId_ and vertexId != endVertexId_ )
                {
                    const MexPoint2d& position = *(graph_.vertex( vertexId ).pPoint_);

					//	The intersects call is definitely wrong because it ends up call ing
					//  intersects( MexPoint2d&, MexPoint2d&, Rule ) - i.e. trying to intersect
					//  a line rather than a point. If there's trouble with the contains
					//  function it might be because of a touch is intersect / touch is not intersect
					//  discrepancy.
//                    if( polygon.intersects( position, Mathex::TOUCH_ISNT_INTERSECT ) )
                    if( polygon.contains( position ) )
                    {
                        CS2VGRA_STREAM( "Can remove vertex at " << position << std::endl );
                        graph_.removeVertex( vertexId );
                    }
                }
            }

            pConfigSpace_->isPolygonEnabled( processingPolygonId_, false );

            CS2VGRA_INSPECT( processingPolygonId_ );

            size_t nVertices = polygon.nVertices();
            for( size_t i = 0; i != nVertices; ++i )
            {
                PhysConfigSpace2d::PolygonId dummyId;

                // Only deal with this vertex if it is contained within the config
                // space i.e. it doesn't intersect with any expanded polygons.

                const bool vertexOk = pConfigSpace_->contains( polygon.vertex( i ), flags, &dummyId );

                if( vertexOk )
                {
                    //generate a vertex id
                    VertexId vertexId = vertexIdGenerator_.next();
                    processingPolygonVertexIds_.push_back( vertexId );

                    CS2VGRA_STREAM( "  Add vertex " << polygon.vertex( i ) << std::endl );

                    //Add a graph vertex
                    graph_.addVertex( vertexId,
                                      VertexData( polygon.vertex( i ), processingPolygonId_ ) );
                }
                else
                {
                    //  This allows us to keep the adjacency of vertices
                    //  correct - important when we're adding arcs. If we
                    //  don't do this we try and add arcs which cut across
                    //  the original polygon because we've missed some
                    //  vertices out.
                    processingPolygonVertexIds_.push_back( dummyVertexId() );
                }
            }

            pConfigSpace_->isPolygonEnabled( processingPolygonId_, true );

            //Set up for arc processing etc
            processingPolygonVertex_ = 0;
            processingGraphVertexId_ = vertexIdGenerator_.begin();
            processingGraphArcId_ = arcIdGenerator_.begin();
            processingOldArcs_ = true;
            processingState_ = POLYGON;
            break;
        }
    }

    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM( "Exit startProcessingPolygon" << std::endl );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dVisibilityGraph::processOldArcs()
{
    PRE( processingState_ == POLYGON );
    PRE( processingOldArcs_ );

    CS2VGRA_STREAM( "Enter processOldArcs" << std::endl );
    CS2VGRA_INDENT( 2 );

    //Get the polygon
    const MexPolygon2d& polygon = pConfigSpace_->polygon( processingPolygonId_ );

    CS2VGRA_STREAM( "Polygon\n" << polygon << std::endl );

    size_t nArcsDone = 0;
    while( nArcsDone < 10  and  processingGraphArcId_ < arcIdGenerator_.end() )
    {
        //Check still exists
        if( graph_.containsArc( processingGraphArcId_ ) )
        {
            //Get the endpoints of the current arc
            VertexId fromId = graph_.fromVertex( processingGraphArcId_ );
            VertexId toId = graph_.toVertex( processingGraphArcId_ );
            const MexPoint2d& fromPoint = *(graph_.vertex( fromId ).pPoint_);
            const MexPoint2d& toPoint = *(graph_.vertex( toId ).pPoint_);

            CS2VGRA_STREAM( "Arc from " << fromPoint << " to " << toPoint << std::endl );

            //See if this arc intersects the polygon, and if so, remove it
            if( polygon.intersects( fromPoint, toPoint, Mathex::TOUCH_ISNT_INTERSECT ) )
            {
                graph_.removeArc( processingGraphArcId_ );

                CS2VGRA_STREAM( "Remove arc" << std::endl );
            }

            //Another one done
            ++nArcsDone;
        }

        //Advance to next arc
        ++processingGraphArcId_;
    }

    //Reset flag if finished all arcs
    processingOldArcs_ = processingGraphArcId_ < arcIdGenerator_.end();

    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM( "Exit processOldArcs" << std::endl );

    return nArcsDone != 0;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dVisibilityGraph::processNewVertexWithOldVertex( ObstacleFlags flags )
{
    PRE( processingState_ == POLYGON );
    PRE( processingPolygonVertex_ < processingPolygonVertexIds_.size() );

    CS2VGRA_STREAM( "Enter processNewVertexWithOldVertex" << std::endl );
    CS2VGRA_INDENT( 2 );

    bool doneOldVertex = false;

    if( processingPolygonVertexIds_[ processingPolygonVertex_ ] != dummyVertexId() )
    {
        const VertexData& processingVertexData =
            graph_.vertex( processingPolygonVertexIds_[ processingPolygonVertex_ ] );

        CS2VGRA_STREAM( "Processing vertex data\n" << processingVertexData );

        MATHEX_SCALAR eps = MexEpsilon::instance() * 50;

        const MexPolygon2d& processingPolygon = pConfigSpace_->polygon( processingPolygonId_ );

        CS2VGRA_STREAM( "Processing polygon " << processingPolygon );

        //See if any more old vertices to join arcs to
        while( not doneOldVertex  and  processingGraphVertexId_ < vertexIdGenerator_.end() )
        {
            if( graph_.containsVertex( processingGraphVertexId_ ) )
            {
                //Get the old vertex data from the graph, and try creating an arc to it,
                //provided it isn't a vertex of the processing polygon.
                const VertexData& oldVertexData = graph_.vertex( processingGraphVertexId_ );
                if( oldVertexData.polygonId_ != processingPolygonId_  and
                    oldVertexData.polygonId_ != PolygonId::invalidId() )
                {
                    //Get the distance from the current processing polygon vertex.
                    //Arcs that are very small will be automatically rejected


                    const MexPoint2d& processingPoint = *processingVertexData.pPoint_;
                    const MexPoint2d& oldPoint = *oldVertexData.pPoint_;
                    MATHEX_SCALAR distance = processingPoint.euclidianDistance( oldPoint );

                    CS2VGRA_INSPECT( processingPoint );
                    CS2VGRA_INSPECT( oldPoint );
                    CS2VGRA_INSPECT( distance );

                    if( distance > eps )
                    {
                        //First check to see if the line intersects the processing polygon,
                        //or the polygon of the old vertex
                        size_t vertexIndex;
                        bool pointIsVertex = processingPolygon.pointIsVertex( processingPoint, &vertexIndex );
                        ASSERT( pointIsVertex, "" );

                        bool contains = not processingPolygon.intersects( vertexIndex, oldPoint );

                        CS2VGRA_INSPECT( contains );

                        if( contains )
                        {
                            const MexPolygon2d& oldVertexPolygon =
                                pConfigSpace_->polygon( oldVertexData.polygonId_ );

                            CS2VGRA_STREAM( "Old vertex polygon\n" << oldVertexPolygon << std::endl );

                            bool oldPointIsVertex = oldVertexPolygon.pointIsVertex( oldPoint, &vertexIndex );
                            ASSERT( oldPointIsVertex, "" );
                            contains = not oldVertexPolygon.intersects( vertexIndex, processingPoint );
                        }

                        CS2VGRA_INSPECT( contains );

                        //If not, we need to check against the rest of the config space,
                        //but disable these 2 polygons, because we know they don't intersect the line
                        if( contains )
                        {
                            pConfigSpace_->isPolygonEnabled( processingPolygonId_, false );
                            pConfigSpace_->isPolygonEnabled( oldVertexData.polygonId_, false );

                            //Does the config space contain this line?
                            PolygonId intersectingId;
                            if( pConfigSpace_->contains( processingPoint, oldPoint, flags, &intersectingId ) )
                            {
                                //Yes, add an arc between these 2 points
                                ArcId arcId = arcIdGenerator_.next();
                                graph_.addArc( processingPolygonVertexIds_[ processingPolygonVertex_ ],
                                               processingGraphVertexId_,
                                               arcId,
                                               ArcData() );
                                graph_.weight( arcId, distance );
                            }

                            pConfigSpace_->isPolygonEnabled( processingPolygonId_, true );
                            pConfigSpace_->isPolygonEnabled( oldVertexData.polygonId_, true );
                        }

                        doneOldVertex = true;
                    }
                }
            }

            //Advance to next one
            ++processingGraphVertexId_;
        }
    }
    else
    {
        processingGraphVertexId_ = vertexIdGenerator_.end();
    }

    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM( "Exit processNewVertexWithOldVertex" << std::endl );

    return doneOldVertex;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dVisibilityGraph::processNewVertexWithNewVertex( ObstacleFlags flags )
{
    CS2VGRA_STREAM( "Enter processNewVertexWithNewVertex" << std::endl );
    CS2VGRA_INDENT( 2 );

    PRE( processingState_ == POLYGON );
    PRE( not processingOldArcs_ );
    PRE( processingPolygonVertex_ < processingPolygonVertexIds_.size() );

    //Get the ids of the current vertex and its predecessor
    VertexId newVertexId = processingPolygonVertexIds_[ processingPolygonVertex_ ];
    size_t previousVertex = ( processingPolygonVertex_ == 0 ?
                              processingPolygonVertexIds_.size() - 1 :
                              processingPolygonVertex_ - 1 );
    VertexId previousVertexId = processingPolygonVertexIds_[ previousVertex ];

    if( newVertexId != dummyVertexId() and previousVertexId != dummyVertexId() )
    {
        //Get the points for these 2
        const VertexData& processingVertexData = graph_.vertex( newVertexId );
        const VertexData& previousVertexData = graph_.vertex( previousVertexId );
        POST( processingVertexData.polygonId_ == previousVertexData.polygonId_ );

        CS2VGRA_STREAM( "Processing vertex data\n" << processingVertexData );
        CS2VGRA_STREAM( "Previous vertex data\n" << previousVertexData );

        const MexPoint2d& processingPoint = *(processingVertexData.pPoint_);
        const MexPoint2d& previousPoint = *(previousVertexData.pPoint_);
        MexPoint3d processingPoint3d( processingPoint.x(), processingPoint.y(), 0 );
        MexPoint3d previousPoint3d( previousPoint.x(), previousPoint.y(), 0 );

        //Ensure we don't get a collision with the new polygon.
        //See if the polygon edge arc intersects any existing polygons.
        PolygonId intersectingId;

        pConfigSpace_->isPolygonEnabled( processingVertexData.polygonId_, false );
        bool ok = pConfigSpace_->contains( processingPoint3d, previousPoint3d, flags, &intersectingId );
        pConfigSpace_->isPolygonEnabled( processingVertexData.polygonId_, true );

        CS2VGRA_INSPECT( ok );

        //If ok, add an arc joining the 2 points
        if( ok )
        {
            ArcId arcId = arcIdGenerator_.next();
            graph_.addArc( newVertexId, previousVertexId, arcId, ArcData() );
            graph_.weight( arcId, processingPoint.euclidianDistance( previousPoint ) );
        }
    }

    //Increment to next processing polygon vertex, and reset to first old graph vertex
    ++processingPolygonVertex_;
    processingGraphVertexId_ = vertexIdGenerator_.begin();

    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM( "Exit processNewVertexWithNewVertex" << std::endl );

    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dVisibilityGraph::endProcessingPolygon()
{
    PRE( processingState_ == POLYGON );

    //Clear out the vector of vertex ids for the processed polygon's graph vertices
    processingPolygonVertexIds_.erase( processingPolygonVertexIds_.begin(),
                                       processingPolygonVertexIds_.end() );

    processingState_ = NONE;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dVisibilityGraph::removePolygon( const PolygonId& polygonId )
{
    //If this polygon is currently being processed, stop processing it
    if( processingState_ == POLYGON  and  processingPolygonId_ == polygonId )
        endProcessingPolygon();

    //Run through all the graph vertices, removing any that were derived from this
    //polygon
    for( VertexId vertexId = vertexIdGenerator_.begin();
         vertexId != vertexIdGenerator_.end(); ++vertexId )
    {
        if( graph_.containsVertex( vertexId ) )
        {
            //Check associated polygon
            if( graph_.vertex( vertexId ).polygonId_ == polygonId )
                graph_.removeVertex( vertexId );
        }
    }

    //Any findPath search is now invalidated
    invalidateFindPath();
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dVisibilityGraph::setFindPath
(
    const MexPoint2d& startPoint, const MexPoint2d& endPoint
)
{
    //Check to see if either point matches an existing search point.
    bool addStartVertex = true;
    bool addEndVertex = true;

    if( findPathIsDefined_ )
    {
        //If old start matches new end or old end matches new start, swap the existing
        //points
        bool swapped = endPoints_[ startPointIndex_ ] == endPoint  or
                       endPoints_[ 1 - startPointIndex_ ] == startPoint;
        if( swapped )
        {
            VertexId temp = startVertexId_;
            startVertexId_ = endVertexId_;
            endVertexId_ = temp;

            startPointIndex_ = 1 - startPointIndex_;
        }

        //If old start superceded, delete the old vertex
        addStartVertex = !(startPoint == endPoints_[ startPointIndex_ ]);
        if( addStartVertex )
            graph_.removeVertex( startVertexId_ );

        //If old end superceded, delete the old vertex
        addEndVertex = !(endPoint == endPoints_[ 1 - startPointIndex_ ]);
        if( addEndVertex )
            graph_.removeVertex( endVertexId_ );
    }

    //If required, add the new start/end vertices
    if( addStartVertex )
    {
        //Allocate id if don't have one
        if( not findPathIsDefined_ ) startVertexId_ = vertexIdGenerator_.next();

        //Store the coords
        endPoints_[ startPointIndex_ ] = startPoint;

        //Add the vertex
        graph_.addVertex( startVertexId_,
                          VertexData( endPoints_[ startPointIndex_ ],
                                      PolygonId::invalidId() )
                        );

        //Ensure the vertex will be joined to all other possible graph vertices, except itself
        //and the end vertex.
        startVertexProcessed_ = false;
        VertexData& vertexData = graph_.vertex( startVertexId_ );
        vertexData.startArcChecked_ = true;
        vertexData.endArcChecked_ = false;
    }

    if( addEndVertex )
    {
        //Allocate id if don't have one
        if( not findPathIsDefined_ ) endVertexId_ = vertexIdGenerator_.next();

        //Store the coords
        endPoints_[ 1 - startPointIndex_ ] = endPoint;

        //Add the vertex
        graph_.addVertex( endVertexId_,
                          VertexData( endPoints_[ 1 - startPointIndex_ ],
                                      PolygonId::invalidId() )
                        );

        //Ensure the vertex will be joined to all other possible graph vertices, except itself
        //and the start vertex.
        endVertexProcessed_ = false;
        VertexData& vertexData = graph_.vertex( endVertexId_ );
        vertexData.startArcChecked_ = addStartVertex;
        vertexData.endArcChecked_ = true;
    }

    //Now we have one
    findPathIsDefined_ = true;

    //Clear any old path finding stuff
    invalidateFindPath();

    //If changed either vertex, the graph is no longer up to date
    if( addStartVertex or addEndVertex )
    {
        upToDate_ = false;

        //Where adding new start or end vertex, mark the existing vertices as needing to check the
        //arc to that vertex again
        updateStartEndArcFlags( addStartVertex, addEndVertex );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dVisibilityGraph::clearFindPath()
{
    //Nothing to do unless we have one
    if( findPathIsDefined_ )
    {
        //Delete the vertices
        graph_.removeVertex( startVertexId_ );
        graph_.removeVertex( endVertexId_ );

        //If was processing the vertices, reset
        if( processingState_ == START_VERTEX or processingState_ == END_VERTEX )
            processingState_ = NONE;

        //Reset the flag
        findPathIsDefined_ = false;

        //get rid of path searching stuff
        invalidateFindPath();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dVisibilityGraph::startProcessingFindPathVertex()
{
    PRE( findPathIsDefined_ );

    //Set processing state for the appropriate vertex, if any
    if( not startVertexProcessed_ ) processingState_ = START_VERTEX;
    else if( not endVertexProcessed_ ) processingState_ = END_VERTEX;

    //Set the existing graph vertex index to first value
    processingGraphVertexId_ = vertexIdGenerator_.begin();
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dVisibilityGraph::processFindPathVertexWithOldVertex( ObstacleFlags flags )
{
    CS2VGRA_STREAM("Enter processFindPathVertexWithOldVertex" << std::endl );
    CS2VGRA_INDENT( 2 );

    PRE( findPathIsDefined_ );
    PRE( processingState_ == START_VERTEX or processingState_ == END_VERTEX );

    //Decide which search vertex to use depending on processing state
    bool doingStartVertex = processingState_ == START_VERTEX;
    VertexId findPathVertexId = ( doingStartVertex ? startVertexId_ : endVertexId_ );

    //Get the data for the new vertex we are processing
    const VertexData& processingVertexData = graph_.vertex( findPathVertexId );
    MATHEX_SCALAR eps = MexEpsilon::instance() * 50;
    MATHEX_SCALAR sqrEps = eps * eps;

    //See if any more old vertices to join arcs to
    bool doneOldVertex = false;
    while( not doneOldVertex  and  processingGraphVertexId_ < vertexIdGenerator_.end() )
    {
        if( graph_.containsVertex( processingGraphVertexId_ ) )
        {
            //Get the old vertex data from the graph, and try creating an arc to it,
            //unless already checked, or the same vertex
            VertexData& oldVertexData = graph_.vertex( processingGraphVertexId_ );

            bool alreadyChecked = false;
            if( doingStartVertex )
            {
                if( oldVertexData.startArcChecked_ )
                {
                    alreadyChecked = true;
                }
                else
                    oldVertexData.startArcChecked_ = true;
            }
            else
            {
                if( oldVertexData.endArcChecked_ )
                    alreadyChecked = true;
                else
                    oldVertexData.endArcChecked_ = true;
            }
            if( not alreadyChecked  and  findPathVertexId != processingGraphVertexId_  )
            {
                //Get the distance from the current processing findPath vertex.
                //Arcs that are very small will be automatically accepted.
                const MexPoint2d& processingPoint = *processingVertexData.pPoint_;
                const MexPoint2d& oldPoint = *oldVertexData.pPoint_;
                MATHEX_SCALAR sqrDistance = processingPoint.sqrEuclidianDistance( oldPoint );

                bool doAddArc = sqrDistance < sqrEps;
                if( not doAddArc )
                {
                    //Can the original config space accomodate this arc?
                    doAddArc = originalSpaceContainsSausage( processingPoint, oldPoint, flags );
                    doneOldVertex = true;
                }

                //Add the arc if required
                if( doAddArc )
                {
                    ArcId arcId = arcIdGenerator_.next();
                    graph_.addArc( findPathVertexId,
                                   processingGraphVertexId_,
                                   arcId,
                                   ArcData() );
A_STAR_WHERE;
                    graph_.weight( arcId, sqrt( sqrDistance ) );
A_STAR_WHERE;
                }
                else
                {
                    //JON_STREAM("  VGraph arc rejected " << processingPoint << " to " << oldPoint << " by polygon " << intersectingId.asScalar() << std::endl;)
                }
            }
        }

        //Advance to next one
        ++processingGraphVertexId_;
    }

    //If we've finished processing this vertex, change state etc
    if( not (processingGraphVertexId_ < vertexIdGenerator_.end()) )
    {
        if( processingState_ == START_VERTEX ) startVertexProcessed_ = true;
        else endVertexProcessed_ = true;

        processingState_ = NONE;
    }

    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM("Exit processFindPathVertexWithOldVertex" << doneOldVertex << std::endl );
    return doneOldVertex;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dVisibilityGraph::updateFindPath( const PhysRelativeTime& maxTime,
    ObstacleFlags flags )
{
    PRE( findPathIsDefined_ );

    CS2VGRA_STREAM( "Enter updateFindPath" << std::endl );
    CS2VGRA_INDENT( 2 );

    CS2VGRA_INSPECT( maxTime );

    //record entry time if any work to do
    PhysAbsoluteTime entryTime = 0;
    if( not upToDate_  or  not findPathCompleted_ ) entryTime = Phys::time();

    //Check for the graph being up to date
    if( not upToDate_ )
    {
        //Remove any existing findPath data
        invalidateFindPath();

        //Update the graph
        updateGraph( maxTime, flags );
    }

    //If graph done, and any time left, work on the pathfind algorithm
    if( upToDate_  and  not findPathCompleted_  and  (Phys::time() - entryTime < maxTime) )
    {
        //Ensure we have a search algorithm instantiated
        if( pFindPathAlg_ == NULL )
        {
            pFindPathAlg_ = _NEW( FindPathAlg );
            pFindPathAlg_->start( graph_, startVertexId_, endVertexId_ );
        }

        //Perform an update for as long as we can
        do
        {
            if( pFindPathAlg_->update() == FindPathAlg::FINISHED ) break;
        }
        while( Phys::time() - entryTime < maxTime );

        //If complete, store the vertices
        findPathCompleted_ = pFindPathAlg_->isFinished();
        if( findPathCompleted_ )
        {
            findPathOutput_ = pFindPathAlg_->output();
            _DELETE( pFindPathAlg_ );
            pFindPathAlg_ = NULL;
        }
    }

    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM( "Exit updateFindPath" << std::endl );

    return upToDate_ and findPathCompleted_;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysCS2dVisibilityGraph::invalidateFindPath()
{
    //Delete the algorithm if it exists
    if( pFindPathAlg_ != NULL )
    {
        _DELETE( pFindPathAlg_ );
        pFindPathAlg_ = NULL;
    }

    //Empty the results
    if( findPathCompleted_ )
    {
         findPathCompleted_ = false;
         findPathOutput_.erase( findPathOutput_.begin(), findPathOutput_.end() );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysConfigSpace2d::Path PhysCS2dVisibilityGraph::findPath() const
{
    PRE( isFindPathFinished() );

    //Need a collection to store the vertices in, with sensible allocation size
    Path output;
    output.reserve( 16 );

    //Put each point into the result collection
    for( VertexIds::const_iterator it = findPathOutput_.begin();
         it != findPathOutput_.end(); ++it )
    {
        output.push_back( *(graph_.vertex( *it ).pPoint_) );
    }

    return output;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dVisibilityGraph::VertexData::VertexData
(
    const MexPoint2d& point, const PolygonId& polygonId
)
:   pPoint_( &point ),
    polygonId_( polygonId ),
    startArcChecked_( false ),
    endArcChecked_( false )
{
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysCS2dVisibilityGraph::VertexData::VertexData()
:   pPoint_( NULL ),
    polygonId_( PolygonId::invalidId() ),
    startArcChecked_( false ),
    endArcChecked_( false )
{
}
//////////////////////////////////////////////////////////////////////////////////////////

bool operator== ( const PhysCS2dVisibilityGraph::VertexData& lhs,
                  const PhysCS2dVisibilityGraph::VertexData& rhs )
{
    return *(lhs.pPoint_) == *(rhs.pPoint_)  and  lhs.polygonId_ == rhs.polygonId_;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool operator< ( const PhysCS2dVisibilityGraph::VertexData& lhs,
                  const PhysCS2dVisibilityGraph::VertexData& rhs )
{
    return lhs.polygonId_ < rhs.polygonId_;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool operator== ( const PhysCS2dVisibilityGraph::ArcData& ,
                  const PhysCS2dVisibilityGraph::ArcData&  )
{
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool operator< ( const PhysCS2dVisibilityGraph::ArcData& ,
                  const PhysCS2dVisibilityGraph::ArcData&  )
{
    return false;
}
//////////////////////////////////////////////////////////////////////////////////////////
//virtual
MATHEX_SCALAR PhysCS2dVisibilityGraph::FindPathAlg::estimatedCost
(
    const VertexData& from, const VertexData& to
) const
{
    return from.pPoint_->euclidianDistance( *to.pPoint_ );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysCS2dVisibilityGraph::originalSpaceContainsSausage
(
    const MexPoint2d& startPoint, const MexPoint2d& endPoint, ObstacleFlags flags
)
{
    CS2VGRA_STREAM( "Enter originalSpaceContainsSausage " <<  startPoint << " " << endPoint << std::endl);
    CS2VGRA_INDENT( 2 );

    //Construct a sausage and a boundary
    MexSausage2d testSausage( startPoint, endPoint, clearance_ );
    MexAlignedBox2d sausageBoundary;
    testSausage.boundary( &sausageBoundary );

    //Test this polygon in the original space
    PhysCS2dImpl& originalImpl = *pOriginalConfigSpace_->pImpl();
    bool result = not originalImpl.intersectsAnyExpanded( testSausage,
                               sausageBoundary,
                               originalImpl.permanentPolygonTree(),
                               Mathex::TOUCH_ISNT_INTERSECT,
                               flags,
                               pExpansionSpace_->expansionDistance() );

    CS2VGRA_INDENT( -2 );
    CS2VGRA_STREAM( "Exit originalSpaceContainsSausage " <<  result << std::endl);
    return result;
}

//virtual
bool PhysCS2dVisibilityGraph::FindPathAlg::vertexAvailable
(
    const VertexData&
) const
{
    return true;
}

void PhysCS2dVisibilityGraph::updateStartEndArcFlags( bool resetStartArcFlag, bool resetEndArcFlag )
{
    //Process each vertex in the graph
    for( VertexId graphVertexId = vertexIdGenerator_.begin(); graphVertexId < vertexIdGenerator_.end(); ++graphVertexId )
    {
        if( graph_.containsVertex( graphVertexId ) )
        {
            //Check not a start/end vertex (they have invalid polygon ids)
            VertexData& vertexData = graph_.vertex( graphVertexId );
            if( vertexData.polygonId_ != PolygonId::invalidId() )
            {
                if( resetStartArcFlag )
                    vertexData.startArcChecked_ = false;

                if( resetEndArcFlag )
                    vertexData.endArcChecked_ = false;
            }
        }
    }
}

ostream& operator<<( ostream& o, const PhysCS2dVisibilityGraph::VertexData& data )
{
    o << "VertexData start " << (void*)&data << std::endl;

    if( data.pPoint_ )
        o << "  point " << *data.pPoint_ << std::endl;
    else
        o << "  no point" << std::endl;

    o << "  polygonId " << data.polygonId_ << std::endl;
    o << "  startArcChecked " << data.startArcChecked_ << std::endl;
    o << "  endArcChecked " << data.endArcChecked_ << std::endl;

    o << "VertexData end " << (void*)&data << std::endl;

    return o;
}

// static
PhysCS2dVisibilityGraph::VertexId PhysCS2dVisibilityGraph::dummyVertexId()
{
    return VertexId::invalidId();
}

/* End CS2VIGRA.CPP *************************************************/
