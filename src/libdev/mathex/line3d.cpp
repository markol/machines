/*
 * L I N E 3 D . C P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#include "mathex/mathex.hpp"

#include "mathex/line3d.hpp"
#include "mathex/cline3d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/point3d.hpp"
#include "mathex/plane3d.hpp"
#include "mathex/epsilon.hpp"
#include "profiler/stktrace.hpp"

#ifndef  _INLINE
    #include "mathex/line3d.ipp"
#endif

////////////////////////////////////////

bool MexPoint3d::contains( const MexLine3d& l) const
{
    return (l.end1() == l.end2() and l.end2() == *this);
}

bool MexPoint3d::intersects(const MexLine3d& l ) const
{
	// intentionally empty
	return l.contains( *this );
}

////////////////////////////////////////


MexLine3d::MexLine3d()
:   length_( 0.0 )
{
	/* intentionally empty */
}


MexLine3d::MexLine3d( const MexPoint3d& p1, const MexPoint3d& p2 )
:	end1_( p1 ),
    end2_( p2 ),
    length_( -1.0 )
{
	
    /* intentionally empty */
}

MexLine3d::MexLine3d( const MexPoint3d& p1, const MexPoint3d& p2, MATHEX_SCALAR length )
:	end1_( p1 ),
    end2_( p2 ),
    length_( length )
{
	PRE( fabs( p1.euclidianDistance( p2 ) - length ) < MexEpsilon::instance() );
}

MexLine3d::MexLine3d( const MexLine3d& rhs )
:	end1_( rhs.end1_ ),
	end2_( rhs.end2_ ),
    length_( rhs.length_ )
{
}


MexLine3d::~MexLine3d()
{
	/* Intentionally empty */
}


MATHEX_SCALAR MexLine3d::length() const
{
    //Compute and store length if not cached already
    if( length_ < 0.0 )
    {
        MexLine3d* pNonConstThis = _CONST_CAST( MexLine3d*, this );
        pNonConstThis->length_ = end1_.euclidianDistance( end2_ );
    }
  
    return length_;  
}
/*

_CODE_INLINE
const MexPoint3d& MexLine3d::centre() const
{
    if( !endPointsValid_ )
        calculateEndPoints();
        
    return centre_;
}
*/

bool MexLine3d::intersects( const MexLine3d& rhs ) const
{

	MexPoint3d l1 = end1();
	MexPoint3d l2 = rhs.end1();
	MexPoint3d d1 = end2();
	MexPoint3d d2 = rhs.end2();

	d1 -= l1;		// direction vector = end2 - end1
	d2 -= l2;   		// direction vector = rhs.end2 - rhs.end1

	MATHEX_SCALAR num 	 = ( ( l1.y() - l2.y() ) * d1.x() ) - ( ( l1.x() - l2.x() ) * d1.y() );
	MATHEX_SCALAR denom = ( d1.x() * d2.y() ) - ( d1.y() * d2.x() );

//	cout << "numerator:   " << num << endl;
//	cout << "denominator: " << denom << endl;

	MATHEX_SCALAR t = ( num / denom );

	num = ( l1.y() - l2.y() ) * d2.x() - ( l1.x() - l2.x() ) * d2.y();
	
	MATHEX_SCALAR s = ( num / denom );

//	cout << "t: " << t << endl << "s: " << s << endl;

	MATHEX_SCALAR x, y, z;
	x = l1.x() + ( d1.x() * s );
	y = l1.y() + ( d1.y() * s );
	z = l1.z() + ( d1.z() * s );

/*	cout << "l1.x = " << l1.x() << "  d1.x = " << d1.x() << endl;
	cout << "l1.y = " << l1.y() << "  d1.y = " << d1.y() << endl;
	cout << "l1.z = " << l1.z() << "  d1.z = " << d1.z() << endl;
*/
	MexPoint3d point1( x, y, z );

/*	cout << "l2.x = " << l2.x() << "  d2.x = " << d2.x() << endl;
	cout << "l2.y = " << l2.y() << "  d2.y = " << d2.y() << endl;
	cout << "l2.z = " << l2.z() << "  d2.z = " << d2.z() << endl;
*/
	x = l2.x() + ( d2.x() * t );
	y = l2.y() + ( d2.y() * t );
	z = l2.z() + ( d2.z() * t );

	MexPoint3d point2( x, y, z );

//	cout.precision( 10 );
//	cout << "point1: " << point1 << endl << "point2: " << point2 << endl;

	point2 -= point1;

//	cout << "new point: " << Mathex::abs( point2.x() ) << ", " 
//		 << Mathex::abs( point2.y() ) << ", " << Mathex::abs( point2.z() ) << endl;

	const MATHEX_SCALAR epsilon = (MATHEX_SCALAR)0.01;

	// if the points are not the same then the lines do not intersect
	if( MATHEX_SCALAR( Mathex::abs( point2.x() ) > epsilon ) or
  	    MATHEX_SCALAR( Mathex::abs( point2.y() ) > epsilon ) or
 	    MATHEX_SCALAR( Mathex::abs( point2.z() ) > epsilon ) )
		return false;

	// if both t and s lie between 0 and 1 then the line SEGMENTS intersect
	// which is what we want
	bool result = ( 
			( t >= -MexEpsilon::instance() )
					and 
				( t <= ( (MATHEX_SCALAR)1.0 + MexEpsilon::instance() ) )
					and 
			( s >= -(MATHEX_SCALAR)MexEpsilon::instance() )
					and 
		( s <= ( (MATHEX_SCALAR)1.0 + MexEpsilon::instance() ) ) );

/*	cout << "result is ";
	if( result ) 
		cout << "true" << endl;
	else	
 		cout << "false" << endl;
*/
	return result;
} 


