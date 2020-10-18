/*
 * M A T E R I A L . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

#include "base/lessptr.hpp"

_CODE_INLINE
bool operator< (const RenMaterial& m1, const RenMaterial& m2)
{
	return less_ptr<RenIMatBody>()(m1.body_, m2.body_);
}

_CODE_INLINE
bool operator== (const RenMaterial& m1, const RenMaterial& m2)
{
	return m1.body_ == m2.body_;
}

/* End MATERIAL.IPP *************************************************/
