/*
 * P L A N E 3 D . C P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#include "mathex/mathex.hpp"
#include "mathex/plane3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/line3d.hpp"
#include "mathex/vec3.hpp"

#ifndef _INLINE
    #include "mathex/plane3d.ipp"
#endif

//////////////////////////////////////////////////////////////////////

bool MexPoint3d::intersects( const MexPlane3d& p ) const
{
    return p.contains(*this);
}

//////////////////////////////////////////////////////////////////////


MexPlane3d::MexPlane3d()
:	a_( 0 ),
	b_( 1 ),
	c_( 0 ),
	d_( 0 ),
	normal_(0,1,0)
{
	/* intentionally Empty */

    TEST_INVARIANT;
}


MexPlane3d::MexPlane3d( const MexPoint3d& p0,
							 const MexPoint3d& p1,
                             const MexPoint3d& p2 )
{
    MexVec3 d0 = p0;
    d0 -= p1;       		// i.e. d0_ = p0 - p1

    MexVec3 d1 = p0;
    d1 -= p2;        		// i.e. d1_ = p0 - p2

    // compute the cross product of d0 and d1.
    // This is the normal vector to the MexPlane3d*/

    a_ = d0.y() * d1.z() - d0.z() * d1.y();
    b_ = d0.z() * d1.x() - d0.x() * d1.z();
    c_ = d0.x() * d1.y() - d0.y() * d1.x();

	normal_ = MexVec3(a_,b_,c_);

    //  Check that the normal vector is not the zero vector. If it is,
    //  the three supplied points were in a straight line so we cannot
    //  create the plane.

    PRE( normal_.modulus() > MexEpsilon::instance() );

    // calculate the constant d

    d_ = -( a_ * p2.x() + b_ * p2.y() + c_ * p2.z() );

    //  Divide the plane equation by the length of the normal so that
    //  we can deal with a unit length normal vector.

	MATHEX_SCALAR length = normal_.modulus();

	normal_ /= length;
	a_ /= length;
	b_ /= length;
	c_ /= length;
	d_ /= length;

    //  Set up the plane transform

    MexVec3    xBasis( d0 );
    xBasis.makeUnitVector();
    MexVec3    yBasis( normal_ );
    MexVec3    zBasis( xBasis );
    zBasis.crossProduct( yBasis );
    MexTransform3d tx( xBasis, yBasis, zBasis, p0 );

    planeTransform_ = tx;

    POST( Mathex::abs( normal_.modulus() - MATHEX_SCALAR( 1 ) ) < MexEpsilon::instance() );
    TEST_INVARIANT;
    POST( contains( p0 ) );
    POST( contains( p1 ) );
    POST( contains( p2 ) );
}


MexPlane3d::MexPlane3d( const MexPlane3d& rhs )
:	a_( rhs.a_ ),
	b_( rhs.b_ ),
	c_( rhs.c_ ),
	d_( rhs.d_ ),
    normal_(rhs.normal_),
    planeTransform_( rhs.planeTransform_ )
{
    //cout << endl << "mexplane3d const ctor  ";
	//cout << rhs << endl;
    //cout << *this << endl << endl;
    TEST_INVARIANT;
}

/*
    Construct a plane from a transform. The plane will lie on the
    XZ plane of the transform.
*/


MexPlane3d::MexPlane3d( const MexTransform3d& tx )
:   planeTransform_( tx )
{
    //  The normal to the plane will be the Y vector of the transform.

    MexVec3 yBasis;

    tx.yBasis( &yBasis );

    a_ = yBasis.x();
    b_ = yBasis.y();
    c_ = yBasis.z();

    normal_.setVector( a_, b_, c_ );

    MexPoint3d  position;
    tx.position( &position );

    d_ = -( a_ * position.x() + b_ * position.y() + c_ * position.z() );

    POST( Mathex::abs( normal_.modulus() - MATHEX_SCALAR( 1 ) ) < MexEpsilon::instance() );
    TEST_INVARIANT;
    POST( contains( position ) );
}


MexPlane3d::~MexPlane3d()
{
    /* intentionally Empty */
}


bool MexPlane3d::contains( const MexPoint3d& rhs ) const
{
	// if the point lies on the plane, the plane is said to contain it
	return pointRelativeToPlane( rhs ) == 0;
}


