/*
 * A V E R A G E . C T P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */
 
#include "device/timer.hpp"

template< class X >
UtlAverager<X>::UtlAverager():
	timer_(_NEW(DevTimer)),
	period_(0.5),
	lastAverage_(0),
	accumulation_(0)
{
	lastSampleTime_ = timer_->time();
	POST(period() == 0.5);
}

template< class X >
UtlAverager<X>::~UtlAverager()
{
	_DELETE(timer_);
}

template< class X >
const X& UtlAverager<X>::average()
{
	const double now = timer_->time();
	const double interval = now - lastSampleTime_;
	ASSERT(interval >= 0, "time went backwards");

	ASSERT(period_ > 0, "period <= zero");	
	if (interval >= period_)
	{
		lastSampleTime_ = now;
		sampleAverageAndClear(interval);
	}

	return lastAverage_;
}

template< class X >
void UtlAverager<X>::sampleAverageAndClear(double interval)
{
	PRE(interval > 0);
	
	const X tmp = accumulation_;
	accumulation_ = 0;

	lastAverage_ = tmp / interval;
}

template< class X >
void UtlAverager<X>::accumulate(const X& x)
{
	accumulation_ += x;
}

template< class X >
double UtlAverager<X>::timeSinceLastSample() const
{
	const double now = timer_->time();
	return now - lastSampleTime_;
}	

template< class X >
double UtlAverager<X>::period() const
{
	return period_;
}

template< class X >
void UtlAverager<X>::period(double p)
{
	PRE(p > 0);
	period_ = p;
}

/* End AVERAGE.CTP **************************************************/
