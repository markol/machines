/* 
 * W 9 5 T I M E . H P P 
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#ifndef DEVICE_WIN95_TIME_HPP_INCLUDED
#define DEVICE_WIN95_TIME_HPP_INCLUDED

#include "base/base.hpp"

// A wrapper for the time maintained by Windows 95.
// Note this file is included *within* the body of the 
// DevTime class, so this class definition is nested.
class OSTime
{
public:
	double 			time() const;
	double 			messageTime() const;
    double 			resolution() const;
private:
    //  The "NoRecord" function is only suppied to allow the sound library
    //  to work without making any non-repeatable recorded calls to the
    //  timers.
    friend double DevTime::timeNoRecord() const;
	double 			timeNoRecord() const;
};

#endif
