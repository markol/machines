/*
 * T I L E D A T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include <stdio.h>

#include "ctl/bitvec.hpp"
#include "ctl/set.hpp"

#include "mathex/grid2d.hpp"
#include "mathex/vec2.hpp"
#include "mathex/line2d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/plane3d.hpp"
#include "mathex/intersec.hpp"
#include "mathex/utility.hpp"
#include "machphys/tiledata.hpp"

#include "render/mesh.hpp"

#ifndef _INLINE
    #include "machphys/tiledata.ipp"
#endif


//Global statics
MachPhysTileData::WorkingData* MachPhysTileData::pWorkingData_ = NULL;
bool MachPhysTileData::foundIntersection_;
MATHEX_SCALAR MachPhysTileData::distance_;
MexLine3d MachPhysTileData::testLine_;
ctl_nb_vector< MachPhysTileData::TriangleId > MachPhysTileData::trianglesDone_;

PER_DEFINE_PERSISTENT( MachPhysTileData );

MachPhysTileData::MachPhysTileData
(
    int nXVertices, int nYVertices, const MexAlignedBox2d& boundary, const RenMesh& mesh
)
:   nXVertices_( nXVertices ),
    nYVertices_( nYVertices ),
    boundary_( boundary )
{
    PRE( nXVertices <= NMAX_VERTICES );
    PRE( nYVertices <= NMAX_VERTICES );

    PATH_PROFILE_STREAM( "TileData " << (void*)this << " " << mesh.pathName() << " " << mesh.meshName() << std::endl );

    //This static collection used to cache lists of triangle ids
    trianglesDone_.reserve( 64 );

    //Get the mesh data using local collections
    size_t nVertices;
    uint nFaces;
    ctl_vector<MexPoint3d> vertices;
    ctl_vector<size_t> faceData;
    vertices.reserve( 81 );
    faceData.reserve( 512 );

    nFaces = mesh.faces( &nVertices, &vertices, &faceData );
    ASSERT( nVertices <= NMAX_TRIANGLES, "" );

    //Store the vertices locally
    vertices_.reserve( nVertices );
    for( size_t i = 0; i != nVertices; ++i )
        vertices_.push_back( vertices[i] );

    //Work through the face data identifying storing triangle
    triangles_.reserve( nFaces );
    size_t j = 0;
    while( nFaces-- )
    {
        //Check vertex count is 3
        uint vCount = faceData[j++];
        ASSERT( vCount == 3, "" );

        //Register the triangle
        ulong index1 = faceData[j++];
        ulong index2 = faceData[j++];
        ulong index3 = faceData[j++];
        ASSERT_INFO( index1 );
        ASSERT_INFO( index2 );
        ASSERT_INFO( index3 );
        ASSERT_INFO( vertices.size() );

        triangles_.push_back( Triangle( index1, index2, index3 ) );
    }

    //Now set up the cell triangle lists
    makeCellLists();

    TEST_INVARIANT;
}

MachPhysTileData::MachPhysTileData( PerConstructor )
{
}

MachPhysTileData::~MachPhysTileData()
{
    TEST_INVARIANT;

}

void MachPhysTileData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachPhysTileData::makeCellLists()
{
    //get useful data
    size_t nTriangles = triangles_.size();
    size_t nXCells = nXVertices_ - 1;
    size_t nYCells = nYVertices_ - 1;

    //Construct the temporary working structure
    pWorkingData_ = _NEW( WorkingData );
    ctl_nb_vector< int >& xMinCell = pWorkingData_->xMinCell_;
    ctl_nb_vector< int >& xMaxCell = pWorkingData_->xMaxCell_;
    ctl_nb_vector< int >& nCellsCovered = pWorkingData_->nCellsCovered_;
    ctl_nb_vector< int >& cellCoords = pWorkingData_->cellCoords_;

    xMinCell.reserve( nXCells );
    xMaxCell.reserve( nXCells );
    nCellsCovered.reserve( nTriangles );
    cellCoords.reserve( 512 );

    //Deal with each triangle
    for( size_t i = 0; i != nTriangles; ++i )
    {
        //Get its coordinates
        Triangle t = triangles_[i];
        const MexPoint3d& p1 = vertices_[ t.v1() ];
        const MexPoint3d& p2 = vertices_[ t.v2() ];
        const MexPoint3d& p3 = vertices_[ t.v3() ];

        //Initialise the min/max vectors with suitable values
        for( size_t j = 0; j != nYCells; ++j )
        {
            xMinCell.push_back( nXCells );
            xMaxCell.push_back( -1 );
        }

        //Get the cells under each line, and store the min/max cell visited
        //for each row in the working data structure
        cellsUnderLine( p1, p2, setMinMaxCellData ) ;
        cellsUnderLine( p2, p3, setMinMaxCellData ) ;
        cellsUnderLine( p3, p1, setMinMaxCellData ) ;

        //Now add every cell to the temporary list for this triangle
        int n = 0;
        for( int y = 0; y != nYCells; ++y )
        {
            int xStart = xMinCell[ y ];
            int xEnd = xMaxCell[ y ];

            for( int x = xStart; x <= xEnd; ++x )
            {
                n++;
                cellCoords.push_back( x );
                cellCoords.push_back( y );
            }
        }

        //Store the total cells covered for this triangle
        nCellsCovered.push_back( n );

        //Clear the min/max vectors
        xMinCell.erase( xMinCell.begin(), xMinCell.end() );
        xMaxCell.erase( xMaxCell.begin(), xMaxCell.end() );
    }

    //Now construct the actual list of triangles per cell.
    //This isn't terribly efficient, but even so it won't take long.
    ushort cumulativeCount = 0;
    cellCounts_.reserve( nXCells * nYCells );
    cellTriangleLists_.reserve( cellCoords.size() / 2 );

    for( int y = 0; y != nYCells; ++y )
    {
        for( int x = 0; x != nXCells; ++x )
        {
            int nTrianglesOverCell = 0;
            int j = 0;
            for( TriangleId id = 0; id != nTriangles; ++id )
            {
                int nCellsUnderThisTriangle = nCellsCovered[id];
                while( nCellsUnderThisTriangle-- )
                {
                    int tx = cellCoords[j++];
                    int ty = cellCoords[j++];
                    if( tx == x  and  ty == y )
                    {
                        nTrianglesOverCell++;
                        cellTriangleLists_.push_back( id );
                    }
                }
            }

            cumulativeCount += nTrianglesOverCell;
            cellCounts_.push_back( cumulativeCount );
        }
    }

    //Delete the working data structure
    _DELETE( pWorkingData_ );
    pWorkingData_ = NULL;
}

int MachPhysTileData::cellsUnderLine
(
    const MexPoint3d& p1, const MexPoint3d& p2, cellCallBackFn* pFn
)
{
    //Get the grid limits and cell sizes
    const MexPoint2d& minPoint = boundary_.minCorner();
    const MexPoint2d& maxPoint = boundary_.maxCorner();
    MATHEX_SCALAR xMin = minPoint.x();
    MATHEX_SCALAR yMin = minPoint.y();
    MATHEX_SCALAR xMax = maxPoint.x();
    MATHEX_SCALAR yMax = maxPoint.y();
    MATHEX_SCALAR xGridSize = xMax - xMin;
    MATHEX_SCALAR yGridSize = yMax - yMin;
    int xCellLimit = nXVertices_ - 1;
    int yCellLimit = nYVertices_ - 1;
    MATHEX_SCALAR xCellSize = xGridSize / xCellLimit;
    MATHEX_SCALAR yCellSize = yGridSize / yCellLimit;

    //Set up start and end coordinates, rationalized to an origin at grid top left
    MATHEX_SCALAR x0 = p1.x() - xMin;
    MATHEX_SCALAR y0 = p1.y() - yMin;
    MATHEX_SCALAR x1 = p2.x() - xMin;
    MATHEX_SCALAR y1 = p2.y() - yMin;
    MATHEX_SCALAR errorRange = 0.001;

    //Determine start and end cells. Modify for any rounding errors.
    int xCell, yCell, xEndCell, yEndCell, xStartCell, yStartCell;
    lineCellRange ( xCellLimit, xCellSize, yCellLimit, yCellSize, errorRange,
                    x0, x1,y0, y1, &xCell, &xEndCell, &yCell, &yEndCell );

    xStartCell = xCell;//These are for debugging purposes
    yStartCell = yCell;

    //Hence output the first cell, and get the total number remaining to output
    int nCells = 0;
    int count = abs( xEndCell - xCell) + abs( yEndCell - yCell );

    ++nCells;
    if( not (*pFn)( *this, xCell, yCell ) )
        count = 0;

    if( count != 0 )
    {
        //Set up gradient and reciprocal for line, using dummy large values for horizontal
        //or vertical, and using absolute gradient.
        MATHEX_SCALAR epsilon = MexEpsilon::instance();
        MATHEX_SCALAR ax = fabs( x1 - x0 );
        MATHEX_SCALAR ay = fabs( y1 - y0 );
        MATHEX_SCALAR f = (ay > epsilon ? ax / ay : 1.0e+25);
        MATHEX_SCALAR g = (ax > epsilon ? ay / ax : 1.0e+25);

        //Set up signed cell increment values
        int ix, iy, jx, jy;
        if( x0 <= x1 )
        {
            ix = 1;
            jx = 0;
        }
        else
        {
            ix = -1;
            jx = 1;
        }

        if( y0 <= y1 )
        {
            iy = 1;
            jy = 0;
        }
        else
        {
            iy = -1;
            jy = 1;
        }

        //Compute the next cells if we advance in x or y, and the corresponding
        //changes in y and x.
        int xNextCell = xCell + ix;
        int yNextCell = yCell + iy;

        MATHEX_SCALAR bx = fabs( (xNextCell + jx) * xCellSize - x0 );
        if( bx > ax )
            bx = ax;

        MATHEX_SCALAR by = fabs( (yNextCell + jy) * yCellSize - y0 );
        if( by > ay )
            by = ay;

        MATHEX_SCALAR cx = f * by;
        MATHEX_SCALAR cy = g * bx;

        //Output the other cells
        while( count-- )
        {
            //Make the shortest move
            if( xCell != xEndCell  and  (yCell == yEndCell  or  (bx + cy < by + cx)) )
            {
                //Move to the next x cell
                xCell = xNextCell;
                xNextCell += ix;
                bx = fabs( (xNextCell + jx) * xCellSize - x0 );
                if( bx > ax )
                    bx = ax;
                cy = g * bx;
            }
            else
            {
                //Move to the next y cell
                yCell = yNextCell;
                yNextCell += iy;
                by = by = fabs( (yNextCell + jy) * yCellSize - y0 );
                if( by > ay )
                    by = ay;
                cx = f * by;
            }

            //Output this cell
            ++nCells;
            if( not (*pFn)( *this, xCell, yCell ) )
                break;
        }
    }

    return nCells;
}

//static
void MachPhysTileData::lineCellRange
(
    int nXCells, MATHEX_SCALAR xCellSize, int nYCells, MATHEX_SCALAR yCellSize,
    MATHEX_SCALAR errorRange,
    MATHEX_SCALAR x0, MATHEX_SCALAR x1,MATHEX_SCALAR y0, MATHEX_SCALAR y1,
    int *pXStartCell, int *pXEndCell, int *pYStartCell, int *pYEndCell
)
{
    //Compute the deltas in x and y
    MATHEX_SCALAR xDiff = x1 - x0;
    MATHEX_SCALAR yDiff = y1 - y0;

    //Get signed version of error to use for rounding to get what we want
    MATHEX_SCALAR xErrorRange, yErrorRange;
    bool xDiffSmall = false;
    bool yDiffSmall = false;


    if( xDiff > errorRange )
        xErrorRange = errorRange;
    else if( xDiff < -errorRange )
        xErrorRange = -errorRange;
    else
    {
        xDiffSmall = true;
        xErrorRange = (yDiff > 0 ? -errorRange : errorRange);
    }

    if( yDiff > errorRange )
        yErrorRange = errorRange;
    else if( yDiff < -errorRange )
        yErrorRange = -errorRange;
    else
    {
        yDiffSmall = true;
        yErrorRange = (xDiff > 0 ? errorRange : -errorRange);
    }

    //Compute start and end cells, using error value to give appropriate cell coverage
    //Watch out for very small range, in which case ensure we get one cell only
    int xStart, xEnd, yStart, yEnd;

    xStart = (x0 + xErrorRange) / xCellSize;
    xEnd = (xDiffSmall ? xStart : (x1 - xErrorRange) / xCellSize);

    yStart = (y0 + yErrorRange) / yCellSize;
    yEnd = (yDiffSmall ? yStart : (y1 - yErrorRange) / yCellSize);

    //Ensure stay in range
    --nXCells;
    --nYCells;

    if( xStart < 0 )
        xStart = 0;
    else if( xStart > nXCells )
        xStart = nXCells;

    if( xEnd < 0 )
        xEnd = 0;
    else if( xEnd > nXCells )
        xEnd = nXCells;

    if( yStart < 0 )
        yStart = 0;
    else if( yStart > nYCells )
        yStart = nYCells;

    if( yEnd < 0 )
        yEnd = 0;
    else if( yEnd > nYCells )
        yEnd = nYCells;

    //return results
    *pXStartCell = xStart;
    *pXEndCell = xEnd;
    *pYStartCell = yStart;
    *pYEndCell = yEnd;
}

//static
bool MachPhysTileData::setMinMaxCellData( MachPhysTileData&, int xCell, int yCell )
{
    PRE( pWorkingData_ != NULL );

    ctl_nb_vector< int >& xMinCell = pWorkingData_->xMinCell_;
    ctl_nb_vector< int >& xMaxCell = pWorkingData_->xMaxCell_;

    if( xCell < xMinCell[ yCell ] )
        xMinCell[ yCell ] = xCell;

    if( xCell > xMaxCell[ yCell ] )
        xMaxCell[ yCell ] = xCell;

    return true;
}

bool MachPhysTileData::intersectsLine
(
    const MexLine3d& line, MATHEX_SCALAR* pDistance
) const
{
  //JON_STREAM("MachPhysTileData::intersectsLine entry " << line.end1() << " to " << line.end2() << std::endl;)
    bool result = false;

    //Cache the line data, and initialise the return data
    testLine_ = line;
    foundIntersection_ = false;

    //Check for intersection between the line and the triangles over each cell
    //MachPhysTileData* nonConstThis = _STATIC_CAST( MachPhysTileData*, this );
    MachPhysTileData* nonConstThis = _CONST_CAST( MachPhysTileData*, this );
    nonConstThis->cellsUnderLine( line.end1(), line.end2(), lineIntersectsCell );

    //Return distance if intersection found
    if( foundIntersection_ )
        *pDistance = distance_;

    //Clear the static check list of processed triangles
    trianglesDone_.erase( trianglesDone_.begin(), trianglesDone_.end() );

  //JON_STREAM("MachPhysTileData::intersectsLine exit " << foundIntersection_ << " " << *pDistance << std::endl;)
    return foundIntersection_;
}

//static
bool MachPhysTileData::lineIntersectsCell( MachPhysTileData& tileData, int xCell, int yCell )
{
  //JON_STREAM("MachPhysTileData::lineIntersectsCell entry (" << xCell << "," << yCell << ")" << (void*)(&tileData) << std::endl;)

    trianglesDone_.reserve( 64 );

    //Get the index of the first entry in the cell triangle list for
    //this cell, and the number of triangles.
    int countIndex = xCell + yCell * (tileData.nXVertices_ - 1);
    int index = (countIndex == 0 ? 0 : tileData.cellCounts_[ countIndex - 1 ] );
    int nTriangles = tileData.cellCounts_[ countIndex ] - index;
  //JON_STREAM("  index " << index << "  nTriangles " << nTriangles << std::endl;)

    //Store local ref to the vertices
    const ctl_nb_vector< MexPoint3d >& vertices = tileData.vertices_;

    //Now deal with each triangle overlapping the cell
    while( nTriangles-- )
    {
        //Get the triangle. See if already checked. If so, ignore it.
        TriangleId id = tileData.cellTriangleLists_[ index++ ];
        if( trianglesDone_.size() == 0  or
            find( trianglesDone_.begin(), trianglesDone_.end(), id ) == trianglesDone_.end() )
        {
            //Add the id check list.
            trianglesDone_.push_back( id );

            //Check this triangle
            Triangle t = tileData.triangles_[ id ];

            foundIntersection_ = testLine_.segmentIntersects( vertices[ t.v1() ], vertices[ t.v2() ],
              vertices[ t.v3() ], &distance_, MexLine3d::INTERSECT_FRONT_SIDE );

          //JON_STREAM("  Done triangle " << id << vertices[ t.v1() ] << vertices[ t.v2() ] << vertices[ t.v3() ] << std::endl;)
            if( foundIntersection_ )
                break;
        }
    }


  //JON_STREAM("MachPhysTileData::lineIntersectsCell exit " << foundIntersection_ << " " << distance_ << std::endl;)
    return not foundIntersection_;
}

ostream& operator <<( ostream& o, const MachPhysTileData& t )
{

    o << "MachPhysTileData " << (void*)&t << " start" << std::endl;
    o << "  Grid vertices " << t.nXVertices_ << " x " << t.nYVertices_ << std::endl;
    o << "  Boundary " << t.boundary_ << std::endl;
    o << "  Vertices" << std::endl;
    for( size_t i = 0; i != t.vertices_.size(); ++i )
        o << "  " << i << "  " << t.vertices_[i] << std::endl;

    o << "  Triangles" << std::endl;
    for( size_t i = 0; i != t.triangles_.size(); ++i )
        o << "  " << i << "  "  << t.triangles_[i].v1()
                       << " " << t.triangles_[i].v2()
                       << " " << t.triangles_[i].v3() << std::endl;

    o << "  Cell coverage" << std::endl;
    ushort cumulativeCount = 0;
    int j = 0, k = 0;
    for( int y = 0; y < t.nYVertices_ - 1; ++y )
        for( int x = 0; x < t.nXVertices_ - 1; ++x )
        {
            o << "  (" << x << "," << y << ")" ;
            ushort count = (ushort)(t.cellCounts_[j++] - cumulativeCount);
            cumulativeCount += count;
            while( count-- )
                o << " " << t.cellTriangleLists_[k++];

            o << std::endl;
        }
    o << "MachPhysTileData " << (void*)&t << " end" << std::endl;

    return o;
}

MATHEX_SCALAR MachPhysTileData::height( MATHEX_SCALAR x, MATHEX_SCALAR y ) const
{
    return height( x, y, NULL );
}

MATHEX_SCALAR MachPhysTileData::height( MATHEX_SCALAR x, MATHEX_SCALAR y, MexVec3* pNormal ) const
{
    PRE_DATA( MATHEX_SCALAR epsilon = MexEpsilon::instance(); )
    PRE( x > boundary_.minCorner().x() - epsilon )
    PRE( x < boundary_.maxCorner().x() + epsilon )
    PRE( y > boundary_.minCorner().y() - epsilon )
    PRE( y < boundary_.maxCorner().y() + epsilon )

    //Check inside the tile, adjusting for any rounding errors
    const MexPoint2d& minCorner = boundary_.minCorner();
    const MexPoint2d& maxCorner = boundary_.maxCorner();
    MATHEX_SCALAR xMin = minCorner.x();
    MATHEX_SCALAR xMax = maxCorner.x();
    MATHEX_SCALAR yMin = minCorner.y();
    MATHEX_SCALAR yMax = maxCorner.y();

    if( x < xMin )
        x = xMin;
    else if( x > xMax )
        x = xMax;

    if( y < yMin )
        y = yMin;
    else if( y > yMax )
        y = yMax;

    //Compute which cell the location is over
    int nXCells = nXVertices_ - 1;
    int nYCells = nYVertices_ - 1;
    int xCell = (int)((x - xMin) / ((xMax - xMin) / nXCells));
    int yCell = (int)((y - yMin) / ((yMax - yMin) / nYCells));
    if( xCell >= nXCells)
        xCell = nXCells - 1;
    if( yCell >= nYCells)
        yCell = nYCells - 1;

    //Construct a vertical line passing through the test point
    MexLine3d testLine( MexPoint3d( x, y, -100.0), MexPoint3d( x, y, 100.0) );

    //Intersect with any triangle over the cell
    int countIndex = xCell + yCell * nXCells;
    int index = (countIndex == 0 ? 0 : cellCounts_[ countIndex - 1 ] );
    int nTriangles = cellCounts_[ countIndex ] - index;

    MATHEX_SCALAR distance;
    bool hit = false;
    while( not hit  and  nTriangles-- )
    {
        //Get the triangle data
        TriangleId id = cellTriangleLists_[ index++ ];
        Triangle t = triangles_[ id ];

        //Check line intersects triangle
        const MexPoint3d& v1 = vertices_[ t.v1() ];
        const MexPoint3d& v2 = vertices_[ t.v2() ];
        const MexPoint3d& v3 = vertices_[ t.v3() ];
        hit = testLine.intersects( v1, v2, v3, &distance );

        //If required, compute a unit normal vector to return
        if( hit and pNormal )
        {
            const MexVec3 v1v2( v1, v2 );
            const MexVec3 v2v3( v2, v3 );
            MexVec3 n = MexVec3::crossProduct( v1v2, v2v3 );
            n.makeUnitVector();

            *pNormal = n;
        }
    }

    MATHEX_SCALAR answer = (hit ? distance - 100.0 : 0.0);
    return answer;
}

/*
void MachPhysTileData::pathProfile( const MexPoint3d& startPoint, const MexPoint3d& finishPoint, Profile* pProfile ) const
{
    PRE_INFO( startPoint );
    PRE_INFO( finishPoint );

    PATH_PROFILE_STREAM( std::endl << "......................................................" << std::endl );
    PATH_PROFILE_STREAM( "Enter MachPhysTileData::pathProfile Start find path profile from " << startPoint << " to " << finishPoint << std::endl );
    PATH_PROFILE_INDENT( 2 );

    PRE_DATA( MATHEX_SCALAR epsilon = MexEpsilon::instance(); )
    PRE( startPoint.x() > boundary_.minCorner().x() - epsilon )
    PRE( startPoint.x() < boundary_.maxCorner().x() + epsilon )
    PRE( startPoint.y() > boundary_.minCorner().y() - epsilon )
    PRE( startPoint.y() < boundary_.maxCorner().y() + epsilon )

    PRE( finishPoint.x() > boundary_.minCorner().x() - epsilon )
    PRE( finishPoint.x() < boundary_.maxCorner().x() + epsilon )
    PRE( finishPoint.y() > boundary_.minCorner().y() - epsilon )
    PRE( finishPoint.y() < boundary_.maxCorner().y() + epsilon )

    PRE( startPoint != finishPoint );
    PRE( MexPoint2d( startPoint ) != MexPoint2d( finishPoint ) );


    //Check inside the tile, adjusting for any rounding errors
    const MexPoint2d& minCorner = boundary_.minCorner();
    const MexPoint2d& maxCorner = boundary_.maxCorner();
    MATHEX_SCALAR xMin = minCorner.x();
    MATHEX_SCALAR xMax = maxCorner.x();
    MATHEX_SCALAR yMin = minCorner.y();
    MATHEX_SCALAR yMax = maxCorner.y();

    MATHEX_SCALAR startX = startPoint.x();
    MATHEX_SCALAR startY = startPoint.y();
    MATHEX_SCALAR finishX = finishPoint.x();
    MATHEX_SCALAR finishY = finishPoint.y();

    startX = mexClamp( startX, xMin, xMax );
    startY = mexClamp( startY, yMin, yMax );
    finishX = mexClamp( finishX, xMin, xMax );
    finishY = mexClamp( finishY, yMin, yMax );


    MexPoint2d  origin( xMin, yMin );
    MexGrid2d   grid( nXCells(), nYCells(), xCellSize(), yCellSize(), origin );

    MexGrid2d::Cells    cells;
    MexGrid2d::Points   points;

    uint nToReserve = nXCells() + nYCells() + 4;
    cells.reserve( nToReserve );
    points.reserve( nToReserve );

    MexLine2d   line( MexPoint2d( startX, startY ), MexPoint2d( finishX, finishY ) );

    grid.intersect( line, &cells, &points );


    PATH_PROFILE_INSPECT( line );
    PATH_PROFILE_STREAM( "Cells:\n" );

    for( MexGrid2d::Cells::const_iterator i = cells.begin(); i != cells.end(); ++i )
    {
        PATH_PROFILE_STREAM( (*i).xIndex() << "  " << (*i).yIndex() << std::endl );
    }

    PATH_PROFILE_STREAM( "Points:\n" << points );

    //  Find out which triangles are candidates for intersection

    ctl_bit_vector  triangleVector( triangles_.size() );

    MexGrid2d::Points::const_iterator point = points.begin();

    cellsToTriangles( cells, &triangleVector );

    //  Now do the intersection with triangles

    IntersectingTriangles   intersectingTriangles;
    intersectingTriangles.reserve( triangleVector.size() );

    for( size_t i = 0; i < triangleVector.size(); ++i )
    {
        if( triangleVector[ i ] )
        {
            MexPoint3d  point1;
            MexPoint3d  point2;

            if( intersectWithTriangle( line, triangles_[ i ], &point1, &point2 ) )
            {
                PATH_PROFILE_WHERE;
                PATH_PROFILE_INSPECT( point1 );
                PATH_PROFILE_INSPECT( point2 );
                intersectingTriangles.push_back( IntersectingTriangle( point1, point2, triangleNormal( triangles_[ i ] ) ) );
            }
        }
    }

    PATH_PROFILE_INSPECT( intersectingTriangles.size() );

    if( intersectingTriangles.size() != 0 )
    {
        PATH_PROFILE_WHERE;

        sortTriangles( startPoint, finishPoint, &intersectingTriangles );

        PATH_PROFILE_WHERE;
        //  Push the first point at which we cross each triangle onto the vector.

        for( IntersectingTriangles::const_iterator i = intersectingTriangles.begin(); i != intersectingTriangles.end(); ++i )
        {
            PATH_PROFILE_WHERE;
            PATH_PROFILE_INSPECT( (*i).point1() );
            PATH_PROFILE_INSPECT( (*i).point2() );

            if( (*i).point1() != (*i).point2() )
            {
                PATH_PROFILE_WHERE;

                MexVec3 xBasis ( (*i).point1(), (*i).point2() );
                pProfile->push_back( MexTransform3d( MexTransform3d::X_XZ, xBasis, (*i).normal(), (*i).point1() ) );
            }

            PATH_PROFILE_WHERE;
        }

        PATH_PROFILE_WHERE;

        //  The last point is a special case - push this on separately
        pProfile->push_back( pProfile->back() );
        pProfile->back().position( intersectingTriangles.back().point2() );
    }

    PATH_PROFILE_WHERE;
    PATH_PROFILE_STREAM( "Profile for tile\n" << *pProfile );

    POST( profileValid( *pProfile ) );

    PATH_PROFILE_INDENT( -2 );
    PATH_PROFILE_STREAM( "Exit  MachPhysTileData::pathProfile Start find path profile from " << startPoint << " to " << finishPoint << std::endl );
    PATH_PROFILE_STREAM( "......................................................" << std::endl );
}
*/

