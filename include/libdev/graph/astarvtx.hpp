/*
 * A S T A R V T X . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    GraAStarVertex

    Stores the necessary information for an expanded/open vertex while
    running the A* FindPath algorithm.
*/

#ifndef _GRAPH_ASTARVTX_HPP
#define _GRAPH_ASTARVTX_HPP

#include "base/base.hpp"

//Parameterized over the graph class for which the algorithm is to be defined
template< class GRA_GRAPH >
class GraAStarVertex
{
public:
    //Data types derived from the Graph class over which parameterized
    typedef typename GRA_GRAPH::Weight Weight;
    typedef typename GRA_GRAPH::ArcId ArcId;

    //ctor. Default required for stl collection instantiation
    GraAStarVertex( void );

    //ctor. The current cost of getting from the start vertex to this vertex is costFromStart.
    //The estimated cost of getting from the vertex to the goal vertex is estimatedCostToEnd.
    //The arc connecting to the previous vertex in current path has id previousArcId.
    GraAStarVertex( const Weight& costFromStart, const Weight& estimatedCostToEnd,
                    const ArcId& previousArcId );

    //dtor.
    ~GraAStarVertex( void );

    //Access methods.
    const Weight& costFromStart( void ) const;
    void costFromStart( const Weight& cost );

    const Weight& estimatedCostToEnd( void ) const;

    const ArcId& previousArcId( void ) const;
    void previousArcId( const ArcId& arcId );

    bool closed( void ) const;
    void closed( bool isClosed );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const GraAStarVertex< GRA_GRAPH >& t );

private:

    //Indicates whether a vertex is open or closed
    enum State { OPEN, CLOSED };

    //Data members
    Weight estimatedCostToEnd_; //Estimated cost to get from this vertex to goal vertex
    Weight costFromStart_; //Actual cost of getting from start vertex to this vertex
    ArcId previousArcId_; //Arc used to reach this vertex on its cheapest path
    State state_; //Indicates whether the vertex has been expanded yet
};

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "graph/astarvtx.ctp"
//#endif

#ifdef _INLINE
    #include "graph/astarvtx.itp"
#endif


#endif

/* End ASTARVTX.HPP *************************************************/
