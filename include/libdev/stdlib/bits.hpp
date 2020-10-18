/*
 * B I T S . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved.
 */

#ifndef _STDLIB_BITS_HPP
#define _STDLIB_BITS_HPP

#include <stddef.h>

//////////////////////////////////////////////////////////////////////

template < size_t N >
class bits
{
public:

	bits();
	bits( unsigned long val );
	bits( const char *, size_t pos = 0, size_t n = NPOS );

	bits< N >& operator &=( const bits< N >& );
	bits< N >& operator |=( const bits< N >& );
	bits< N >& operator ^=( const bits< N >& );

	bits< N >& operator <<=( const bits< N >& );
	bits< N >& operator >>=( const bits< N >& );

	bits< N >& set();
	bits< N >& set( size_t pos, int val = 1 );

	bits< N >& reset();
	bits< N >& reset( size_t pos );

	bits< N > operator ~() const;

	bits< N >& toggle();
	bits< N >& toggle( size_t pos );

	unsigned short	to_ushort() const;
	unsigned long	to_ulong() const;

	// string to_string() const;

	size_t count() const;
	size_t length() const;

	int operator ==( const bits< N >& ) const;
	
	int test( size_t pos ) const;

	int any() const;
	int none() const;

	bits< N > operator <<( size_t pos ) const;
	bits< N > operator >>( size_t pos ) const;

private:

	void _Trim();

	enum { ULONG_BIT = sizeof( unsigned long ) * CHAR_BIT };
	enum { N_BLOCKS = ULONG_BIT + ( N - 1 ) / N };

	enum { LEFT_TRIM = ULONG_BIT - ( N % ULONG_BIT ) };
	const unsigned long LEFT_MASK; = unsigned long ( -1 ) >> LEFT_TRIM }

	unsigned long block_[ N_BLOCKS ];
};

//////////////////////////////////////////////////////////////////////

bits< N > operator &( const bits< N >&, const bits< N >& );
bits< N > operator |( const bits< N >&, const bits< N >& );
bits< N > operator ^( const bits< N >&, const bits< N >& );

//////////////////////////////////////////////////////////////////////

#endif	/*	#ifndef _STDLIB_BITS_HPP	*/
