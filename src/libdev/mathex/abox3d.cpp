/*
 * A B O X 3 D . C P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#include <algorithm>

#include "mathex/mathex.hpp"
#include "mathex/point3d.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/plane3d.hpp"
#include "mathex/line3d.hpp"

#ifndef _INLINE
    #include "mathex/abox3d.ipp"
#endif

//////////////////////////////////////////////////////////////////////

MexAlignedBox3d::MexAlignedBox3d( const MexPoint3d& minCorner, const MexPoint3d& maxCorner )
:   minCorner_( minCorner ),
    maxCorner_( maxCorner ),
    isEmpty_( false ),
    spinSafe_( false )
{
    TEST_INVARIANT;
}


MexAlignedBox3d::MexAlignedBox3d( const MATHEX_SCALAR x1, const MATHEX_SCALAR y1, const MATHEX_SCALAR z1,
                                  const MATHEX_SCALAR x2, const MATHEX_SCALAR y2, const MATHEX_SCALAR z2 )
: isEmpty_( false ),
  spinSafe_( false )
{
    PRE( x1 <= x2 and y1 <= y2 and z1 <= z2 );

    minCorner_.setPoint( x1, y1, z1 );
    maxCorner_.setPoint( x2, y2, z2 );
    TEST_INVARIANT;
}


MexAlignedBox3d::MexAlignedBox3d( const MexAlignedBox3d& rhs )
:   minCorner_( rhs.minCorner_ ),
    maxCorner_( rhs.maxCorner_ ),
    isEmpty_( rhs.isEmpty_ ),
    spinSafe_( rhs.spinSafe_ )
{
    POST( *this == rhs );
}


MexAlignedBox3d::MexAlignedBox3d( const MexPoint3d& centre,
          MATHEX_SCALAR xDimension, MATHEX_SCALAR yDimension, MATHEX_SCALAR zDimension )
: isEmpty_( false ),
  spinSafe_( false )
{
    PRE( xDimension >= 0 );
    PRE( yDimension >= 0 );
    PRE( zDimension >= 0 );

    minCorner_.setPoint( centre.x() - xDimension / 2,
                         centre.y() - yDimension / 2,
                         centre.z() - zDimension / 2 );

    maxCorner_.setPoint( centre.x() + xDimension / 2,
                         centre.y() + yDimension / 2,
                         centre.z() + zDimension / 2 );

    POST_DATA( MexPoint3d minCorner( centre ) );
    POST_DATA( minCorner -= MexVec3( xDimension / 2, yDimension / 2, zDimension / 2 ) );

    POST_DATA( MexPoint3d maxCorner( centre ) );
    POST_DATA( maxCorner += MexVec3( xDimension / 2, yDimension / 2, zDimension / 2 ) );

    POST( minCorner_ == minCorner );
    POST( maxCorner_ == maxCorner );
}

MexAlignedBox3d::MexAlignedBox3d( const MexAlignedBox3d& rhs, const MexTransform3d& t )
: isEmpty_( rhs.isEmpty_ ),
  spinSafe_( rhs.spinSafe_ )
{
    //Get the transformed min corner of rhs
    MexPoint3d transformedCorner( rhs.minCorner_ );
    t.transform( &transformedCorner );

    //Construct vectors representing the displacements along the edges
    //from the world transformed minCorner. Bear in mind that the
    //basis vectors of the world transform are precisely the images
    //of the x, y and z axis direction vectors under the transform.
    //So we just take these vectors and multiply by the original
    //aligned box side lengths.
    MexVec3 xEdge = t.xBasis();
    MexVec3 yEdge = t.yBasis();
    MexVec3 zEdge = t.zBasis();
    MexVec3 diagonal( rhs.minCorner_, rhs.maxCorner_ );

    xEdge *= diagonal.x();
    yEdge *= diagonal.y();
    zEdge *= diagonal.z();

    //hence compute the transformed rhs aligned boundary points
    minCorner_ = MexPoint3d(
        transformedCorner.x() + boxMin( xEdge.x(), yEdge.x(), zEdge.x() ),
        transformedCorner.y() + boxMin( xEdge.y(), yEdge.y(), zEdge.y() ),
        transformedCorner.z() + boxMin( xEdge.z(), yEdge.z(), zEdge.z() ) );

    maxCorner_ = MexPoint3d(
        transformedCorner.x() + boxMax( xEdge.x(), yEdge.x(), zEdge.x() ),
        transformedCorner.y() + boxMax( xEdge.y(), yEdge.y(), zEdge.y() ),
        transformedCorner.z() + boxMax( xEdge.z(), yEdge.z(), zEdge.z() ) );

}

MexAlignedBox3d::~MexAlignedBox3d( )
{
    /* Intentionally Empty    */
}


