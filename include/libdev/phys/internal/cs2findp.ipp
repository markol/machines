/*
 * C S 2 F I N D P . I P P 
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
bool PhysCS2dFindPath::isFinished( void ) const
{
    return state_ == FINISHED;
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End CS2FINDP.IPP *************************************************/
