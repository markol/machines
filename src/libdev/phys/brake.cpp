/*
 * B R A K E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/utility.hpp"

#include "phys/brake.hpp"
#include "mathex/epsilon.hpp"

Brake::Brake(
    MATHEX_SCALAR minValue,
    MATHEX_SCALAR maxValue,
    MATHEX_SCALAR brakeBandFraction )
: minValue_( minValue ),
  maxValue_( maxValue ),
  brakeBandWidth_( brakeBandFraction * ( maxValue - minValue ) ),
  maxBandValue_( maxValue - brakeBandFraction * ( maxValue - minValue ) ),
  minBandValue_( minValue + brakeBandFraction * ( maxValue - minValue ) )
{
    PRE( maxValue >= minValue );
    PRE( 0.0 <= brakeBandFraction and brakeBandFraction <= 1.0 );

    TEST_INVARIANT;
}

Brake::~Brake()
{
    TEST_INVARIANT;

}

void Brake::setLimits(
    MATHEX_SCALAR minValue,
    MATHEX_SCALAR maxValue,
    MATHEX_SCALAR brakeBandFraction )
{
    TEST_INVARIANT;

    PRE( maxValue >= minValue );
    PRE( 0.0 <= brakeBandFraction and brakeBandFraction <= 1.0 );

    minValue_ = minValue;
    maxValue_ = maxValue;
    maxBandValue_ = maxValue - brakeBandFraction * ( maxValue - minValue );
    minBandValue_ = minValue + brakeBandFraction * ( maxValue - minValue );

    TEST_INVARIANT;
}

// MATHEX_SCALAR Brake::brakeFactor(
//     MATHEX_SCALAR currentValue,
//     MATHEX_SCALAR desiredDeltaValue ) const
// {
//     MATHEX_SCALAR   desiredValue = currentValue + desiredDeltaValue;
//
//     MATHEX_SCALAR   factor = 1.0;
//
//     if( desiredDeltaValue > 0 )
//     {
//         if( desiredValue > maxBandValue_ )
//         {
//             factor = 1.0 - ( desiredValue - maxBandValue_ ) / brakeBandWidth_;
//             factor = max( factor, 0.0 );
//         }
//     }
//     else
//     {
//         if( desiredValue < minBandValue_ )
//         {
//             factor = ( desiredValue - minBandValue_ ) / brakeBandWidth_;
//             factor = max( factor, 0.0 );
//         }
//     }
//
//     POST( 0.0 <= factor and factor <= 1.0 );
//
//     return factor;
// }

MATHEX_SCALAR Brake::newDelta( MATHEX_SCALAR currentValue, MATHEX_SCALAR desiredDeltaValue ) const
{
    MATHEX_SCALAR   result = desiredDeltaValue;

    //Compute a braking factor based on the current value
    MATHEX_SCALAR factor = 1.0;
    if( desiredDeltaValue > 0.0 )
    {
        if( currentValue > maxBandValue_ )
            factor = 1.0 - 0.8 * ((currentValue - maxBandValue_) / brakeBandWidth_);
    }
    else
    {
        if( currentValue < minBandValue_ )
            factor = 1.0 - 0.8 * ((minBandValue_ - currentValue) / brakeBandWidth_);
    }

    factor = std::min( factor, 1.0 );
    factor = std::max( factor, 0.2 );

    result = factor * desiredDeltaValue;

    //  Clamp to make sure we're in range
    MATHEX_SCALAR newValue = currentValue + result;
    if( newValue > maxValue_ )
        result = maxValue_ - currentValue;
    else if( newValue < minValue_ )
        result = minValue_ - currentValue;

    POST_DATA( MATHEX_SCALAR eps = MexEpsilon::instance() );
    POST_INFO( minValue_ );
    POST_INFO( maxValue_ );
    POST_INFO( currentValue );
    POST_INFO( result );
    POST( minValue_ <= currentValue + result + eps  and  currentValue + result - eps <= maxValue_ );

    return result;
}

void Brake::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const Brake& t )
{

    o << "Brake " << (void*)&t << " start" << std::endl;
    o << "Brake " << (void*)&t << " end" << std::endl;

    return o;
}

/* End BRAKE.CPP ****************************************************/