const MexAlignedBox3d& MexAlignedBox3d::operator =( const MexAlignedBox3d& b )
{
    minCorner_ = b.minCorner_;
    maxCorner_ = b.maxCorner_;
    isEmpty_ = b.isEmpty_;
    spinSafe_ = spinSafe_;

    POST( *this == b );

    return *this;
}

void MexAlignedBox3d::makeSpinSafe()
{
    //Compute the diagonal length of the box, and hence the radius of its
    //bounding sphere
    MexPoint3d& minCorner = minCorner_;
    MexPoint3d& maxCorner = maxCorner_;

    MATHEX_SCALAR radius = minCorner.euclidianDistance( maxCorner ) / 2;

    //Construct the existing centroid
    MexPoint3d centre ( ( minCorner.x() + maxCorner.x() ) /2,
                        ( minCorner.y() + maxCorner.y() ) /2,
                        ( minCorner.z() + maxCorner.z() ) /2 );

    //Hence redefine the corners so that we have a new cube that bounds
    //the sphere which bound the original
    minCorner.setPoint( centre.x() - radius,
                        centre.y() - radius,
                        centre.z() - radius );

    maxCorner.setPoint( centre.x() + radius,
                        centre.y() + radius,
                        centre.z() + radius );

    //Set the spinsafe flag
    spinSafe_ = true;
}

//static
MATHEX_SCALAR MexAlignedBox3d::boxMin( MATHEX_SCALAR x,  MATHEX_SCALAR y,  MATHEX_SCALAR z )
{
    //Compute the minimum of 0, x, y, z, x+y, x+z, y+z, x+y+z
    MATHEX_SCALAR a = ( x < y ? x : y );
    MATHEX_SCALAR b = a + z;
    MATHEX_SCALAR c = x + y;

    b = ( b < c ? b : c );
    c += z;
    b = ( b < c ? b : c );
    a = ( a < z ? a : z );
    c = ( a < b ? a : b );

    return ( c > 0 ? 0 : c );
}

//static
MATHEX_SCALAR MexAlignedBox3d::boxMax( MATHEX_SCALAR x,  MATHEX_SCALAR y,  MATHEX_SCALAR z )
{
    //Compute the maximum of 0, x, y, z, x+y, x+z, y+z, x+y+z
    MATHEX_SCALAR a = ( x > y ? x : y );
    MATHEX_SCALAR b = a + z;
    MATHEX_SCALAR c = x + y;

    b = ( b > c ? b : c );
    c += z;
    b = ( b > c ? b : c );
    a = ( a > z ? a : z );
    c = ( a > b ? a : b );

    return (c < 0 ? 0 : c );
}

const MexAlignedBox3d& MexAlignedBox3d::zeroBox()
{
    static MexAlignedBox3d zero_ = MexAlignedBox3d( 0, 0, 0, 0, 0, 0 );
    return zero_;
}

ostream& operator <<( ostream& o, const MexAlignedBox3d& rhs )
{
    return o << rhs.minCorner_ << "  ----  " << rhs.maxCorner_;
}


