/*
 * V E R T E X . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _GRA_VERTEX_HPP
#define _GRA_VERTEX_HPP

#include "ctl/vector.hpp"

/* //////////////////////////////////////////////////////////////// */

// GraVertex associates an item and a sequence of arc
// identifiers with a vertex of an undirected graph.

template < 	class VERTEX_ITEM, class ARC_ID >
class GraVertex
// parameterwise cannonical
{
public:

	typedef VERTEX_ITEM			VertexItem;
	typedef ARC_ID				ArcId;
	typedef ctl_vector< ArcId >	ArcIds;
	typedef unsigned long		Arity;

	GraVertex();
	// POST( arity() == 0 );

	GraVertex( const VertexItem& item );
	// POST( arity() == 0 );

	Arity arity() const;

	bool containsArc( const ArcId& id ) const;

	void addArc( const ArcId& id );
	// PRE( not containsArc( id ) );
	// PRE( containsArc( id ) );
	// POST( arity() == old( arity() ) + 1 );

	void removeArc( const ArcId& id );
	// PRE( containsArc( id ) );
	// PRE( not containsArc( id ) );
	// POST( arity() == old( arity() ) - 1 );

	ArcIds& arcs();
	const ArcIds& arcs() const;

	operator VertexItem&();
	operator const VertexItem&() const;

private:

	ArcIds		arcs_;
	VertexItem	item_;

	friend bool operator  <( const GraVertex& a, const GraVertex& b );
	friend bool operator ==( const GraVertex& a, const GraVertex& b );
};

/* //////////////////////////////////////////////////////////////// */

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "graph/vertex.ctp"
//#endif

/* //////////////////////////////////////////////////////////////// */

#endif	/* #ifndef _GRA_VERTEX_HPP	*/

/* End VERTEX.HPP ***************************************************/