MexVec3 MachPhysTileData::triangleNormal( const Triangle& triangle ) const
{
    const MexPoint3d& vertex1 = vertices_[ triangle.v1() ];
    const MexPoint3d& vertex2 = vertices_[ triangle.v2() ];
    const MexPoint3d& vertex3 = vertices_[ triangle.v3() ];

    MexVec3 normal = MexVec3::crossProduct(
      MexVec3( vertex1, vertex2 ), MexVec3( vertex2, vertex3 ) );

    return normal;
}

bool MachPhysTileData::profileValid( const Profile& profile ) const
{
    bool    result = true;

PATH_PROFILE_STREAM( "TileData " << (void*)this << std::endl );

    //  Check that if two points have the same X and Y coordinates
    //  that they also have the same Z coordinate - i.e. no vertical moves

    for( Profile::const_iterator i = profile.begin(); i != profile.end() and result; ++i )
    {
        Profile::const_iterator j = i;
        ++j;

        if( j != profile.end() )
        {
            MexPoint3d  point1 = (*i).position();
            MexPoint3d  point2 = (*j).position();

            MATHEX_SCALAR xyDistance = MexPoint2d( point1 ).euclidianDistance( point2 );

            ASSERT_INFO( point1 );
            ASSERT_INFO( point2 );

            PATH_PROFILE_INSPECT( point1 );
            PATH_PROFILE_INSPECT( point2 );

            if( xyDistance < (MATHEX_SCALAR)MexEpsilon::instance() )
            {
                if( fabs( point1.z() - point2.z() ) > (MATHEX_SCALAR)MexEpsilon::instance() )
                {
                    result = false;
                    ASSERT_INFO( "Vertical move" );
                    PATH_PROFILE_STREAM( "Vertical move" << std::endl );
                }
            }
        }
    }

    return result;
}

