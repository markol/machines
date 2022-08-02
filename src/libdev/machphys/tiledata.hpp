/*
 * T I L E D A T A . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysTileData

    Represents the surface of a terrain tile in compact form.
    Provides efficient methods to get the height above the terrain at any (x,y) point,
    and detect any point of intersection with a line.
*/

#ifndef _MACHPHYS_TILEDATA_HPP
#define _MACHPHYS_TILEDATA_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "ctl/nbvector.hpp"
#include "ctl/vector.hpp"
#include "ctl/utility.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/line3d.hpp"
#include "mathex/grid2d.hpp"

//Forward refs
class RenMesh;
class ctl_bit_vector;
class MexPoint2d;

//orthodox canonical( revoked )
class MachPhysTileData
{
public:
    //ctor. The tile mesh is defined over a grid with extent boundary, and having
    //nXVertices and nYVertices grid lines in x and y respectively.
    //The mesh itself is mesh.
    MachPhysTileData( int nXVertices, int nYVertices, const MexAlignedBox2d& boundary,
                      const RenMesh& mesh );

    //z coordinate of polygon at coords (x, y) over the tile.
    //The version with pUnitNormal will compute and return a unit normal vector to the terrain
    //at the intersection into pUnitNormal, (unless it is NULL)
    MATHEX_SCALAR height( MATHEX_SCALAR x, MATHEX_SCALAR y ) const;
    MATHEX_SCALAR height( MATHEX_SCALAR x, MATHEX_SCALAR y, MexVec3* pUnitNormal ) const;

    //True if line intersects the tile boundary.
    //If so returns the distance along the line from its start point to
    //the point of intersection.
    bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance) const;

    //dtor.
    ~MachPhysTileData();

    typedef ctl_vector< MexTransform3d >    Profile;
    //  Get the profile of a path between the two given points.
    void pathProfile( const MexPoint3d& startPoint, const MexPoint3d& finishPoint, Profile* pProfile ) const;

    size_t nTriangles() const;
    void triangle( size_t index, MexPoint3d* pPoint1, MexPoint3d* pPoint2, MexPoint3d* pPoint3 ) const;
    // PRE( index < nTriangles() );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysTileData& t );

    PER_MEMBER_PERSISTENT( MachPhysTileData );
    PER_FRIEND_READ_WRITE( MachPhysTileData );

