/*
 * C S 2 D M F N D . I P P 
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
bool PhysCS2dDomainFindPath::isFinished( void ) const
{
    return state_ == FINISHED;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
MATHEX_SCALAR PhysCS2dDomainFindPath::clearance() const
{
    return clearance_;
}

//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysCS2dDomainFindPath::ObstacleFlags   PhysCS2dDomainFindPath::flags() const
{
    return flags_;
}

//////////////////////////////////////////////////////////////////////////////////////////
/* End CS2DMFND.IPP *************************************************/
