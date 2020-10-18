/*
 * G R A P H . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _GRA_GRAPH_HPP
#define _GRA_GRAPH_HPP

#include "graph/vertex.hpp"
#include "graph/arc.hpp"
#include "graph/base.hpp"

/* //////////////////////////////////////////////////////////////// */

// GraGraph is a weighted, undirected graph, parameterised
// by it's vertex and arc collections.

template <	class MAP_VERTEX_ID_TO_GRA_VERTEX,
			class MAP_ARC_ID_TO_GRA_ARC >
class GraGraph
: public GraGraphBase<	typename MAP_VERTEX_ID_TO_GRA_VERTEX::Key,
						typename MAP_VERTEX_ID_TO_GRA_VERTEX::Value::VertexItem,
						typename MAP_ARC_ID_TO_GRA_ARC::Key,
						typename MAP_ARC_ID_TO_GRA_ARC::Value::ArcItem,
						typename MAP_VERTEX_ID_TO_GRA_VERTEX::Value::ArcIds,
						typename MAP_ARC_ID_TO_GRA_ARC::Value::Weight >
{
public:

	void constraints()
	{
		// MAP_VERTEX_ID_TO_GRA_VERTEX IS-A FtlMap
		// MAP_ARC_ID_TO_GRA_ARC IS-A FtlMap
	};

	using GraphBase = GraGraphBase<	typename MAP_VERTEX_ID_TO_GRA_VERTEX::Key,
						typename MAP_VERTEX_ID_TO_GRA_VERTEX::Value::VertexItem,
						typename MAP_ARC_ID_TO_GRA_ARC::Key,
						typename MAP_ARC_ID_TO_GRA_ARC::Value::ArcItem,
						typename MAP_VERTEX_ID_TO_GRA_VERTEX::Value::ArcIds,
						typename MAP_ARC_ID_TO_GRA_ARC::Value::Weight >;

	typedef typename MAP_VERTEX_ID_TO_GRA_VERTEX::Value::VertexItem VertexItem;
	typedef typename MAP_ARC_ID_TO_GRA_ARC::Value::ArcItem ArcItem;
	typedef typename MAP_ARC_ID_TO_GRA_ARC::Value::Weight Weight;
	typedef typename GraphBase::VertexId VertexId;
	typedef typename GraphBase::ArcId ArcId;

	typedef MAP_VERTEX_ID_TO_GRA_VERTEX	Vertices;
	typedef MAP_ARC_ID_TO_GRA_ARC		Arcs;

	typedef GraVertex< VertexItem, ArcId >		Vertex;
	typedef GraArc< ArcItem, VertexId, Weight >	Arc;

	///////////////////////////////

	// inherited from GraGraphBase...
	virtual bool containsVertex( const VertexId& a ) const;
	virtual bool isConnected( const VertexId& from, const VertexId& to ) const;
	// PRE( containsVertex( from ) );
	// PRE( containsVertex( to ) );

	virtual VertexItem& vertex( const VertexId& id );
	// PRE( containsVertex( id ) );

	virtual const VertexItem& vertex( const VertexId& id ) const;
	// PRE( containsVertex( id ) );

	///////////////////////////////

	virtual const Arcs& arcs() const;

	virtual bool containsArc( const ArcId& a ) const;

	virtual VertexId fromVertex( const ArcId& id ) const;
	// PRE( containsArc( id ) );

	virtual VertexId toVertex( const ArcId& id ) const;
	// PRE( containsArc( id ) );

	///////////////////////////////

	virtual ArcItem& arc( const ArcId& id );
	// PRE( containsArc( id ) );

	virtual const ArcItem& arc( const ArcId& id ) const;
	// PRE( containsArc( id ) );

	///////////////////////////////

	virtual Weight weight( const ArcId& id ) const;
	// PRE( containsArc( id ) );

	virtual void weight( const ArcId& id, Weight w );
	// PRE( containsArc( id ) );
	// POST( weight( id ) == w );

	///////////////////////////////

	virtual typename GraphBase::ArcCount arity( const VertexId& id ) const;

	virtual typename GraphBase::ArcIds& arcs( const VertexId& id );
	// PRE( containsVertex( id ) );

	virtual const typename GraphBase::ArcIds& arcs( const VertexId& id ) const;
	// PRE( containsVertex( id ) );

	virtual typename GraphBase::ArcIds arcs( const VertexId& from, const VertexId& to ) const;
	// PRE( isConnected( from, to ) );

protected:

	// PRE( not containsVertex( id ) );
	virtual void doAddVertex( const VertexId& id, const VertexItem& item );
	// POST( vertex( id ) == item );

	// PRE( containsVertex( id ) );
	virtual void doRemoveVertex( const VertexId& id );
	// POST( not containsVertex( id ) );

	///////////////////////////////

	// PRE( containsVertex( from ) );
	// PRE( containsVertex( to ) );
	// PRE( not containsArc( arcId ) );
	// POST( arcs( from ).contains( arcId ) );
	// POST( arcs( to ).contains( arcId ) );
	virtual void doAddArc( const VertexId& from,
					const VertexId& to,
					const ArcId& arcId,
					const ArcItem& arcItem );
	// POST( arc( arcId ) == arcItem );

	// PRE( containsArc( id ) );
	// POST( not arcs( from ).contains( arcId ) );
	// POST( not arcs( to ).contains( arcId ) );
	virtual void doRemoveArc( const ArcId& id );
	// POST( not containsArc( id ) );

private:

	Vertices	vertices_;
	Arcs		arcs_;
};

/* //////////////////////////////////////////////////////////////// */

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "graph/graph.ctp"
//#endif

/* //////////////////////////////////////////////////////////////// */

#endif	/* #ifndef _GRA_GRAPH_HPP	*/

/* End GRAPH.HPP ****************************************************/
