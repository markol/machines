/*
 * A R C . C T P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "profiler/profiler.hpp"

/* //////////////////////////////////////////////////////////////// */

template < class ARC_ITEM, class VERTEX_ID, class WEIGHT >
GraArc< ARC_ITEM, VERTEX_ID, WEIGHT >::GraArc()
: weight_( 1 )
{
//A_STAR_WHERE;
//A_STAR_INSPECT( (void*)this );
//A_STAR_INSPECT( weight_ );
	POST( weight() == 1 );
}

template < class ARC_ITEM, class VERTEX_ID, class WEIGHT >
GraArc< ARC_ITEM, VERTEX_ID, WEIGHT
>::GraArc( const ArcItem& item,
				const VertexId& from,
				const VertexId& to )
: item_( item ),
  fromVertex_( from ),
  toVertex_( to ),
  weight_( 1 )
{
//A_STAR_WHERE;
//A_STAR_INSPECT( (void*)this );
//A_STAR_INSPECT( weight_ );
//A_STAR_INSPECT( fromVertex_ );
//A_STAR_INSPECT( toVertex_ );
	POST( weight() == 1 );
}

/* //////////////////////////////////////////////////////////////// */

template < class ARC_ITEM, class VERTEX_ID, class WEIGHT >
typename GraArc< ARC_ITEM, VERTEX_ID, WEIGHT >::VertexId
GraArc< ARC_ITEM, VERTEX_ID, WEIGHT
>::opposingVertex( const VertexId& id ) const
{
	PRE( id == fromVertex() or id == toVertex() );

	VertexId result = id == fromVertex_ ? toVertex_ : fromVertex_;

	POST( iff( id == fromVertex(), result == toVertex() ) );
	POST( iff( id == toVertex(), result == fromVertex() ) );

	return result;
}

/* //////////////////////////////////////////////////////////////// */

template < class ARC_ITEM, class VERTEX_ID, class WEIGHT >
typename GraArc< ARC_ITEM, VERTEX_ID, WEIGHT >::VertexId
GraArc< ARC_ITEM, VERTEX_ID, WEIGHT
>::fromVertex() const
{
	return fromVertex_;
}

template < class ARC_ITEM, class VERTEX_ID, class WEIGHT >
typename GraArc< ARC_ITEM, VERTEX_ID, WEIGHT >::VertexId
GraArc< ARC_ITEM, VERTEX_ID, WEIGHT
>::toVertex() const
{
	return toVertex_;
}

/* //////////////////////////////////////////////////////////////// */

template < class ARC_ITEM, class VERTEX_ID, class WEIGHT >
typename GraArc< ARC_ITEM, VERTEX_ID, WEIGHT >::Weight
GraArc< ARC_ITEM, VERTEX_ID, WEIGHT
>::weight() const
{
//A_STAR_WHERE;
//A_STAR_INSPECT( (void*)this );
//A_STAR_INSPECT( weight_ );
//A_STAR_INSPECT( fromVertex_ );
//A_STAR_INSPECT( toVertex_ );
	return weight_;
}

template < class ARC_ITEM, class VERTEX_ID, class WEIGHT >
void
GraArc< ARC_ITEM, VERTEX_ID, WEIGHT
>::weight( Weight w )
{
	weight_ = w;
//A_STAR_WHERE;
//A_STAR_INSPECT( (void*)this );
//A_STAR_INSPECT( weight_ );
//A_STAR_INSPECT( fromVertex_ );
//A_STAR_INSPECT( toVertex_ );

	POST( weight() == w );
}

/* //////////////////////////////////////////////////////////////// */

template < class ARC_ITEM, class VERTEX_ID, class WEIGHT >
GraArc< ARC_ITEM, VERTEX_ID, WEIGHT
>::operator ARC_ITEM&()
{
	return item_;
};

template < class ARC_ITEM, class VERTEX_ID, class WEIGHT >
GraArc< ARC_ITEM, VERTEX_ID, WEIGHT
>::operator const ARC_ITEM&() const
{
	return item_;
};

/* //////////////////////////////////////////////////////////////// */

template < class A, class V, class W >
bool operator ==( const GraArc< A, V, W >& a, const GraArc< A, V, W >& b )
{
	return a.fromVertex_ == b.fromVertex_
			and a.toVertex_ == b.toVertex_
				and a.weight_ == b.weight_
					and a.item_ == b.item_;
}

template < class A, class V, class W >
bool operator <( const GraArc< A, V, W >& a, const GraArc< A, V, W >& b )
{
	return a.weight_ < b.weight_ and a.item_ < b.item_;
}

/* //////////////////////////////////////////////////////////////// */

/* End ARC.CTP ******************************************************/
