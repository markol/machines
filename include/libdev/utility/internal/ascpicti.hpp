/*
 * A S C P I C T I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    UtlAsciiPictureImpl

    A brief description of the class should go in here
*/

#ifndef _UTILITY_ASCPICTI_HPP
#define _UTILITY_ASCPICTI_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point2d.hpp"

#include "ctl/vector.hpp"
#include <string>

class MexAlignedBox2d;
class MexPolygon2d;
class MexLine2d;
class UtlAsciiPictureImpl;

class UtlAsciiPictureImpl
// Canonical form revoked
{
public:
    //  Constructor where the number of rows is calculated automatically to
    //  maintain the aspect ratio.
    UtlAsciiPictureImpl(
        MATHEX_SCALAR minX, MATHEX_SCALAR minY,
        MATHEX_SCALAR maxX, MATHEX_SCALAR maxY,
        uint nColumns );
    //  Constructor where it is possible to have different scales on the
    //  X and Y axes
    UtlAsciiPictureImpl(
        MATHEX_SCALAR minX, MATHEX_SCALAR minY,
        MATHEX_SCALAR maxX, MATHEX_SCALAR maxY,
        uint nColumns, uint nRows );

    ~UtlAsciiPictureImpl( void );

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

    void title( const std::string& title );
    void underText( const std::string& text );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const UtlAsciiPictureImpl& t );

    UtlAsciiPictureImpl( const UtlAsciiPictureImpl& );
    UtlAsciiPictureImpl& operator =( const UtlAsciiPictureImpl& );


    MATHEX_SCALAR minX_;
    MATHEX_SCALAR minY_;
    MATHEX_SCALAR maxX_;
    MATHEX_SCALAR maxY_;

    //  Informations used in formatting the axis labels
    int xFieldWidth_;
    int xPrecision_;
    int yFieldWidth_;
    int yPrecision_;

    ctl_vector< std::string >    picture_;

    ctl_vector< std::string >    xLabels_;
    ctl_vector< std::string >    yLabels_;

    std::string                  title_;
    ctl_vector< std::string >    underText_;
};


#endif

/* End ASCPICTI.HPP *************************************************/
