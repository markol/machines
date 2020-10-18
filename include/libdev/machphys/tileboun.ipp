/*
 * T I L E B O U N . I P P 
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
MachPhysTileBoundary::CellId MachPhysTileBoundary::cellId( Coordinate xCell,
												   		   Coordinate yCell ) const
{
	return (CellId)( xCell + yCell * (nXVertices_ - 1) );
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End TILEBOUN.IPP *************************************************/
