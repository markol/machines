/*
 * B N D R Y 2 D . C T P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */
 
#include "mathex/tri2d.hpp"

//////////////////////////////////////////////////////////////////////
// class MexTriangle2d
//////////////////////////////////////////////////////////////////////

// The edges and vertices are carefully set-up so that:
//   edge1_.bisect(vertices_[0]) == Mathex::VANILLA
//   edge2_.bisect(vertices_[1]) == Mathex::VANILLA
//   edge3_.bisect(vertices_[2]) == Mathex::VANILLA
// i.e., an edge always contains the opposite vertex.
MexTriangle2d::MexTriangle2d(const MexPoint2d& v1, const MexPoint2d& v2, const MexPoint2d& v3):
	MexPolygon2d(NUMBER_OF_VERTICES_),
	edge1_(MexHalfPlane2d(v2, v3)),
	edge2_(MexHalfPlane2d(v3, v1)),
	edge3_(MexHalfPlane2d(v1, v2))
{
	vertices_[0] = v1;
	vertices_[1] = v2;
	vertices_[2] = v3;

	// We need to get the sense of the half-planes correct, otherwise
	// the boundary could end up containing the points outside the triangle.
	if (edge1_.bisect(vertices_[0]) != Mathex::VANILLA)
		edge1_ = MexHalfPlane2d(v3, v2);
	if (edge2_.bisect(vertices_[1]) != Mathex::VANILLA)
		edge2_ = MexHalfPlane2d(v1, v3);
	if (edge3_.bisect(vertices_[2]) != Mathex::VANILLA)
		edge3_ = MexHalfPlane2d(v2, v1);
	
	TEST_INVARIANT;
}

MexTriangle2d::MexTriangle2d( const MexTriangle2d& rhs):
	MexPolygon2d(NUMBER_OF_VERTICES_),
	edge1_(rhs.edge1_),
	edge2_(rhs.edge2_),
	edge3_(rhs.edge3_)
{
	PRE(rhs.nVertices() == NUMBER_OF_VERTICES_);
	vertices_[0] = rhs.vertices_[0];
	vertices_[1] = rhs.vertices_[1];
	vertices_[2] = rhs.vertices_[2];
	TEST_INVARIANT;
}

MexTriangle2d::~MexTriangle2d()
{
}

const MexTriangle2d& MexTriangle2d::operator=( const MexTriangle2d& rhs )
{
	PRE(rhs.nVertices() == NUMBER_OF_VERTICES_);
	if( this != &rhs )
	{
		edge1_ = rhs.edge1_;
		edge2_ = rhs.edge2_;
		edge3_ = rhs.edge3_;
		vertices_[0] = rhs.vertices_[0];
		vertices_[1] = rhs.vertices_[1];
		vertices_[2] = rhs.vertices_[2];
	}

	TEST_INVARIANT;
	return *this;
}


bool MexTriangle2d::operator==(const MexTriangle2d& rhs) const
{
	TEST_INVARIANT;
    return vertices_[0] == rhs.vertices_[0] && 
           vertices_[1] == rhs.vertices_[1] && 
           vertices_[2] == rhs.vertices_[2];
}	


double MexTriangle2d::cross_product
(
	const MexPoint2d& v1, 
	const MexPoint2d& v2, 
	const MexPoint2d& v3
) const
{
	return (v1.x()-v2.x()) * (v3.y()-v2.y()) - (v1.y()-v2.y()) * (v3.x()-v2.x());
}	

/*
bool MexTriangle2d::contains(const MexPoint2d& pt) const
{
	TEST_INVARIANT;
    return edge1_.bisect(pt) == Mathex::VANILLA && 
		   edge2_.bisect(pt) == Mathex::VANILLA && 
		   edge3_.bisect(pt) == Mathex::VANILLA;
}	
*/

const MexPoint2d& MexTriangle2d::vertex( size_t i ) const
{
	PRE(i < nVertices());
	return vertices_[i];
}	


void MexTriangle2d::geometric_invariant() const
{
	// An edge should always contain the point on the opposite
	// side of the triangle.
	ASSERT(edge1_.bisect(vertices_[0]) == Mathex::VANILLA, logic_error());
	ASSERT(edge2_.bisect(vertices_[1]) == Mathex::VANILLA, logic_error());
	ASSERT(edge3_.bisect(vertices_[2]) == Mathex::VANILLA, logic_error());
}	

ostream& operator<<( ostream& ostr, const MexTriangle2d& t )
{
	//ostr << _STATIC_CAST( MexPolygon2d, t );

	for(int i = 0; i<MexTriangle2d::NUMBER_OF_VERTICES_; ++i)
		ostr << t.vertices_[i];

	ostr << t.edge1_;
	ostr << t.edge2_;	
	ostr << t.edge3_;

	return ostr;
}

PER_DEFINE_PERSISTENT( MexTriangle2d );

void perWrite( PerOstream& stream, const MexTriangle2d& t )
{
    const MexPolygon2d& base = t;
    
    stream << base;
    
	for(int i = 0; i<MexTriangle2d::NUMBER_OF_VERTICES_; ++i)
		stream << t.vertices_[i];

	stream << t.edge1_;
	stream << t.edge2_;	
	stream << t.edge3_;
}

void perRead( PerIstream& stream, MexTriangle2d& t )
{

	stream >> _STATIC_CAST( MexPolygon2d&, t );
	for(int i = 0; i<MexTriangle2d::NUMBER_OF_VERTICES_; ++i)
		stream >> t.vertices_[i];

	stream >> t.edge1_;
	stream >> t.edge2_;	
	stream >> t.edge3_;
}

MexTriangle2d::MexTriangle2d( PerConstructor con )
: MexPolygon2d( con )
{
}

//////////////////////////////////////////////////////////////////////