MATHEX_SCALAR MexAlignedBox3d::sqrEuclidianDistance( const MexPoint3d& p ) const
{
    //Compute the distance in each direction
    MATHEX_SCALAR x = p.x();
    MATHEX_SCALAR y = p.y();
    MATHEX_SCALAR z = p.z();
    MATHEX_SCALAR xOffset, yOffset, zOffset;

    const MexPoint3d& qMin = minCorner_;
    const MexPoint3d& qMax = maxCorner_;
    MATHEX_SCALAR xMin = qMin.x();
    MATHEX_SCALAR xMax = qMax.x();
    MATHEX_SCALAR yMin = qMin.y();
    MATHEX_SCALAR yMax = qMax.y();
    MATHEX_SCALAR zMin = qMin.z();
    MATHEX_SCALAR zMax = qMax.z();
    if( x > xMax)
        xOffset = x - xMax;
    else if( x < xMin )
        xOffset = xMin - x;
    else
        xOffset = 0.0;

    if( y > yMax)
        yOffset = y - yMax;
    else if( y < yMin )
        yOffset = yMin - y;
    else
        yOffset = 0.0;

    if( z > zMax)
        zOffset = z - zMax;
    else if( z < zMin )
        zOffset = zMin - z;
    else
        zOffset = 0.0;

    return xOffset*xOffset + yOffset*yOffset + zOffset*zOffset;
}


bool MexAlignedBox3d::intersects
(
    const MexLine3d& line, MATHEX_SCALAR* pEntryDistance, MATHEX_SCALAR* pExitDistance,
    Mathex::TouchingRule rule
) const
{
    bool result = false;

    //Get the line start point and a unit direction vector towards second point
    const MexPoint3d& p = line.end1();
    MexVec3 d( line.unitDirectionVector() );
    MATHEX_SCALAR px = p.x();
    MATHEX_SCALAR py = p.y();
    MATHEX_SCALAR pz = p.z();
    MATHEX_SCALAR dx = d.x();
    MATHEX_SCALAR dy = d.y();
    MATHEX_SCALAR dz = d.z();

    //Get refs to the limits and store them locally
    const MexPoint3d& minPoint = minCorner_;
    const MexPoint3d& maxPoint = maxCorner_;
    MATHEX_SCALAR xMin = minPoint.x();
    MATHEX_SCALAR xMax = maxPoint.x();
    MATHEX_SCALAR yMin = minPoint.y();
    MATHEX_SCALAR yMax = maxPoint.y();
    MATHEX_SCALAR zMin = minPoint.z();
    MATHEX_SCALAR zMax = maxPoint.z();

    //Produce modified values depending on the touching rule
    MATHEX_SCALAR epsilon = MexEpsilon::instance();
    MATHEX_SCALAR ruleEpsilon = epsilon;
    if( rule == Mathex::TOUCH_ISNT_INTERSECT )
        ruleEpsilon = -ruleEpsilon;

    MATHEX_SCALAR exMin = xMin - ruleEpsilon;
    MATHEX_SCALAR exMax = xMax + ruleEpsilon;
    MATHEX_SCALAR eyMin = yMin - ruleEpsilon;
    MATHEX_SCALAR eyMax = yMax + ruleEpsilon;
    MATHEX_SCALAR ezMin = zMin - ruleEpsilon;
    MATHEX_SCALAR ezMax = zMax + ruleEpsilon;

    //Check each face, getting distance to intersection point.
    MATHEX_SCALAR lambda, denom, x, y, z, aHitDistances[6];
    int nHits = 0;

    //Deal with xmin and xmax faces
    denom = d.x();
    if( fabs( denom ) > epsilon )
    {
        x = xMin;
        for( int i = 0; i != 2 ; x = xMax, ++i )
        {
            lambda = (x - px) / denom;
            y = py + (lambda * dy);
            if( y > eyMin and y < eyMax )
            {
                z = pz + (lambda * dz);
                if( z > ezMin and z < ezMax )
                    aHitDistances[ nHits++ ] = lambda;
            }
        }
    }

    //Deal with ymin and ymax faces
    denom = d.y();
    if( fabs( denom ) > epsilon )
    {
        y = yMin;
        for( int i = 0; i != 2 ; y = yMax, ++i )
        {
            lambda = (y - py) / denom;
            x = px + (lambda * dx);
            if( x > exMin and x < exMax )
            {
                z = pz + (lambda * dz);
                if( z > ezMin and z < ezMax )
                    aHitDistances[ nHits++ ] = lambda;
            }
        }
    }

    //Deal with zmin and zmax faces
    denom = d.z();
    if( fabs( denom ) > epsilon )
    {
        z = zMin;
        for( int i = 0; i != 2 ; z = zMax, ++i )
        {
            lambda = (z - pz) / denom;
            x = px + (lambda * dx);
            if( x > exMin and x < exMax )
            {
                y = py + (lambda * dy);
                if( y > eyMin and y < eyMax )
                    aHitDistances[ nHits++ ] = lambda;
            }
        }
    }

    //Select the lowest and highest hit distances if any
    result = nHits != 0;

    if( result )
    {
        MATHEX_SCALAR entryDistance = aHitDistances[0];
        MATHEX_SCALAR exitDistance = entryDistance;

        for( int i = 1; i != nHits; ++i )
        {
            MATHEX_SCALAR distance = aHitDistances[i];

            if( distance < entryDistance )
                entryDistance = distance;

            if( distance > exitDistance )
                exitDistance = distance;
        }

        //Check the hits on the infinite line overlap the line segment
        result = exitDistance > -ruleEpsilon  and  entryDistance < line.length() + ruleEpsilon;
        if( result )
        {
            *pEntryDistance = entryDistance;
            *pExitDistance = exitDistance;
        }
    }

    return result;
}


