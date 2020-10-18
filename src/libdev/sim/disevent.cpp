/*
 * D I S E V E N T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "sim/disevent.hpp"
#include "sim/evdiary.hpp"

#ifndef _INLINE
    #include "sim/disevent.ipp"
#endif

PER_DEFINE_PERSISTENT_ABSTRACT( SimDiscreteEvent );

/////////////////////////////////////////////////////////////////////////////////////////

SimDiscreteEvent::SimDiscreteEvent( const PhysAbsoluteTime& time )
:   time_( time ),
    pDiary_( NULL )
{
    TEST_INVARIANT;
}
/////////////////////////////////////////////////////////////////////////////////////////

SimDiscreteEvent::~SimDiscreteEvent()
{
    TEST_INVARIANT;

    //Since deletion occurs when reference count reaches zero,
    // this must not be in a diary
    PRE( pDiary_ == NULL );
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimDiscreteEvent::time( const PhysAbsoluteTime& time )
{
    //Change the time
    time_ = time;

    //Notify the diary, if any
    if (pDiary_)
        pDiary_->eventTimeChanged();
}
/////////////////////////////////////////////////////////////////////////////////////////

void SimDiscreteEvent::CLASS_INVARIANT
{
	INVARIANT( this );
}
/////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const SimDiscreteEvent& t )
{

    o << "SimDiscreteEvent " << (void*)&t << " start" << std::endl;
    t.doOutputOperator( o );
    o << "SimDiscreteEvent " << (void*)&t << " end" << std::endl;

    return o;
}
/////////////////////////////////////////////////////////////////////////////////////////

//virtual
void SimDiscreteEvent::doOutputOperator( ostream& ) const
{
    //Default implementation does nothing
}

void perWrite( PerOstream& ostr, const SimDiscreteEvent& event )
{
	ostr << event.time_;
}

void perRead( PerIstream& istr, SimDiscreteEvent& event )
{
    istr >> event.time_;
}

SimDiscreteEvent::SimDiscreteEvent( PerConstructor )
{
}


/* End DISEVENT.CPP *************************************************/
