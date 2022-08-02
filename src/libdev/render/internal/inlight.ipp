/*
 * I N L I G H T . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
void RenILight::illuminator(RenIIlluminator* i)
{
	// A light should never be associated with more than one illuminator.
	PRE(implies(i, !illuminator_));
	illuminator_ = i;
}

_CODE_INLINE
bool RenILight::on() const
{
	return on_;
}

/* End INLIGHT.IPP **************************************************/
