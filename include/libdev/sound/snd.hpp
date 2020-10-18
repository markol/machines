/***********************************************************

  S N D . H P P

***********************************************************/

/*

  Just typedefs and constants

*/

#ifndef _SND_HPP
#define _SND_HPP

#include "base/base.hpp"

class Snd
{
public:
	Snd();

	enum SampleRates
	{
		ELEVEN_THOUSAND_HZ=11025,
		TWENTY_TWO_THOUSAND_HZ=22500,
		FORTY_FOUR_THOUSAND_HZ=44100
	};

	enum SoundQuality
	{
		SIXTEEN_BIT=16,
		EIGHT_BIT=8,
	};

	enum SpeakerType
	{
		STEREO=2,
		MONO=1,
	};

	enum SamplePriority {
		HIGH = 2,
		MEDIUM = 1,
		LOW	= 0
	};

	enum AudioType {
		TWO_D,
		THREE_D
	};

    typedef unsigned long 	SoundID;
    typedef unsigned long 	ByteOffset;
    typedef unsigned int 	Polyphony;
    typedef unsigned int 	SampleLoopCount;
    typedef int 			SampleVolume;
    typedef size_t 			Volume;
    typedef size_t 			LoopCount;
    typedef size_t 			Distance;

	typedef float			RelativeTime;
//	typedef float 			AbsoluteTime;

	static Volume 			defaultSampleVolume();
	static SamplePriority 	defaultSamplePriority();
	static Volume 			maxVolume();
	static Volume 			minVolume();
	static size_t			maxMemoryBuffers();
	static Distance			defaultMaxDistance();
	static Distance			defaultMinDistance();

};
////////////////////////////////////////////////////////////

#endif /* _SND_HPP ****************************************/