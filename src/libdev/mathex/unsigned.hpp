/*
 * U N S I G N E D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

/*
 *  MexUnsigned
 *  A class which behaves like an unsigned except for
 *  allowing the cardinal value +infinity. 
 *  As many operations as are meaningful are implemented.
 */

#ifndef _MATHEX_UNSIGNED_HPP
#define _MATHEX_UNSIGNED_HPP

#include <math.h>
#include "base/base.hpp"

//////////////////////////////////////////////////////////////////////

class MexUnsigned
// bitwise cannonical
{
public:

	MexUnsigned( unsigned d  = 0 );
	
	bool isInfinite() const;

	operator unsigned() const;
	// PRE( not isInfinite() );

	MexUnsigned operator -() const;
	// PRE( not isInfinite() );

	const MexUnsigned& operator +=( const MexUnsigned& );
	const MexUnsigned& operator -=( const MexUnsigned& );
	const MexUnsigned& operator *=( const MexUnsigned& );
	const MexUnsigned& operator /=( const MexUnsigned& );
	const MexUnsigned& operator %=( const MexUnsigned& );

	static const MexUnsigned& infinity();

	friend ostream& operator <<( ostream&, const MexUnsigned& );

private:
	
	enum Cardinality
	{
		FINITE			= 0x0000, 
		PLUS_INFINITY 	= 0x0001
	};

	enum { CARD_BITS = 1 };

	enum Classification
	{ 
		A_FINITE_B_FINITE		= ( FINITE << CARD_BITS ) | FINITE,
		A_FINITE_B_PLUS_INF		= ( FINITE << CARD_BITS ) | PLUS_INFINITY,
		A_PLUS_INF_B_FINITE		= ( PLUS_INFINITY << CARD_BITS ) | FINITE,
		A_PLUS_INF_B_PLUS_INF	= ( PLUS_INFINITY << CARD_BITS ) | PLUS_INFINITY
	};

	static Classification classification( const MexUnsigned& a, const MexUnsigned& b );

	MexUnsigned( Cardinality );

	unsigned		value_;
	Cardinality		cardinality_;

	friend bool operator  <( const MexUnsigned&, const MexUnsigned& );
	friend bool operator ==( const MexUnsigned&, const MexUnsigned& );
    friend MexUnsigned sqrt( const MexUnsigned& d );
};

//////////////////////////////////////////////////////////////////////

MexUnsigned operator  +( const MexUnsigned&, const MexUnsigned& );
MexUnsigned operator  -( const MexUnsigned&, const MexUnsigned& );
MexUnsigned operator  *( const MexUnsigned&, const MexUnsigned& );
MexUnsigned operator  /( const MexUnsigned&, const MexUnsigned& );
MexUnsigned operator  %( const MexUnsigned&, const MexUnsigned& );

//////////////////////////////////////////////////////////////////////

#include "mathex/unsigned.ipp"

#endif	//	#ifndef _MATHEX_UNSIGNED_HPP

/* End UNSIGNED.HPP *************************************************/
