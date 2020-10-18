/*
 * A S C P I C T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "utility/ascpict.hpp"
#include "utility/internal/ascpicti.hpp"

UtlAsciiPicture::UtlAsciiPicture(
    MATHEX_SCALAR minX, MATHEX_SCALAR minY,
    MATHEX_SCALAR maxX, MATHEX_SCALAR maxY,
    uint nColumns )
: pImpl_( _NEW( UtlAsciiPictureImpl( minX, minY, maxX, maxY, nColumns ) ) )
{
    PRE( nColumns > 0 );

    TEST_INVARIANT;
}

UtlAsciiPicture::~UtlAsciiPicture()
{
    TEST_INVARIANT;

    _DELETE( pImpl_ );
}

uint UtlAsciiPicture::nColumns() const
{
    return pImpl_->nColumns();
}

uint UtlAsciiPicture::nRows() const
{
    return pImpl_->nRows();
}

void UtlAsciiPicture::pixel( uint x, uint y, char c )
{
    PRE_INFO( x );
    PRE_INFO( nColumns() );
    PRE_INFO( y );
    PRE_INFO( nRows() );

    PRE( x < nColumns() );
    PRE( y < nRows() );

    pImpl_->pixel( x, y, c );
}

char UtlAsciiPicture::pixel( uint x, uint y ) const
{
    PRE_INFO( x );
    PRE_INFO( nColumns() );
    PRE_INFO( y );
    PRE_INFO( nRows() );

    PRE( x < nColumns() );
    PRE( y < nRows() );

    return pImpl_->pixel( x, y );
}

void UtlAsciiPicture::fill( const MexPolygon2d& poly, char c )
{
    pImpl_->fill( poly, c );
}

void UtlAsciiPicture::outline( const MexPolygon2d& poly, char c )
{
    pImpl_->outline( poly, c );
}

void UtlAsciiPicture::outline( const MexAlignedBox2d& box, char c )
{
    pImpl_->outline( box, c );
}

int UtlAsciiPicture::column( MATHEX_SCALAR x ) const
{
    return pImpl_->column( x );
}

int UtlAsciiPicture::row( MATHEX_SCALAR y ) const
{
    return pImpl_->row( y );
}

MexPoint2d  UtlAsciiPicture::point( int x, int y ) const
{
    return pImpl_->point( x, y );
}

MATHEX_SCALAR UtlAsciiPicture::xRange() const
{
    return pImpl_->xRange();
}

MATHEX_SCALAR UtlAsciiPicture::yRange() const
{
    return pImpl_->yRange();
}

MATHEX_SCALAR UtlAsciiPicture::pixelWidth() const
{
    return pImpl_->pixelWidth();
}

MATHEX_SCALAR UtlAsciiPicture::pixelHeight() const
{
    return pImpl_->pixelHeight();
}

void UtlAsciiPicture::line( const MexLine2d& line, char c )
{
    pImpl_->line( line, c );
}

void UtlAsciiPicture::grid()
{
    pImpl_->grid();
}

void UtlAsciiPicture::title( const std::string& title )
{
    pImpl_->title( title );
}

void UtlAsciiPicture::underText( const std::string& text )
{
    pImpl_->underText( text );
}

void UtlAsciiPicture::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const UtlAsciiPicture& t )
{
    o << *t.pImpl_;

    return o;
}

/* End ASCPICT.CPP **************************************************/
