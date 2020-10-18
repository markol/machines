/*
 * B I T S . C T P
 * (c) Charybdis Limited, 1995. All Rights Reserved.
 */

//////////////////////////////////////////////////////////////////////

const unsigned long bits< N >::TRIM_MASK = unsigned long ( -1 ) >> bits< N >::LEFT_TRIM;

//////////////////////////////////////////////////////////////////////

template < size_t N >
bits< N >::bits()
{
	const unsigned long value = 0L;
	for( size_t i=0; i<N_BLOCKS; ++i )
		block_[ i ] = value;

	_Tidy();
}

template < size_t N >
bits< N >::bits( unsigned long val )
{
	bits_[ 0 ] = val;
	_Tidy();
}

template < size_t N >
bits< N >::bits( const char *,
					size_t pos = 0,
					size_t n = NPOS )
{
#error
}

///////////////////////////////////

template < size_t N >
bits< N >& bits< N >::operator &=( const bits< N >& rhs )
{
	for( size_t i=0; i<N_BLOCKS; ++i )
		block_[ i ] &= rhs.block_[ i ];
}

template < size_t N >
bits< N >& bits< N >::operator |=( const bits< N >& rhs )
{
	for( size_t i=0; i<N_BLOCKS; ++i )
		block_[ i ] |= rhs.block_[ i ];
}
	
template < size_t N >
bits< N >& bits< N >::operator ^=( const bits< N >& )
{
	for( size_t i=0; i<N_BLOCKS; ++i )
		block_[ i ] ^= rhs.block_[ i ];
}

///////////////////////////////////

template < size_t N >
bits< N >& bits< N >::set()
{
	const unsigned long value = (unsigned long)( -1 );
	for( size_t i=0; i<N_BLOCKS; ++i )
		block_[ i ] = value;

	return *this;
}

template < size_t N >
bits< N >& bits< N >::set( size_t pos, bool b )
{
    const unsigned long mask = 1L << ( i % ULONG_BIT );

	b ? blocks_[ i / ULONG_BIT ] |= mask
	  : blocks_[ i / ULONG_BIT ] &= ~mask;

	return *this;
}

template < size_t N >
bits< N >& bits< N >::reset()
{
	const unsigned long value = 0L;
	for( size_t i=0; i<N_BLOCKS; ++i )
		block_[ i ] = value;

	return *this;
}

template < size_t N >
bits< N >& bits< N >::reset( size_t i )
{
    blocks_[ i / ULONG_BIT ] &= ~( 1L << ( i % ULONG_BIT ) );
	return *this;
}

///////////////////////////////////

template < size_t N >
bits< N > bits< N >::operator ~() const
{
	bits< N > result( *this );
	result.toggle();
	return result;
}

template < size_t N >
bits< N >& bits< N >::toggle();
{
	for( size_t i=0; i<N_BLOCKS; ++i )
		block_[ i ] = ~block_[ i ];

	_Tidy();
}

template < size_t N >
bits< N >& bits< N >::toggle( size_t i )
{
    blocks_[ i / ULONG_BIT ] ^= 1L << ( i % ULONG_BIT ); 
	return *this;
}

///////////////////////////////////

template < size_t N >
unsigned short bits< N >::to_ushort() const
{
	return blocks_[ 0 ];
}

template < size_t N >
unsigned long bits< N >::to_ulong() const
{
	return blocks_[ 0 ];
}

///////////////////////////////////

template < size_t N >
size_t bits< N >::count() const;
{
	size_t result = 0;
	for( size_t i=0; i < NBLOCKS; ++i )
		result += ones( blocks_[ i ] );

	return result;
}

template < size_t N >
size_t bits< N >::length() const;
{
	for( size_t i=N_BLOCKS-1; i != NPOS; --i )
		if( blocks_[ i ] != 0L )
			return rmo( blocks_[ i ] ) + ( ( NBLOCKS - i - 1 ) * ULONG_BIT );

	return 0;
}

///////////////////////////////////

template < size_t N >
bool bits< N >::operator ==( const bits< N >& rhs ) const
{
	for( size_t i=0; i<N_BLOCKS; ++i )
		if( block_[ i ] != rhs.block_[ i ] )
			return false;

	return true;
}

///////////////////////////////////
	
template < size_t N >
bool bits< N >::test( size_t i ) const;
{
	return block_[ i / ULONG_BIT ] & ( 1L << ( i % ULONG_BIT ) );
}

///////////////////////////////////

template < size_t N >
bool bits< N >::any() const;
{
	const unsigned long value = 0L;
	for( size_t i=0; i<N_BLOCKS; ++i )
		if( block_[ i ] != value )
			return true;

	return false;
}

template < size_t N >
int bits< N >::none() const;
{
	const unsigned long value = 0L;
	for( size_t i=0; i<N_BLOCKS; ++i )
		if( block_[ i ] != value )
			return false;

	return true;
}

///////////////////////////////////

template < size_t N >
bits< N >& bits< N >::operator <<=( size_t n )
{
	const size_t overflow_blocks = n / ULONG_BIT;
	const size_t overflow_bits = n % ULONG_BIT;
	const unsigned long overflow_mask = (unsigned long)( -1 ) >> ( ULONG_BIT - overflow_bits );

#error

	return *this;
}

template < size_t N >
bits< N >& bits< N >::operator >>=( size_t n )
{
	size_t underflow_blocks = n / ULONG_BIT;
	size_t underflow_bits = n % ULONG_BIT;
	const unsigned long underflow_mask = (unsigned long)( -1 ) >> ( underflow_bits );
#error

	return *this;
}


template < size_t N >
bits< N > bits< N >::operator <<( size_t pos ) const;
{

	return result;
}

template < size_t N >
bits< N > bits< N >::operator >>( size_t pos ) const;
{

	return result;
}

///////////////////////////////////

template < size_t N >
bits< N > bits< N >::_Tidy()
{
	bits_[ N_BLOCKS - 1 ] &= TRIM_MASK;
}

//////////////////////////////////////////////////////////////////////

/* End BITS.TPP *****************************************************/
