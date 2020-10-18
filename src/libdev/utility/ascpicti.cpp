/*
 * A S C P I C T I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "utility/internal/ascpicti.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/grid2d.hpp"
#include "mathex/line2d.hpp"

UtlAsciiPictureImpl::UtlAsciiPictureImpl(
    MATHEX_SCALAR minX, MATHEX_SCALAR minY,
    MATHEX_SCALAR maxX, MATHEX_SCALAR maxY,
    uint nColumns )
: minX_( minX ),
  minY_( minY ),
  maxX_( maxX ),
  maxY_( maxY )
{
    PRE( nColumns > 0 );

    const uint nRows = nColumns * ( maxY - minY ) / ( maxX - minX );

    const std::string row( ' ', nColumns );
    picture_.reserve( nRows );

    for( size_t i = 0; i < nRows; ++i )
        picture_.push_back( row );

// bob    initialiseLabels();

    TEST_INVARIANT;
}

UtlAsciiPictureImpl::~UtlAsciiPictureImpl()
{
    TEST_INVARIANT;

}

uint UtlAsciiPictureImpl::nColumns() const
{
    return picture_.front().length();
}

uint UtlAsciiPictureImpl::nRows() const
{
    return picture_.size();
}

void UtlAsciiPictureImpl::pixel( uint x, uint y, char c )
{
    PRE_INFO( x );
    PRE_INFO( nColumns() );
    PRE_INFO( y );
    PRE_INFO( nRows() );

    PRE( x < nColumns() );
    PRE( y < nRows() );

    picture_[ y ][ x ] = c;
}

char UtlAsciiPictureImpl::pixel( uint x, uint y ) const
{
    PRE_INFO( x );
    PRE_INFO( nColumns() );
    PRE_INFO( y );
    PRE_INFO( nRows() );

    PRE( x < nColumns() );
    PRE( y < nRows() );

    return picture_[ y ][ x ];
}

void UtlAsciiPictureImpl::fill( const MexPolygon2d& poly, char c )
{
    MexAlignedBox2d aBox;

    poly.boundary( &aBox );

    uint x1 = column( aBox.minCorner().x() );
    uint x2 = column( aBox.maxCorner().x() );
    uint y1 = row( aBox.minCorner().y() );
    uint y2 = row( aBox.maxCorner().y() );

    if( x1 > 0 )
        --x1;
    if( x2 < nColumns() )
        ++x2;
    if( y1 > 0 )
        --y1;
    if( y2 < nRows() )
        ++y2;

    for( uint x = x1; x < x2; ++x )
    {
        for( uint y = y1; y < y2; ++y )
        {
            MexPoint2d point2d = point( x, y );

            if( poly.contains( point2d ) )
                pixel( x, y, c );
        }
    }
}

void UtlAsciiPictureImpl::outline( const MexPolygon2d& poly, char c )
{
    for( size_t i = 0; i < poly.nVertices(); ++i )
    {
        const MexPoint2d& end1 = poly.vertex( i );
        const MexPoint2d& end2 = ( i == poly.nVertices() - 1 ) ? poly.vertex( 0 ) : poly.vertex( i + 1 );

        line( MexLine2d( end1, end2 ), c );
    }
}

void UtlAsciiPictureImpl::outline( const MexAlignedBox2d& box, char c )
{
    const MexPoint2d& p1 = box.minCorner();
    const MexPoint2d p2( box.minCorner().x(), box.maxCorner().y() );
    const MexPoint2d& p3 = box.maxCorner();
    const MexPoint2d p4( box.maxCorner().x(), box.minCorner().y() );

    line( MexLine2d( p1, p2 ), c );
    line( MexLine2d( p2, p3 ), c );
    line( MexLine2d( p3, p4 ), c );
    line( MexLine2d( p4, p1 ), c );
}

int UtlAsciiPictureImpl::column( MATHEX_SCALAR x ) const
{
    return ( x - minX_ ) / pixelWidth();
}

int UtlAsciiPictureImpl::row( MATHEX_SCALAR y ) const
{
    return ( y - minY_ ) / pixelHeight();
}

MexPoint2d  UtlAsciiPictureImpl::point( int x, int y ) const
{
    //  Return the point at the top left of the pixel
    MexPoint2d result(
        minX_ + x * pixelWidth(),
        minY_ + y * pixelHeight() );

    return result;
}

MATHEX_SCALAR UtlAsciiPictureImpl::xRange() const
{
    return maxX_ - minX_;
}

MATHEX_SCALAR UtlAsciiPictureImpl::yRange() const
{
    return maxY_ - minY_;
}

MATHEX_SCALAR UtlAsciiPictureImpl::pixelWidth() const
{
    return ( maxX_ - minX_ ) / ( nColumns() - 1 );
}

MATHEX_SCALAR UtlAsciiPictureImpl::pixelHeight() const
{
    return ( maxY_ - minY_ ) / ( nRows() - 1 );
}

void UtlAsciiPictureImpl::line( const MexLine2d& line, char c )
{
    const MATHEX_SCALAR xCellSize = xRange() / nColumns();
    const MATHEX_SCALAR yCellSize = yRange() / nRows();
    const MexPoint2d origin( minX_, minY_ );

    MexGrid2d grid( nColumns(), nRows(),
      pixelWidth(), pixelHeight(), origin );

    MexGrid2d::Cells cells;
    MexGrid2d::Points points;
    uint nToReserve = nColumns() + nRows() + 4;
    cells.reserve( nToReserve );
    points.reserve( nToReserve );

    grid.intersect( line, &cells, &points );

    for( size_t i = 0; i < cells.size(); ++i )
    {
        const uint x = cells[ i ].xIndex();
        const uint y = cells[ i ].yIndex();

        if( x < nColumns() and y < nRows() )
            pixel( cells[ i ].xIndex(), cells[ i ].yIndex(), c );
    }
}

void UtlAsciiPictureImpl::grid()
{
    const int minXSpacing = 10;
    const int minYSpacing = 10;

    MATHEX_SCALAR xMarkGap = xRange() * minXSpacing / nColumns();

    MATHEX_SCALAR p = ceil( log10( xMarkGap ) );

    MATHEX_SCALAR xMarkGap2 = pow( 10, p );

    if( xMarkGap2 / 2 >= xMarkGap )
        xMarkGap = xMarkGap2 / 2;
    else
        xMarkGap = xMarkGap2;

    MATHEX_SCALAR yMarkGap = yRange() * minYSpacing / nRows();

    p = ceil( log10( yMarkGap ) );

    MATHEX_SCALAR yMarkGap2 = pow( 10, p );

    if( yMarkGap2 / 2 >= yMarkGap )
        yMarkGap = yMarkGap2 / 2;
    else
        yMarkGap = yMarkGap2;

    for( MATHEX_SCALAR x = 0.0; x <= maxX_; x += xMarkGap )
    {
        if( x >= minX_ )
        {
            line( MexLine2d( MexPoint2d( x, minY_ ), MexPoint2d( x, maxY_ ) ), '|' );
        }
    }

    for( MATHEX_SCALAR x = 0.0; x >= minX_; x -= xMarkGap )
    {
        if( x <= maxX_ )
        {
            line( MexLine2d( MexPoint2d( x, minY_ ), MexPoint2d( x, maxY_ ) ), '|' );
        }
    }

    for( MATHEX_SCALAR y = 0.0; y <= maxY_; y += yMarkGap )
    {
        if( y >= minY_ )
        {
            line( MexLine2d( MexPoint2d( minX_, y ), MexPoint2d( maxX_, y ) ), '-' );
        }
    }

    for( MATHEX_SCALAR y = 0.0; y >= minY_; y -= yMarkGap )
    {
        if( y <= maxY_ )
        {
            line( MexLine2d( MexPoint2d( minX_, y ), MexPoint2d( maxX_, y ) ), '-' );
        }
    }
}

void UtlAsciiPictureImpl::title( const std::string& title )
{
    title_ = title;
}

void UtlAsciiPictureImpl::underText( const std::string& text )
{
    underText_.push_back( text );
}

// bobvoid UtlAsciiPictureImpl::initialiseLabels()
// bob{
// bob    nXRows = 5;
// bob    nXColumns = ????
// bob
// bob    nYRows = ???
// bob    nYColumns = ????
// bob}

void UtlAsciiPictureImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );

//    INVARIANT( picture_.size() != 0 );
//    INVARIANT( xLabels_.size() != 0 );
//    INVARIANT( yLabels_.size() != 0 );

    INVARIANT_INFO( picture_[ 0 ].length() );
    INVARIANT_INFO( xLabels_[ 0 ].length() );
    INVARIANT_INFO( yLabels_[ 0 ].length() );

//    INVARIANT( picture_[ 0 ].length() + 2 * yLabels_[ 0 ].length() == xLabels_[ 0 ].length() );

}

ostream& operator <<( ostream& o, const UtlAsciiPictureImpl& t )
{
    if( t.title_.length() != 0 )
        o << t.title_ << std::endl << std::endl;

    o << "Top left ( " << t.minX_ << " " << t.minY_ << " )" << std::endl;
    o << "Bottom right   ( " << t.maxX_ << " " << t.maxY_ << " )" << std::endl;
    o << std::endl;

    o << "One character = " << t.pixelWidth() << " x " << t.pixelHeight() << std::endl;

    o << std::endl;
    o << "    ->+X" << std::endl;
    o << "   |" << std::endl;
    o << "   v" << std::endl;
    o << "  +Y" << std::endl;
    o << std::endl;

    for( size_t i = 0; i < t.nRows(); ++i )
        o << t.picture_[ i ] << std::endl;

    o << std::endl << std::endl;

    for( size_t i = 0; i < t.underText_.size(); ++i )
        o << t.underText_[ i ];

    return o;
}

/* End ASCPICTI.CPP *************************************************/