bool MexLine3d::contains( const MexPoint3d& p) const
{
	return isPointOnLine( p );
}
        

bool MexLine3d::contains( const MexLine3d& l2 ) const
{
	return contains( l2.end1() ) and contains( l2.end2() );
		
}
/*        

bool MexLine3d::contains( const MexAlignedBox3d& rhs ) const
{
	PRE( isValid( rhs ) );

	MexIntersectionData* inter;
	return ((rhs.minCorner() == rhs.maxCorner()) and rhs.intersects(*this, inter));
}
*/
/*

bool MexLine3d::contains( const MexUnalignedBox3d& u) const
{
	PRE (isValid(u));

	MexPoint3d v = u.dimensions();
    MATHEX_SCALAR x = v.x();
	MATHEX_SCALAR y = v.y();
	MATHEX_SCALAR z = v.z();

	MexVec3 nVec(x,y,z);
	if( nVec.isZeroVector() == false )
		return false;

	return isPointOnLine( v );
}
*/ 
        
/*
bool MexLine3d::contains( const MexBall3d& b) const
{
	PRE( isValid( b ) );

	// intentionally empty
	return FALSE;
}*/
        
/*

INLINE
bool MexLine3d::contains( const MexHalfSpace3d& ) const
{
//PRE( isValid( p ) );

    return false;	   
}
*/
/*
template < class MATHEX_SCALAR >
_CODE_INLINE
bool MexLine3d::intersects( const MexBoundary3d& b, MexIntersectionData* i) const

{
    return b.intersects( *this, i );
}
*/


bool MexLine3d::intersects( const MexPoint3d& v ) const
{
	if ( ( end1() == v ) or ( end2() == v ) )
		return true;

	return isPointOnLine( v );
}
/*        

bool MexLine3d::intersects( const MexAlignedBox3d& a, MexIntersectionData* i ) const
{
	PRE( isValid( a ) );
	
	return a.intersects( *this, i );	
}
*/
/*

_CODE_INLINE        
bool MexLine3d::intersects( const MexUnalignedBox3d& a, MexIntersectionData* i) const
{
	PRE( isValid( a ) );

	return a.intersects( *this, i );
}
*/
/*

_CODE_INLINE        
bool MexLine3d::intersects( const MexBall3d& b) const
{
	PRE( isValid( b ) );

	//intentionally empty
	return false;
}*/


bool MexLine3d::intersects( const MexPlane3d& p ) const
{
	return p.intersects( *this );

/*	return	(p.distanceFrom(end1_) >= 0 and p.distanceFrom(end2_) <= 0) or
			(p.distanceFrom(end1_) <= 0 and p.distanceFrom(end2_) >= 0);  */
}


/*

_CODE_INLINE
bool MexLine3d::intersects( const MexHalfSpace3d& p, MexIntersectionData* i) const
{
    PRE( isValid( p ) );

	return p.intersects( *this, i );
}
*/


