/*
 * P A T H . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved.
 */

#ifndef _GRA_PATH_HPP
#define _GRA_PATH_HPP

// GraShortestPath calculates the shortest path between vertices,
// The use of functionoids as template parameters allow the algorithm to
// be formulated independantly of the underlying vertex
// adjacency representation.

//////////////////////////////////////////////////////////////////////


#include "base/base.hpp"

#include "ctl/map.hpp"
#include "ctl/list.hpp"
#include "ctl/utility.hpp"

//////////////////////////////////////////////////////////////////////

template < class VERTEX_ID, class ADJACENT_VERTICES, class IS_DEST_VERTEX >
class GraShortestPath
// memberwise canonical
{
public:

	typedef GraShortestPath< VERTEX_ID, ADJACENT_VERTICES, IS_DEST_VERTEX > Self;

	typedef VERTEX_ID						VertexId;
	typedef	ADJACENT_VERTICES				AdjacentVertices;
	typedef	IS_DEST_VERTEX					IsDestVertex;
	typedef AdjacentVertices::result_type	AdjacentVerticesResult;
	typedef ctl_list< VertexId >			Vertices;

	///////////////////////////////

	GraShortestPath( const AdjacentVertices&,
						const VertexId& source,
						const IsDestVertex& isDest );

	bool isFinished() const;

	void restart( const VertexId& source );

	void iterate();
	// PRE( not isFinished() );

	bool isAdjacentTo( const VertexId& from, const VertexId& to ) const;

	const Vertices& result() const
	{
		PRE( isFinished() );
		return result_;
	}

	const Vertices& operator ()( const VertexId& source )
	{
		restart( source );
		while( not isFinished() )
			iterate();

		return result();
	}

private:

	void search();
	// PRE( state_ == SEARCHING );
	// PRE( not pendingVertices_.empty() );

	void searchBackwards();
	// PRE( state_ == FOUND );
	// PRE( not result_.empty() );

	enum State { SEARCHING, FOUND, FINISHED };

	typedef int JourneyId;
	typedef ctl_map< VertexId, JourneyId, less< VertexId > > 		MapVertexToJourneyId;
	typedef ctl_multimap< JourneyId, VertexId, less< JourneyId > >	MapJourneyIdToVertexId;

	typedef pair< MapJourneyIdToVertexId::const_iterator,
					MapJourneyIdToVertexId::const_iterator > MapJourneyIdToVertexIdRange;

	VertexId				source_;
	Vertices				pendingVertices_;
	Vertices				result_;
	AdjacentVertices		adjacentVertices_;
	MapVertexToJourneyId	mapVertexToJourneyId_;
	JourneyId				journeyId_;
	State					state_;
	VertexId 				currentVertexId_;
	IsDestVertex			isDest_;
	MapJourneyIdToVertexId	mapJourneyIdToVertexId_;
};

//////////////////////////////////////////////////////////////////////

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "graph/path.ctp"
//#endif

//////////////////////////////////////////////////////////////////////

#endif	// #ifndef _GRA_PATH_HPP

/* End **************************************************************/