bool MexPlane3d::contains( const MexLine3d& rhs ) const
{
	// true if both points of the line lie on the plane
	return ( pointRelativeToPlane( rhs.end1() ) == 0 ) and
		   ( pointRelativeToPlane( rhs.end2() ) == 0 );
}


bool MexPlane3d::contains( const MexPlane3d& rhs ) const
{
	return *this == rhs;
}


bool MexPlane3d::intersects( const MexPoint3d& rhs ) const
// if the vector lies on the plane
{
    TEST_INVARIANT;

	MATHEX_SCALAR result = a_ * rhs.x() + b_ * rhs.y() + c_ * rhs.z() + d_;

    TEST_INVARIANT;
	return Mathex::abs( result ) < MexEpsilon::instance();
}


bool MexPlane3d::intersects( const MexLine3d& rhs ) const
{

	if  ( contains( rhs.end1() )  or contains( rhs.end2() ))
	{
		//cout << " returned true here - contains both points " << endl;
		return true;
	}

	if ( pointRelativeToPlane( rhs.end1() ) != pointRelativeToPlane( rhs.end2() ))
	{
		//cout << " returned true here - both points on same side of plane " << endl;
		return true;
	}
	else
	{

 	//cout << "points on same side of plane" << endl;

	return false;
	}
}


bool MexPlane3d::intersects( const MexPlane3d& rhs ) const
{
    TEST_INVARIANT;

	//cout << "intersecting plane-plane" ;

	if (*this == rhs)
	  {
	   //cout << "*****same bloody plane*****" << endl;
	   //cout << *this << endl << rhs << endl << endl;
	   return true;
	  }

	MexVec3 normRhs(rhs.normal());
	MexVec3 normalThis(this->normal());

    if (Mathex::abs(normRhs.x() - normalThis.x()) < MexEpsilon::instance()
    and Mathex::abs(normRhs.y() - normalThis.y()) < MexEpsilon::instance()
    and	Mathex::abs(normRhs.z() - normalThis.z()) < MexEpsilon::instance()
       )
	  {
	   //cout << "******* Didn't Intersect *******" ;
	   return false;
	  }
    //cout << " ***dropped thru all conds*** " << endl;
    //cout <<  normRhs  << endl;
	//cout << normalThis << endl;
    return true;
}


MATHEX_SCALAR MexPlane3d::distanceFrom( const MexPoint3d& pos ) const
{
    TEST_INVARIANT;

    MATHEX_SCALAR result = a_ * pos.x() + b_ * pos.y() + c_ * pos.z() + d_;

    //  Since we turn a_, b_ & c_ into the unit vector when we create
    //  the plane we don't need to divide through by them now.

//    result /= sqrt( a_ * a_ + b_ * b_ + c_ * c_ );

    return result;
}


MexPlane3d::PlaneSide MexPlane3d::pointRelativeToPlane( const MexPoint3d& point ) const
{
    MATHEX_SCALAR result = a_ * point.x() + b_ * point.y() + c_ * point.z() + d_;

	//cout << "result of point rel to plane = " << result << endl;

    if( result < MATHEX_SCALAR( -MexEpsilon::instance() ) )
        return NEGATIVE_PLANE_SIDE;
    else if( Mathex::abs( result ) <= MexEpsilon::instance() )
        return ON_PLANE;

	// the result must be > 0 here
	return POSITIVE_PLANE_SIDE;
}

void MexPlane3d::definePlane( const MexPoint3d& p0,
					      		   const MexPoint3d& p1,
								   const MexPoint3d& p2 )
{
    MexPlane3d plane( p0, p1, p2 );
    *this = plane;

    TEST_INVARIANT;
    POST( contains( p0 ) );
    POST( contains( p1 ) );
    POST( contains( p2 ) );
}


void MexPlane3d::definePlane( const MexTransform3d& tx)
{

    MexPlane3d plane( tx );
    *this = plane;

    TEST_INVARIANT;
}



const MexPlane3d& MexPlane3d::operator =( const MexPlane3d& rhs )
{

    if( this != &rhs )
    {
        a_ = rhs.a_;
        b_ = rhs.b_;
        c_ = rhs.c_;
        d_ = rhs.d_;
		normal_ = rhs.normal_;
        planeTransform_ = rhs.planeTransform_;
    }

    return *this;
}