bool MexLine3d::isPointOnLine( const MexPoint3d& v ) const
{
	MexVec3 d(end2().x(),end2().y(),end2().z());

	// calculate direction vector d	
	MexVec3 s(end1().x(),end1().y(),end1().z());
	d -= s;
		 
	MATHEX_SCALAR t;

	// first check to see if the points are the same
	if( end1() == end2() )
		return end1() == v;

	// calculate a value for t whilst being wary of the division by zero

//	MATHEX_SCALAR a = Mathex::abs( d.x() );

	if( Mathex::abs( d.x() ) > (MATHEX_SCALAR)MexEpsilon::instance() )
	{
		t = ( v.x() - end1().x() ) / d.x();
//		cout << "calculated t from x" << endl;
	}
	else if( Mathex::abs( d.y() ) > (MATHEX_SCALAR)MexEpsilon::instance() )
	{
		t = ( v.y() - end1().y() ) / d.y();
//		cout << "calculated t from y" << endl;
	}
	else if( Mathex::abs( d.z() ) > MexEpsilon::instance() )
	{
		t = ( v.z() - end1().z() ) / d.z();
//		cout << "calculated t from z" << endl;
	}

	// result = end1 + ( d * t )
	
	d *= t;
	MexVec3 result(s);
	result += d;

	// see if the points are the same
/*	cout << "t = " << t << endl;
	cout << "d = " << d << endl;
	cout << "v = " << v << endl;
	cout << "res = " << result << endl << endl;
*/
	
	MexVec3 v2(v.x(),v.y(),v.z());
	return result == v2;
}
												   
const MexLine3d& MexLine3d::operator =( const MexLine3d& rhs )
{
	if ( this != &rhs )
	{
		end1_ = rhs.end1();
		end2_ = rhs.end2();
        length_ = rhs.length_;
	}

	return *this;
}

bool MexLine3d::operator ==( const MexLine3d& rhs ) const
{
	return end1() == rhs.end1() and end2() == rhs.end2();
}

/*
template < class MATHEX_SCALAR >
ostream& operator <<( ostream& o, MexLine3d& rhs )
{
	PRE( isValid( o ) );
	PRE( isValid( rhs ) );

	return o << rhs.end1() << "    " << rhs.end2();
}
*/

