/*
 * D I S E V E N T . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#include "phys/phys.hpp"

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

/////////////////////////////////////////////////////////////////////////////////////////

_CODE_INLINE
const PhysAbsoluteTime& SimDiscreteEvent::time() const
{
	return time_;
}
/////////////////////////////////////////////////////////////////////////////////////////

_CODE_INLINE
void SimDiscreteEvent::diary( SimEventDiary* pDiary )
{
	//Mustn't already be in a diary except the new one
	PRE( (pDiary == NULL) or (pDiary_ == NULL) or (pDiary_ == pDiary) );

	pDiary_ = pDiary;
}
/////////////////////////////////////////////////////////////////////////////////////////

_CODE_INLINE
SimEventDiary* SimDiscreteEvent::diary( void ) const
{
	return pDiary_;
}
/////////////////////////////////////////////////////////////////////////////////////////
/* End DISEVENT.IPP *************************************************/
