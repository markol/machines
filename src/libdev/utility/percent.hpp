
#ifndef _UTILITY_PERCENT_HPP
#define _UTILITY_PERCENT_HPP

#include "utility/subrange.hpp"

//////////////////////////////////////////////////////////////////////

class UtlPercentage
: private UtlSubrange< double >
{
public:
    typedef UtlSubrange<double> Base;

    UtlPercentage( const Value& value )

    : UtlSubrange<double>(Range(0.0, 100.0))
    {
        // Assertions are in value().
        Base::value(value);
    }

    using Base::operator const Value&;
    using Base::value;
    using Base::range;

    using Base::isValid;
    using Base::length;
} ;

//////////////////////////////////////////////////////////////////////

#endif // #ifndef _UTILITY_PERCENT_HPP

/* End PERCENT.HPP **************************************************/

