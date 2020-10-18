/*
 * A V E R A G E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef UTL_AVERAGE_HPP
#define UTL_AVERAGE_HPP

#include "base/base.hpp"

class DevTimer;

// Accumulate a numerical value between client defined sample points.
// This class times the intervals between samples and computes the
// average.  The type X must have:
//   X& operator+=(const X&) and
//   const X& operator/ (const X&, double) or similar
//   initialisation which takes 0 as an arg
template <class X> class UtlAverager
{
public:
    UtlAverager();	// POST(period() == 0.5);
    ~UtlAverager();

	void accumulate(const X&);
	const X& average();
	double timeSinceLastSample() const;

	double period() const;
	void period(double d);	// PRE(d > 0);

private:
	void sampleAverageAndClear(double interval);

	DevTimer*	timer_;
	double		lastSampleTime_, period_;
	X			accumulation_, lastAverage_;

    UtlAverager( const UtlAverager< X >& );
    UtlAverager< X >& operator =( const UtlAverager< X >& );
};

//#ifdef _INSTANTIATE_TEMPLATE_CLASSES
    #include "w4dtest/averager.ctp"
//#endif


#endif

/* End AVERAGE.HPP **************************************************/