void    MachPhysTileData::cellsToTriangles( const MexGrid2d::Cells& cells, ctl_bit_vector* pTriangleVector ) const
{
    for( MexGrid2d::Cells::const_iterator i = cells.begin(); i != cells.end(); ++i )
    {
        size_t  cellIndex = (*i).xIndex() + (*i).yIndex() * nXCells();

        size_t triangleIndex = ( cellIndex == 0 ? 0 : cellCounts_[ cellIndex - 1 ] );
        size_t nTriangles = cellCounts_[ cellIndex ] - triangleIndex;

        for( size_t i = 0; i < nTriangles; ++i )
        {
            //Get the triangle data
            TriangleId id = cellTriangleLists_[ triangleIndex++ ];
            (*pTriangleVector)[ id ] = true;
        }
    }
}

bool    MachPhysTileData::triangleVertical( const Triangle& triangle ) const
{
    MexPoint2d v1 = vertices_[ triangle.v1() ];
    MexPoint2d v2 = vertices_[ triangle.v2() ];
    MexPoint2d v3 = vertices_[ triangle.v3() ];

    return MexUtility::inStraightLine( v1, v2, v3 );
}

bool    MachPhysTileData::intersectWithTriangle( const MexLine2d& line, const Triangle& triangle,
  MexPoint3d* pPoint1, MexPoint3d* pPoint2 ) const
{
    PRE_INFO( vertices_[ triangle.v1() ] );
    PRE_INFO( vertices_[ triangle.v2() ] );
    PRE_INFO( vertices_[ triangle.v3() ] );

    PATH_PROFILE_STREAM( "Triangle : " <<
      vertices_[ triangle.v1() ] << " " <<
      vertices_[ triangle.v2() ] << " " <<
      vertices_[ triangle.v3() ] << std::endl );

    PATH_PROFILE_INSPECT( line );

    MexPoint3d  intersectionPoint;
    bool        result = false;


    MexPoint2d  startPoint( line.end1() );
    MexVec2     direction( line.end1(), line.end2() );
    direction.makeUnitVector();

    const MexPoint3d& v1 = vertices_[ triangle.v1() ];
    const MexPoint3d& v2 = vertices_[ triangle.v2() ];
    const MexPoint3d& v3 = vertices_[ triangle.v3() ];

    MATHEX_SCALAR   distance1;
    MATHEX_SCALAR   distance2;

    //  Test for the infinte line intersecting with the triangle
    uint nIntersections = MexIntersections::lineVsTriangle2d(
        startPoint, direction, v1, v2, v3, &distance1, &distance2 );

    //  See whether the line segment actually intersects
    //  with the triangle

    bool    intersects = false;

    MATHEX_SCALAR   length = line.length();

    if( nIntersections == 2 )
    {
        intersects = true;

        //  See if the line segment is entirely to one side
        //  of the triangle
        if( distance1 <= 0.0 and distance2 <= 0.0 )
            intersects = false;

        if( distance1 >= length and distance2 >= length )
            intersects = false;
    }

    if( intersects )
    {
        result = true;

        PATH_PROFILE_STREAM( "Distance 1 " << distance1 << "  distance 2 " << distance2 << std::endl );

        if( distance1 >= 0.0 and distance1 <= length )
            *pPoint1 = line.pointAtDistance( distance1 );
        else
            *pPoint1 = line.end1();

        if( distance2 >= 0.0 and distance2 <= length )
            *pPoint2 = line.pointAtDistance( distance2 );
        else
            *pPoint2 = line.end2();

        MexPlane3d  trianglePlane( v1, v2, v3 );

        //  Test for a vertical triangle
        if( trianglePlane.c() == 0 )
        {
            result = false;
        }
        else
        {
            pPoint1->z( trianglePlane.z( pPoint1->x(), pPoint1->y() ) );
            pPoint2->z( trianglePlane.z( pPoint2->x(), pPoint2->y() ) );
        }
    }

    if( *pPoint1 == *pPoint2 )
        result = false;

    if( result )
    {
        PATH_PROFILE_STREAM( "Triangle intersects line at " << *pPoint1 << " and " << *pPoint2 << std::endl );
    }
    else
    {
        PATH_PROFILE_STREAM( "Triangle does not intersect line" << std::endl );
    }

    POST( implies( result, not triangleVertical( triangle ) ) );

    return result;
}

