/*
 * A S T A R A L G . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    GraAStarAlg

    Implements the A* FindPath algorithm over an undirected graph, and a map
    from the graph vertex id to the GraAStarVertex class used to store vertex data
    for each vertex visited during the search.
*/

#ifndef _GRAPH_ASTARALG_HPP
#define _GRAPH_ASTARALG_HPP

#include "base/base.hpp"
#include "ctl/list.hpp"
#include "ctl/vector.hpp"

//GRA_GRAPH IS_A GraGraph
//VERTEX_MAP IS_A FtlMap mapping GRA_GRAPH::VertexId to GraAStarVertex< GRA_GRAPH >
template< class GRA_GRAPH, class VERTEX_MAP >
class GraAStarAlg
{
public:
    typedef GRA_GRAPH Graph;
    typedef VERTEX_MAP AStarVertices;
    typedef typename AStarVertices::Value AStarVertex;
    typedef typename Graph::VertexId VertexId;
    typedef typename Graph::ArcId ArcId;
    typedef typename Graph::Weight Weight;
    typedef typename Graph::VertexItem VertexItem;
    typedef ctl_vector< VertexId > Vertices;
    typedef ctl_vector< ArcId > Arcs;

    //Algorithm state
    enum State { UNDEFINED, RUNNING, FINISHED };

    //ctor
    GraAStarAlg( void );

    //dtor.
    virtual ~GraAStarAlg( void );

    //Start/restart the algorithm finding a path from startVertex to endVertex in graph.
    void start( const Graph& graph, const VertexId& startVertex, const VertexId& endVertex );
    //PRE( graph.containsVertex( startVertex ) )
    //PRE( graph.containsVertex( endVertex ) )
    //PRE( startVertex != endVertex )

    //Advance the algorithm a little, returning its resulting state
    State update( void );
    //PRE( isDefined() )

    //The algorithm's current state
    State state( void ) const;
    bool isFinished( void ) const;
    bool isRunning( void ) const;
    bool isDefined( void ) const;

    //Derived class must supply the estimated cost of getting from from to to.
    //Ensure not an overestimate, or the algorithm may not find the best route.
    virtual Weight estimatedCost( const VertexItem& from, const VertexItem& to ) const = 0;

    //  Derived class must specify a function which tells us whether a vertex
    //  is available for path finding.
    virtual bool vertexAvailable( const VertexItem& vertex ) const = 0;

    Vertices output( void ) const;
    //PRE( isFinished() )

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const GraAStarAlg< GRA_GRAPH, VERTEX_MAP >& t );

private:
    // Operations deliberately revoked
    GraAStarAlg( const GraAStarAlg& );
    GraAStarAlg& operator =( const GraAStarAlg& );
    bool operator ==( const GraAStarAlg& );

    //Expands the vertex with id expandId. The current cost to this node is costToExpandVertex
    void expand( const VertexId& expandId, const Weight& costToExpandVertex );

    //Adds id to the opne vertex list. Its current estimated total path cost is
    //estimatedTotalCost.
    void addOpenVertex( const VertexId& id, const Weight& estimatedTotalCost );

    //Update the journey costs for every vertex after id which has been visited.
    //The new better path found to id has cost costToVertex.
    void propogateBetterPath( const VertexId& id, const Weight& costToVertex );

    //Recursive method used to output vertices on best path in correct order
    void outputVertex( const VertexId& id, Vertices* pVertices ) const;

    //Data members
    const Graph* pGraph_; //The graph over which we will do the search
    AStarVertices* pAStarVertices_;//Vertex map for current search
    ctl_list< VertexId > openVertices_; //List of open vertices
    POST_DATA( Vertices closedVertices_;) //List of open vertices
    VertexId startVertexId_; //Start vertex id
    VertexId endVertexId_; //Goal vertex id
    State state_; //Current state
    Weight bestPathCost_; //Cost of best path found to date to end vertex
    bool havePath_; //True when a path has been found to end vertex
};

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "graph/astaralg.ctp"
//#endif


#endif

/* End ASTARALG.HPP *************************************************/
