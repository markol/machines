/***********************************************************

  W A V E F M T . I P P

***********************************************************/

#ifdef _INLINE
	#define _CODE_INLINE inline
#else
	#define _CODE_INLINE
#endif

////////////////////////////////////////////////////////////
_CODE_INLINE
void WaveFormat::channels( Channels c )
{
	channels_ = c;
}
////////////////////////////////////////////////////////////
_CODE_INLINE
void WaveFormat::sampleRateHz( SampleRateHz r )
{
	sampleRateHz_ = r;
}
////////////////////////////////////////////////////////////
_CODE_INLINE
void WaveFormat::sampleRateKHz( SampleRateKHz r )
{
	sampleRateHz_ = r * 1000;
}
////////////////////////////////////////////////////////////
_CODE_INLINE
void WaveFormat::bitsPerSample( BitsPerSample b )
{
	bitsPerSample_ = b;
}
////////////////////////////////////////////////////////////
_CODE_INLINE
Channels WaveFormat::channels( void ) const
{
	return channels_;
}
////////////////////////////////////////////////////////////
_CODE_INLINE
SampleRateHz WaveFormat::sampleRateHz( void ) const
{
	return sampleRateHz_;
}
////////////////////////////////////////////////////////////
_CODE_INLINE
SampleRateKHz WaveFormat::sampleRateKHz( void ) const
{
	SampleRateKHz pSRKHz;
	pSRKHz = (sampleRateHz_ / 1000);
	return pSRKHz;
}
////////////////////////////////////////////////////////////
_CODE_INLINE
BitsPerSample WaveFormat::bitsPerSample( void ) const
{
	return bitsPerSample_;
}
////////////////////////////////////////////////////////////
_CODE_INLINE
int WaveFormat::nBytesPerSec( void ) const
{
	return channels() * sampleRateHz() * bitsPerSample() / 8;
}
////////////////////////////////////////////////////////////
