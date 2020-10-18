/*
 * S N D P A R A M . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
*/

/*

  SndMixerParameters

  Simple class that is used to intialise DevSoundMixer

*/

#ifndef _DEVICE_SOUND_PARAMETERS
#define _DEVICE_SOUND_PARAMETERS

////////////////////////////////////////////////////////////
#include "sound/snd.hpp"

class SndMixerParametersImpl;

class SndMixerParameters
{
public:

	enum SoundSystem
	{
		DIRECTSOUND,
		UNKNOWN
	};

	SndMixerParameters(
						Snd::Polyphony polyphony,
						SoundSystem soundSystem,
						Snd::SampleRates sampleRate,
						Snd::SoundQuality soundQuality,
						Snd::SpeakerType speakerType,
						Snd::AudioType audioType,
						size_t noOfChannels);
	~SndMixerParameters();

	Snd::Polyphony polyphony( void ) const;
	SoundSystem soundSystem() const;
	Snd::SampleRates sampleRate() const;
	Snd::SoundQuality soundQuality() const;
	Snd::SpeakerType speakerType() const;
	Snd::AudioType audioType() const;
	size_t noOfChannels() const;

private:
    //Revoked
    SndMixerParameters( const SndMixerParameters& );

	SndMixerParametersImpl* pImpl_;
};

#endif /* _DEVICE_SOUND_PARAMETERS ************************/
