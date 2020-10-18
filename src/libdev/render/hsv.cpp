/*
 * H S V . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdlib.h>
#include <algorithm>
#include <cmath>

#include "render/colour.hpp"
#include "render/hsv.hpp"

RenColourHSV::RenColourHSV()
: saturation_( 0.0 ),
  value_( 0.0 ),
  hueDefined_( false )
{

    TEST_INVARIANT;
}

RenColourHSV::RenColourHSV( const RenColour& rgbValue )
{
    double red = rgbValue.r();
    double green = rgbValue.g();
    double blue = rgbValue.b();

// RenColour components greater than 1.0 are now legal.  Does this break
// the conversion algorithm?
//    ASSERT( 0.0 <= red and red <= 1.0, "" );
//    ASSERT( 0.0 <= green and green <= 1.0, "" );
//    ASSERT( 0.0 <= blue and blue <= 1.0, "" );

    double maxRGB = std::max( red, std::max( green, blue ) );
    double minRGB = std::min( red, std::min( green, blue ) );

    value_ = maxRGB;

    if( maxRGB != 0 )
    {
        saturation_ = ( maxRGB - minRGB ) / maxRGB;
    }
    else
        saturation_ = 0.0;

    if( saturation_ == 0.0 )
    {
        hueDefined_ = false;
    }
    else
    {
        hueDefined_ = true;

        double delta = maxRGB - minRGB;

        if( red == maxRGB )
            hue_ = ( green - blue ) / delta;
        else if ( green == maxRGB )
            hue_ = 2 + (blue - red ) / delta;
        else if ( blue == maxRGB )
            hue_ = 4 + ( red - green ) / delta;

        hue_ *= 60.0;

        if( hue_ < 0.0 )
            hue_ += 360.0;
    }

    TEST_INVARIANT;

//     POST_INFO( (int)rgb().red() );
//     POST_INFO( (int)rgb().green() );
//     POST_INFO( (int)rgb().blue() );
//     POST_INFO( (int)rgbValue.red() );
//     POST_INFO( (int)rgbValue.green() );
//     POST_INFO( (int)rgbValue.blue() );
//     POST( rgbValue == rgb() );
}

RenColourHSV::~RenColourHSV()
{
    TEST_INVARIANT;

}

RenColour RenColourHSV::rgb( void ) const
{
    double  red = 0;
    double  green = 0;
    double  blue = 0;

    if( saturation() == 0.0 )
    {
        ASSERT( not hueDefined(), "" );

        red = value();
        green = value();
        blue = value();
    }
    else
    {
        double h = hue();

        if( h == 360.0 )
            h = 0.0;

        h /= 60.0;
        int i = floor( h );
        double f = h - i;
        double p = value() * ( 1.0 - saturation() );
        double q = value() * ( 1.0 - ( saturation() * f ) );
        double t = value() * ( 1.0 - ( saturation() * ( 1 - f ) ) );

        switch( i )
        {
            case 0:
                red = value();
                green = t;
                blue = p;
                break;
            case 1:
                red = q;
                green = value();
                blue = p;
                break;
            case 2:
                red = p;
                green = value();
                blue = t;
                break;
            case 3:
                red = p;
                green = q;
                blue = value();
                break;
            case 4:
                red = t;
                green = p;
                blue = value();
                break;
            case 5:
                red = value();
                green = p;
                blue = q;
                break;
        }
    }

    RenColour result( red, green, blue );

    return result;
}

bool    RenColourHSV::hueDefined( void ) const
{
    return hueDefined_;
}

double    RenColourHSV::hue( void ) const
{
    PRE( hueDefined() );

    double result = hue_;

    POST( 0.0 <= result and result <= 360.0 );

    return result;
}

double    RenColourHSV::saturation( void ) const
{
    return saturation_;
}

double    RenColourHSV::value( void ) const
{
    return value_;
}

void    RenColourHSV::hue( double newHue )
{
    hueDefined_ = true;

    hue_ = newHue;

    while( hue_ < 0.0 )
        hue_ += 360.0;

    while( hue_ > 360.0 )
        hue_ -= 360.0;

    POST( hue_ >= 0.0 );
    POST( hue_ <= 360.0 );
}

void    RenColourHSV::saturation( double newSaturation )
{
    PRE( newSaturation >= 0.0 );
    PRE( newSaturation <= 1.0 );

    saturation_ = newSaturation;

    if( saturation_ == 0.0 )
        hueDefined_ = false;

    POST( saturation_ >= 0.0 );
    POST( saturation_ <= 1.0 );
}

void    RenColourHSV::value( double newValue )
{
    PRE( newValue >= 0.0 );
    PRE( newValue <= 1.0 );

    value_ = newValue;

    POST( value_ >= 0.0 );
    POST( value_ <= 1.0 );
}

void RenColourHSV::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const RenColourHSV& t )
{
    o << "H(";
    if( t.hueDefined() )
        o << t.hue();
    o << ") ";

    o << "S(" << t.saturation() << ") ";
    o << "V(" << t.value() << ")";

    return o;
}

/* End RenColourHSV.CPP ******************************************************/