//static
MATHEX_SCALAR MexLine3d::sqrEuclidianDistance
(
    const MexPoint3d& a1, const MexPoint3d& b1, MATHEX_SCALAR l1,
    const MexPoint3d& a2, const MexPoint3d& b2, MATHEX_SCALAR l2
)
{
    MATHEX_SCALAR epsilon = MexEpsilon::instance();

    PRE(l1 > epsilon);
    PRE(l2 > epsilon);
    PRE( fabs( a1.euclidianDistance( b1 ) - l1 ) < epsilon );
    PRE( fabs( a2.euclidianDistance( b2 ) - l2 ) < epsilon );

    MATHEX_SCALAR result;

	// compute the unit directional vector for each line
	MexVec3 d1( a1, b1, l1);
	MexVec3 d2( a2, b2, l2);

	MATHEX_SCALAR d1d2=d1.dotProduct(d2);
	MATHEX_SCALAR d1d2d1d2=d1d2*d1d2;

	// Are the two lines parallel ?
	if (1-d1d2d1d2<epsilon)
	{
	   // yes compute point to line distance for each of the four points and keep the smallest
	   MATHEX_SCALAR delta1=MexLine3d::sqrEuclidianDistance(a1,b1,l1,a2);
	   MATHEX_SCALAR delta2=MexLine3d::sqrEuclidianDistance(a1,b1,l1,b2);
	   MATHEX_SCALAR delta3=MexLine3d::sqrEuclidianDistance(a2,b2,l2,a1);
	   MATHEX_SCALAR delta4=MexLine3d::sqrEuclidianDistance(a2,b2,l2,b1);
	   result=(delta1<delta2) ?	delta1 : delta2;
	   if (delta3<result)
	    result=delta3;
	   if (delta4<result)
	    result=delta4;
	}
	else
	{
	  // no the two lines are not parallel
	  // compute lambda1 and lambda2 such that q1q2 is orthogonal to d1 and to d2
	  // with a1q1=lambda1 d1 
	  // and  a2q2=lambda2 d2

	  MexVec3 a1a2=MexVec3(a1,a2);

	  MexVec3 d2d1d2(d2);
	  d2d1d2*=d1d2;
	  MexVec3 tmp1(d1);
	  tmp1-=d2d1d2;
	  MATHEX_SCALAR lambda1=a1a2.dotProduct(tmp1)/(1-d1d2d1d2);

	  MexVec3 d1d1d2(d1);
	  d1d1d2*=d1d2;
	  MexVec3 tmp2(d2);
	  tmp2-=d1d1d2;
	  MATHEX_SCALAR lambda2=-a1a2.dotProduct(tmp2)/(1-d1d2d1d2);

	  MexPoint3d closest1;
	  MexPoint3d closest2;

	  // q1 could be outside the segment a1b1, and q2 outside a2b2
	  // check for these particular cases
	  if (lambda1<=0 or lambda1>=l1)
	  {
		// Which point is the closest to line 2 a1 or b1 ?
		closest1=(lambda1<=0) ? a1 : b1;
		if (lambda2<=0 or lambda2>=l2)
		{
		// Which point is the closest to line 1 a2 or b2 ?
		  closest2=(lambda2<=0) ? a2 : b2;
		  MATHEX_SCALAR result1=MexLine3d::sqrEuclidianDistance(a1,b1,l1,closest2);
		  MATHEX_SCALAR result2=MexLine3d::sqrEuclidianDistance(a2,b2,l2,closest1);
		  result=(result1<result2) ? result1 : result2;
		}
		else
		{
		  result=MexLine3d::sqrEuclidianDistance(a2,b2,l2,closest1);
		}
	  }
	  else
	  {
		if (lambda2<=0 or lambda2>=l2)
		{
		// Which point is the closest to line 1 a2 or b2 ?
	      closest2=(lambda2<=0) ? a2 : b2;
		  result=MexLine3d::sqrEuclidianDistance(a1,b1,l1,closest2);
		}
		else
		{
  		  // the points q1 qnd q2 are in their segments
  		  //  i.e q1=a1+lambda1 d1
  		  //  i.e q2=a2+lambda2 d2
    	  MexPoint3d q1(a1);
  	      MexPoint3d q2(a2);
  	      MexVec3 s1(d1);
  		  s1*=lambda1;
  		  q1+=s1;
  	      MexVec3 s2(d2);
  		  s2*=lambda2;
  		  q2+=s2;
  	      result =q1.sqrEuclidianDistance(q2);
		}
	  }	  

	}

	return result;
}

MATHEX_SCALAR MexLine3d::sqrEuclidianDistance
(
        const MexPoint3d& a, const MexPoint3d& b, MATHEX_SCALAR l,
        const MexPoint3d& p
)
{
    MATHEX_SCALAR epsilon = MexEpsilon::instance();

    PRE(l > epsilon);
    PRE( fabs( a.euclidianDistance( b ) - l ) < epsilon );

    MATHEX_SCALAR result;

	MexPoint3d q;

	// compute the unit directional vector of the line
	MexVec3 d( a, b, l);
	MexVec3 ap(a,p);

	// p projects on ab on a point q such that aq = lambda d
	// the result should thus be d(p,q)
	// first compute lambda
	MATHEX_SCALAR lambda=ap.dotProduct(d);

	// check for particular cases 
	// (i.e. the projection is outsidethe segment ab)
	if (lambda<=0)
	{
	    q=a;
	}
	else if (lambda>=l)
	{
	    q=b;
	}
	else
	{
		// p projects in the segment i.e q=a+lambda d
	    MexVec3 s(d);
		s*=lambda;
		q=a;
		q+=s;
	}

	result=p.sqrEuclidianDistance(q);

	return result;
}

void MexLine3d::setLine( const MexPoint3d& p1, const MexPoint3d& p2 )
{
	end1_ = p1;
	end2_ = p2;
    length_ = -1.0;
}

MexVec3 MexLine3d::unitDirectionVector() const
{
    //Get the length
    MATHEX_SCALAR s = length();

    //If just a point return any vector, otherwise make one
    if( s == 0.0 )
        return MexVec3(1.0, 0.0, 0.0 );
    else
        return MexVec3( end1_, end2_, s );
}

void MexLine3d::transform( const MexTransform3d& transform )
{
    //Transform the endpoints. Length is unchanged
    transform.transform( &end1_ );
    transform.transform( &end2_ );
}


void MexLine3d::transformInverse( const MexTransform3d& transform )
{
    //InverseTransform the endpoints. Length is unchanged
    transform.transformInverse( &end1_ );
    transform.transformInverse( &end2_ );
}

