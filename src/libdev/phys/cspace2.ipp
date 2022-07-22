/*
 * C S P A C E 2 . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const PhysCS2dImpl& PhysConfigSpace2d::impl( void ) const
{
    return impl_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dImpl* PhysConfigSpace2d::pImpl( void )
{
    return &impl_;
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End CSPACE2.IPP **************************************************/