bool    MachPhysTileData::withinTriangle( const MexPoint2d& point,
  const Triangle& triangle, MexPoint3d* pIntersectionPoint ) const
{
    const MexPoint3d& vertex1 = vertices_[ triangle.v1() ];
    const MexPoint3d& vertex2 = vertices_[ triangle.v2() ];
    const MexPoint3d& vertex3 = vertices_[ triangle.v3() ];

    PATH_PROFILE_STREAM( "Check for point " << point << " in triangle:" << std::endl );
    PATH_PROFILE_STREAM( vertex1 << std::endl << vertex2 << std::endl << vertex3 << std::endl );

    MATHEX_SCALAR maxZ = std::max( vertex1.z(), std::max( vertex2.z(), vertex3.z() ) ) + 100.0;
    MATHEX_SCALAR minZ = std::min( vertex1.z(), std::min( vertex2.z(), vertex3.z() ) ) - 100.0;

    MexLine3d testLine( MexPoint3d( point.x(), point.y(), minZ ), MexPoint3d( point.x(), point.y(), maxZ ) );

    PATH_PROFILE_INSPECT( testLine.end1() );
    PATH_PROFILE_INSPECT( testLine.end2() );

    MATHEX_SCALAR   distance;
    bool within = testLine.intersects( vertices_[ triangle.v1() ], vertices_[ triangle.v2() ], vertices_[ triangle.v3() ],
                                   &distance );

    PATH_PROFILE_INSPECT( within );

    if( within )
    {
        pIntersectionPoint->x( point.x() );
        pIntersectionPoint->y( point.y() );
        pIntersectionPoint->z( minZ + distance );
    }

    return within;
}

