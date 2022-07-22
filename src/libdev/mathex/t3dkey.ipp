/*
 * T 3 D K E Y . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

//---------------------------------------------------------------------
_CODE_INLINE
MexTransform3dKey::MexTransform3dKey( void )
{
	key_ = ++lastKey_;
}
//---------------------------------------------------------------------
//---------------------------------------------------------------------
_CODE_INLINE
MexTransform3dKey::MexTransform3dKey( const MexTransform3dKey& other)
{
	key_ = other.key_;
}
//---------------------------------------------------------------------
//---------------------------------------------------------------------
_CODE_INLINE
MexTransform3dKey& MexTransform3dKey::operator++()
{
	//Increment to next unique key
	key_ = ++lastKey_;
	return *this;
}
//---------------------------------------------------------------------
//---------------------------------------------------------------------
_CODE_INLINE
MexTransform3dKey& MexTransform3dKey::operator=( const MexTransform3dKey& rhs)
{
	key_ = rhs.key_;
	return *this;
}
//---------------------------------------------------------------------
//---------------------------------------------------------------------
_CODE_INLINE
bool operator==( const MexTransform3dKey& lhs, const MexTransform3dKey& rhs)
{
	return lhs.key_ == rhs.key_;
}
//---------------------------------------------------------------------
//---------------------------------------------------------------------
_CODE_INLINE
bool operator!=( const MexTransform3dKey& lhs, const MexTransform3dKey& rhs)
{
	return not (lhs.key_ == rhs.key_);
}
//---------------------------------------------------------------------
/* End T3DKEY.IPP ***************************************************/
