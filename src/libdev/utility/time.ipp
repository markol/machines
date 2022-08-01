/*
 * T I M E . I P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#include <time.h>

unsigned int RDTSC(uint32[ 2 ]);
#pragma aux RDTSC =   \
0x0f 0x31 /* RDTSC */ \
" mov [esi],eax   "   \
" mov [esi+4],edx "   \
parm [esi] modify [eax edx] value [eax];

//  Create a timer that takes its time from
//  the master timer.
inline
UtlDebugTimer::UtlDebugTimer( void )
: paused_( false )
{
	//RDTSC( offset_.data() );
}

inline
UtlDebugTimer::UtlDebugTimer( InitialState state )
: paused_( state == PAUSED )
{
	//RDTSC( offset_.data() );

    if( paused_ )
       	pausedTime_ = offset_;
}

//  Return the built in master timer
// static
inline
UtlDebugTimer&  UtlDebugTimer::masterTimer( void )
{
    static  UtlDebugTimer  masterTimer_( MASTER_TIMER );

    return masterTimer_;
}

//  Create the master timer
inline
UtlDebugTimer::UtlDebugTimer( TimerType )
: paused_( false )
{
	//RDTSC( offset_.data() );
}

inline
UtlDebugTimer::~UtlDebugTimer()
{
}


//  static
inline
void    UtlDebugTimer::calibrate( double calibrationTimeSeconds )
{
    startCalibration();
    finishCalibration( calibrationTimeSeconds );
}

//  static
inline
void    UtlDebugTimer::startCalibration( void )
{
    //  First wait for the clock to change to get a reasonably
    //  accurate base point.
    size_t  initialClockValue = clock();

    while( clock() == initialClockValue )
        /* Do nothing */ ;

    calibrationInitialClock() = clock();
    calibrationInitialTicks() = pentiumTicks();

}

//  static
inline
void    UtlDebugTimer::finishCalibration( double minCalibrationTimeSeconds )
{
    size_t minClocksToWait =  minCalibrationTimeSeconds * CLOCKS_PER_SEC;


    while( clock() <= calibrationInitialClock() + minClocksToWait )
    {
        //  Do nothing
    }

    size_t  lastClock = clock();

    while ( clock() == lastClock );
    {
        //  Do nothing
    }

    const size_t  finalClock = clock();
    const double finalTicks = pentiumTicks();

    pentiumTicksPerSecond() =
      ( finalTicks - calibrationInitialTicks() ) * CLOCKS_PER_SEC / ( finalClock - calibrationInitialClock() );

    calibrated() = true;
}

inline
double UtlDebugTimer::pentiumTicks( void )
{
    uint32  timer[ 2 ];

    // TODO check this
    //RDTSC( timer );

    double  ticks;

    ticks = timer[ 1 ];

    ticks *= 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2;
    ticks *= 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2;
    ticks *= 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2;
    ticks *= 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2;
    ticks += timer[ 0 ];

    return ticks;
}

inline
UtlDebugTimer::Time    UtlDebugTimer::time( void ) const
{
    PRE( calibrated() );

	Time realTime;

    if( paused_ )
    {
        realTime.reading_ = pausedTime_;
        realTime.offset_ = offset_;
    }
    else
    {
    	//RDTSC( realTime.reading_.data() );
        realTime.offset_ = offset_;
    }

	return realTime;
}

inline
void    UtlDebugTimer::time( Time* pTime ) const
{
    PRE( calibrated() );

    if( paused_ )
    {
        pTime->reading_ = pausedTime_;
        pTime->offset_ = offset_;
    }
    else
    {
    	//RDTSC( pTime->reading_.data() );
        pTime->offset_ = offset_;
    }
}

inline
void    UtlDebugTimer::time( double newTime )
{
    PRE( calibrated() );

    newTime *= UtlDebugTimer::pentiumTicksPerSecond();

    if( paused_ )
    {
        pausedTime_ = offset_;
        pausedTime_ += newTime;
    }
    else
    {
    	//RDTSC( offset_.data() );
        offset_ -= newTime;
    }
}

inline
void UtlDebugTimer::pause()
{
    PRE( calibrated() );

	if( !paused_ )
	{

    	//RDTSC( pausedTime_.data() );

		paused_ = true;
	}
}

inline
void UtlDebugTimer::resume()
{
    PRE( calibrated() );

	if( paused_ )
	{
        UtlUint64 newOffset;

    	//RDTSC( newOffset.data() );

        newOffset -= pausedTime_;
        newOffset += offset_;

        offset_ = newOffset;

		paused_ = false;
	}
}

inline
bool UtlDebugTimer::paused() const
{
    return paused_;
}

// static
inline
size_t& UtlDebugTimer::calibrationInitialClock( void )
{
    static  size_t  calibrationInitialClock_;

    return calibrationInitialClock_;
}

// static
inline
double& UtlDebugTimer::calibrationInitialTicks( void )
{
    static  double  calibrationInitialTicks_;

    return calibrationInitialTicks_;
}

// static
inline
double& UtlDebugTimer::pentiumTicksPerSecond( void )
{
    static  double  pentiumTicksPerSecond_;

    return pentiumTicksPerSecond_;
}

//  static
inline
bool&   UtlDebugTimer::calibrated( void )
{
    static  bool    calibrated_ = false;

    return calibrated_;
}

inline
ostream& operator<<( ostream& o, const UtlDebugTimer& t )
{
    o << t.time().asDouble();

    return o;
}

inline
double  UtlDebugTimer::Time::asDouble() const
{
    PRE( UtlDebugTimer::calibrated() );

    UtlUint64  u64 = reading_;
    u64 -= offset_;

    return u64.asDouble() / UtlDebugTimer::pentiumTicksPerSecond();
}

inline
ostream& operator<<( ostream& o, const UtlDebugTimer::Time& t )
{
    o << t.asDouble();

    return o;
}

inline
UtlDebugBlockTimer::UtlDebugBlockTimer( UtlDebugTimer& t )
: timer_( t )
{
    timer_.resume();
}

inline
UtlDebugBlockTimer::~UtlDebugBlockTimer()
{
    timer_.pause();
}

/* End TIME.IPP ****************************************************/
