/*
 * B A S E . C T P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "ctl/algorith.hpp"

/* //////////////////////////////////////////////////////////////// */

template < class VI, class VT, class AI,
			class AT, class AIS, class W >
GraGraphBase< VI, VT, AI, AT, AIS, W >::GraGraphBase()
: nArcs_( 0 ),
  nVertices_( 0 )
{
	POST( nArcs() == 0 );
	POST( nVertices() == 0 );
}

template < class VI, class VT, class AI,
			class AT, class AIS, class W >
GraGraphBase< VI, VT, AI, AT, AIS, W >::~GraGraphBase()
{
	/* Intentionally Empty	*/
}

/* //////////////////////////////////////////////////////////////// */

template < class VI, class VT, class AI,
			class AT, class AIS, class W >
typename GraGraphBase< VI, VT, AI, AT, AIS, W >::ArcCount
GraGraphBase< VI, VT, AI, AT, AIS, W >::nArcs() const
{
	return nArcs_;
}

template < class VI, class VT, class AI,
			class AT, class AIS, class W >
typename GraGraphBase< VI, VT, AI, AT, AIS, W >::VertexCount
GraGraphBase< VI, VT, AI, AT, AIS, W >::nVertices() const
{
	return nVertices_;
}

template < class VI, class VT, class AI,
			class AT, class AIS, class W >
typename GraGraphBase< VI, VT, AI, AT, AIS, W >::Version
GraGraphBase< VI, VT, AI, AT, AIS, W >::version() const
{
	return version_;
}

/* //////////////////////////////////////////////////////////////// */

template < class VI, class VT, class AI,
			class AT, class AIS, class W >
void
GraGraphBase< VI, VT, AI, AT, AIS, W
>::addVertex( const VertexId& id, const VertexItem& item )
{
	PRE( not containsVertex( id ) );
	POST_DATA( VertexCount old_nVertices = nVertices() );

	doAddVertex( id, item );
	++nVertices_;
    ++version_;

	POST( vertex( id ) == item );
	POST( nVertices() == old_nVertices + 1 );
}

template < class VI, class VT, class AI,
			class AT, class AIS, class W >
void
GraGraphBase< VI, VT, AI, AT, AIS, W
>::removeVertex( const VertexId& id )
{
	PRE( containsVertex( id ) );
	POST_DATA( VertexCount old_nVertices = nVertices() );

	doRemoveVertex( id );
	--nVertices_;
    ++version_;

	POST( not containsVertex( id ) );
	POST( nVertices() == old_nVertices - 1 );
}

/* //////////////////////////////////////////////////////////////// */

template < class VI, class VT, class AI,
			class AT, class AIS, class W >
void
GraGraphBase< VI, VT, AI, AT, AIS, W
>::addArc( const VertexId& from,
				const VertexId& to,
				const ArcId& arcId,
				const ArcItem& arcItem )
{
	PRE( containsVertex( from ) );
	PRE( containsVertex( to ) );
	PRE( not containsArc( arcId ) );
	PRE( from != to );

	POST_DATA( ArcCount old_nArcs = nArcs() );

	doAddArc( from, to, arcId, arcItem );
	++nArcs_;
    ++version_;

	POST( arc( arcId ) == arcItem );
	POST( ctl_contains( &arcs( from ), arcId ) );
	POST( ctl_contains( &arcs( to ), arcId ) );
	POST( nArcs() == old_nArcs + 1 );
}

template < class VI, class VT, class AI,
			class AT, class AIS, class W >
void
GraGraphBase< VI, VT, AI, AT, AIS, W
>::removeArc( const ArcId& id )
{
	PRE( containsArc( id ) );

	POST_DATA( VertexId fromVert = fromVertex( id ) );
	POST_DATA( VertexId toVert = toVertex( id ) );

	POST_DATA( ArcCount old_nArcs = nArcs() );

	doRemoveArc( id );
	--nArcs_;
    ++version_;

	POST( not containsArc( id ) );
	POST( not ctl_contains( &arcs( fromVert ), id ) );
	POST( not ctl_contains( &arcs( toVert ), id ) );
	POST( nArcs() == old_nArcs - 1 );
}

/* //////////////////////////////////////////////////////////////// */

/* End BASE.CTP *****************************************************/
