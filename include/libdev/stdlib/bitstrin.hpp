/*
 * B I T S T R I N . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved.
 */

#ifndef _STDLIB_BITSTRIN_HPP
#define _STDLIB_BITSTRIN_HPP

#include <stddef.h>

//////////////////////////////////////////////////////////////////////

class bitstring
{
public:

	bitstring();
	bitstring( unsigned long val, size_t n );
	bitstring( const bitstring&, size_t pos = 0, size_t n = NPOS );
	bitstring( const char *, size_t pos = 0, size_t n = NPOS );

	bitstring& operator +=( const bitstring& );

	bitstring& operator &=( const bitstring& );
	bitstring& operator |=( const bitstring& );
	bitstring& operator ^=( const bitstring& );

	bitstring& operator <<=( size_t );
	bitstring& operator >>=( size_t );

	bitstring& append( const bitstring&, size_t pos = 0, size_t n = NPOS );
	bitstring& assign( const bitstring&, size_t pos = 0, size_t n = NPOS );

	bitstring& insert( size_t pos1, const bitstring&, size_t pos2 = 0, size_t n = NPOS );

	bitstring& remove( size_t pos = 0, size_t n = NPOS );

	bitstring& replace( size_t pos1, size_t n1,
						const bitstring&,
						size_t pos2 = 0, size_t n = NPOS );

	bitstring& set();
	bitstring& set( size_t pos, int val = 1 );

	bitstring& reset();
	bitstring& reset( size_t pos );

	bitstring& toggle();
	bitstring& toggle( size_t pos );

	string to_string() const;

	size_t count() const;
	size_t length() const;

	size_t resize( size_t n, int val = 0 );

	size_t trim();

	size_t find( int val, size_t pos = 0, size_t n = NPOS ) const;
	size_t rfind( int val, size_t pos = 0, size_t n = NPOS ) const;

	bitstring substr( size_t pos, size_t n = NPOS ) const;

	int operator ==( const bitstring& ) const;
	
	int test( size_t pos ) const;

	int any() const;
	int none() const;

	bitstring operator <<( size_t pos ) const;
	bitstring operator >>( size_t pos ) const;

	bitstring operator ~() const;

private:

};

//////////////////////////////////////////////////////////////////////

#endif	/*	#ifndef _STDLIB_BITSTRING_HPP	*/
