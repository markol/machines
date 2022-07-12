/*
 * T I M E R . C P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */


#include "device/time.hpp"
#include "device/timer.hpp"

DevTimer::DevTimer( Direction direction )
: paused_( false ),
  direction_( direction ),
  offset_( DevTime::instance().time() )
{
    //  Intentionally empty
}

DevTimer::~DevTimer()
{
    //  Intentionally empty
}

double	DevTimer::time () const
{
    double  result;
    
	if ( paused_ )
        result = offset_;
    else
	{
		if ( direction_ == COUNT_UP )
			result = DevTime::instance().time() - offset_;
		else
			result = offset_ - DevTime::instance().time();
	}

	return result;

}

double	DevTimer::timeNoRecord() const
{
    double  result;
    
	if ( paused_ )
        result = offset_;
    else
	{
		if ( direction_ == COUNT_UP )
			result = DevTime::instance().timeNoRecord() - offset_;
		else
			result = offset_ - DevTime::instance().timeNoRecord();
	}

	return result;

}

void	DevTimer::time ( double newTime )
{
	if ( paused_ )
		offset_ = newTime;
    else
	{
		if ( direction_ == COUNT_UP )
			offset_ = DevTime::instance().time() - newTime;
		else
			offset_ = DevTime::instance().time() + newTime;
	}
}

void    DevTimer::pause()
{
    offset_ = time();
    paused_ = true;
}

void    DevTimer::resume()
{
    if( paused_ )
    {
    	if ( direction_ == COUNT_UP )
    		offset_ = DevTime::instance().time() - offset_;
    	else
    		offset_ = DevTime::instance().time() + offset_;
            
        paused_ = false;
    }
}

bool    DevTimer::paused() const
{
    return paused_;
}

DevTimer::Direction	DevTimer::direction () const
{
    return  direction_;
}

void    DevTimer::direction ( DevTimer::Direction newDirection )
{
  	if ( direction_ != newDirection )
	{
		offset_ = 2.0 * DevTime::instance().time() - offset_;
        direction_ = newDirection;
	}
}

ostream& operator <<( ostream& o, const DevTimer& t )
{
    o << t.time();

    return o;
}

void	DevTimer::timeNoRecord ( double newTime )
{
	if ( paused_ )
		offset_ = newTime;
    else
	{
		if ( direction_ == COUNT_UP )
			offset_ = DevTime::instance().timeNoRecord() - newTime;
		else
			offset_ = DevTime::instance().timeNoRecord() + newTime;
	}
}

/* End TIMER.CPP ****************************************************/
