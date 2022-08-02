/*
 * E V D I A R Y . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

/////////////////////////////////////////////////////////////////////////////////////////

_CODE_INLINE
bool SimEventDiary::EventOrder::operator ()( const SimDiscreteEventPtr& lhs, const SimDiscreteEventPtr& rhs) const
{
	return lhs->time() < rhs->time();
}
/////////////////////////////////////////////////////////////////////////////////////////

_CODE_INLINE
PhysAbsoluteTime SimEventDiary::nextEventTime() const
{
	//Check the diary is not empty
	PRE( not isEmpty() );

	//Ensure in order
	validateOrder();

	//Return the time of the first event
	return events_.front()->time();
}
/////////////////////////////////////////////////////////////////////////////////////////

_CODE_INLINE
bool SimEventDiary::isEmpty() const
{
	return events_.empty();
}
/////////////////////////////////////////////////////////////////////////////////////////

_CODE_INLINE
void SimEventDiary::eventTimeChanged()
{
	//Just set flag to indicate no longer sorted
	eventsSorted_ = false;
}
/////////////////////////////////////////////////////////////////////////////////////////

_CODE_INLINE
void SimEventDiary::clear()
{
    while( not isEmpty() )
    {
        SimDiscreteEventPtr event = events_.back();
        events_.pop_back();
        event->diary( NULL );
    }
    
    POST( isEmpty() );
}

/////////////////////////////////////////////////////////////////////////////////////////
/* End EVDIARY.IPP **************************************************/
