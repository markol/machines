/*
 * A S T A R A L G . C T P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline template methods

#define GraAStarAlg__ GraAStarAlg< GRA_GRAPH, VERTEX_MAP >

//////////////////////////////////////////////////////////////////////////////////////////
template< class GRA_GRAPH, class VERTEX_MAP >
GraAStarAlg__::GraAStarAlg()
:   pGraph_( NULL ),
    pAStarVertices_( NULL ),
    state_( UNDEFINED )
{

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class GRA_GRAPH, class VERTEX_MAP >
GraAStarAlg__::~GraAStarAlg()
{
    TEST_INVARIANT;

    if( pAStarVertices_ != NULL) _DELETE( pAStarVertices_ );
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class GRA_GRAPH, class VERTEX_MAP >
void GraAStarAlg__::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class GRA_GRAPH, class VERTEX_MAP >
ostream& operator <<( ostream& o, const GraAStarAlg__& t )
{

    o << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
    o << "GraAStarAlg " << (void*)&t << " start" << std::endl;
    if( t.state_ == GraAStarAlg__::UNDEFINED ) o << "UNDEFINED" << std::endl;
    else
    {
        o << (t.state_ == GraAStarAlg__::RUNNING ? "RUNNING" : "FINISHED")
          << " from " << t.startVertexId_ << " to " << t.endVertexId_ << std::endl;
        if( t.havePath_ )
            o << "Best path cost " << t.bestPathCost_ << std::endl;
        else
            o << "No path yet" << std::endl;

        //Write the open vertices
        o << "------------Open Vertices---------------" << std::endl;
        for( typename ctl_list< typename GraAStarAlg__::GraphBase::VertexId >::const_iterator it = t.openVertices_.begin();
             it != t.openVertices_.end(); ++it )
        {
            typename GraAStarAlg__::VertexId id = *it;
            o << "(" << id << ") " << (*t.pAStarVertices_)[id];
        }

#ifndef NDEBUG
        //Write the closed vertices
        o << "------------Closed Vertices---------------" << std::endl;
        for( typename GraAStarAlg__::Vertices::const_iterator it = t.closedVertices_.begin();
             it != t.closedVertices_.end(); ++it )
        {
            typename GraAStarAlg__::VertexId id = *it;
            o << "(" << id << ") " << (*t.pAStarVertices_)[id];
        }
#endif
    }
    o << "GraAStarAlg " << (void*)&t << " end" << std::endl;
    o << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class GRA_GRAPH, class VERTEX_MAP >
void GraAStarAlg__::start
(
    const Graph& graph, const VertexId& startVertex, const VertexId& endVertex
)
{
    //A_STAR_STREAM( "Enter start " << (void*)this << std::endl );
    //A_STAR_INDENT( 2 );

    PRE( graph.containsVertex( startVertex ) );
    PRE( graph.containsVertex( endVertex ) );
    PRE( startVertex != endVertex );

    //Clear away any old search data
    if( pAStarVertices_ != NULL) _DELETE( pAStarVertices_ );
    if( openVertices_.size() != 0 )
        openVertices_.erase( openVertices_.begin(), openVertices_.end() );
    POST_DATA( closedVertices_.erase( closedVertices_.begin(), closedVertices_.end() );)

    //Store the data
    pGraph_ = &graph;
    pAStarVertices_ = _NEW( AStarVertices );
    startVertexId_ = startVertex;
    endVertexId_ = endVertex;

    //A_STAR_INSPECT( startVertex );
    //A_STAR_INSPECT( endVertex );

    //Make the start vertex the only open vertex
    openVertices_.push_back( startVertex );
    const VertexItem& startItem = pGraph_->vertex( startVertex );
    const VertexItem& endItem = pGraph_->vertex( endVertex );

    ASSERT( vertexAvailable( startItem ), "" );

    pAStarVertices_->add
    (
        typename AStarVertices::KeyValueAssoc( startVertex,
                                      AStarVertex( Weight( 0 ),
                                                   estimatedCost( startItem, endItem ),
                                                   ArcId( 0 ) ) )
    );

    //Set the state
    state_ = RUNNING;
    havePath_ = false;

    //A_STAR_INDENT( -2 );
    //A_STAR_STREAM( "Exit start " << (void*)this << std::endl );
}

//////////////////////////////////////////////////////////////////////////////////////////

template< class GRA_GRAPH, class VERTEX_MAP >
typename GraAStarAlg__::State GraAStarAlg__::update( void )
{
    //A_STAR_STREAM( "Enter update " << (void*)this << std::endl );
    //A_STAR_INDENT( 2 );

    PRE( state_ != UNDEFINED );

    if( state_ == RUNNING )
    {
        //Check there is an open node to expand. If not the algorithm is finished.
        if( openVertices_.size() == 0 )
        {
            //A_STAR_STREAM( "No more open nodes - algorithm finished" << std::endl );

            state_ = FINISHED;
        }
        else
        {
            AStarVertices& algVertices = *pAStarVertices_;

            //Pop the front node from the open list
            VertexId currentVertexId = openVertices_.front();
            openVertices_.pop_front();
            AStarVertex& currentAStarVertex = algVertices[ currentVertexId ];

            //A_STAR_STREAM( "Next open id " << currentVertexId << std::endl );

            //If we already have a path to the end node, and it is shorter than
            //the estimate for this vertex, we can stop

            //A_STAR_INSPECT( havePath_ );
            //A_STAR_INSPECT( bestPathCost_ );

            ASSERT( vertexAvailable( pGraph_->vertex( currentVertexId ) ), "" );

            if( havePath_ and currentAStarVertex.estimatedCostToEnd() >= bestPathCost_ )
            {
                //A_STAR_STREAM( "Already have a better path - algorithm finished" << std::endl );
                state_ = FINISHED;
            }
            else
            {
                //A_STAR_STREAM( "Set vertex " << currentVertexId << " to closed" << std::endl );

                //Expand this vertex
                currentAStarVertex.closed( true );
                expand( currentVertexId, currentAStarVertex.costFromStart() );
                POST_DATA( closedVertices_.push_back( currentVertexId ) );
            }
        }
    }

    //A_STAR_INDENT( -2 );
    //A_STAR_STREAM( "Exit update " << (void*)this << std::endl );

    return state_;
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class GRA_GRAPH, class VERTEX_MAP >
void GraAStarAlg__::expand( const VertexId& expandId, const Weight& costToExpandVertex )
{
    //A_STAR_STREAM( "Enter expand " << (void*)this << std::endl );
    //A_STAR_INDENT( 2 );

    //A_STAR_STREAM( "Expand vertex " << expandId << std::endl );

    AStarVertices& algVertices = *pAStarVertices_;

    //Get the arcs from the vertex we are expanding
    const Arcs& arcs = pGraph_->arcs( expandId );
    for( typename Arcs::const_iterator ia = arcs.begin(); ia != arcs.end(); ++ia )
    {
        //Get the arc id for this arc
        ArcId arcId = *ia;

        //Get the id of the other vertex, and the cost of the arc
        Weight arcCost = pGraph_->weight( arcId );
        VertexId otherVertexId = pGraph_->fromVertex( arcId );
        if( otherVertexId == expandId ) otherVertexId = pGraph_->toVertex( arcId );

        //A_STAR_INSPECT( arcId );
        //A_STAR_INSPECT( arcCost );
        //A_STAR_INSPECT( otherVertexId );
        //A_STAR_INSPECT( vertexAvailable( pGraph_->vertex( otherVertexId ) ) );

        //  If the vertex is available, process it, otherwise have nothing to do with it.
        if( vertexAvailable( pGraph_->vertex( otherVertexId ) ) )
        {
            //Compute the accumulated path cost to the other vertex via the expand vertex
            Weight costToOtherVertex = costToExpandVertex + arcCost;

            //A_STAR_INSPECT( costToExpandVertex );
            //A_STAR_INSPECT( costToOtherVertex );

            //Decide whether the other vertex has already been visited.
            //If not add a GraAStarVertex entry to the visited vertices map
            bool newVertex = not pAStarVertices_->contains( otherVertexId );

            //A_STAR_STREAM( "Vertex " << otherVertexId << " has " << ( newVertex ? "not " : "" ) << "been visited" << std::endl );

            if( newVertex )
            {
                const VertexItem& otherItem = pGraph_->vertex( otherVertexId );
                const VertexItem& endItem = pGraph_->vertex( endVertexId_ );
                pAStarVertices_->add
                (
                    typename AStarVertices::KeyValueAssoc
                    (
                        otherVertexId,
                        AStarVertex( costToOtherVertex,
                                     estimatedCost( otherItem, endItem ),
                                     arcId )
                    )
                );
            }

            //We connect a path to this vertex if not seen before, or this path is
            //better than any previous path
            AStarVertex& otherAStarVertex = algVertices[ otherVertexId ];
            if( newVertex or costToOtherVertex < otherAStarVertex.costFromStart() )
            {
                //A_STAR_STREAM( "Connect a path to vertex " << otherVertexId << std::endl );

                //Set the new cost to this node, and store the arc used to connect to it,
                //unless a new vertex, in which case this was done above.
                if( not newVertex )
                {
                    otherAStarVertex.costFromStart( costToOtherVertex );
                    otherAStarVertex.previousArcId( arcId );
                }

                //Check for target vertex
                if( otherVertexId == endVertexId_ )
                {
                    //A_STAR_STREAM( "Have reached end vertex" << std::endl );

                    //Store the best path cost
                    if( havePath_ )
                    {
                        if( costToOtherVertex < bestPathCost_ )
                            bestPathCost_ = costToOtherVertex;
                    }
                    else
                    {
                        bestPathCost_ = costToOtherVertex;
                        havePath_ = true;
                    }

                    //A_STAR_INSPECT( bestPathCost_ );

                }
                else if( newVertex )
                {
                    //Add this vertex id to the open list
                    Weight estimatedTotalCost = costToOtherVertex +
                                                otherAStarVertex.estimatedCostToEnd();
                    addOpenVertex( otherVertexId, estimatedTotalCost );
                }
                else
                {
           			//Update the journey costs for every node after it in current graph
                    propogateBetterPath( otherVertexId, costToOtherVertex );
                }
            }
        }
    }

    //A_STAR_INDENT( -2 );
    //A_STAR_STREAM( "Exit expand " << (void*)this << std::endl );
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class GRA_GRAPH, class VERTEX_MAP >
void GraAStarAlg__::addOpenVertex( const VertexId& id, const Weight& estimatedTotalCost )
{
    //A_STAR_STREAM( "Enter addOpenVertex " << (void*)this << std::endl );
    //A_STAR_INDENT( 2 );

    //A_STAR_INSPECT( id );
    //A_STAR_INSPECT( estimatedTotalCost );

    AStarVertices& algVertices = *pAStarVertices_;

    //Iterate along the existing list looking for first entry with estimated cost
    //greater than the one supplied.
    typename ctl_list< VertexId >::iterator it = openVertices_.begin();
    for( ; it != openVertices_.end(); ++it )
    {
        //Compute estimated cost for this item
        VertexId oldId = *it;
        AStarVertex& oldAStarVertex = algVertices[ oldId ];

        ASSERT( vertexAvailable( pGraph_->vertex( oldId ) ), "" );

        Weight oldCost = oldAStarVertex.costFromStart() +
                         oldAStarVertex.estimatedCostToEnd();

        //Stop if found insert position
        if( estimatedTotalCost < oldCost ) break;
    }

    openVertices_.insert( it, id );

    //A_STAR_INDENT( -2 );
    //A_STAR_STREAM( "Exit addOpenVertex " << (void*)this << std::endl );
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class GRA_GRAPH, class VERTEX_MAP >
void GraAStarAlg__::propogateBetterPath( const VertexId& vertexId, const Weight& costToVertex )
{
    //A_STAR_STREAM( "Enter propogateBetterPath " << (void*)this << std::endl );
    //A_STAR_INDENT( 2 );

    //A_STAR_INSPECT( vertexId );
    //A_STAR_INSPECT( costToVertex );

    AStarVertices& algVertices = *pAStarVertices_;

    PRE( algVertices.contains( vertexId ) );

    //Action depends on whether open or closed
    AStarVertex& inAStarVertex = algVertices[ vertexId ];
    if( inAStarVertex.closed() )
    {
        //A_STAR_STREAM( "Vertex " << vertexId << " is closed" << std::endl );

        //Closed vertex. Consider each vertex connected to this one, by using its arc list
        const Arcs& arcs = pGraph_->arcs( vertexId );
        for( typename Arcs::const_iterator ia = arcs.begin(); ia != arcs.end(); ++ia )
        {
            //Get the arc id for this arc
            ArcId arcId = *ia;

            //Get the id of the other vertex, and the cost of the arc
            Weight arcCost = pGraph_->weight( arcId );
            VertexId otherVertexId = pGraph_->fromVertex( arcId );
            if( otherVertexId == vertexId ) otherVertexId = pGraph_->toVertex( arcId );

            //Compute the cost to this other vertex via the argument vertex
            Weight costToOtherVertex = costToVertex + arcCost;

            //A_STAR_INSPECT( arcId );
            //A_STAR_INSPECT( arcCost );
            //A_STAR_INSPECT( otherVertexId );
            //A_STAR_INSPECT( costToOtherVertex );

            //If this is a better route, propogate the improvement
            AStarVertex& otherAStarVertex = algVertices[ otherVertexId ];
            if( costToOtherVertex < otherAStarVertex.costFromStart() )
            {
                //A_STAR_STREAM( "This is a better route" << std::endl );

                //Store the new cost
                otherAStarVertex.costFromStart( costToOtherVertex );

                //Update the previous arc to go via the argument vertex
                otherAStarVertex.previousArcId( arcId );

                //Use recursive call to continue the propogation
                propogateBetterPath( otherVertexId, costToOtherVertex );
            }
        }
    }
    else if( vertexId != endVertexId_ )
    {
        //Open vertex. Remove from the open list and add it again to ensure it
        //maintains its correct position
        typename ctl_list< VertexId >::iterator it =
            find( openVertices_.begin(), openVertices_.end(), vertexId );
        POST( it != openVertices_.end() );
        openVertices_.erase( it );

        ASSERT( vertexAvailable( pGraph_->vertex( vertexId ) ), "" );

        Weight estimatedTotalCost = costToVertex + inAStarVertex.estimatedCostToEnd();
        addOpenVertex( vertexId, estimatedTotalCost );
    }

    //A_STAR_INDENT( -2 );
    //A_STAR_STREAM( "Exit propogateBetterPath " << (void*)this << std::endl );
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class GRA_GRAPH, class VERTEX_MAP >
typename GraAStarAlg__::State GraAStarAlg__::state( void ) const
{
    return state_;
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class GRA_GRAPH, class VERTEX_MAP >
bool GraAStarAlg__::isFinished( void ) const
{
    return state_ == FINISHED;
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class GRA_GRAPH, class VERTEX_MAP >
bool GraAStarAlg__::isRunning( void ) const
{
    return state_ == RUNNING;
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class GRA_GRAPH, class VERTEX_MAP >
bool GraAStarAlg__::isDefined( void ) const
{
    return state_ != UNDEFINED;
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class GRA_GRAPH, class VERTEX_MAP >
typename GraAStarAlg__::Vertices GraAStarAlg__::output( void ) const
{
    PRE( isFinished() );

    //Create a vertex list to return
    Vertices vertices;
    if( havePath_ ) outputVertex( endVertexId_, &vertices );

    return vertices;
}
//////////////////////////////////////////////////////////////////////////////////////////

template< class GRA_GRAPH, class VERTEX_MAP >
void GraAStarAlg__::outputVertex( const VertexId& id, Vertices* pVertices ) const
{
    //A_STAR_STREAM( "Enter outputVertex " << (void*)this << std::endl );
    //A_STAR_INDENT( 2 );

    //If not the start vertex, find its predecessor and output it first
    if( id != startVertexId_ )
    {
        //Get the arc id leading to the previous vertex in the path
        const AStarVertex& idAStarVertex = (*pAStarVertices_)[ id ];
        ArcId previousArcId = idAStarVertex.previousArcId();

        //Get the preceding vertex
        VertexId previousVertexId = pGraph_->fromVertex( previousArcId );
        if( previousVertexId == id) previousVertexId = pGraph_->toVertex( previousArcId );

        //output it
        outputVertex( previousVertexId, pVertices );
    }

    //Add this vertex
    pVertices->push_back( id );

    //A_STAR_INSPECT( id );

    //A_STAR_INDENT( -2 );
    //A_STAR_STREAM( "Exit outputVertex " << (void*)this << std::endl );
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End ASTARALG.CTP *************************************************/
