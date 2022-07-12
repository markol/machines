/* 
 * T I M E . C P P 
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */
// Given a OS-specific object which provides the current
// wall-clock time according to your operating system of choice,
// these bits of the DevTime class don't vary across systems.

#include "device/time.hpp"

DevTime::DevTime()
: rate_( 1.0 ),
  paused_( false ),
  offset_( 0.0 ),
  pausedValue_( 0.0 )
{
}

DevTime& DevTime::instance()
{
    static  DevTime  instance;
    
    return instance;
}

DevTime::~DevTime()
{
}

double DevTime::resolution() const
{
	return OSTime_.resolution();
}	

double DevTime::time() const
{
	double realTime;

	if( paused_ )
	{
		realTime = pausedValue_;
	}
	else
	{
		realTime = OSTime_.time() - offset_;
	}

	return realTime * rate_;
}

double DevTime::timeNoRecord() const
{
	double realTime;

	if( paused_ )
	{
		realTime = pausedValue_;
	}
	else
	{
		realTime = OSTime_.timeNoRecord() - offset_;
	}

	return realTime * rate_;
}

void DevTime::rate( double newRate )
{
	rate_ = newRate;
}

double DevTime::rate() const
{
	return rate_;
}

void DevTime::pause()
{
	if( !paused_ )
	{
		pausedValue_ = OSTime_.time() - offset_;
		paused_ = true;
	}
}

void DevTime::resume()
{
	if( paused_ )
	{
		offset_ = OSTime_.time() - pausedValue_;
		paused_ = false;
	}
}

// Windows 95 only: like Win32 GetMessageTime but the resulting value will 
// be on the same time scale as the results of the time() method.
#if defined _WIN95APP
double DevTime::getMessageTime() const
{
	double realTime;

	if( paused_ )
	{
		realTime = pausedValue_;
	}
	else
	{
		realTime = OSTime_.messageTime() - offset_;
	}

	return realTime * rate_;
}
#endif
