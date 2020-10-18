/*
 * S C A L P L A N . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
const PhysRelativeTime& W4dScalePlan::duration() const
{
    return duration_;
}

/* End SCALPLAN.IPP *************************************************/