private:
    // Operations deliberately revoked
    MachPhysTileData( const MachPhysTileData& );
    MachPhysTileData& operator =( const MachPhysTileData& );
    bool operator ==( const MachPhysTileData& );

    //constants
    enum
    {
        NMAX_VERTICES = 11, //Maximum number of vertices in either direction
        NMAX_TRIANGLES = 255, //Maximum number of triangles in the mesh
    };

    //Private data types used in implementation

    //Represents compressed triangle
    class Triangle
    {
    public:
        Triangle( uint32 v1, uint32 v2, uint32 v3 )
        : j_( (v1 << 16) | (v2 << 8) | v3 ) {};

        Triangle()
        : j_( 0 ) {};

        size_t v1() const { return (size_t)( j_ >> 16 ); };
        size_t v2() const { return (size_t)( (j_ >> 8) & 255 ); };
        size_t v3() const { return (size_t)( j_ & 255 ); };

        PER_MEMBER_PERSISTENT_DEFAULT_INLINE( Triangle );
        PER_FRIEND_READ_WRITE( Triangle );
    private:
        uint32 j_; //Compressed vertex indices
    };

    PER_FRIEND_READ_WRITE( Triangle );
    typedef ushort TriangleId; //Index into triangles_

    //Working structure used temporarily while building the cell triangle lists
    struct WorkingData
    {
        ctl_nb_vector< int > xMinCell_; //Minimum x cell indexed on y for current triangle
        ctl_nb_vector< int > xMaxCell_; //Maximum x cell indexed on y for current triangle
        ctl_nb_vector< int > nCellsCovered_; //Number of cells covered per triangle,
                                             //indexed on triangle id.
        ctl_nb_vector< int > cellCoords_; //x then y coordinate for each cell for all
                                          //triangles starting with id 0, and working up.
    };

    size_t  nXCells() const;
    size_t  nYCells() const;

    MATHEX_SCALAR   xCellSize() const;
    MATHEX_SCALAR   yCellSize() const;

    //Call back used by the cellsUnderLine() method.
    typedef bool cellCallBackFn( MachPhysTileData& tileData, int xCell, int yCell );

    //Set up the contents of the cellTriangleLists_ and cellIndex_ vectors
    void makeCellLists();

    //Calls pFn for each cell under the line from p1 to p2.
    //Stops processing if pFn returns false.
    //Returns number of calls made.
    int cellsUnderLine( const MexPoint3d& p1, const MexPoint3d& p2, cellCallBackFn* pFn );

    //Computes the range of cells covered by a line running from (x0, y0) to (x1,y1)
    //across a grid of nXCells by nYCells cells of dimension xCellSize by yCellSize.
    //Doesn't include cells only penetrated by distance errorRange.
    //Returns indeces of first and last cell covered in pXStartCell, pXEndCell,
    //pYStartCell, pYEndCell.
    static void lineCellRange
    (
        int nXCells, MATHEX_SCALAR xCellSize, int nYCells, MATHEX_SCALAR yCellSize,
        MATHEX_SCALAR errorRange,
        MATHEX_SCALAR x0, MATHEX_SCALAR x1,MATHEX_SCALAR y0, MATHEX_SCALAR y1,
        int *pXStartCell, int *pXEndCell, int *pYStartCell, int *pYEndCell
    );

    //Used during construction of cell triangle list
    static bool setMinMaxCellData( MachPhysTileData& tileData, int xCell, int yCell );

    //FALSE if the specified cell intersects testLine_.
    //If it does intersect, sets foundIntersection_ true, and stores the distance in distance_.
    static bool lineIntersectsCell( MachPhysTileData& tileData, int xCell, int yCell );

    //  Work out which triangles intersect with the given cells and
    //  add them to the triangle vector
    void    cellsToTriangles( const MexGrid2d::Cells& cells, ctl_bit_vector* pTriangleVector ) const;

    //  Intersect a line with a triangle
    bool    intersectWithTriangle( const MexLine2d& line, const Triangle& triangle, MexPoint3d* pPoint1, MexPoint3d* pPoint2 ) const;
    // POST( implies( result, pPoint1 and pPoint2 will both be set (they might be identical) ) )

    bool    withinTriangle( const MexPoint2d&, const Triangle&, MexPoint3d* pPoint3d ) const;

    MexVec3 triangleNormal( const Triangle& triangle ) const;

    //  Intersect a line with an edge defined by two points. The intersection
    //  occurs in 2D (i.e. only using x and y coordinates), however the result
    //  is returned as 3D.
    bool    intersectWithEdge( const MexLine2d& line,
      const MexPoint3d& end1, const MexPoint3d& end2,
      MexPoint3d* pIntersectionPoint ) const;

    //  Store the data resulting from a triangle intersection
    CTL_TRIPLE( IntersectingTriangle,
       MexPoint3d, point1, MexPoint3d, point2,
       MexVec3, normal );

    typedef ctl_vector< IntersectingTriangle >  IntersectingTriangles;

    //  This is a friend purely to allow it to instantiate IntersectingTriangles
    friend void MachDummyFunction1();

    void sortTriangles( const MexPoint3d& startPoint, const MexPoint3d& finishPoint, IntersectingTriangles* ) const;

    static  bool    increasingX( const MexPoint3d& a, const MexPoint3d& b );
    static  bool    decreasingX( const MexPoint3d& a, const MexPoint3d& b );
    static  bool    increasingY( const MexPoint3d& a, const MexPoint3d& b );
    static  bool    decreasingY( const MexPoint3d& a, const MexPoint3d& b );

    //  Check that a profile is valid - i.e. no vertical climbs
    bool profileValid( const Profile& profile ) const;

    bool triangleVertical( const Triangle& triangle ) const;


    ///////////////////////////////////////////////////
    //Implementation functions for pathProfile()

    //Macro setting up class used to store path profile intersections
    CTL_QUAD( TriangleIntersectData,
              	MATHEX_SCALAR, entryDistance,
              	MexPoint3d, entryPoint,
              	MexPoint3d, exitPoint,
              	MexVec3, normal );

    typedef ctl_nb_vector< TriangleIntersectData > TriangleIntersectDatas;

    void profileOutput( TriangleIntersectDatas& intersectDatas, Profile* pProfile ) const;

    bool profileTriangleIntersect
    (
    	MATHEX_SCALAR xStart, MATHEX_SCALAR yStart,
    	MATHEX_SCALAR xUnitDir, MATHEX_SCALAR yUnitDir,	MATHEX_SCALAR lineLength,
    	TriangleId triangleId,
    	TriangleIntersectDatas* pIntersectDatas
    ) const;
    ///////////////////////////////////////////////////

    //data members
    int nXVertices_; // Number of x vertices in the grid
    int nYVertices_; // Number of y vertices in the grid
    MexAlignedBox2d boundary_; //Limits of grid in x and y
    ctl_nb_vector< MexPoint3d > vertices_; //The mesh vertex coordinates
    ctl_nb_vector< Triangle > triangles_; //The triangles, each stored as triple of vertex
                                       //indeces compacted
    ctl_nb_vector< TriangleId > cellTriangleLists_; //Lists of triangle ids - one for each cell in the
                                                 //grid.
    ctl_nb_vector< ushort > cellCounts_; //Cumulative count of triangles covered by each cell.
                                     //Index is (x + y*(nXVertices_-1)) for cell (x,y)

    //static data members
    static WorkingData* pWorkingData_; //Temporary object used while constructing cell triangle lists

    //cache results for callback methods
    static bool foundIntersection_;
    static MATHEX_SCALAR distance_;
    static MexLine3d testLine_;
    static ctl_nb_vector< TriangleId > trianglesDone_; //Stores ids of triangles already checked
                                                       //in functions covering more than 1 cell.

};

PER_DECLARE_PERSISTENT( MachPhysTileData );

#ifdef _INLINE
    #include "machphys/tiledata.ipp"
#endif


#endif

/* End TILEDATA.HPP *************************************************/
