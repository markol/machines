/*
 * B I T V E C . I P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */


/* START BITVEC.IPP ***************************************************/
                            

#ifdef  _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE	/* space for GNU */
#endif

////////////////////////////////////////////////////////////////////////
//
// Class ctl_bit_vector
//
////////////////////////////////////////////////////////////////////////

_CODE_INLINE
ctl_bit_vector::~ctl_bit_vector() 
{ 
	TEST_INVARIANT;
	_DELETE_ARRAY( vec_ );
}


_CODE_INLINE
ctl_bit_vector::Bit ctl_bit_vector::operator []( size_type i )
{

	TEST_INVARIANT;

	PRE( i < size() );

    return ctl_bit_vector::Bit( *this, i / ELEM_BIT, i % ELEM_BIT );
}


_CODE_INLINE
const ctl_bit_vector::Bit ctl_bit_vector::operator []( size_type i ) const
{
	TEST_INVARIANT;

	PRE( i < size() );
   	
    ctl_bit_vector * nonConstThis = (ctl_bit_vector *)this;
    return ctl_bit_vector::Bit( *nonConstThis, i / ELEM_BIT, i % ELEM_BIT );
}


_CODE_INLINE
ctl_bit_vector::size_type ctl_bit_vector::size() const 
{ 
	TEST_INVARIANT;

	return size_; 
}


////////////////////////////////////////////////////////////////////////
//
// Class Bit
//
////////////////////////////////////////////////////////////////////////

_CODE_INLINE
ctl_bit_vector::Bit::~Bit()
{
    // intentionally empty    
}


_CODE_INLINE
ctl_bit_vector::Bit::Bit( ctl_bit_vector& b, size_type blockIndex, unsigned bitIndex )
: vec_( b ),
  blockIndex_( blockIndex ),
  bitIndex_( bitIndex )
{
	mask_ = 1L << bitIndex_;
}

_CODE_INLINE
void ctl_bit_vector::Bit::flip()
{
	vec_.vec_[ blockIndex_ ] = vec_.vec_[ blockIndex_ ] ^ mask_;
}

_CODE_INLINE
ctl_bit_vector::Bit::operator bool() const
{
	return ( ( vec_.vec_[blockIndex_] ) & mask_ ) != 0;
}

_CODE_INLINE
ctl_bit_vector::Bit&    ctl_bit_vector::Bit::operator =( bool b )
{
	b ?	vec_.vec_[ blockIndex_ ] |= mask_
	  : vec_.vec_[ blockIndex_ ] &= ~mask_;

	return *this;
}


/* END BITVEC.IPP *****************************************************/