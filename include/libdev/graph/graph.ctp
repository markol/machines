/*
 * G R A P H . C T P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

/* //////////////////////////////////////////////////////////////// */

template < class VERTICES, class ARCS >
bool
GraGraph< VERTICES, ARCS
>::containsVertex( const VertexId& a ) const
{
	return vertices_.contains( a );
}

template < class VERTICES, class ARCS >
bool
GraGraph< VERTICES, ARCS
>::isConnected( const VertexId& from, const VertexId& to ) const
{
	PRE( containsVertex( from ) );
	PRE( containsVertex( to ) );

	return not arcs( from, to ).empty();
}

/* //////////////////////////////////////////////////////////////// */

template < class VERTICES, class ARCS >
typename GraGraph< VERTICES, ARCS >::VertexItem&
GraGraph< VERTICES, ARCS
>::vertex( const VertexId& id )
{
	PRE( containsVertex( id ) );
	return vertices_[ id ];
}

template < class VERTICES, class ARCS >
const typename GraGraph< VERTICES, ARCS >::VertexItem&
GraGraph< VERTICES, ARCS
>::vertex( const VertexId& id ) const
{
	PRE( containsVertex( id ) );
	return vertices_[ id ];
}

/* //////////////////////////////////////////////////////////////// */

template < class VERTICES, class ARCS >
bool
GraGraph< VERTICES, ARCS
>::containsArc( const ArcId& a ) const
{
	return arcs_.contains( a );
}

template < class VERTICES, class ARCS >
typename GraGraph< VERTICES, ARCS >::VertexId
GraGraph< VERTICES, ARCS
>::fromVertex( const ArcId& id ) const
{
	PRE( containsArc( id ) );
	return arcs_[ id ].fromVertex();
}

template < class VERTICES, class ARCS >
typename GraGraph< VERTICES, ARCS >::VertexId
GraGraph< VERTICES, ARCS
>::toVertex( const ArcId& id ) const
{
	PRE( containsArc( id ) );
	return arcs_[ id ].toVertex();
}

/* //////////////////////////////////////////////////////////////// */

template < class VERTICES, class ARCS >
typename GraGraph< VERTICES, ARCS >::ArcItem&
GraGraph< VERTICES, ARCS
>::arc( const ArcId& id )
{
	PRE( containsArc( id ) );
	return arcs_[ id ];
}

template < class VERTICES, class ARCS >
const typename GraGraph< VERTICES, ARCS >::ArcItem&
GraGraph< VERTICES, ARCS >::arc( const ArcId& id ) const
{
	PRE( containsArc( id ) );
	return arcs_[ id ];
}

/* //////////////////////////////////////////////////////////////// */

template < class VERTICES, class ARCS >
typename GraGraph< VERTICES, ARCS >::Weight
GraGraph< VERTICES, ARCS >::weight( const ArcId& id ) const
{
	PRE( containsArc( id ) );
	return arcs_[ id ].weight();
}

template < class VERTICES, class ARCS >
void GraGraph< VERTICES, ARCS
>::weight( const ArcId& id, Weight w )
{
	PRE( containsArc( id ) );

	arcs_[ id ].weight( w );

	POST( weight( id ) == w );
}

/* //////////////////////////////////////////////////////////////// */

template < class VERTICES, class ARCS >
typename GraGraph< VERTICES, ARCS
>::GraphBase::ArcCount
GraGraph< VERTICES, ARCS
>::arity( const VertexId& id ) const
{
	return vertices_[ id ].arity();
}

template < class VERTICES, class ARCS >
typename GraGraph< VERTICES, ARCS >::GraphBase::ArcIds&
GraGraph< VERTICES, ARCS
>::arcs( const VertexId& id )
{
	PRE( containsVertex( id ) );
	return vertices_[ id ].arcs();
}

template < class VERTICES, class ARCS >
const typename GraGraph< VERTICES, ARCS >::GraphBase::ArcIds&
GraGraph< VERTICES, ARCS
>::arcs( const VertexId& id ) const
{
	PRE( containsVertex( id ) );
	return vertices_[ id ].arcs();
}

template < class VERTICES, class ARCS >
typename GraGraph< VERTICES, ARCS >::GraphBase::ArcIds
GraGraph< VERTICES, ARCS
>::arcs( const VertexId& from, const VertexId& to ) const
{
	PRE( isConnected( from, to ) );
	typename GraGraph< VERTICES, ARCS >::GraphBase::ArcIds result;
	set_intersection( vertices_[ from ].arcs().begin(),
								vertices_[ from ].arcs().end(),
								vertices_[ to ].arcs().begin(),
								vertices_[ to ].arcs().end(),
								result.begin() );

	return result;
}

/* //////////////////////////////////////////////////////////////// */

template < class VERTICES, class ARCS >
void
GraGraph< VERTICES, ARCS
>::doAddVertex( const VertexId& id, const VertexItem& item )
{
	vertices_.add( typename Vertices::KeyValueAssoc( id, Vertex( item ) ) );
	POST( vertex( id ) == item );
}

template < class VERTICES, class ARCS >
void
GraGraph< VERTICES, ARCS
>::doRemoveVertex( const VertexId& id )
{
	typename GraGraph< VERTICES, ARCS >::GraphBase::ArcIds& arcIds = vertices_[ id ].arcs();

    while( arcIds.size() != 0 )
    {
        POST_DATA( size_t nVertexArcs = arcIds.size() );
        ArcId arcId = arcIds.back();
        this->removeArc( arcId );
        POST( nVertexArcs - 1 == arcIds.size() );
    }
    /*
	ArcIds::iterator i = arcIds.begin();
	for( ; i != arcIds.end(); ++i )
		vertices_[ arcs_[ *i ].opposingVertex( id ) ].removeArc( *i );
    */
	vertices_.remove( id );

	POST( not containsVertex( id ) );
}

/* //////////////////////////////////////////////////////////////// */

template < class VERTICES, class ARCS >
void
GraGraph< VERTICES, ARCS
>::doAddArc( const VertexId& from,
				const VertexId& to,
				const ArcId& arcId,
				const ArcItem& arcItem )
{
	vertices_[ from ].addArc( arcId );
	vertices_[ to ].addArc( arcId );

	arcs_.add( typename Arcs::KeyValueAssoc( arcId, Arc( arcItem, from, to ) ) );

	POST( arc( arcId ) == arcItem );
}

template < class VERTICES, class ARCS >
void
GraGraph< VERTICES, ARCS
>::doRemoveArc( const ArcId& id )
{
	Arc& arc = arcs_[ id ];
	vertices_[ arc.fromVertex() ].removeArc( id );
	vertices_[ arc.toVertex() ].removeArc( id );

	arcs_.remove( id );

	POST( not containsArc( id ) );
}

// virtual
template < class VERTICES, class ARCS >
const typename GraGraph< VERTICES, ARCS>::Arcs& GraGraph< VERTICES, ARCS >::arcs() const
{
    return arcs_;
}

/* //////////////////////////////////////////////////////////////// */

/* End GRAPH.CTP ****************************************************/
