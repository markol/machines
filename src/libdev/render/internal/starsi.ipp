/***********************************************************

  S T A R S I . I P P
  (c) Charybdis Limited, 1998. All Rights Reserved

***********************************************************/

#ifdef _INLINE
	#define _CODE_INLINE inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
RenStars::Configuration RenIStarsImpl
::configuration() const
{
	return configuration_;
}

_CODE_INLINE
uint RenIStarsImpl::nStars() const
{
	return nStars_;
}

_CODE_INLINE
MATHEX_SCALAR RenIStarsImpl::radius() const
{
	return radius_;
}

/* STARS.IPP **********************************************/

