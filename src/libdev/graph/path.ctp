
#include "ctl/list.ctp"

#include "ctl/algorith.hpp"
#include "ctl/utility.hpp"

//////////////////////////////////////////////////////////////////////

template < class VERTEX_ID, class ADJACENT_VERTICES, class IS_DEST >
GraShortestPath< VERTEX_ID, ADJACENT_VERTICES, IS_DEST
>::GraShortestPath( const AdjacentVertices& adj,
						const VertexId& source, 
						const IsDestVertex& isDest )
: source_( source ),
  adjacentVertices_( adj ),
  journeyId_( 0 ),
  state_( SEARCHING ),
  currentVertexId_( source ),
  isDest_( isDest )
{
	restart( source );
}

//////////////////////////////////////////////////////////////////////

template < class VERTEX_ID, class ADJACENT_VERTICES, class IS_DEST >
bool
GraShortestPath< VERTEX_ID, ADJACENT_VERTICES, IS_DEST 
>::isFinished() const
{
	return state_ == FINISHED;
}

//////////////////////////////////////////////////////////////////////

template < class VERTEX_ID, class ADJACENT_VERTICES, class IS_DEST >
void
GraShortestPath< VERTEX_ID, ADJACENT_VERTICES, IS_DEST 
>::restart( const VertexId& source )
{
	source_ = source;
	pendingVertices_.erase( pendingVertices_.begin(), pendingVertices_.end() );
	pendingVertices_.push_back( source );
	result_.erase( result_.begin(), result_.end() );
	mapVertexToJourneyId_.erase( mapVertexToJourneyId_.begin(), mapVertexToJourneyId_.end() );
	journeyId_ = 0;
	mapVertexToJourneyId_.insert( source_, journeyId_ );
	state_ = SEARCHING;
	currentVertexId_ = source;
	mapJourneyIdToVertexId_.erase( mapJourneyIdToVertexId_.begin(), mapJourneyIdToVertexId_.end() );
	mapJourneyIdToVertexId_.insert( ctl_make_const_first_pair( journeyId_, source_ ) );
}

template < class VERTEX_ID, class ADJACENT_VERTICES, class IS_DEST >
void
GraShortestPath< VERTEX_ID, ADJACENT_VERTICES, IS_DEST 
>::iterate() 
{
	PRE( not isFinished() );
	
	if( state_ == SEARCHING )
	{
		if( pendingVertices_.empty() )
			state_ = FINISHED;
		else
			search();
	}
	else if( state_ == FOUND )
		searchBackwards();
}

template < class VERTEX_ID, class ADJACENT_VERTICES, class IS_DEST >
void
GraShortestPath< VERTEX_ID, ADJACENT_VERTICES, IS_DEST 
>::search() 
{
	PRE( state_ == SEARCHING );
	PRE( not pendingVertices_.empty() );
	VertexId vId = pendingVertices_.front();

	pendingVertices_.pop_front();

	ASSERT( mapVertexToJourneyId_.find( vId ) != mapVertexToJourneyId_.end(), "logic error" );

	journeyId_ = mapVertexToJourneyId_[  vId ];
	++journeyId_;

	AdjacentVerticesResult adj = adjacentVertices_( vId );
	AdjacentVerticesResult::const_iterator i = adj.begin();
	for( ; state_ == SEARCHING and i != adj.end(); ++i )
	{

		bool isMarked = mapVertexToJourneyId_.find( *i ) != mapVertexToJourneyId_.end();

		if( isDest_( *i ) )
		{
			state_ = FOUND;
			result_.push_back( *i );
			currentVertexId_ = *i;
		}
		else 
		{
			if( not isMarked )
				pendingVertices_.push_back( *i );
		}

		if( not isMarked )
		{
			mapVertexToJourneyId_.insert( *i, journeyId_ );
			mapJourneyIdToVertexId_.insert( ctl_make_const_first_pair( journeyId_, *i ) );
		}
	}
}

template < class VERTEX_ID, class ADJACENT_VERTICES, class IS_DEST >
bool
GraShortestPath< VERTEX_ID, ADJACENT_VERTICES, IS_DEST 
>::isAdjacentTo( const VertexId& from, const VertexId& to ) const
{
	AdjacentVerticesResult adj = adjacentVertices_( from );
	AdjacentVerticesResult::const_iterator i = adj.begin();
	bool found = false;
	for( ; not found and i != adj.end(); ++i )
		found = (*i) == to;

	return found;
}

template < class VERTEX_ID, class ADJACENT_VERTICES, class IS_DEST >
void
GraShortestPath< VERTEX_ID, ADJACENT_VERTICES, IS_DEST 
>::searchBackwards() 
{
	PRE( state_ == FOUND );
	PRE( not result_.empty() );
// cout << "searching backwards" << endl;	
	--journeyId_;

	const Self * pConstThis = (const Self *)this;
	MapJourneyIdToVertexIdRange r = pConstThis->mapJourneyIdToVertexId_.equal_range( journeyId_ );

	ASSERT( r.first != r.second, "" );

	bool nextFound = false;
	MapJourneyIdToVertexId::const_iterator ri = r.first;
	for( ; not nextFound and ri != r.second; ++ri )
	{
		if( isAdjacentTo( (*ri).second, currentVertexId_ ) )
		{
			nextFound = true;
			currentVertexId_ = (*ri).second;
			result_.push_front( (*ri).second );
			if( (*ri).second == source_ )
				state_ = FINISHED;
		}
	}

	ASSERT( nextFound, "logic error" );
}

//////////////////////////////////////////////////////////////////////

/* End **************************************************************/