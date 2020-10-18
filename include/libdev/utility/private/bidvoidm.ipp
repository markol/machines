/*
 * B I D V O I D M . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
UtlId UtlBoundedIdVoidPMap::upperBound() const
{
    return upperBound_;
}

_CODE_INLINE
bool UtlBoundedIdVoidPMap::contains( UtlId id ) const
{
    PRE( id < upperBound_ );
    return aPointers_[id] != NULL;
}

_CODE_INLINE
void* UtlBoundedIdVoidPMap::entry( UtlId id ) const
{
    PRE( contains( id ) );
    return aPointers_[id];
}
/* End BIDVOIDM.IPP *************************************************/