bool    MachPhysTileData::intersectWithEdge( const MexLine2d& line,
  const MexPoint3d& end1, const MexPoint3d& end2,
  MexPoint3d* pIntersectionPoint ) const
{
    bool    result = false;

    MexLine2d   edge( end1, end2 );

    MexPoint2d  intersectionPoint2d;

    if( line.segmentIntersects( edge, &intersectionPoint2d ) )
    {
        *pIntersectionPoint = intersectionPoint2d;

        if( end1.x() == end2.x() )
            pIntersectionPoint->z( mexInterpolate( intersectionPoint2d.y(), end1.y(), end2.y(), end1.z(), end2.z() ) );
        else
            pIntersectionPoint->z( mexInterpolate( intersectionPoint2d.x(), end1.x(), end2.x(), end1.z(), end2.z() ) );

        result = true;

PATH_PROFILE_STREAM( "Triangle intersection at " << *pIntersectionPoint << std::endl );
    }

    return result;
}

void    MachPhysTileData::sortTriangles(
  const MexPoint3d& startPoint,
  const MexPoint3d& finishPoint,
  IntersectingTriangles* pIntersectingTriangles ) const
{
    PATH_PROFILE_STREAM( "Enter sort triangles " << (void*)this << std::endl );
    PATH_PROFILE_INDENT( 2 );

    typedef    bool (*PointSortFunction)( const MexPoint3d&, const MexPoint3d& );

    PointSortFunction pSortFn;

    PATH_PROFILE_INSPECT( startPoint );
    PATH_PROFILE_INSPECT( finishPoint );

    //  Find out which sort function we need to use - use the
    //  axis with the greatest difference to avoid rounding errors

    if( fabs( startPoint.x() - finishPoint.x() ) >
      fabs( startPoint.y() - finishPoint.y() ) )
    {
        if( startPoint.x() < finishPoint.x() )
            pSortFn = increasingX;
        else
            pSortFn = decreasingX;
    }
    else
    {
        if( startPoint.y() < finishPoint.y() )
            pSortFn = increasingY;
        else
            pSortFn = decreasingY;
    }

    //  First make sure that the points within each triangle are in order
    for( IntersectingTriangles::iterator i = pIntersectingTriangles->begin(); i != pIntersectingTriangles->end(); ++i )
    {
        PATH_PROFILE_INSPECT( (*i).point1() );
        PATH_PROFILE_INSPECT( (*i).point2() );

        if( not (pSortFn)( (*i).point1(), (*i).point2() ) )
        {
            MexPoint3d temp = (*i).point1();
            (*i).point1( (*i).point2() );
            (*i).point2( temp );
        }

        PATH_PROFILE_INSPECT( (*i).point1() );
        PATH_PROFILE_INSPECT( (*i).point2() );
    }

    class Sorter
    {
    public:
        Sorter( PointSortFunction pSort ) : pSort_( pSort ) {}
        bool operator ()( const IntersectingTriangle& a, const IntersectingTriangle& b )
        {
            return (*pSort_)( a.point1(), b.point1() );
        }
    private:
        PointSortFunction pSort_;
    };

    sort( pIntersectingTriangles->begin(), pIntersectingTriangles->end(), Sorter( pSortFn ) );

    PATH_PROFILE_INDENT( -2 );
    PATH_PROFILE_STREAM( "Exit  sort triangles " << (void*)this << std::endl );
}

