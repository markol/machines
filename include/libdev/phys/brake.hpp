/*
 * B R A K E . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    Brake

    Apply a brake to a one dimensional movement. The movement has fixed
    upper and lower bounds and and area leading up to those bounds in which
    it will be slowed down.
*/

#ifndef _PHYS_BRAKE_HPP
#define _PHYS_BRAKE_HPP

#include "base/base.hpp"

#include "mathex/mathex.hpp"

class Brake
{
public:
    //  The bandFraction is the fraction of the distance between minValue
    //  and maxValue in which the brake should operatate
    Brake( MATHEX_SCALAR minValue, MATHEX_SCALAR maxValue, MATHEX_SCALAR brakeBandFraction );
    // PRE( maxValue >= minValue );
    // PRE( 0.0 <= brakeBandFraction and brakeBandFraction <= 1.0 );
    
    ~Brake( void );

    void setLimits( MATHEX_SCALAR minValue, MATHEX_SCALAR maxValue, MATHEX_SCALAR brakeBandFraction );
    // PRE( maxValue >= minValue );
    // PRE( 0.0 <= brakeBandFraction and brakeBandFraction <= 1.0 );

//     MATHEX_SCALAR brakeFactor( MATHEX_SCALAR currentValue, MATHEX_SCALAR desiredDeltaValue ) const;
//     // POST( 0.0 <= result and result <= 1.0 );
    
    MATHEX_SCALAR newDelta( MATHEX_SCALAR currentValue, MATHEX_SCALAR desiredDeltaValue ) const;
    // POST( minValue <= currentValue + result and currentValue + result <= maxValue );
    
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const Brake& t );

private:
    // Operation deliberately revoked
    Brake( const Brake& );

    // Operation deliberately revoked
    Brake& operator =( const Brake& );

    // Operation deliberately revoked
    bool operator ==( const Brake& );

    MATHEX_SCALAR minValue_;
    MATHEX_SCALAR maxValue_;
    MATHEX_SCALAR brakeBandWidth_;
    MATHEX_SCALAR minBandValue_;
    MATHEX_SCALAR maxBandValue_;
};


#endif

/* End BRAKE.HPP ****************************************************/
