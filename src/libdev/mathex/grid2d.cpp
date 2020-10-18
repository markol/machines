/*
 * G R I D 2 D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <cstdio>

#include "mathex/grid2d.hpp"
#include "mathex/line2d.hpp"

MexGrid2d::MexGrid2d( size_t nXCells, size_t nYCells,
    MATHEX_SCALAR xSize, MATHEX_SCALAR ySize,
    const MexPoint2d& origin )
: nXCells_( nXCells ),
  nYCells_( nYCells ),
  origin_( origin ),
  xCellSize_( xSize ),
  yCellSize_( ySize )
{
    PRE_INFO( nXCells  );
    PRE_INFO( nYCells );
    PRE_INFO( origin );
    PRE_INFO( xSize );
    PRE_INFO( ySize );

    PRE( xSize > 0 );
    PRE( ySize > 0 );

    MEX_GRID2D_INSPECT( nXCells  );
    MEX_GRID2D_INSPECT( nYCells );
    MEX_GRID2D_INSPECT( xSize  );
    MEX_GRID2D_INSPECT( ySize );
    MEX_GRID2D_INSPECT( origin );

    TEST_INVARIANT;
}

MexGrid2d::~MexGrid2d()
{
    TEST_INVARIANT;

}

void MexGrid2d::intersect( const MexLine2d& line, Cells* pCells, Points* pPoints )
{
    MEX_GRID2D_INSPECT( line );

    PRE( pCells->size() == 0 );
    PRE( pPoints->size() == 0 );

    //  Scale everything so that we're working on a grid with cells
    //  that are 1x1 with the origin at ( 0, 0 )

    MexPoint2d  end1(
      ( line.end1().x() - origin_.x() ) / xCellSize_,
      ( line.end1().y() - origin_.y() ) / yCellSize_ );

    MexPoint2d  end2(
      ( line.end2().x() - origin_.x() ) / xCellSize_,
      ( line.end2().y() - origin_.y() ) / yCellSize_ );

    MEX_GRID2D_INSPECT( end1 );
    MEX_GRID2D_INSPECT( end2 );

    //Work out sensible reserve size for the local vectors
    uint nToReserve = 8;

    //  Get the x coordinates where the line crosses cell boundaries
    //  (these will all be integers because of the scaling)

    Scalars    xCrossings;
    xCrossings.reserve( nToReserve );

    //  The epsilon check is a protection against rounding errors in floor within getIntegersBetween
    //  It must be done with the unscaled values otherwise we get into trouble with
    //  rounding errors near the edges of cells.
    if( not MexEpsilon::isWithinEpsilonOf( line.end1().x(), line.end2().x() ) )
        getIntegersBetween( end1.x(), end2.x(), &xCrossings );

    MEX_GRID2D_INSPECT( xCrossings );

    MATHEX_SCALAR   previousY = end1.y();

    Points  internalPoints;
    internalPoints.reserve( nToReserve );

    if( end1.x() >= 0 and end1.x() <= nXCells_ and
        end1.y() >= 0 and end1.y() <= nYCells_ )
    {
        internalPoints.push_back( end1 );
    }

    //  The +1 is to include the last point in the calculations
    for( size_t i = 0; i < xCrossings.size() + 1; ++i )
    {
        MATHEX_SCALAR   x;
        MATHEX_SCALAR   y;

        if( i == xCrossings.size() )
        {
            x = end2.x();
            y = end2.y();
        }
        else
        {
            x = xCrossings[ i ];
            y = mexInterpolate( x, end1.x(), end2.x(), end1.y(), end2.y() );
        }

        //  Check for in range
        if( x >= 0 and x <= nXCells_ )
        {

            Scalars    yCrossings;
            yCrossings.reserve( nToReserve );

            const MATHEX_SCALAR originalScalePreviousY = previousY * yCellSize_ + origin_.y();
            const MATHEX_SCALAR originalScaleY = y * yCellSize_ + origin_.y();

            //  The epsilon check is a protection against rounding errors in floor within getIntegersBetween
            //  It must be done with the unscaled values otherwise we get into trouble with
            //  rounding errors near the edges of cells.

            if( not MexEpsilon::isWithinEpsilonOf( originalScalePreviousY, originalScaleY ) )
                getIntegersBetween( previousY, y, &yCrossings );

            MEX_GRID2D_INSPECT( yCrossings );

            for( Scalars::const_iterator j = yCrossings.begin(); j != yCrossings.end(); ++j )
            {
                MATHEX_SCALAR y = *j;
                MATHEX_SCALAR x = mexInterpolate( y, end1.y(), end2.y(), end1.x(), end2.x() );

                internalPoints.push_back( MexPoint2d( x, y ) );
            }

            internalPoints.push_back( MexPoint2d( x, y ) );

            previousY = y;
        }
    }

    Points::iterator j = internalPoints.begin();

    MexPoint2d  convertedI;

    if( j != internalPoints.end() )
    {
        convertedI = MexPoint2d(
          (*j).x() * xCellSize_ + origin_.x(),
          (*j).y() * yCellSize_ + origin_.y() );

        ++j;
    }


    for( Points::iterator i = internalPoints.begin(); i != internalPoints.end(); ++i, ++j )
    {
        MexPoint2d  convertedJ;

        bool    useThisPoint = true;

        //  Work out the cell that the line segment from *i to *j
        //  intersects with.
        if( j != internalPoints.end() )
        {
            convertedJ = MexPoint2d(
              (*j).x() * xCellSize_ + origin_.x(),
              (*j).y() * yCellSize_ + origin_.y() );

            if( convertedI == convertedJ )
                useThisPoint = false;
            else if( not inRange( convertedI ) )
                useThisPoint = false;
            else
            {
                MEX_GRID2D_INSPECT( (*i) );
                MEX_GRID2D_INSPECT( (*j) );

                //Compute the cell indices. If the line runs along the right or bottom edge of the grid,
                //we can get values for the indices off the grid. Hence convert them to the right/bottom
                //legal cells.
                int xCellIndex = ( (*i).x() + (*j).x() ) / 2.0;
                int yCellIndex = ( (*i).y() + (*j).y() ) / 2.0;

                MEX_GRID2D_INSPECT( xCellIndex );
                MEX_GRID2D_INSPECT( yCellIndex );

                if( xCellIndex >= nXCells_ )
                    xCellIndex = nXCells_ - 1;

                if( yCellIndex >= nYCells_ )
                    yCellIndex = nYCells_ - 1;

                MEX_GRID2D_INSPECT( xCellIndex );
                MEX_GRID2D_INSPECT( yCellIndex );

                pCells->push_back( CellIndex( xCellIndex, yCellIndex ) );
            }
        }
        else if( not inRange( convertedI ) )
            useThisPoint = false;

        if( useThisPoint )
            pPoints->push_back( convertedI );

        convertedI = convertedJ;
    }

    POST_INFO( pCells->size() );
    POST_INFO( pPoints->size() );

//    POST( ( pCells->size() == 0 and pPoints->size() == 0 ) or
//      ( pCells->size() == pPoints->size() - 1 ) );

    POST( valid( *pCells, line ) );
    POST( valid( *pPoints, line ) );
}

void MexGrid2d::getIntegersBetween( MATHEX_SCALAR end1, MATHEX_SCALAR end2, Scalars* pIntegers )
{
    PRE( pIntegers->size() == 0 );

    MEX_GRID2D_STREAM( "Get integers between " << end1 << " and " << end2 << std::endl );

    int intEnd1 = floor( end1 );
    ASSERT_INFO( intEnd1 );
    ASSERT_INFO( end1 );

    ASSERT( intEnd1 <= end1, "Paranoia check that conversion will truncate rather than round" );

    int intEnd2 = floor( end2 );
    ASSERT( intEnd2 <= end2, "Paranoia check that conversion will truncate rather than round" );

    if( intEnd1 == intEnd2 )
    {
    }
    else if( intEnd1 > intEnd2 )
    {
        for( int i = ceil( end1 ) - 1; i > end2; --i )
            pIntegers->push_back( i );
    }
    else
    {
        //  intEnd1 < intEnd2

        for( int i = intEnd1 + 1; i < end2; ++i )
            pIntegers->push_back( i );
    }

    MEX_GRID2D_STREAM( "Integers between:\n" << *pIntegers << std::endl );
}

void MexGrid2d::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MexGrid2d& t )
{

    o << "Grid " << t.nXCells_ << "x" << t.nYCells_ << "  ";
    o << "cell size " << t.xCellSize_ << "x" << t.yCellSize_ << "  ";
    o << "origin " << t.origin_;

    return o;
}

bool MexGrid2d::valid( const Cells& cells, const MexLine2d& line ) const
{
    bool isValid = true;

    for( size_t i = 0; i < cells.size() and isValid; ++i )
    {
        ASSERT_INFO( cells[ i ].xIndex() );
        ASSERT_INFO( cells[ i ].yIndex() );

        if( cells[ i ].xIndex() >= nXCells_ )
        {
            ASSERT_FAIL( "Cell xIndex out of range" );
            isValid = false;
        }

        if( cells[ i ].yIndex() >= nYCells_ )
        {
            ASSERT_FAIL( "Cell yIndex out of range" );
            isValid = false;
        }
    }

    //  This has all been commented out because rounding errors can cause
    //  this to be untrue in some cases. The algorithm should be rewritten
    //  to handle these cases correctly, however at this late stage in the
    //  project it isn't going to be. Bob.
    //  TBD: Rewrite the algorithm so that this test can be reintroduced.

//     if( cells.size() > 1 )
//     {
//         bool increasingX = true;
//         if( line.end1().x() > line.end2().x() )
//             increasingX = false;
//
//         bool increasingY = true;
//         if( line.end1().y() > line.end2().y() )
//             increasingY = false;
//
//         for( size_t i = 0; i < cells.size() - 1 and isValid; ++i )
//         {
//             if( increasingX )
//             {
//                 if( cells[ i + 1 ].xIndex() < cells[ i ].xIndex() )
//                 {
//                     ASSERT_FAIL( "X should be increasing" );
//                     ASSERT_INFO( "X should be increasing" );
//                     isValid = false;
//                 }
//             }
//             else
//             {
//                 if( cells[ i + 1 ].xIndex() > cells[ i ].xIndex() )
//                 {
//                     ASSERT_FAIL( "X should be decreasing" );
//                     ASSERT_INFO( "X should be decreasing" );
//                     isValid = false;
//                 }
//             }
//
//             if( increasingY )
//             {
//                 if( cells[ i + 1 ].yIndex() < cells[ i ].yIndex() )
//                 {
//                     ASSERT_FAIL( "Y should be increasing" );
//                     ASSERT_INFO( "Y should be increasing" );
//                     isValid = false;
//                 }
//             }
//             else
//             {
//                 if( cells[ i + 1 ].yIndex() > cells[ i ].yIndex() )
//                 {
//                     ASSERT_FAIL( "Y should be decreasing" );
//                     ASSERT_INFO( "Y should be decreasing" );
//                     isValid = false;
//                 }
//             }
//         }
//     }

    return isValid;
}

bool MexGrid2d::valid( const Points& points, const MexLine2d& line ) const
{
    bool isValid = true;


    for( size_t i = 0; i < points.size() and isValid; ++i )
    {
        ASSERT_INFO( points[ i ] );
        ASSERT_INFO( xMin() );
        ASSERT_INFO( xMax() );
        ASSERT_INFO( yMin() );
        ASSERT_INFO( yMax() );

        if( not inRange( points[ i ] ) )
        {
            ASSERT_FAIL( "Point out of range" );
            isValid = false;
        }
    }


    if( points.size() > 1 )
    {
        bool increasingX = true;
        if( line.end1().x() > line.end2().x() )
            increasingX = false;


        bool increasingY = true;
        if( line.end1().y() > line.end2().y() )
            increasingY = false;


        for( size_t i = 0; i < points.size() - 1 and isValid; ++i )
        {

            ASSERT_INFO( line );
            ASSERT_INFO( points[ i ] );
            ASSERT_INFO( points[ i + 1 ] );

            if( increasingX )
            {
                if( points[ i + 1 ].x() < points[ i ].x() - MexEpsilon::instance() )
                {
                    ASSERT_FAIL( "X should be increasing" );
                    ASSERT_INFO( "X should be increasing" );
                    isValid = false;
                }
            }
            else
            {
                if( points[ i + 1 ].x() > points[ i ].x() + MexEpsilon::instance() )
                {
                    ASSERT_FAIL( "X should be decreasing" );
                    ASSERT_INFO( "X should be decreasing" );
                    isValid = false;
                }
            }

            if( increasingY )
            {
                if( points[ i + 1 ].y() < points[ i ].y() - MexEpsilon::instance() )
                {
                    ASSERT_FAIL( "Y should be increasing" );
                    ASSERT_INFO( "Y should be increasing" );
                    isValid = false;
                }
            }
            else
            {
                if( points[ i + 1 ].y() > points[ i ].y() + MexEpsilon::instance() )
                {
                    ASSERT_FAIL( "Y should be decreasing" );
                    ASSERT_INFO( "Y should be decreasing" );
                    isValid = false;
                }
            }
        }
    }


    return isValid;
}

bool MexGrid2d::inRange( const MexPoint2d& point ) const
{
    bool result = true;

    if( point.x() < xMin() or
        point.x() > xMax() or
        point.y() < yMin() or
        point.y() > yMax() )
    {
        result = false;
    }

    return result;
}

MATHEX_SCALAR MexGrid2d::xMin() const
{
    return origin_.x();
}

MATHEX_SCALAR MexGrid2d::xMax() const
{
    return origin_.x() + xCellSize_ * nXCells_;
}

MATHEX_SCALAR MexGrid2d::yMin() const
{
    return origin_.y();
}

MATHEX_SCALAR MexGrid2d::yMax() const
{
    return origin_.y() + yCellSize_ * nYCells_;
}

/* End GRID2D.CPP ***************************************************/
