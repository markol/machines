/***********************************************************

  D X S . H P P

***********************************************************/

/*

  Declarations of constants and typedefs.

*/

#ifndef DIRECT_X_SOUND_HPP
	#define DIRECT_X_SOUND_HPP

////////////////////////////////////////////////////////////

	typedef short Channels;
	const Channels DXS_CHANNELS_MONO =1;
	const Channels DXS_CHANNELS_STEREO =2;

////////////////////////////////////////////////////////////

	typedef unsigned int SampleRateHz;
	typedef float SampleRateKHz;
//	const SampleRateKHz ELEVEN_KHZ =11.025f;
//	const SampleRateKHz TWENTYTWO_KHZ =22.05f;
//	const SampleRateKHz FOURTYFOUR_KHZ =44.1f;

////////////////////////////////////////////////////////////

	typedef short BitsPerSample;

////////////////////////////////////////////////////////////

	const int PCM_FORMAT_TAG = 1;

	const int DXS_MAX_VOLUME = 10000;
	const int DXS_MIN_VOLUME = 0;
	const int DXS_MAX_PAN_LEFT = -10000;
	const int DXS_MAX_PAN_RIGHT = 10000;
	const int DXS_PAN_CENTRE = 0;

	enum SampleStore {
		SYSTEM_MEMORY,
		HARDWARE_MEMORY
	};


////////////////////////////////////////////////////////////

#endif /* DIRECT_X_SOUND_HPP ******************************/
