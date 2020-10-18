/***********************************************************

  W A V E F M T . C P P

***********************************************************/

#include "sound/internal/wavefmt.hpp"

#ifndef _INLINE
	#include "sound/internal/wavefmt.ipp"
#endif
////////////////////////////////////////////////////////////

WaveFormat::WaveFormat( Channels c, SampleRateHz r,	BitsPerSample b )
	:channels_( c ), sampleRateHz_( r ), bitsPerSample_( b )
{
}

WaveFormat::WaveFormat( Channels c, SampleRateKHz r, BitsPerSample b )
	:channels_( c ), bitsPerSample_( b )
{
	sampleRateKHz( r );
}

WaveFormat::WaveFormat( const WaveFormat& w )
{
	channels( w.channels() );
	sampleRateHz( w.sampleRateHz() );
	bitsPerSample( w.bitsPerSample() );
}

////////////////////////////////////////////////////////////

void WaveFormat::format( Channels c, SampleRateHz r,
	BitsPerSample b )
{
	channels( c );
	sampleRateHz( r );
	bitsPerSample( b );
}

void WaveFormat::format( Channels c, SampleRateKHz r,
	BitsPerSample b )
{
	channels( c );
	sampleRateKHz( r );
	bitsPerSample( b );
}

////////////////////////////////////////////////////////////

WaveFormat& WaveFormat::operator =( const WaveFormat& w )
{
	channels( w.channels() );
	sampleRateHz( w.sampleRateHz() );
	bitsPerSample( w.bitsPerSample() );

	return *this;
}

/** FRIEND **/
bool operator ==( const WaveFormat& a, const WaveFormat& b )
{
	return ( a.channels() == b.channels() ) and
		( a.sampleRateHz() == b.sampleRateHz() ) and
		( a.bitsPerSample() == b.bitsPerSample() );
}

/** FRIEND **/
// added for compliance with STL/CTL
bool operator <( const WaveFormat& a, const WaveFormat& b )
{
	return a.nBytesPerSec() < b.nBytesPerSec();
}

////////////////////////////////////////////////////////////

/* WAVEFORMAT.CPP *****************************************/
