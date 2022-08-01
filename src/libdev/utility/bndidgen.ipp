/*
 * B N D I D G E N . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
UtlId UtlBoundedIdGenerator::upperBound() const
{
    return upperBound_;
}

_CODE_INLINE
ulong UtlBoundedIdGenerator::nUnusedIds() const
{
    return nUnused_;
}

_CODE_INLINE
bool UtlBoundedIdGenerator::isAllocated( UtlId id ) const
{
    PRE( id < upperBound() );

    return aFlags_[ id ] != 0;
}


/* End BNDIDGEN.IPP *************************************************/
