/*
 * V E R T E X . C T P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

/* //////////////////////////////////////////////////////////////// */

template < class VERTEX_ITEM, class ARC_ID >
typename GraVertex< VERTEX_ITEM, ARC_ID >::Arity
GraVertex< VERTEX_ITEM, ARC_ID >::arity() const
{
	return arcs_.size();
}

template < class VERTEX_ITEM, class ARC_ID >
GraVertex< VERTEX_ITEM, ARC_ID >::GraVertex()
{
	POST( arity() == 0 );
}

template < class VERTEX_ITEM, class ARC_ID >
GraVertex< VERTEX_ITEM, ARC_ID
>::GraVertex( const VertexItem& item )
: item_( item )
{
	POST( arity() == 0 );
}

template < class VERTEX_ITEM, class ARC_ID >
bool
GraVertex< VERTEX_ITEM, ARC_ID
>::containsArc( const ArcId& id ) const
{
	bool found = false;
	typename ArcIds::const_iterator i = arcs_.begin();
	for( ; i != arcs_.end(); ++i )
	{
		found = ( *i == id );
		if( found )
			break;
	}

	return found;
}

template < class VERTEX_ITEM, class ARC_ID >
void
GraVertex< VERTEX_ITEM, ARC_ID
>::addArc( const ArcId& id )
{
	PRE( not containsArc( id ) );
	POST_DATA( Arity old_arity = arity() );

    if( arcs_.size() == 0 ) arcs_.reserve( 4 );
	arcs_.insert( arcs_.end(), id );

	PRE( containsArc( id ) );
	POST( arity() == old_arity + 1 );
}

/* ///////////////////////////////////////////////////////////////// */

template< class Container, class T >
inline void
ctl_erase( Container * c, const T& t )
{
	c->erase( remove( c->begin(), c->end(), t ), c->end() );
}

template < class VERTEX_ITEM, class ARC_ID >
void
GraVertex< VERTEX_ITEM, ARC_ID
>::removeArc( const ArcId& id )
{
	PRE( containsArc( id ) );
	POST_DATA( Arity old_arity = arity() );

	ctl_erase( &arcs_, id );

	PRE( not containsArc( id ) );
	POST( arity() == old_arity - 1 );
}

template < class VERTEX_ITEM, class ARC_ID >
typename GraVertex< VERTEX_ITEM, ARC_ID
>::ArcIds&
GraVertex< VERTEX_ITEM, ARC_ID
>::arcs()
{
	return arcs_;
}

template < class VERTEX_ITEM, class ARC_ID >
const typename GraVertex< VERTEX_ITEM, ARC_ID
>::ArcIds&
GraVertex< VERTEX_ITEM, ARC_ID
>::arcs() const
{
	return arcs_;
}

template < class VERTEX_ITEM, class ARC_ID >
GraVertex< VERTEX_ITEM, ARC_ID
>::operator VERTEX_ITEM&()
{
	return item_;
};

template < class VERTEX_ITEM, class ARC_ID >
GraVertex< VERTEX_ITEM, ARC_ID
>::operator const VERTEX_ITEM&() const
{
	return item_;
};

/* //////////////////////////////////////////////////////////////// */

template < class V, class A >
bool operator <( const GraVertex< V, A >& a, const GraVertex< V, A >& b )
{
	return a.item_ < b.item_;
}

template < class V, class A >
bool operator ==( const GraVertex< V, A >& a, const GraVertex< V, A >& b )
{
	return a.item_ == b.item_ and a.arcs_ == b.arcs_;
}

/* //////////////////////////////////////////////////////////////// */

/* End VERTEX.CTP ***************************************************/
