/*
 * S U B R A N G E . H P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#ifndef _UTL_SUBRANGE_HPP
#define _UTL_SUBRANGE_HPP

#include "base/base.hpp"

#include "utility/interval.hpp"
#include "utility/constrai.hpp"

//////////////////////////////////////////////////////////////////////

template < class NUMERIC >
class UtlSubrange
: private UtlTypeBase< NUMERIC >
// memberwise cannonical according as NUMERIC
{
public:

    typedef NUMERIC Value;
    typedef UtlOpenInterval< Value > Range;

    UtlSubrange( const Range& range )
    : UtlTypeBase< NUMERIC >::UtlTypeBase( range.lower() ),
    range_( range )
    {
        PRE_INFO( UtlTypeBase< NUMERIC >::value_ );
        PRE( isValid( UtlTypeBase< NUMERIC >::value_ ) );
    }

    Range range( void ) const
    {
        return range_;
    }

    operator const Value&() const
    {
        PRE_INFO( UtlTypeBase< NUMERIC >::value_ );
        PRE( isValid( UtlTypeBase< NUMERIC >::value_ ) );
        return UtlTypeBase< NUMERIC >::value_;
    }

    void value(const Value& newValue)
    {
        PRE_INFO(newValue);
        PRE(isValid(newValue));
        UtlTypeBase< NUMERIC >::value_ = newValue;
    }

    bool isValid( const Value& t ) const
    {
        return range_.contains( t );
    }

    NUMERIC length( void ) const
    {
        return range_.length();
    }

    UtlSubrange& operator =( const Value& t )
    {
        PRE_INFO( t );
        PRE( isValid( t ) );
        UtlTypeBase< NUMERIC >::value_ = t;
        return *this;
    }

private:

    Range	range_;
} ;

//////////////////////////////////////////////////////////////////////

template < class INTEGRAL_TYPE >
class UtlIntegralSubrangeBase
// memberwise cannonical
{
public:

    typedef INTEGRAL_TYPE					value_type;
    typedef UtlIntegralSubrangeBase			Base;
    typedef UtlOpenInterval< value_type >	Range;

    UtlIntegralSubrangeBase( value_type value )
    : value_( value )
    {
        // Intentionally Empty
    }

    operator value_type() const
    {
        return value_;
    }

protected:

    value_type	value_;
} ;

//////////////////////////////////////////////////////////////////////

template < int LOWER, int UPPER >
class UtlIntSubrange
: private UtlIntegralSubrangeBase< int >
// memberwise cannonical
{
public:

    void constraints()
    {
        ASSERT_COMPILE_TIME( LOWER <= UPPER );
    }

    UtlIntSubrange()
    : Base( LOWER )
    {
        PRE( isInRange( *this ) );
    }

    UtlIntSubrange( value_type value )
    : Base( value )
    {
        PRE( isInRange( value ) );
    }

    using Base::operator value_type;

    UtlIntSubrange& operator =( value_type newValue )
    {
        PRE( isInRange( newValue ) );
        value_ = newValue;
        return *this;
    }

    static bool isInRange( value_type v )
    {
        return v >= LOWER and v <= UPPER;
    }

    static value_type length()
    {
        return UPPER - LOWER;
    };

    static Range range()
    {
        return Range( LOWER, UPPER );
    }
} ;

//////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
#define UTL_SUBRANGE( TYPE, LOWER, UPPER, VARNAME ) UtlSubrange< TYPE > VARNAME( UtlOpenInterval< TYPE >( LOWER, UPPER ) )
#else
#define UTL_SUBRANGE( TYPE, LOWER, UPPER, VARNAME ) TYPE VARNAME
#endif // #ifndef NDEBUG

#ifndef NDEBUG
#define UTL_INT_SUBRANGE( LOWER, UPPER, TYPENAME ) typedef UtlIntSubrange< LOWER, UPPER > TYPENAME
#else
#define UTL_INT_SUBRANGE( LOWER, UPPER, TYPENAME ) typedef int TYPENAME
#endif // #ifndef NDEBUG

//////////////////////////////////////////////////////////////////////

#endif /* #ifndef _UTL_SUBRANGE_HPP	*/

/* End SUBRANGE.HPP **************************************************/
