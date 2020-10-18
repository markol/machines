/*
 * T I L E B O U N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/vector.hpp"
#include "mathex/vec2.hpp"
#include "mathex/tri2d.hpp"
#include "mathex/plane3d.hpp"
#include "mathex/point3d.hpp"

#include "machphys/tileboun.hpp"

#ifndef _INLINE
    #include "machphys/tileboun.ipp"
#endif

//////////////////////////////////////////////////////////////////////////////////////////

MachPhysTileBoundary::MachPhysTileBoundary( uint nXVertices, uint nYVertices, uint nTriangles,
                         MATHEX_SCALAR xMin, MATHEX_SCALAR xMax,
                         MATHEX_SCALAR yMin, MATHEX_SCALAR yMax )
:   nXVertices_( nXVertices ),
    nYVertices_( nYVertices ),
    nTriangles_( nTriangles ),
    nextTriangleId_( 0 ),
    xMin_( xMin ),
    xMax_( xMax ),
    yMin_( yMin ),
    yMax_( yMax ),
    doneBuilding_( false ),
    cacheId_( (TriangleId)nTriangles )
{
    //Check the data falls in required range
    PRE( nXVertices > 1 and nYVertices > 1);
    PRE( nXVertices <= maxXYVertices and nYVertices <= maxXYVertices );
    PRE( nTriangles <= (nXVertices - 1) * (nYVertices - 1) * 2 );
    PRE( xMin < xMax and yMin < yMax );

    //Set the sizes for the triangle collection
    triangles_.reserve( nTriangles_ );

    //The vector of cell/triangle pair entries should grow in chunks of 32
    cellTriangles_.reserve( 32 );

    //Initialize all the heights to 0
    heights_.reserve( nXVertices * nYVertices );
    heights_.insert( heights_.begin(), nXVertices * nYVertices, 0 );

    //Set error value based on tile size
    epsilon_ = (xMax - xMin) * 0.001;

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachPhysTileBoundary::add( const MexPoint3d& v1, const MexPoint3d& v2,
                                const MexPoint3d& v3 )
{
    PRE( nextTriangleId_ < nTriangles_ );

    //Initialise vertex reference array
    const MexPoint3d* vertices[3] = {&v1, &v2, &v3};

    //Compute the grid coordinates for the x and y coordinates
    Coordinate x[3], y[3];
    for( size_t i = 0; i < 3; i++ )
    {
        x[i] = xCoordinate( vertices[i]->x() );
        y[i] = yCoordinate( vertices[i]->y() );
    }

    //Construct a triangle to store the defn and add to the collection
    TriangleData triangle;
    triangle.x0 = x[0];
    triangle.y0 = y[0];
    triangle.x1 = x[1];
    triangle.y1 = y[1];
    triangle.x2 = x[2];
    triangle.y2 = y[2];

    triangles_.push_back( triangle );

    //Visit each cell covered by the triangles bounding rectangle
    Coordinate cellXMin = std::min( x[0], x[1] );
    cellXMin = std::min( cellXMin, x[2] );
    Coordinate cellXMax = std::max( x[0], x[1] );
    cellXMax = std::max( cellXMax, x[2] );
    Coordinate cellYMin = std::min( y[0], y[1] );
    cellYMin = std::min( cellYMin, y[2] );
    Coordinate cellYMax = std::max( y[0], y[1] );
    cellYMax = std::max( cellYMax, y[2] );

    //Register every cell covered by the rectangle bounding the triangle
    for( Coordinate xCell = cellXMin; xCell < cellXMax; ++xCell)
    {
        //For each cell, check it does not already have 2 triangles associated
        //with it, and associate the new triangle id with it.
        for( Coordinate yCell = cellYMin; yCell < cellYMax; ++yCell)
        {
            cellTriangles_.push_back( CellPair( cellId( xCell, yCell ), nextTriangleId_ ) );
        }
    }

    //Store the z coordinates in the heights collection
    for( size_t i = 0; i < 3; i++ )
    {
        heights_[x[i] + y[i] * nXVertices_] = vertices[i]->z();
    }

    //Increment the next triangle id
    ++nextTriangleId_;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachPhysTileBoundary::doneBuilding( void )
{
    //Check the correct number of tiles have been registered
    PRE( nextTriangleId_ == nTriangles_ );

    //Check that all cells have at least 2 triangles associated with them
    size_t nPairs = cellTriangles_.size();
    for( Coordinate xCell = 0; xCell < nXVertices_ - 1; ++xCell )
    {
        for( Coordinate yCell = 0; yCell < nYVertices_ - 1; ++yCell )
        {
            uint count = 0;
            CellId id = cellId( xCell, yCell );
            for( size_t i = 0; i < nPairs; ++i )
            {
                if( cellTriangles_[i].first == id )
                {
                    if( ++count > 1 ) break;
                }
            }

            POST( count > 1 );
        }
    }

    //Set the flag
    doneBuilding_ = true;
}
//////////////////////////////////////////////////////////////////////////////////////////

MATHEX_SCALAR MachPhysTileBoundary::height( MATHEX_SCALAR x, MATHEX_SCALAR y ) const
{
    //We'll need to modify some mutable data members
    MachPhysTileBoundary* nonConstThis = (MachPhysTileBoundary*)this;

    //Check the coordinates are over the tile
    if( x < xMin_ ) x = xMin_;
    else if( x > xMax_) x = xMax_;
    if( y < yMin_ ) y = yMin_;
    else if( y > yMax_) y = yMax_;

    //Compute the cell number we are over
    Coordinate cellX = ( Coordinate )xCell( x );
    Coordinate cellY = ( Coordinate )yCell( y );
    CellId idCell = cellId( cellX, cellY );

    //Run down the vector of cell/triangle pairs checking each triangle possibly
    //over the cell
    size_t nPairs = cellTriangles_.size();
    bool foundTriangle = false;
    for( size_t i = 0; i < nPairs; ++i )
    {
        if( cellTriangles_[i].first == idCell )
        {
            //We have a match. See if we are over this triangle
            TriangleId idTriangle = cellTriangles_[i].second;
            nonConstThis->cacheLoad( idTriangle );
            foundTriangle = isOverCachedTriangle( x, y );
            if( foundTriangle ) break;
        }
    }

    //Check we found one
    POST( foundTriangle );

    //Now compute the height at these coordinates
    return heightOverCachedTriangle( x, y );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachPhysTileBoundary::cacheLoad( TriangleId id )
{
    PRE( id < nextTriangleId_ );

    //Check not already loaded
    if( id != cacheId_ )
    {
        //Get the triangle data
        TriangleData data = triangles_[id];
        x_[0] = xCoordinate( data.x0 );
        y_[0] = yCoordinate( data.y0 );
        z_[0] = zCoordinate( data.x0, data.y0 );

        x_[1] = xCoordinate( data.x1 );
        y_[1] = yCoordinate( data.y1 );
        z_[1] = zCoordinate( data.x1, data.y1 );

        x_[2] = xCoordinate( data.x2 );
        y_[2] = yCoordinate( data.y2 );
        z_[2] = zCoordinate( data.x2, data.y2 );

        cacheId_ = id;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MachPhysTileBoundary::isOverCachedTriangle( MATHEX_SCALAR x, MATHEX_SCALAR y ) const
{
    //Construct 3 2d vectors representing the triangle corners, and hence a 2d triangle
    MexVec2 a( x_[0], y_[0] );
    MexVec2 b( x_[1], y_[1] );
    MexVec2 c( x_[2], y_[2] );

    MexTriangle2d triangle2d( a, b, c );

    //Construct a 2d vector for the test coordinate
    MexVec2 testPoint( x, y );

    //See if the triangle contains the test point
    return triangle2d.contains( testPoint );
}
//////////////////////////////////////////////////////////////////////////////////////////

MATHEX_SCALAR MachPhysTileBoundary::heightOverCachedTriangle( MATHEX_SCALAR x, MATHEX_SCALAR y ) const
{
    //Construct 3 3d points from the triangle data, and hence the plane through them
    MexPoint3d p1( x_[0], y_[0], z_[0] );
    MexPoint3d p2( x_[1], y_[1], z_[1] );
    MexPoint3d p3( x_[2], y_[2], z_[2] );

    MexPlane3d plane( p1, p2, p3 );

    //Hence compute the z coordinate
    PRE( plane.c() != 0 );
    return - ( plane.a() * x + plane.b() * y + plane.d() ) / plane.c();
}
//////////////////////////////////////////////////////////////////////////////////////////

MachPhysTileBoundary::Coordinate MachPhysTileBoundary::xCoordinate( MATHEX_SCALAR x ) const
{
    //Check in range
    PRE( x > xMin_ - epsilon_  and  x < xMax_ + epsilon_ );

    //Compute grid coordinate and check in range
    int m = (int)( (MATHEX_SCALAR)(nXVertices_ - 1) * (x -xMin_ + epsilon_) / (xMax_ - xMin_) );
    POST( m >= 0  and  m < nXVertices_ );

    //Check close enough to grid line
    POST( abs( xCoordinate( (Coordinate)m ) - x ) < epsilon_ );

    return (Coordinate)m;
}
//////////////////////////////////////////////////////////////////////////////////////////

MachPhysTileBoundary::Coordinate MachPhysTileBoundary::yCoordinate( MATHEX_SCALAR y ) const
{
    //Check in range
    PRE( y > yMin_ - epsilon_  and  y < yMax_ + epsilon_ );

    //Compute grid coordinate and check in range
    int m = (int)( (MATHEX_SCALAR)(nYVertices_ - 1) * (y -yMin_ + epsilon_) / (yMax_ - yMin_) );
    POST( m >= 0  and  m < nXVertices_ );

    //Check close enough to grid line
    POST( abs( yCoordinate( (Coordinate)m ) - y ) < epsilon_ );

    return (Coordinate)m;
}
//////////////////////////////////////////////////////////////////////////////////////////

MATHEX_SCALAR MachPhysTileBoundary::xCoordinate( Coordinate x ) const
{
    //Check in range
    PRE( x < nXVertices_ );

    return xMin_ + (xMax_ - xMin_) * ((MATHEX_SCALAR)x / (MATHEX_SCALAR)(nXVertices_ - 1));
}
//////////////////////////////////////////////////////////////////////////////////////////

MATHEX_SCALAR MachPhysTileBoundary::yCoordinate( Coordinate y ) const
{
    //Check in range
    PRE( y < nYVertices_ );

    return yMin_ + (yMax_ - yMin_) * ((MATHEX_SCALAR)y / (MATHEX_SCALAR)(nYVertices_ - 1));
}
//////////////////////////////////////////////////////////////////////////////////////////

MATHEX_SCALAR MachPhysTileBoundary::zCoordinate( Coordinate x, Coordinate y ) const
{
    return heights_[x + y * nXVertices_];
}
//////////////////////////////////////////////////////////////////////////////////////////

uint MachPhysTileBoundary::xCell( MATHEX_SCALAR x ) const
{
    PRE( x >= xMin_  and  x <= xMax_ );
    MATHEX_SCALAR f = (x - xMin_) / (xMax_ - xMin_);
    uint cell = (uint)( f * (MATHEX_SCALAR)(nXVertices_ - 1) );
    if( cell >= (nXVertices_ - 1) )
        cell = nXVertices_ - 2;

    return cell;
}
//////////////////////////////////////////////////////////////////////////////////////////

uint MachPhysTileBoundary::yCell( MATHEX_SCALAR y ) const
{
    PRE( y >= yMin_  and  y <= yMax_ );
    MATHEX_SCALAR f = (y - yMin_) / (yMax_ - yMin_);
    uint cell = (uint)( f * (MATHEX_SCALAR)(nYVertices_ - 1) );
    if( cell >= (nYVertices_ - 1) )
        cell = nYVertices_ - 2;

    return cell;
}
//////////////////////////////////////////////////////////////////////////////////////////

MachPhysTileBoundary::~MachPhysTileBoundary()
{
    TEST_INVARIANT;

}
//////////////////////////////////////////////////////////////////////////////////////////

void MachPhysTileBoundary::CLASS_INVARIANT
{
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachPhysTileBoundary& t )
{

    o << "MachPhysTileBoundary " << (void*)&t << " start" << std::endl;
    o << "MachPhysTileBoundary " << (void*)&t << " end" << std::endl;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachPhysTileBoundary::profile
(
    MATHEX_SCALAR x1, MATHEX_SCALAR y1, MATHEX_SCALAR x2, MATHEX_SCALAR y2,
    ctl_vector< MexPoint3d >* pPoints
) const
{
    MATHEX_SCALAR eps = MexEpsilon::instance();
    PRE( (x1 >= xMin_ - eps)  and  (x1 <= xMax_ + eps) );
    PRE( (y1 >= yMin_ - eps)  and  (y1 <= yMax_ + eps) );
    PRE( (x2 >= xMin_ - eps)  and  (x2 <= xMax_ + eps) );
    PRE( (y2 >= yMin_ - eps)  and  (y2 <= yMax_ + eps) );

    //Compute the cell spacing
    MATHEX_SCALAR xSpacing = (xMax_ - xMin_) / (nXVertices_ - 1);
    MATHEX_SCALAR ySpacing = (yMax_ - yMin_) / (nYVertices_ - 1);

    //We will work by finding points along the line from (x1,y1) to (x2, y2)
    //at the points where the line crosses the tile grid lines. The height
    //of the terrain at each such point is computed, and hence we have a profile.
    //These points are derived in terms of their proportional distance along
    //the argument line.

    //Compute proportional distance to start at in x, and the increment per grid line
    MATHEX_SCALAR xDiv = x2 - x1;
    MATHEX_SCALAR xFirst, xProportionalIncrement;

    if( xDiv > eps )
        { xFirst = xMin_ + xSpacing; xProportionalIncrement = xSpacing / xDiv; }
    else if( xDiv < -eps )
        { xFirst = xMax_ - xSpacing; xProportionalIncrement = - xSpacing / xDiv; }
    else
        { xFirst = x2; xProportionalIncrement = 1.0; }

    MATHEX_SCALAR xNextProp = (xFirst - x1) / xDiv;

    //Compute proportional distance to start at in y, and the increment per grid line
    MATHEX_SCALAR yDiv = y2 - y1;
    MATHEX_SCALAR yFirst, yProportionalIncrement;

    if( yDiv > eps )
        { yFirst = yMin_ + ySpacing; yProportionalIncrement = ySpacing / yDiv; }
    else if( yDiv < -eps )
        { yFirst = yMax_ - ySpacing; yProportionalIncrement = - ySpacing / yDiv; }
    else
        { yFirst = y2; yProportionalIncrement = 1.0; }

    MATHEX_SCALAR yNextProp = (yFirst - y1) / yDiv;

    //Push the first endpoint
    pPoints->push_back( MexPoint3d( x1, y1, height( x1, y1 ) ) );

    //Now run along the line, pushing the points in order
    while( true )
    {
        MATHEX_SCALAR prop = 0;

        //Update to next proportional distance
        if( fabs( xNextProp - yNextProp) < eps ) yNextProp += yProportionalIncrement;

        if( xNextProp < yNextProp )
            { prop = xNextProp; xNextProp += xProportionalIncrement; }
        else
            { prop = yNextProp; yNextProp += yProportionalIncrement; }

        if( prop >= 1 - eps ) break; //Finished
        if( prop > 0 )
        {
            MATHEX_SCALAR x = x1 * (1 - prop) + x2 * prop;
            MATHEX_SCALAR y = y1 * (1 - prop) + y2 * prop;
            pPoints->push_back( MexPoint3d( x, y, height( x, y ) ) );
        }
    }

    //Push the second endpoint
    pPoints->push_back( MexPoint3d( x2, y2, height( x2, y2 ) ) );
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End TILEBOUN.CPP *************************************************/
