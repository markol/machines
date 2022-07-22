/*
 * I N T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

/*
 *  MexInt
 *  A class which behaves like an int except for
 *  allowing two cardinal values, infinity and
 *  -infinity. As many operations as are meaningful
 *  are implemented on these values.
 */

#ifndef _MATHEX_INT_HPP
#define _MATHEX_INT_HPP

#include "base/base.hpp"

//////////////////////////////////////////////////////////////////////

class MexInt
// bitwise cannonical
{
public:

	MexInt( int d  = 0 );
	
	bool isPlusInfinity() const;
	bool isMinusInfinity() const;

	bool isInfinite() const;
	// POST( iff( result, isPlusInfinity() or isMinusInfinity() ) );

	operator int() const;
	// PRE( not isInfinite() );

	MexInt operator -() const;

	const MexInt& operator +=( const MexInt& );
	const MexInt& operator -=( const MexInt& );
	const MexInt& operator *=( const MexInt& );
	const MexInt& operator /=( const MexInt& );
	const MexInt& operator %=( const MexInt& );

	static const MexInt& plusInfinity();
	static const MexInt& minusInfinity();

	friend ostream& operator <<( ostream&, const MexInt& );

private:
	
	enum Cardinality
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

	static Classification classification( const MexInt& a, const MexInt& b );

	MexInt( Cardinality );

	int		value_;
	Cardinality	cardinality_;

	friend bool operator  <( const MexInt&, const MexInt& );
	friend bool operator ==( const MexInt&, const MexInt& );
    friend MexInt sqrt( const MexInt& d );
};

//////////////////////////////////////////////////////////////////////

MexInt operator  +( const MexInt&, const MexInt& );
MexInt operator  -( const MexInt&, const MexInt& );
MexInt operator  *( const MexInt&, const MexInt& );
MexInt operator  /( const MexInt&, const MexInt& );
MexInt operator  %( const MexInt&, const MexInt& );

//////////////////////////////////////////////////////////////////////

#include "mathex/int.ipp"

#endif	//	#ifndef _MATHEX_INT_HPP

/* End INT.HPP ******************************************************/