// static
bool    MachPhysTileData::increasingX( const MexPoint3d& a, const MexPoint3d& b )
{
PATH_PROFILE_WHERE;
    return a.x() < b.x();
}

// static
bool    MachPhysTileData::decreasingX( const MexPoint3d& a, const MexPoint3d& b )
{
PATH_PROFILE_WHERE;
    return a.x() > b.x();
}

// static
bool    MachPhysTileData::increasingY( const MexPoint3d& a, const MexPoint3d& b )
{
PATH_PROFILE_WHERE;
    return a.y() < b.y();
}

// static
bool    MachPhysTileData::decreasingY( const MexPoint3d& a, const MexPoint3d& b )
{
PATH_PROFILE_WHERE;
    return a.y() > b.y();
}

void perWrite( PerOstream& ostr, const MachPhysTileData& tileData )
{
    ostr << tileData.nXVertices_;
    ostr << tileData.nYVertices_;
    ostr << tileData.boundary_;
    ostr << tileData.vertices_;
    ostr << tileData.triangles_;
    ostr << tileData.cellTriangleLists_;
    ostr << tileData.cellCounts_;
}

void perRead( PerIstream& istr, MachPhysTileData& tileData )
{
    istr >> tileData.nXVertices_;
    istr >> tileData.nYVertices_;
    istr >> tileData.boundary_;
    istr >> tileData.vertices_;
    istr >> tileData.triangles_;
    istr >> tileData.cellTriangleLists_;
    istr >> tileData.cellCounts_;
}

void perWrite( PerOstream& ostr, const MachPhysTileData::Triangle& triangle )
{
    ostr << triangle.j_;
}

void perRead( PerIstream& istr, MachPhysTileData::Triangle& triangle )
{
    istr >> triangle.j_;
}

size_t  MachPhysTileData::nXCells() const
{
    return nXVertices_ - 1;
}

size_t  MachPhysTileData::nYCells() const
{
    return nYVertices_ - 1;
}

MATHEX_SCALAR   MachPhysTileData::xCellSize() const
{
    return ( boundary_.maxCorner().x() - boundary_.minCorner().x() ) / nXCells();
}

MATHEX_SCALAR   MachPhysTileData::yCellSize() const
{
    return ( boundary_.maxCorner().y() - boundary_.minCorner().y() ) / nYCells();
}

size_t MachPhysTileData::nTriangles() const
{
    return triangles_.size();
}

void MachPhysTileData::triangle( size_t index, MexPoint3d* pPoint1, MexPoint3d* pPoint2, MexPoint3d* pPoint3 ) const
{
    PRE( index < nTriangles() );

    const Triangle& t = triangles_[ index ];

    *pPoint1 = vertices_[ t.v1() ];
    *pPoint2 = vertices_[ t.v2() ];
    *pPoint3 = vertices_[ t.v3() ];
}