void MexAlignedBox3d::uniteWith( const MexAlignedBox3d& rhs, const MexTransform3d& t )
{
    MexAlignedBox3d transformedBox( rhs, t );

    const MexPoint3d& rhsMinPoint = transformedBox.minCorner();
    const MexPoint3d& rhsMaxPoint = transformedBox.maxCorner();

    //Now modify the boundary data to include the rhs transformed boundary
    MexPoint3d& minPoint = minCorner_;
    MexPoint3d& maxPoint = maxCorner_;

    if( rhsMinPoint.x() < minPoint.x() )
        minPoint.x( rhsMinPoint.x() );

    if( rhsMinPoint.y() < minPoint.y() )
        minPoint.y( rhsMinPoint.y() );

    if( rhsMinPoint.z() < minPoint.z() )
        minPoint.z( rhsMinPoint.z() );

    if( rhsMaxPoint.x() > maxPoint.x() )
        maxPoint.x( rhsMaxPoint.x() );

    if( rhsMaxPoint.y() > maxPoint.y() )
        maxPoint.y( rhsMaxPoint.y() );

    if( rhsMaxPoint.z() > maxPoint.z() )
        maxPoint.z( rhsMaxPoint.z() );
}

void MexAlignedBox3d::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}



PER_DEFINE_PERSISTENT( MexAlignedBox3d );

void perWrite( PerOstream& str, const MexAlignedBox3d& t )
{
    str << t.minCorner_;
    str << t.maxCorner_;

    bool    temp = t.isEmpty_;
    PER_WRITE_RAW_OBJECT( str, temp );
    temp = t.spinSafe_;
    PER_WRITE_RAW_OBJECT( str, temp );
}

void perRead( PerIstream& str, MexAlignedBox3d& t )
{
    str >> t.minCorner_;
    str >> t.maxCorner_;

    bool flag;
    PER_READ_RAW_OBJECT( str, flag );
    t.isEmpty_ = flag;
    PER_READ_RAW_OBJECT( str, flag );
    t.spinSafe_ = flag;
}

MexPoint3d MexAlignedBox3d::centroid() const
{
    return MexPoint3d( (minCorner_.x() + maxCorner_.x()) * 0.5,
                       (minCorner_.y() + maxCorner_.y()) * 0.5,
                       (minCorner_.z() + maxCorner_.z()) * 0.5 );
}
