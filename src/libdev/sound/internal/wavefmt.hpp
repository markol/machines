/***********************************************************

  W A V E F M T . H P P

***********************************************************/

/*

  A standard class implementation with nothing more
  than get and set methods.

  Be careful when constructing or using format()
  as they both are overloaded with the second
  argument taking an int or a float.

  format( 0, (SampleRateHz)0, 0 );
   or
  format( 0, (SampleRateKHz)0, 0 );
   or
  format( 0, 0.0f, 0 );

  Which demonstrates the next point, there are
  two sample rate types with on one letter difference.
  They are defined as follows in dxs.hpp:

  typedef unsigned int SampleRateHz;
  typedef float SampleRateKHz;

  Remember this when using either of the following
  methods of WaveFormat

  sampleRateHz();
  sampleRateKHz();

  The nBytesPerSec() method has been added because
  the algorithm was used a lot in the implementation
  and is also used in the operator <() implementation.

*/

#ifndef WAVE_FORMAT_HPP
	#define WAVE_FORMAT_HPP

#include "base/base.hpp"
#include "sound/internal/dxs.hpp"
////////////////////////////////////////////////////////////

	class WaveFormat
	{
	public:
		WaveFormat( Channels, SampleRateHz, BitsPerSample );
		WaveFormat( Channels, SampleRateKHz, BitsPerSample );
		WaveFormat( const WaveFormat& );
		WaveFormat& operator =( const WaveFormat& );

		void format( Channels, SampleRateHz, BitsPerSample );
		void format( Channels, SampleRateKHz, BitsPerSample );

		void channels( Channels );
		void sampleRateHz( SampleRateHz );
		void sampleRateKHz( SampleRateKHz );
		void bitsPerSample( BitsPerSample );

		Channels channels() const;
		SampleRateHz sampleRateHz() const;
		SampleRateKHz sampleRateKHz() const;
		BitsPerSample bitsPerSample() const;
		int nBytesPerSec() const;

	    friend bool operator ==( const WaveFormat& a, const WaveFormat& b);
		friend bool operator <( const WaveFormat& a, const WaveFormat& b);

	private:
		Channels channels_;
		SampleRateHz sampleRateHz_;
		BitsPerSample bitsPerSample_;
	};

////////////////////////////////////////////////////////////

#ifdef _INLINE
	#include "wavefmt.ipp"
#endif

////////////////////////////////////////////////////////////

#endif /* WAVE_FORMAT_HPP *********************************/
