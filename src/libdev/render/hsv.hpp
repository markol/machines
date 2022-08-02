/*
 * H S V . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


/*
    RenColourHSV

    Implements the hue, saturation, value (HSV) colour model (this is also
    known as the hue, saturation and brightness colour model (HSB) ) and
    allows conversion to and from the RGB colour model.
*/

#ifndef _REN_RENCOLOURHSV_HPP
#define _REN_RENCOLOURHSV_HPP

#include "base/base.hpp"

class RenColour;

class RenColourHSV
{
public:
    RenColourHSV();
    ~RenColourHSV();

    RenColourHSV( const RenColour& rgb );

    RenColour rgb() const;

    bool    hueDefined() const;
    double  hue() const;
    // PRE( hueDefined );
    // POST( 0.0 <= result and result <= 360.0 );
    
    double  saturation() const;
    double  value() const;

    void    hue( double newHue );
    void    saturation( double newSaturation );
    void    value( double newValue );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const RenColourHSV& t );

private:
    // Operation deliberately revoked
    RenColourHSV( const RenColourHSV& );

    // Operation deliberately revoked
    RenColourHSV& operator =( const RenColourHSV& );

    // Operation deliberately revoked
    bool operator ==( const RenColourHSV& );

    bool    hueDefined_;
    double  hue_;
    double  saturation_;
    double  value_;
};


#endif

/* End RenColourHSV.HPP ******************************************************/
