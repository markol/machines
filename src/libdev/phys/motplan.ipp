/*
 * M O T P L A N . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#include "phys/phys.hpp"

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
PhysMotionPlan::PhysMotionPlan( const PhysRelativeTime& duration )
: duration_( duration )
{
	PRE( duration >= 0.0 );
	TEST_INVARIANT
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const PhysRelativeTime& PhysMotionPlan::duration( void ) const
{
	return duration_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
bool PhysMotionPlan::isDone( const PhysRelativeTime& timeOffset ) const
{
	return timeOffset >= duration_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
void PhysMotionPlan::duration( const PhysRelativeTime& newDuration)
{
	PRE( newDuration >= 0.0 );
	duration_ = newDuration;
	TEST_INVARIANT
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End MOTPLAN.IPP **************************************************/