bool MexLine3d::segmentIntersects( const MexPoint3d& p1, const MexPoint3d& p2, const MexPoint3d& p3, 
                            MATHEX_SCALAR* pDistance, IntersectSide side ) const
{
    MATHEX_SCALAR length;
    bool result = intersects( p1, p2, p3, pDistance, &length, side );

    //As well as checking for the infinite line intersecting, also check 
    //that this intersection distance falls within the range of this line
    //segment. Allow for rounding errors though.
    if ( result )
    {
        MATHEX_SCALAR eps = MexEpsilon::instance();
        MATHEX_SCALAR distance = *pDistance;
        if( distance < -eps  or  distance > (length + eps) )
            result = false;
    }

    return result;
}

bool MexLine3d::intersects( 
    const MexPoint3d& p1, const MexPoint3d& p2, const MexPoint3d& p3, 
    MATHEX_SCALAR* pDistance, MATHEX_SCALAR* pLength, IntersectSide side ) const
{
    PRE( pDistance != NULL );
    PRE( pLength != NULL );
    
    //Construct edge vectors for the triangle
    const MexVec3 p1p2( p1, p2 );
    const MexVec3 p2p3( p2, p3 );
    const MexVec3 p3p1( p3, p1 );

    //Hence construct a normal vector, and plane equation constant
    MexVec3 n = MexVec3::crossProduct( p1p2, p2p3 );
    MATHEX_SCALAR h = n.dotProduct( MexVec3( p1 ) );


    //Hence compute the distance along the line to the point of intersection,
    //provided there is one
    *pLength = length();
    const MexVec3 d( end1_, end2_, *pLength );

    const MATHEX_SCALAR denom = d.dotProduct( n );
    const MATHEX_SCALAR eps = MexEpsilon::instance();
    bool result = fabs( denom ) > eps;

    if( result and side == INTERSECT_FRONT_SIDE )
    {
        //The dot product of the normal with our incoming vector tells us 
        //which side we're approaching from

        result = denom < 0.0;
    }
    
    if( result )
    {
        //Compute the distance to triangle plane
        MATHEX_SCALAR lambda = (h - n.dotProduct( MexVec3( end1_ ) )) / denom;

        //Compute the point of intersection
        MexPoint3d a( end1_.x() + lambda * d.x(),
                      end1_.y() + lambda * d.y(),
                      end1_.z() + lambda * d.z() );

        //Perform point inside triangle test
        result = n.dotProduct( MexVec3::crossProduct( p1p2, MexVec3( p1, a ) ) ) >= -eps and
                 n.dotProduct( MexVec3::crossProduct( p2p3, MexVec3( p2, a ) ) ) >= -eps and
                 n.dotProduct( MexVec3::crossProduct( p3p1, MexVec3( p3, a ) ) ) >= -eps;

        //If scored a hit, return the distance
        if( result )
            *pDistance = lambda;
    }

    return result;
}

bool MexLine3d::intersects( const MexPoint3d& p1, const MexPoint3d& p2, const MexPoint3d& p3, 
                            MATHEX_SCALAR* pDistance, IntersectSide side ) const
{
    MATHEX_SCALAR length;
    bool result = intersects( p1, p2, p3, pDistance, &length, side );

    return result;
}

MexPoint3d MexLine3d::pointAtDistance( MATHEX_SCALAR distance ) const
{
    //Get direction vector etc
    MexVec3 d = unitDirectionVector();

    return MexPoint3d( end1_.x() + distance * d.x(),
                       end1_.y() + distance * d.y(),
                       end1_.z() + distance * d.z() );
}

ostream& operator<<(ostream& os, const MexLine3d& line)
{
	os << "3D line: " << line.end1() << " to " << line.end2();
	return os;
}

PER_DEFINE_PERSISTENT( MexLine3d );

void perWrite( PerOstream& str, const MexLine3d& t )
{
    str << t.end1_;
    str << t.end2_;
	str << t.length_;
}

void perRead( PerIstream& str, MexLine3d& t )
{
    str >> t.end1_;
    str >> t.end2_;
	str >> t.length_;
}

MexLine3d::MexLine3d( const MexCompressedLine3d& line )
:	end1_( line.end1_ ),
	end2_( line.end2_ ),
    length_( -1.0 )
{
}

/* End LINE3D.CPP *****************************************************/
