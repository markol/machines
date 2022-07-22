/*
 * B I T V E C . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 *
 *
 *  Classes:    ctl_bit_vector  denotes a vector of bits.
 *
 *
 *              Bit             References a bit within ctl_bit_vector.
 *
 *
 *
 *	Info:		constructors
 *				1)  an integer denoting the the number of bits to be stored.
 *					all bits are initialised to zero.
 *
 *				2)	a string consisting of ASCII chars which can be either a
 *					'1' or a '0'.   the size of the ctl_bit_vector will be
 *					represented by the length of the string.    the MSB of
 *					the string represents the first element of the ctl_bit_vector
 */

/* START BITVEC.HPP ***************************************************/

#ifndef _CTL_BITVEC_HPP
#define _CTL_BITVEC_HPP

#include "base/base.hpp"

#include <limits.h>
#include "stdlib/string.hpp"


/////////////////////////////////////////////////////////////////////

// forward refs


////////////////////////////////////////////////////////////////////////
//
// Class ctl_bit_vector
//
////////////////////////////////////////////////////////////////////////

class ctl_bit_vector
{
public:

	typedef size_t size_type;

	ctl_bit_vector( size_type sz );
	// PRE( sz != 0 );

	ctl_bit_vector( const string& s );
	// PRE( s[] == '1' or s[] == '0' );
	// POST( s.length() == size() )

    ctl_bit_vector( const ctl_bit_vector& b );

	~ctl_bit_vector();

	void clear();
	// sets all bits to zero

	ctl_bit_vector& operator =( const ctl_bit_vector& b );

	///////////////////////////////
	class Bit
	{

	public:

		~Bit();

		operator bool() const;

		void flip();

		Bit& operator =( bool b );

	private:

		Bit( ctl_bit_vector& b, size_type blockIndex, unsigned bitIndex );

		ctl_bit_vector&	vec_;
		size_type		blockIndex_;
		unsigned		bitIndex_;

		unsigned long	mask_;

        friend class    ctl_bit_vector;
	};


	///////////////////////////////////////////
	Bit         operator [] ( size_type i );
    //	PRE( i < size() );
	const Bit   operator [] ( size_type i ) const;
    //	PRE( i < size() );

	size_type size() const;

	friend ostream& operator << ( ostream& o, ctl_bit_vector& b );
	friend bool operator == ( const ctl_bit_vector& a, const ctl_bit_vector& b );

    void CLASS_INVARIANT;

private:

    // Operations revoked
	ctl_bit_vector();

	/////////////////////////////////////////////////
	static size_type nBlocksRequired( size_type sz );

	enum { ELEM_BIT = sizeof( unsigned long ) * CHAR_BIT };

	size_t  size_;
	size_t  nBlocks_;
	unsigned long *	vec_;

    friend  class Bit;
};


#ifdef _INLINE
    #include "ctl/bitvec.ipp"
#endif


#endif

/* End BITVEC.HPP ***************************************************/

