/*
 * D O U B L E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

/*
    MexDouble

    A class which behaves like a double except for
    allowing two cardinal values, infinity and
    -infinity. As many operations as are meaningful
    are implemented on these values.
*/

#ifndef _MATHEX_DOUBLE_HPP
#define _MATHEX_DOUBLE_HPP

#include "base/persist.hpp"
#include "base/base.hpp"

/* //////////////////////////////////////////////////////////////// */

class MexDouble
// bitwise cannonical
{
public:

	MexDouble( double d  = 0.0 );

	bool isPlusInfinity() const;
	bool isMinusInfinity() const;

	bool isInfinite() const;
	// POST( iff( result, isPlusInfinity() or isMinusInfinity() ) );

	operator double() const;
	// PRE( not isInfinite() );

    MexDouble operator -() const;

	const MexDouble& operator +=( const MexDouble& );
	const MexDouble& operator -=( const MexDouble& );
	const MexDouble& operator *=( const MexDouble& );
	const MexDouble& operator /=( const MexDouble& );
	const MexDouble& operator %=( const MexDouble& );

	static const MexDouble& plusInfinity();
	static const MexDouble& minusInfinity();

	friend ostream& operator <<( ostream&, const MexDouble& );

    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT( MexDouble );
    PER_FRIEND_READ_WRITE( MexDouble );

private:

	enum Cardinality : unsigned char
	{
		FINITE			= 0x0000,
		MINUS_INFINITY	= 0x0001,
		PLUS_INFINITY 	= 0x0002
	};

	enum { CARD_BITS = 2 };

	enum Classification
	{
		A_MINUS_INF_B_MINUS_INF	= ( MINUS_INFINITY << CARD_BITS ) | MINUS_INFINITY,
		A_MINUS_INF_B_FINITE	= ( MINUS_INFINITY << CARD_BITS ) | FINITE,
		A_MINUS_INF_B_PLUS_INF  = ( MINUS_INFINITY << CARD_BITS ) | PLUS_INFINITY,
		A_FINITE_B_MINUS_INF	= ( FINITE << CARD_BITS ) | MINUS_INFINITY,
		A_FINITE_B_FINITE		= ( FINITE << CARD_BITS ) | FINITE,
		A_FINITE_B_PLUS_INF		= ( FINITE << CARD_BITS ) | PLUS_INFINITY,
		A_PLUS_INF_B_MINUS_INF	= ( PLUS_INFINITY << CARD_BITS ) | MINUS_INFINITY,
		A_PLUS_INF_B_FINITE		= ( PLUS_INFINITY << CARD_BITS ) | FINITE,
		A_PLUS_INF_B_PLUS_INF	= ( PLUS_INFINITY << CARD_BITS ) | PLUS_INFINITY
	};

	static Classification classification( const MexDouble& a, const MexDouble& b );

	MexDouble( Cardinality );

	// important
	#pragma pack(push,1)
    struct  IDouble
    {
        IDouble( double d, Cardinality c ) : value_( d ), cardinality_( c ) { }

    	double		value_;
	    Cardinality	cardinality_;
    };
    #pragma pack(pop)

    IDouble rep_;

	friend bool operator  <( const MexDouble&, const MexDouble& );
	friend bool operator  >( const MexDouble&, const MexDouble& );
	friend bool operator  <=( const MexDouble&, const MexDouble& );
	friend bool operator  >=( const MexDouble&, const MexDouble& );
	friend bool operator ==( const MexDouble&, const MexDouble& );
	friend bool operator !=( const MexDouble&, const MexDouble& );
    friend MexDouble sqrt( const MexDouble& d );
};

PER_DECLARE_PERSISTENT( MexDouble );

/* //////////////////////////////////////////////////////////////// */

MexDouble operator  +( const MexDouble&, const MexDouble& );
MexDouble operator  -( const MexDouble&, const MexDouble& );
MexDouble operator  *( const MexDouble&, const MexDouble& );
MexDouble operator  /( const MexDouble&, const MexDouble& );
MexDouble operator  %( const MexDouble&, const MexDouble& );

MexDouble sqrt( const MexDouble& );

/* //////////////////////////////////////////////////////////////// */

#include "mathex/double.ipp"

#endif	//	#ifndef _MATHEX_DOUBLE_HPP

/* End DOUBLE.HPP *************************************************/