void MachPhysTileData::pathProfile( const MexPoint3d& startPoint, const MexPoint3d& finishPoint, Profile* pProfile ) const
{
    PATH_PROFILE_STREAM( std::endl << "......................................................" << std::endl );
    PATH_PROFILE_STREAM( "Enter MachPhysTileData::pathProfile Start find path profile from " << startPoint << " to " << finishPoint << std::endl );
    PATH_PROFILE_INDENT( 2 );

    PRE_INFO( startPoint );
    PRE_INFO( finishPoint );
	PRE( xCellSize() > 0.0 );
	PRE( yCellSize() > 0.0 );
	PRE( startPoint != finishPoint );
    PRE_DATA( MATHEX_SCALAR epsilon = MexEpsilon::instance(); )
    PRE( startPoint.x() > boundary_.minCorner().x() - epsilon )
    PRE( startPoint.x() < boundary_.maxCorner().x() + epsilon )
    PRE( startPoint.y() > boundary_.minCorner().y() - epsilon )
    PRE( startPoint.y() < boundary_.maxCorner().y() + epsilon )

    PRE( finishPoint.x() > boundary_.minCorner().x() - epsilon )
    PRE( finishPoint.x() < boundary_.maxCorner().x() + epsilon )
    PRE( finishPoint.y() > boundary_.minCorner().y() - epsilon )
    PRE( finishPoint.y() < boundary_.maxCorner().y() + epsilon )

    PRE( startPoint != finishPoint );
    PRE( MexPoint2d( startPoint ) != MexPoint2d( finishPoint ) );

    //Check inside the tile, adjusting for any rounding errors
    const MexPoint2d& minCorner = boundary_.minCorner();
    const MexPoint2d& maxCorner = boundary_.maxCorner();
    MATHEX_SCALAR xMin = minCorner.x();
    MATHEX_SCALAR xMax = maxCorner.x();
    MATHEX_SCALAR yMin = minCorner.y();
    MATHEX_SCALAR yMax = maxCorner.y();

    MATHEX_SCALAR startX = startPoint.x();
    MATHEX_SCALAR startY = startPoint.y();
    MATHEX_SCALAR finishX = finishPoint.x();
    MATHEX_SCALAR finishY = finishPoint.y();

    startX = mexClamp( startX, xMin, xMax );
    startY = mexClamp( startY, yMin, yMax );
    finishX = mexClamp( finishX, xMin, xMax );
    finishY = mexClamp( finishY, yMin, yMax );

	//Store tile layout data locally
	MATHEX_SCALAR xCellLength = xCellSize();
	MATHEX_SCALAR yCellLength = yCellSize();
	uint nXCell = nXCells();
	uint nYCell = nYCells();
	uint profileSizeOnEntry = pProfile->size();

	PATH_PROFILE_INSPECT( xCellLength );
	PATH_PROFILE_INSPECT( yCellLength );
	PATH_PROFILE_INSPECT( nXCell );
	PATH_PROFILE_INSPECT( nYCell );

	//Set the accuracy locally. 1cm is neither here nor there for our purposes
	const MATHEX_SCALAR useEps = 0.01;
	const MATHEX_SCALAR negUseEps = -useEps;

	//Get the 2d line length and x/y components
	MATHEX_SCALAR xLineLength = finishX - startX;
	MATHEX_SCALAR yLineLength = finishY - startY;
	MATHEX_SCALAR lineLength = sqrt( xLineLength * xLineLength + yLineLength * yLineLength  );

	PATH_PROFILE_INSPECT( xLineLength );
	PATH_PROFILE_INSPECT( yLineLength );
	PATH_PROFILE_INSPECT( lineLength );

	//Check for a very short line, and treat it specially below
	if( lineLength > 2.1 * useEps )
	{
		//We'll need a unit vector in the direction of our 2d line
		MATHEX_SCALAR xUnitDir = xLineLength / lineLength;
		MATHEX_SCALAR yUnitDir = yLineLength / lineLength;

		PATH_PROFILE_INSPECT( xUnitDir );
		PATH_PROFILE_INSPECT( yUnitDir );

		//Compute the starting cell, but offset a short distance along the line, so that
		//we don't pick up an unnecessary cell if start point lies on a grid line.
		int xCurrentCell( ( startX + (xUnitDir * useEps) - xMin) / xCellLength );
		int yCurrentCell( ( startY + (yUnitDir * useEps) - yMin) / yCellLength );

        //Correct for lines on the right or bottom edge of the tile
        if( xCurrentCell >= nXCell )
            xCurrentCell = nXCell - 1;

        if( yCurrentCell >= nYCell )
            yCurrentCell = nYCell - 1;

		PATH_PROFILE_INSPECT( xCurrentCell );
		PATH_PROFILE_INSPECT( yCurrentCell );

		//Set up x and y grid increments etc
		MATHEX_SCALAR xDistanceIncrement;
		MATHEX_SCALAR xNextDistance;
		int xCellIncrement;

		if( xLineLength > useEps )
		{
			MATHEX_SCALAR xGradient = lineLength / xLineLength;
			xDistanceIncrement = xCellLength * xGradient;
			xNextDistance = (xMin + (xCurrentCell + 1) * xCellLength - startX) * xGradient;
			xCellIncrement = 1;
		}
		else if( xLineLength < negUseEps )
		{
			MATHEX_SCALAR xGradient = lineLength / xLineLength;
			xDistanceIncrement = -xCellLength * xGradient;
			xNextDistance = (xMin + xCurrentCell * xCellLength - startX) * xGradient;
			xCellIncrement = -1;
		}
		else
		{
			xDistanceIncrement = lineLength;
			xNextDistance = lineLength;
			xCellIncrement = 0;
		}

		PATH_PROFILE_INSPECT( xDistanceIncrement );
		PATH_PROFILE_INSPECT( xNextDistance );
		PATH_PROFILE_INSPECT( xCellIncrement );

		MATHEX_SCALAR yDistanceIncrement;
		MATHEX_SCALAR yNextDistance;
		int yCellIncrement;

		if( yLineLength > useEps )
		{
			MATHEX_SCALAR yGradient = lineLength / yLineLength;
			yDistanceIncrement = yCellLength * yGradient;
			yNextDistance = (yMin + (yCurrentCell + 1) * yCellLength - startY) * yGradient;
			yCellIncrement = 1;
		}
		else if( yLineLength < negUseEps )
		{
			MATHEX_SCALAR yGradient = lineLength / yLineLength;
			yDistanceIncrement = -yCellLength * yGradient;
			yNextDistance = (yMin + yCurrentCell * yCellLength - startY) * yGradient;
			yCellIncrement = -1;
		}
		else
		{
			yNextDistance = yDistanceIncrement = lineLength;
			yCellIncrement = 0;
		}

		PATH_PROFILE_INSPECT( yDistanceIncrement );
		PATH_PROFILE_INSPECT( yNextDistance );
		PATH_PROFILE_INSPECT( yCellIncrement );

		//We should only intersect with each triangle once, although it may cover several cells, so
		//use an array to prevent duplication
		bool aHadTriangle[ NMAX_TRIANGLES + 1 ];
		memset( aHadTriangle, 0, sizeof( bool ) * (NMAX_TRIANGLES + 1) );

		TriangleIntersectDatas intersectDatas;
		intersectDatas.reserve( 32 );

		//Beginning with the current cell, advance through all the cells traversed by the line
		bool done = false;
		MATHEX_SCALAR limitDistance = lineLength - useEps;
		while( not done )
		{
			//Process current cell. Iterate through its triangles
            size_t cellIndex = xCurrentCell + yCurrentCell * nXCell;
	        size_t triangleIndex = ( cellIndex == 0 ? 0 : cellCounts_[ cellIndex - 1 ] );
        	size_t nTriangles = cellCounts_[ cellIndex ] - triangleIndex;

			PATH_PROFILE_INSPECT( xCurrentCell );
			PATH_PROFILE_INSPECT( yCurrentCell );
			PATH_PROFILE_INSPECT( triangleIndex );
			PATH_PROFILE_INSPECT( nTriangles );

	        for( size_t i = nTriangles; i--; )
	        {
	            //Get the triangle data
	            TriangleId id = cellTriangleLists_[ triangleIndex++ ];
				if( not aHadTriangle[ id ] )
				{
					//set flag so we don't process this one again.
					aHadTriangle[ id ] = true;

					//Find any section overlap with this triangle, if so adding a data item to intersectDatas
					profileTriangleIntersect( startX, startY, xUnitDir, yUnitDir, lineLength, id, &intersectDatas );
				}
	        }

			//Advance to next cell
			bool xAdvance = true;
			bool yAdvance = true;
			MATHEX_SCALAR diff = yNextDistance - xNextDistance;

			if( diff > useEps )
				yAdvance = false;
			else if( diff < negUseEps )
				xAdvance = false;

			if( xAdvance )
			{
				if( xNextDistance > limitDistance )
					done = true;
				else
				{
					xCurrentCell += xCellIncrement;
					xNextDistance += xDistanceIncrement;
				}
			}

			if( yAdvance )
			{
				if( yNextDistance > limitDistance )
					done = true;
				else
				{
					yCurrentCell += yCellIncrement;
					yNextDistance += yDistanceIncrement;
				}
			}
		}

		//Sort the intersection data into sections along the path, and add the output transforms
		//to the output vector.
		profileOutput( intersectDatas, pProfile );
	}

	//If we haven't added any profile data, we'd better put some default in going from start to
	//end point. This can happen for very short sections.
	if( profileSizeOnEntry == pProfile->size() )
	{
		PATH_PROFILE_WHERE;

		MexVec3 normal;
		MATHEX_SCALAR startZ = height( startX, startY, &normal );
		MATHEX_SCALAR finishZ = height( finishX, finishY, &normal );
		MexPoint3d p1( startX, startY, startZ );
		MexPoint3d p2( finishX, finishY, finishZ );

		MexVec3 xBasis( p1, p2 );
        pProfile->push_back( MexTransform3d( MexTransform3d::X_XZ, xBasis, normal, p1 ) );
	    pProfile->push_back( pProfile->back() );
	    pProfile->back().position( p2 );
	}

    POST( profileValid( *pProfile ) );

    PATH_PROFILE_INDENT( -2 );
    PATH_PROFILE_STREAM( "Exit  MachPhysTileData::pathProfile Start find path profile from " << startPoint << " to " << finishPoint << std::endl );
    PATH_PROFILE_STREAM( "......................................................" << std::endl );
}

