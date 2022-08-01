/*
 * A S C P I C T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    UtlAsciiPicture

    A brief description of the class should go in here
*/

#ifndef _UTILITY_ASCPICT_HPP
#define _UTILITY_ASCPICT_HPP

#include "base/base.hpp"

#include "mathex/mathex.hpp"
#include "mathex/point2d.hpp"

#include <string>

class MexAlignedBox2d;
class MexPolygon2d;
class MexLine2d;
class UtlAsciiPictureImpl;

class UtlAsciiPicture
// Canonical form revoked
{
public:

    //  Constructor where the number of rows is calculated automatically to
    //  maintain the aspect ratio.
    UtlAsciiPicture(
        MATHEX_SCALAR minX, MATHEX_SCALAR minY,
        MATHEX_SCALAR maxX, MATHEX_SCALAR maxY,
        uint nColumns );

    //  Constructor where it is possible to have different scales on the
    //  X and Y axes
    UtlAsciiPicture(
        MATHEX_SCALAR minX, MATHEX_SCALAR minY,
        MATHEX_SCALAR maxX, MATHEX_SCALAR maxY,
        uint nColumns, uint nRows );

    ~UtlAsciiPicture();

    uint nColumns() const;
    uint nRows() const;

    void clear();

    void pixel( uint x, uint y, char );
    // PRE( x < nColumns() );
    // PRE( y < nRows() );

    char pixel( uint x, uint y ) const;
    // PRE( x < nColumns() );
    // PRE( y < nRows() );

    void fill( const MexPolygon2d&, char );
    void outline( const MexPolygon2d&, char );

    void outline( const MexAlignedBox2d&, char );

    int column( MATHEX_SCALAR x ) const;
    int row( MATHEX_SCALAR y ) const;

    MexPoint2d  point( int x, int y ) const;

    void line( const MexLine2d&, char );

    MATHEX_SCALAR pixelWidth() const;
    MATHEX_SCALAR pixelHeight() const;

    MATHEX_SCALAR xRange() const;
    MATHEX_SCALAR yRange() const;

    void grid();

    //  Specify a title to be displayed above the picture. Only the
    //  last title defined will be displayed
    void title( const std::string& title );

    //  Specify a string to be printed under the picture. All strings will
    //  be displayed in the order they were given with no extra line feeds
    //  between them. This is intended to be used for giving a key e.g.

    //  underText( "*  obstacles\n" );
    //  underText( ".  domains\n" );

    void underText( const std::string& text );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const UtlAsciiPicture& t );

    UtlAsciiPicture( const UtlAsciiPicture& );
    UtlAsciiPicture& operator =( const UtlAsciiPicture& );

    UtlAsciiPictureImpl* pImpl_;
};


#endif

/* End ASCPICT.HPP **************************************************/