bool MexPlane3d::operator ==( const MexPlane3d& rhs ) const
{
    TEST_INVARIANT;

    bool x =  a_ == rhs.a_ and
           b_ == rhs.b_ and
		   c_ == rhs.c_ and
           d_ == rhs.d_ and
           normal_ == rhs.normal_;

/* if (x==true)
    { cout << "This is an equal case ";
      cout << *this << endl << rhs;
	}*/

    return x;
}

//  Return the X coordinate of a point on the plane given the Y and Z coordinates
MATHEX_SCALAR   MexPlane3d::x( MATHEX_SCALAR y, MATHEX_SCALAR z ) const
{
    PRE( a() != 0.0 );

    return - ( b() * y + c() * z + d() ) / a();
}

//  Return the Y coordinate of a point on the plane given the X and Z coordinates
MATHEX_SCALAR   MexPlane3d::y( MATHEX_SCALAR x, MATHEX_SCALAR z ) const
{
    PRE( b() != 0.0 );

    return - ( a() * x + c() * z + d() ) / b();
}

//  Return the Z coordinate of a point on the plane given the X and Y coordinates
MATHEX_SCALAR   MexPlane3d::z( MATHEX_SCALAR x, MATHEX_SCALAR y ) const
{
    PRE( c() != 0.0 );

    return - ( a() * x + b() * y + d() ) / c();
}


/*
void MexPlane3d::calculatePlaneEquation( const MexVec3& pos )
{
    PRE( isValid( pos ) );

    // compute the cross product of d0_ and d1_.
    // This is a normal vector to the MexPlane3d
    a_ = d0_.y() * d1_.z() - d0_.z() * d1_.y();
    b_ = d0_.z() * d1_.x() - d0_.x() * d1_.z();
    c_ = d0_.x() * d1_.y() - d0_.y() * d1_.x();

    // calculate the constant d
    d_ = - ( a * pos.x() + b * pos.y() + c * pos.z() );
}


void MexPlane3d::makeDirectionVectors( const MexVec3& p0,
                        const MexVec3& p1,
                        const MexVec3& p2 )
{
    PRE( isValid( p0 ) and isValid( p1 ) and isValid( p2 ) );

    d0_ = p1 - p2;
    d1_ = p1 - p3;
}
*/
/*

MexPoint3d MexPlane3d::createRandomPlanePoint() const
{
    PRE( Mathex::abs(a_) > 0 or Mathex::abs(b_) > 0 or Mathex::abs(c_) > 0);
    TEST_INVARIANT;

	T x, y, z;

    if ( Mathex::abs( c_ ) > 0)
	{
		 x = rand(); // randomFloat(1000);
		 y = rand(); // randomFloat(1000);
		 z = -( x * a_ + y * b_ + d_ ) / c_;
	}
	else if ( Mathex::abs( b_ ) > 0 )
	{
	 	 x = rand(); // randomFloat(1000);
		 z = rand(); // randomFloat(1000);
		 y = -( x * a_ + z * c_ + d_ ) / b_;
	}
	else if ( Mathex::abs( a_ ) > 0)
	{
		 y = rand(); // randomFloat(1000);
		 z = rand(); // randomFloat(1000);
		 x = -( y * b_ + z * c_ + d_ ) / a_;
	}
	else
	{
		cout << "disaster" << endl;
		POST( true == false );
	}

	MexVec3 v( x, y, z );

	return v;
}
*/

ostream& operator <<( ostream& o, const MexPlane3d& p )
{

    return o << p.a() << " " << p.b() << " " << p.c() << " " << p.d() << std::endl
	        /*<< o*/ << p.normal() << std::endl;
}

PER_DEFINE_PERSISTENT( MexPlane3d );

void perWrite( PerOstream& stream, const MexPlane3d& t )
{
	stream << t.a_;
	stream << t.b_;
	stream << t.c_;
	stream << t.d_;
	stream << t.planeTransform_;
	stream << t.normal_;
}

void perRead( PerIstream& stream, MexPlane3d& t )
{
	stream >> t.a_;
	stream >> t.b_;
	stream >> t.c_;
	stream >> t.d_;
	stream >> t.planeTransform_;
	stream >> t.normal_;
}

/* End PLANE3D.CPP *****************************************************/