bool MachPhysTileData::profileTriangleIntersect
(
	MATHEX_SCALAR xStart, MATHEX_SCALAR yStart,
	MATHEX_SCALAR xUnitDir, MATHEX_SCALAR yUnitDir,	MATHEX_SCALAR lineLength,
	TriangleId triangleId,
	TriangleIntersectDatas* pIntersectDatas
) const
{
	PATH_PROFILE_STREAM( "profileTriangleIntersect entry: triangleId " << triangleId << std::endl );
    PATH_PROFILE_INDENT( 2 );

	//Get the triangle points
	const Triangle& triangle = triangles_[ triangleId ];
    const MexPoint3d& vertex1 = vertices_[ triangle.v1() ];
    const MexPoint3d& vertex2 = vertices_[ triangle.v2() ];
    const MexPoint3d& vertex3 = vertices_[ triangle.v3() ];

	PATH_PROFILE_INSPECT( vertex1 );
	PATH_PROFILE_INSPECT( vertex2 );
	PATH_PROFILE_INSPECT( vertex3 );

	//Projecting each triangle point down into the z=0 plane, calculate its distance along
	//and from the infinite line.
	MATHEX_SCALAR dx, dy, s[3], h[3], z[3];

	dx = vertex1.x() - xStart;
	dy = vertex1.y() - yStart;
	s[0] = dx * xUnitDir + dy * yUnitDir;
	h[0] = dy * xUnitDir - dx * yUnitDir;
	z[0] = vertex1.z();

	dx = vertex2.x() - xStart;
	dy = vertex2.y() - yStart;
	s[1] = dx * xUnitDir + dy * yUnitDir;
	h[1] = dy * xUnitDir - dx * yUnitDir;
	z[1] = vertex2.z();

	dx = vertex3.x() - xStart;
	dy = vertex3.y() - yStart;
	s[2] = dx * xUnitDir + dy * yUnitDir;
	h[2] = dy * xUnitDir - dx * yUnitDir;
	z[2] = vertex3.z();

	PATH_PROFILE_INSPECT( s[0] );
	PATH_PROFILE_INSPECT( h[0] );
	PATH_PROFILE_INSPECT( z[0] );
	PATH_PROFILE_INSPECT( s[1] );
	PATH_PROFILE_INSPECT( h[1] );
	PATH_PROFILE_INSPECT( z[1] );
	PATH_PROFILE_INSPECT( s[2] );
	PATH_PROFILE_INSPECT( h[2] );
	PATH_PROFILE_INSPECT( z[2] );

	//Set up the epsilon values
	MATHEX_SCALAR eps = MexEpsilon::instance();
	MATHEX_SCALAR negEps = -eps;

	MATHEX_SCALAR sOut[2], zOut[2];
	int nOut = 0;
	bool haveSection = false;

	//Deal with each edge
	for( int i = 0; i != 3; ++i )
	{
		int j = (i + 1) % 3;

		//See if the edge crosses the line, or first point lies on it
		bool crossesOver = false;

		if( h[i] > eps )
			crossesOver = h[j] < negEps;
		else if( h[i] < negEps )
			crossesOver = h[j] > eps;
		else
		{
			//lies on line
			sOut[nOut] = s[i];
			zOut[nOut++] = z[i];
		}

		if( crossesOver)
		{
			MATHEX_SCALAR div = 1.0 / (h[j] - h[i]);
			sOut[nOut] = (h[j]*s[i] - h[i]*s[j]) * div;
			zOut[nOut++] = (h[j]*z[i] - h[i]*z[j]) * div;
		}

		if( nOut == 2 )
		{
			haveSection = true;
			break;
		}
	}

	//See if we got an intersection
	if( haveSection )
	{
		haveSection = false;

		//Ensure the points are in order along the line
		if( sOut[0] > sOut[1] )
		{
			MATHEX_SCALAR temp = sOut[1];
			sOut[1] = sOut[0];
			sOut[0] = temp;

			temp = zOut[1];
			zOut[1] = zOut[0];
			zOut[0] = temp;
		}

		PATH_PROFILE_INSPECT( sOut[0] );
		PATH_PROFILE_INSPECT( zOut[0] );
		PATH_PROFILE_INSPECT( sOut[1] );
		PATH_PROFILE_INSPECT( zOut[1] );

		//Check the overlap isn't off the end of the line
		if( sOut[0] < (lineLength - eps)  and  sOut[1] > eps )
		{
			MATHEX_SCALAR separation = sOut[1] - sOut[0];
			if( separation > eps )
			{
				//Trim the first end if before line start
				if( sOut[0] < 0.0 )
				{
					PATH_PROFILE_WHERE;

					zOut[0] = (zOut[0]*sOut[1] - zOut[1]*sOut[0]) / separation;
					sOut[0] = 0.0;
					separation = sOut[1];
				}

				//Trim the second end if off the end of the line
				if( sOut[1] > lineLength )
				{
					PATH_PROFILE_WHERE;

					zOut[1] = (zOut[0]*(sOut[1] - lineLength) + zOut[1]*(lineLength - sOut[0])) / separation;
					sOut[1] = lineLength;
					separation = sOut[1] - sOut[0];
				}

				//Finally we may have a section to output
				if( separation > eps )
				{
					haveSection = true;

					//Compute the triangle normal
				    MexVec3 normal = MexVec3::crossProduct( MexVec3( vertex1, vertex2 ), MexVec3( vertex2, vertex3 ) );

					//Add the data to the output vector
					pIntersectDatas->push_back
					(
						TriangleIntersectData
						(
							sOut[0],
							MexPoint3d( xStart + sOut[0]*xUnitDir, yStart + sOut[0]*yUnitDir, zOut[0] ),
							MexPoint3d( xStart + sOut[1]*xUnitDir, yStart + sOut[1]*yUnitDir, zOut[1] ),
							normal
						)
					);

					PATH_PROFILE_INSPECT( pIntersectDatas->back().entryDistance() );
					PATH_PROFILE_INSPECT( pIntersectDatas->back().entryPoint() );
					PATH_PROFILE_INSPECT( pIntersectDatas->back().exitPoint() );
					PATH_PROFILE_INSPECT( pIntersectDatas->back().normal() );
				}
			}
		}
	}

    PATH_PROFILE_INDENT( -2 );
	PATH_PROFILE_STREAM( "profileTriangleIntersect entry: haveSection " << haveSection << std::endl );
	return haveSection;
}


void MachPhysTileData::profileOutput( TriangleIntersectDatas& intersectDatas, Profile* pProfile ) const
{
	PATH_PROFILE_STREAM( "profileOutput entry" << std::endl );
    PATH_PROFILE_INDENT( 2 );

	//Do a bubble sort on the intersect data collection, using an index array.
	//This is good, because the data should already be fairly well sorted.
	size_t aDataIndex[ NMAX_TRIANGLES + 1 ];
	size_t nDatas = 0;

	TriangleIntersectDatas::iterator itEnd = intersectDatas.end();
	TriangleIntersectDatas::iterator itBegin = intersectDatas.begin();
	for( TriangleIntersectDatas::iterator it = itBegin; it != itEnd; ++it, ++nDatas )
	{
		MATHEX_SCALAR itDistance = (*it).entryDistance();

		size_t index = nDatas;
		while( index != 0 )
		{
            size_t j = aDataIndex[ index - 1 ];
			MATHEX_SCALAR alreadySortedDistance = (*( itBegin+j ) ).entryDistance();
			if( itDistance < alreadySortedDistance )
			{
				aDataIndex[ index ] = j;
				--index;
			}
			else
				break;
		}

		aDataIndex[ index ] = nDatas;
	}

	//Now add the data to the return collection
	for( size_t index = 0; index != nDatas; ++index )
	{
		bool lastOne = index == (nDatas - 1);

		PATH_PROFILE_INSPECT( aDataIndex[ index ] );

		TriangleIntersectData& data = *(itBegin+aDataIndex[ index ]);

		if( not lastOne )
		{
			//Check for the entry point being within 1cm of the next one.
			//If so, we don't need it.
			MATHEX_SCALAR nextEntryDistance = (*(itBegin+aDataIndex[ index+1 ])).entryDistance();
			if( (nextEntryDistance - data.entryDistance()) < 0.01 )
				continue;
		}

		MexVec3 xBasis( data.entryPoint(), data.exitPoint() );
        pProfile->push_back( MexTransform3d( MexTransform3d::X_XZ, xBasis, data.normal(), data.entryPoint() ) );

		PATH_PROFILE_INSPECT( pProfile->back() );

		if( lastOne )
		{
			//Finally add the end transform. This has the same orientation as the entry position on the final section,
			//but has its location at the exit point.
		    pProfile->push_back( pProfile->back() );
		    pProfile->back().position( data.exitPoint() );
		}
	}

    PATH_PROFILE_INDENT( -2 );
	PATH_PROFILE_STREAM( "profileOutput exit" << std::endl );
}

/* End TILEDATA.CPP *************************************************/
