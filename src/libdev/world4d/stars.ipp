/***********************************************************

  S T A R S . I P P
  (c) Charybdis Limited, 1998. All Rights Reserved

***********************************************************/

#ifdef _INLINE
	#define _CODE_INLINE inline
#else
	#define _CODE_INLINE
#endif

#include "mathex/angle.hpp"

_CODE_INLINE
RenStars::Configuration W4dStars::configuration() const
{
	TEST_INVARIANT;

	return pStars_->configuration();
}

_CODE_INLINE
MATHEX_SCALAR W4dStars::radius( void ) const
{
	TEST_INVARIANT;

	return pStars_->radius();
}

_CODE_INLINE
uint W4dStars::nStars( void ) const
{
	TEST_INVARIANT;

	return pStars_->nStars();
}

_CODE_INLINE
UtlPercentage W4dStars::opacity( void ) const
{
	return opacity_;
}

_CODE_INLINE
void W4dStars::opacity(UtlPercentage newOpacity)
{
	opacity_ = newOpacity;
}

/* End STARS.HPP ******************************************/

