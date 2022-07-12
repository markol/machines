/*

S N D P A R A M . C P P

*/
#include "sound/sndparam.hpp"

class SndMixerParametersImpl
{
public:
	SndMixerParametersImpl();
	~SndMixerParametersImpl();

	Snd::Polyphony polyphony_;
	SndMixerParameters::SoundSystem	soundSystem_;
	Snd::SampleRates sampleRate_;
	Snd::SoundQuality soundQuality_;
	Snd::SpeakerType speakerType_;
	Snd::AudioType audioType_;
	size_t noOfChannels_;
};

SndMixerParametersImpl::SndMixerParametersImpl():
polyphony_(0),
soundSystem_(SndMixerParameters::UNKNOWN),
sampleRate_(Snd::ELEVEN_THOUSAND_HZ),
soundQuality_(Snd::EIGHT_BIT),
speakerType_(Snd::MONO),
audioType_(Snd::TWO_D),
noOfChannels_(0)
{
}

SndMixerParametersImpl::~SndMixerParametersImpl()
{
	//Empty
}

////////////////////////////////////////////////////////

SndMixerParameters::SndMixerParameters( 
					Snd::Polyphony polyphony,
					SoundSystem soundSystem,
					Snd::SampleRates sampleRate,
					Snd::SoundQuality soundQuality,
					Snd::SpeakerType speakerType,
					Snd::AudioType audioType,
					size_t noOfChannels )
{
	pImpl_ = _NEW(SndMixerParametersImpl());

	CB_DEPIMPL(Snd::Polyphony, polyphony_);
	CB_DEPIMPL(SoundSystem , soundSystem_);
	CB_DEPIMPL(Snd::SampleRates, sampleRate_);
	CB_DEPIMPL(Snd::SoundQuality, soundQuality_);
	CB_DEPIMPL(Snd::SpeakerType, speakerType_);
	CB_DEPIMPL(Snd::AudioType, audioType_);
	CB_DEPIMPL(size_t, noOfChannels_);

	polyphony_ = polyphony;
	soundSystem_ = soundSystem;
	sampleRate_ = sampleRate;
	soundQuality_ = soundQuality;
	speakerType_ = speakerType;
	audioType_ = audioType;
	noOfChannels_ = noOfChannels;
}

SndMixerParameters::~SndMixerParameters()
{
	_DELETE(pImpl_);
}

Snd::Polyphony SndMixerParameters::polyphony() const
{
	CB_DEPIMPL(Snd::Polyphony, polyphony_);

	return polyphony_;
}

SndMixerParameters::SoundSystem SndMixerParameters::soundSystem() const
{
	CB_DEPIMPL(SoundSystem , soundSystem_);

	return soundSystem_;
}

Snd::SampleRates SndMixerParameters::sampleRate() const
{
	CB_DEPIMPL(Snd::SampleRates, sampleRate_);

	return sampleRate_;
}

Snd::SoundQuality SndMixerParameters::soundQuality() const
{
	CB_DEPIMPL(Snd::SoundQuality, soundQuality_);

	return soundQuality_;
}

Snd::SpeakerType SndMixerParameters::speakerType() const
{
	CB_DEPIMPL(Snd::SpeakerType, speakerType_);

	return speakerType_;
}

Snd::AudioType SndMixerParameters::audioType() const
{
	CB_DEPIMPL(Snd::AudioType, audioType_);

	return audioType_;
}

size_t SndMixerParameters::noOfChannels() const
{
	CB_DEPIMPL(size_t, noOfChannels_);

	return noOfChannels_;
}
//End of SNDPARAM.CPP
