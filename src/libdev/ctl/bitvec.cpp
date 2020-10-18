/*
 * B I T V E C . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */


/* START BITVEC.CPP ***************************************************/

#include "ctl/bitvec.hpp"

#ifndef _INLINE
	#include "ctl/bitvec.ipp"
#endif

#include <cstring>

ostream& operator << ( ostream& o, ctl_bit_vector& b )
{
    for( size_t i=0; i< b.nBlocks_; i++ )
    {
        unsigned long j=1L;
        for( unsigned k=0; j; j<<=1, k++ )
            if( b.vec_[ i ] & j )
                o << '\t' << k + ( i * ctl_bit_vector::ELEM_BIT ) << std::endl;
    }

	return o;
}

bool operator ==( const ctl_bit_vector& a, const ctl_bit_vector& b )
{
	return 	a.size() == b.size()
			and
			memcmp( a.vec_, b.vec_, a.nBlocks_ * sizeof( unsigned long ) ) == 0;
}

void ctl_bit_vector::CLASS_INVARIANT
{
	INVARIANT( size_ > 0 );
    INVARIANT( nBlocks_ > 0 );
    INVARIANT( nBlocks_ == nBlocksRequired( size() ) );
}

////////////////////////////////////////////////////////////////////////
//
// Class ctl_bit_vector
//
////////////////////////////////////////////////////////////////////////

ctl_bit_vector::ctl_bit_vector( size_type sz )
: size_( sz )
{
	PRE( sz != 0 );

   	nBlocks_ = nBlocksRequired( size_ );

    vec_ = _NEW_ARRAY( unsigned long, nBlocks_ );
	clear();

	TEST_INVARIANT;
}

ctl_bit_vector::ctl_bit_vector( const string& s )
:	size_( s.length() )
{
	PRE( size() == s.length() );

	nBlocks_ = nBlocksRequired( size_ );

	vec_ = _NEW_ARRAY( unsigned long, nBlocks_ );

	clear();

	// set up the values in the string
	for( size_t i = 0; i < size(); ++i )
	{
		ASSERT( s[ i ] == '1' or s[ i ] == '0', "Input characters must be either a 0 or a 1" );

		if( s[ i ] == '1' )
			(*this)[ i ].flip();
	}

	TEST_INVARIANT;
}

ctl_bit_vector::ctl_bit_vector( const ctl_bit_vector& b )
: size_( b.size() ),
  nBlocks_( b.nBlocks_ )
{
    vec_ = _NEW_ARRAY( unsigned long, nBlocks_ );
	memcpy( vec_, b.vec_, sizeof( unsigned long ) * nBlocks_ );

	TEST_INVARIANT;
}

void ctl_bit_vector::clear()
{
	TEST_INVARIANT;
	// sets all bits to zero
	memset( vec_, 0, sizeof( unsigned long ) * nBlocks_ );

	TEST_INVARIANT;
}

ctl_bit_vector& ctl_bit_vector::operator =( const ctl_bit_vector& b )
{
	TEST_INVARIANT;
    if( this != &b )
	{
		nBlocks_ = b.nBlocks_;
		unsigned long * newVec = _NEW_ARRAY( unsigned long, nBlocks_ );
		_DELETE_ARRAY( vec_ );
    	vec_ = newVec;
		memcpy( vec_, b.vec_, sizeof( unsigned long ) * nBlocks_ );
	}

	TEST_INVARIANT;
    return *this;
}


ctl_bit_vector::size_type
ctl_bit_vector::nBlocksRequired( size_t szInBits )
{
	size_t result = szInBits / ELEM_BIT;
	if( ( szInBits % ELEM_BIT ) != 0 )
		++result;

	return result;
}

////////////////////////////////////////////////////////////////////////
//
// Class Bit
//
////////////////////////////////////////////////////////////////////////

// see bitvec.ipp


/* END BITVEC.CPP *****************************************************/
