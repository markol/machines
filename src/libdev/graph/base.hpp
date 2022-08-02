/*
 * B A S E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _GRA_BASE_HPP
#define _GRA_BASE_HPP

#include "base/base.hpp"
#include "ftl/serialid.hpp"

/* //////////////////////////////////////////////////////////////// */

// GraGraphBase is an abstract interface for a
// weighted, undirected collection of vertices and arcs,
// together with the connectivity relationships between them.

template <	class VERTEX_ID,
			class VERTEX_ITEM,
			class ARC_ID,
			class ARC_ITEM,
			class CONST_ARC_IDS,
			class WEIGHT >
class GraGraphBase
// cannonical form revoked
{
public:

	typedef GraGraphBase	GraphBase;
	typedef VERTEX_ID		VertexId;
	typedef VERTEX_ITEM		VertexItem;
	typedef ARC_ID			ArcId;
	typedef ARC_ITEM		ArcItem;
	typedef CONST_ARC_IDS 	ArcIds;
	typedef WEIGHT			Weight;
    typedef FtlSerialId     Version;

	typedef unsigned long	ArcCount;
	typedef unsigned long 	VertexCount;

	///////////////////////////////

	GraGraphBase();
	// POST( nArcs() == 0 );
	// POST( nVertices() == 0 );

	virtual ~GraGraphBase();

	ArcCount nArcs() const;
	VertexCount nVertices() const;
    Version version() const;

	///////////////////////////////

	virtual bool containsVertex( const VertexId& a ) const = 0;

	virtual bool isConnected( const VertexId& from, const VertexId& to ) const = 0;
	// PRE( containsVertex( from ) );
	// PRE( containsVertex( to ) );

	virtual VertexItem& vertex( const VertexId& id ) = 0;
	// PRE( containsVertex( id ) );

	virtual const VertexItem& vertex( const VertexId& id ) const = 0;
	// PRE( containsVertex( id ) );

	void addVertex( const VertexId& id, const VertexItem& item );
	// PRE( not containsVertex( id ) );
	// POST( vertex( id ) == item );
	// POST( nVertices() == old( nVertices() ) + 1 );

	void removeVertex( const VertexId& id );
	// PRE( containsVertex( id ) );
	// POST( not containsVertex( id ) );
	// POST( nVertices() == old( nVertices() ) - 1 );

	///////////////////////////////

	virtual bool containsArc( const ArcId& a ) const = 0;

	virtual ArcItem& arc( const ArcId& id ) = 0;
	// PRE( containsArc( id ) );

	///////////////////////////////

	virtual VertexId fromVertex( const ArcId& id ) const = 0;
	// PRE( containsArc( id ) );

	virtual VertexId toVertex( const ArcId& id ) const = 0;
	// PRE( containsArc( id ) );

	///////////////////////////////

	virtual const ArcItem& arc( const ArcId& id ) const = 0;
	// PRE( containsArc( id ) );

	void addArc( const VertexId& from,
					const VertexId& to,
					const ArcId& arcId,
					const ArcItem& arcItem );
	// PRE( from != to );
	// PRE( containsVertex( from ) );
	// PRE( containsVertex( to ) );
	// PRE( not containsArc( arcId ) );
	// POST( arc( arcId ) == arcItem );
	// POST( arcs( from ).contains( arcId ) );
	// POST( arcs( to ).contains( arcId ) );
	// POST( nArcs() == old( nArcs() ) + 1 );

	void removeArc( const ArcId& id );
	// PRE( containsArc( id ) );
	// POST( not containsArc( id ) );
	// POST( not arcs( from ).contains( arcId ) );
	// POST( not arcs( to ).contains( arcId ) );
	// POST( nArcs() == old( nArcs() ) - 1 );

	///////////////////////////////

	virtual Weight weight( const ArcId& a ) const = 0;
	// PRE( containsArc( id ) );

	virtual void weight( const ArcId& a, Weight w ) = 0;
	// PRE( containsArc( id ) );
	// POST( weight( id ) == w );

	///////////////////////////////

	virtual ArcCount arity( const VertexId& id ) const = 0;

	virtual ArcIds& arcs( const VertexId& a ) = 0;
	// PRE( containsVertex( a ) );

	virtual const ArcIds& arcs( const VertexId& a ) const = 0;
	// PRE( containsVertex( a ) );

	virtual ArcIds arcs( const VertexId& from, const VertexId& to ) const = 0;
	// PRE( isConnected( from, to ) );

protected:

	// PRE( not containsVertex( id ) );
	virtual void doAddVertex( const VertexId& id, const VertexItem& item ) = 0;
	// POST( vertex( id ) == item );

	// PRE( containsVertex( id ) );
	virtual void doRemoveVertex( const VertexId& id ) = 0;
	// POST( not containsVertex( id ) );

	// PRE( containsVertex( from ) );
	// PRE( containsVertex( to ) );
	// PRE( not containsArc( arcId ) );
	virtual void doAddArc( const VertexId& from,
					const VertexId& to,
					const ArcId& arcId,
					const ArcItem& arcItem ) = 0;
	// POST( arc( arcId ) == arcItem );

	// PRE( containsArc( id ) );
	virtual void doRemoveArc( const ArcId& id ) = 0;
	// POST( not containsArc( id ) );

private:

	GraGraphBase( const GraGraphBase& );
	GraGraphBase& operator =( const GraGraphBase& );
	bool operator ==( const GraGraphBase& );

	ArcCount	nArcs_;
	VertexCount	nVertices_;
    Version     version_;
};

/* //////////////////////////////////////////////////////////////// */

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "graph/base.ctp"
//#endif

/* //////////////////////////////////////////////////////////////// */

#endif	/* #ifndef _GRA_BASE_HPP	*/

/* End BASE.HPP *****************************************************/
