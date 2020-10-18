/*
 * T I L E B O U N . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysTileBoundary

    Represent the boundary of a Machines terrain tile in compact format.
    The terrain tile consists of a mesh with the following constraints.
    1) Mesh consists only of triangles
    2) Its x/y profile is orthogonally rectangular
    3) The vertices all lie in an evenly spaced x/y rectangular grid
    4) The maximum number of grid points in x and y is 16.
    5) All of the triangles are right-angle triangles
*/

#ifndef _TILEBOUN_HPP
#define _TILEBOUN_HPP

#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point3d.hpp"

//Forward declarations
class MexPoint3d;

//Orthodox canonical (revoked)
class MachPhysTileBoundary
{
public:
    enum {maxXYVertices = 16};

    //ctor. The grid consists of nXVertices by nYVertices points.
    //The total number of triangles in the mesh is nTriangles.
    //The coordinates of diagonally opposite corner points of the grid
    //are (xMin, yMin) and (xMax, yMax).
    MachPhysTileBoundary( uint nXVertices, uint nYVertices, uint nTriangles,
                         MATHEX_SCALAR xMin, MATHEX_SCALAR xMax,
                         MATHEX_SCALAR yMin, MATHEX_SCALAR yMax );
    //PRE( nXVertices > 1 and nYVertices > 1)
    //PRE( nXVertices <= maxXYVertices and nYVertices <= maxXYVertices)
    //PRE( nTriangles <= (nXVertices - 1) * (nYVertices - 1) * 2 )
    //PRE( xMin < xMax and yMin < yMax )

    //dtor
    ~MachPhysTileBoundary( void );

    //Add another triangle with vertices
    void add( const MexPoint3d& v1, const MexPoint3d& v2, const MexPoint3d& v3 );

    //Must be called after adding all vertices as an integrity check,
    //before trying to use the data.
    void doneBuilding( void );

    //z coordinate of polygon at coords (x, y) over the tile
    MATHEX_SCALAR height( MATHEX_SCALAR x, MATHEX_SCALAR y ) const;

    //Returns the profile of the terrain between (x1,y1) and (x2,y2) by pushing
    //a sequence of points into pPoints
    void profile( MATHEX_SCALAR x1, MATHEX_SCALAR y1, MATHEX_SCALAR x2, MATHEX_SCALAR y2,
                  ctl_vector< MexPoint3d >* pPoints ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysTileBoundary& t );

private:
    // Operations deliberately revoked
    MachPhysTileBoundary( const MachPhysTileBoundary& );
    MachPhysTileBoundary& operator =( const MachPhysTileBoundary& );
    bool operator ==( const MachPhysTileBoundary& );

    //Private data types used in implementation
    typedef unsigned short TriangleId;
    typedef unsigned short CellId;
    typedef signed char Coordinate;
    typedef ctl_vector<MATHEX_SCALAR> Heights;
    typedef std::pair<CellId, TriangleId> CellPair;

    struct TriangleData
    {
        Coordinate x0;
        Coordinate y0;
        Coordinate x1;
        Coordinate y1;
        Coordinate x2;
        Coordinate y2;
    };

    typedef ctl_vector<TriangleData> Triangles;
    typedef ctl_vector<CellPair> CellPairs;

    //return x grid coordinate for coordinate x
    Coordinate xCoordinate( MATHEX_SCALAR x ) const;

    //return y grid coordinate for coordinate y
    Coordinate yCoordinate( MATHEX_SCALAR y ) const;


    //X value for grid x coordinate x
    MATHEX_SCALAR xCoordinate( Coordinate x ) const;

    //y value for grid y coordinate y
    MATHEX_SCALAR yCoordinate( Coordinate y ) const;

    //z value for grid coordinates (x, y)
    MATHEX_SCALAR zCoordinate( Coordinate x, Coordinate y ) const;

    //The x grid cell coordinate x is over
    uint xCell( MATHEX_SCALAR x ) const;

    //The y grid cell coordinate y is over
    uint yCell( MATHEX_SCALAR y ) const;

    //Index for the cell at position (xCell, yCell)
    CellId cellId( Coordinate xCell, Coordinate yCell ) const;

    //load data into cache for the triangle with given id
    void cacheLoad( TriangleId id );

    //True iff (x, y) is over the cached triangle
    bool isOverCachedTriangle( MATHEX_SCALAR x, MATHEX_SCALAR y ) const;

    //z coordinate at (x, y) for the plane of the cached triangle
    MATHEX_SCALAR heightOverCachedTriangle( MATHEX_SCALAR x, MATHEX_SCALAR y ) const;

    //data members
    uint nXVertices_;
    uint nYVertices_;
    uint nTriangles_;
    MATHEX_SCALAR xMin_;
    MATHEX_SCALAR xMax_;
    MATHEX_SCALAR yMin_;
    MATHEX_SCALAR yMax_;
    MATHEX_SCALAR epsilon_;
    TriangleId nextTriangleId_;
    bool doneBuilding_;
    Heights heights_;
    Triangles triangles_;
    CellPairs cellTriangles_;

    //cached data for a single triangle
    TriangleId cacheId_;
    MATHEX_SCALAR x_[3];
    MATHEX_SCALAR y_[3];
    MATHEX_SCALAR z_[3];
};

#ifdef _INLINE
    #include "machphys/tileboun.ipp"
#endif


#endif

/* End TILEBOUN.HPP *************************************************/
