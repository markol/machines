/*******************************************************************
 *
 *    DESCRIPTION: SND.CPP - Contains various definitions for the
 *							 sound library.
 *
 *    AUTHOR:
 *
 *    HISTORY:
 *
 *******************************************************************/

#include "sound/snd.hpp"

/////////////////////////////////////////////////////////////////////
Snd::Snd()
{
}

/////////////////////////////////////////////////////////////////////
Snd::Volume Snd::defaultSampleVolume()
{
	static Volume defaultVolume = 95;
	return defaultVolume;
}
/////////////////////////////////////////////////////////////////////

Snd::SamplePriority 	Snd::defaultSamplePriority()
{
	static SamplePriority defaultPriority = MEDIUM;
	return defaultPriority;
}
/////////////////////////////////////////////////////////////////////
Snd::Volume Snd::maxVolume()
{
	static Volume maxVolume = 100;
	return maxVolume;
}
/////////////////////////////////////////////////////////////////////
Snd::Volume Snd::minVolume()
{
	static Volume minVolume = 0;
	return minVolume;
}
/////////////////////////////////////////////////////////////////////
size_t Snd::maxMemoryBuffers()
{
	static size_t maxMemoryBuffers = 8;
	return maxMemoryBuffers;
}
/////////////////////////////////////////////////////////////////////
Snd::Distance Snd::defaultMaxDistance()
{
	static Snd::Distance defaultDistance = 25;
	return defaultDistance;
}

/////////////////////////////////////////////////////////////////////
Snd::Distance Snd::defaultMinDistance()
{
	const Snd::Distance defaultDistance = 5;
	return defaultDistance;
}
