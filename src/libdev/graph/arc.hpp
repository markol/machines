/*
 * A R C . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _GRA_ARC_HPP
#define _GRA_ARC_HPP

/* //////////////////////////////////////////////////////////////// */

// GraArc associates an item with an arc in a weighted graph.

template < class ARC_ITEM, class VERTEX_ID, class WEIGHT >
class GraArc
// parameterwise cannonical
{
public:

	typedef VERTEX_ID	VertexId;
	typedef ARC_ITEM	ArcItem;
	typedef WEIGHT		Weight;

	void constraints()
	{
		VertexId vid1, vid2;
		bool b1 = vid1 == vid2;
		ArcItem i1;
        ArcItem* pI1 = &i1;//Prevents unused warning on i1
		Weight w1( 1 );
	}

	GraArc();
	// POST( weight() == 1 );

	GraArc( const ArcItem& item,
				const VertexId& from,
				const VertexId& to );
	// POST( weight() == 1 );

	VertexId opposingVertex( const VertexId& id ) const;
	// PRE( id == fromVertex() or id == toVertex() );
	// POST( iff( id == fromVertex(), result == toVertex() ) );
	// POST( iff( id == toVertex(), result == fromVertex() ) );

	VertexId fromVertex() const;
	VertexId toVertex() const;

	Weight weight() const;
	void weight( Weight w );
	// POST( weight() == w );

	operator ArcItem&();
	operator const ArcItem&() const;

private:

	ArcItem		item_;
	VertexId	fromVertex_;
	VertexId	toVertex_;
	Weight		weight_;

	friend bool operator  <( const GraArc& a, const GraArc& b );
	friend bool operator ==( const GraArc& a, const GraArc& b );
};

/* //////////////////////////////////////////////////////////////// */

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "graph/arc.ctp"
//#endif

/* //////////////////////////////////////////////////////////////// */

#endif	/* #ifndef _GRA_ARC_HPP	*/

/* End ARC.HPP ******************************************************/
