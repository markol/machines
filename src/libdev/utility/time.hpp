/*
 * T I M E . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

/*
    This class should not be used. It has been left here so that
    the code doesn't get lost, however DevTimer should now be used
    for timing. DevTimer will be as accurate as this class was.
*/

#ifdef PRODUCTION
    #error UtlDebugTimer must not be used in production code
#endif

#ifndef _UTILITY_TIME_HPP_INCLUDED
#define _UTILITY_TIME_HPP_INCLUDED

#include <stdlib.h>
#include <stddef.h>
#include <sstream>

#include "base/base.hpp"

#include "utility/private/uint64.hpp"

// Implement a high-resolution, low-overhead timer based on the Pentium's
// special register.  This should be used for debugging only as it only
// works with Intel CPUs and is non-portable. This class will only work
// if the environment variable "CHARYBDIS" has been set - it will abort
// program execution otherwise.

class UtlDebugTimer
{
public:

    class Time
    {
    public:
        double  asDouble() const;

    private:
        friend class UtlDebugTimer;

        UtlUint64  reading_;
        UtlUint64  offset_;
    };

    //  Create a timer that takes its time from
    //  the master timer.
    UtlDebugTimer();
    //  POST( not paused() );

    enum    InitialState { PAUSED, RUNNING };

    UtlDebugTimer( InitialState state );

    //  Return the built in master timer
    static  UtlDebugTimer&  masterTimer();

    ~UtlDebugTimer();

    //  There are two calls to get the time back. Use the first one
    //  for maximum speed,
    void    time( Time* pTime ) const;
    Time    time() const;

    //  Set the current time
    void    time( double newTime );

    void    pause();
    void    resume();

    bool    paused() const;

    //  Calibrate over the given number of seconds
    static  void    calibrate( double calibrationTimeSeconds = 3.0 );

    //  Calibrate but without locking up the machine for the calibration time.
    //  E.g. call startCalibrate before initialising other stuff in your
    //  application, then finishCalibrate at a later time. The value supplied
    //  to finishCalibrate is the minimum time delay it will use for the calibration.

    static  void    startCalibration();
    static  void    finishCalibration( double minCalibrationTimeSeconds = 3.0 );

private:
	static  double pentiumTicks();
	static  double pentiumTime();

    enum TimerType{ MASTER_TIMER };

    //  Create the master timer
    UtlDebugTimer( TimerType );

    // Operations deliberately revoked
    UtlDebugTimer( const UtlDebugTimer& );
    const UtlDebugTimer& operator =( const UtlDebugTimer& );
    bool operator ==( const UtlDebugTimer& ) const;

    static  size_t& calibrationInitialClock();
    static  double& calibrationInitialTicks();

    friend  class Time;
    static  double& pentiumTicksPerSecond();

    static  bool&   calibrated();

    bool    paused_;
    UtlUint64  pausedTime_;
    UtlUint64  offset_;
};

ostream& operator<<( ostream&, const UtlDebugTimer& );
ostream& operator<<( ostream&, const UtlDebugTimer::Time& );

// Use for computing the time spent in any code block.  Useful for timing
// functions. Resumes the given timer on construction, pauses it on
// destruction.

class UtlDebugBlockTimer
{
public:
	UtlDebugBlockTimer( UtlDebugTimer& t );
	~UtlDebugBlockTimer();

private:
	UtlDebugTimer& timer_;
};


#include "utility/time.ipp"

#endif

/* End TIME.HPP ****************************************************/
