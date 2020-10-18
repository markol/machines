/*
 * Q U A D 3 D . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

// For initial testing purposes, do the invariants:
#define _TEST_INVARIANTS

#include "mathex/quad3d.hpp"
#include "mathex/plane3d.hpp"

#ifndef _INLINE
    #include "mathex/quad3d.ipp"
#endif

MexQuad3d::MexQuad3d
(
	const MexPoint3d& v1, 
	const MexPoint3d& v2, 
	const MexPoint3d& v3, 
	const MexPoint3d& v4
)
{
	vertices_[0] = v1;
	vertices_[1] = v2;
	vertices_[2] = v3;
	vertices_[3] = v4;
	
    TEST_INVARIANT;
}

MexQuad3d::~MexQuad3d()
{
    TEST_INVARIANT;
}

MexQuad3d::MexQuad3d( const MexQuad3d& q )
{
	vertices_[0] = q.vertices_[0];
	vertices_[1] = q.vertices_[1];
	vertices_[2] = q.vertices_[2];
	vertices_[3] = q.vertices_[3];
}

MexQuad3d& MexQuad3d::operator =( const MexQuad3d& q )
{
	vertices_[0] = q.vertices_[0];
	vertices_[1] = q.vertices_[1];
	vertices_[2] = q.vertices_[2];
	vertices_[3] = q.vertices_[3];
	return *this;
}

bool MexQuad3d::operator ==( const MexQuad3d& q )
{
	return vertices_[0] == q.vertices_[0] &&
		   vertices_[1] == q.vertices_[1] &&
		   vertices_[2] == q.vertices_[2] &&
		   vertices_[3] == q.vertices_[3];
}

void MexQuad3d::CLASS_INVARIANT
{
	// Verify that all four vertices are co-planar.
	MexPlane3d plane(vertices_[0], vertices_[1], vertices_[2]);
	INVARIANT(plane.contains(vertices_[3]));
}

const MexPoint3d& MexQuad3d::vertex(uint i) const
{
	PRE(i < 4);
	return vertices_[i];
}	

ostream& operator <<( ostream& o, const MexQuad3d& t )
{
    o << "MexQuad3d: " 
	  << t.vertices_[0] << ", " 
	  << t.vertices_[1] << ", " 
	  << t.vertices_[2] << ", " 
	  << t.vertices_[3];

    return o;
}

PER_DEFINE_PERSISTENT( MexQuad3d );

void perWrite( PerOstream& stream, const MexQuad3d& t )
{
	for(int i=0; i<4; ++i)
		stream << t.vertices_[i];
}

void perRead( PerIstream& stream, MexQuad3d& t )
{
	for(int i=0; i<4; ++i)
		stream >> t.vertices_[i];
}

MexQuad3d::MexQuad3d( PerConstructor )
{
}

/* End QUAD3D.CPP ***************************************************/
