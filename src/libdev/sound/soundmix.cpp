/*
 * S O U N D M I X . C P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
*/

/*
*/

#include "stdlib/string.hpp"
//included for getenv only
//#include <stdlib.h>

#include "sound/soundmix.hpp"
#include "sound/sndparam.hpp"
#include "sound/internal/sample.hpp"
#include "sound/internal/alsample.hpp"
#include "sound/internal/alsound.hpp"
#include "sound/internal/waveman.hpp"
#include "sound/sndwavid.hpp"

#include "mathex/transf3d.hpp"

#include "utility/bndidgen.hpp"

#include "recorder/recorder.hpp"
#include "recorder/private/recpriv.hpp"

extern void debugTiming( const char*, bool );

//////////////////////////////////////////////////

class SndMixerImpl
{
	SndMixerImpl();
	~SndMixerImpl();

    static UtlId noOfChannels_;
	static bool& isInitialised_();

	SndMixer::SampleList activeSamples_;
	SndMixer::SampleIdVector idVector_;
    Snd::Polyphony polyphony_;
	SndMixerParameters::SoundSystem	soundSystem_;
    UtlBoundedIdGenerator* pSoundIDGenerator_;
	bool sortRequired_;
	Snd::Volume	masterSampleVolume_;
	Snd::Volume	originalMasterSampleVolume_;

	bool isAllocated(const SndSampleHandle& handle) const;
	void sort();
	Snd::Volume	hardwareMasterSampleVolume() const;
	void hardwareMasterSampleVolume( Snd::Volume );
	void freeAllResources();

	friend class SndMixer;
};

SndMixerImpl::SndMixerImpl() :
activeSamples_(noOfChannels_, NULL),
sortRequired_(false),
masterSampleVolume_( hardwareMasterSampleVolume() ),
polyphony_(0),
soundSystem_(SndMixerParameters::UNKNOWN),
pSoundIDGenerator_(NULL)
{
    idVector_.reserve( noOfChannels_ );
}

SndMixerImpl::~SndMixerImpl()
{
	//Empty
}

// static
//Is the sound system initialised
bool& SndMixerImpl::isInitialised_()
{
	static bool isInit_ = false;

    return isInit_;
}

bool SndMixerImpl::isAllocated(const SndSampleHandle& handle) const
{
	bool allocated = false;
	if(handle < pSoundIDGenerator_->upperBound())
		allocated = pSoundIDGenerator_->isAllocated((UtlId)handle);

	return allocated;

}

//Prioritise the active samples
void SndMixerImpl::sort()
{
	ASSERT(false, "No sorting done at present");
}

Snd::Volume SndMixerImpl::hardwareMasterSampleVolume() const
{
	Snd::Volume theVol = 95;
	return theVol;
}

void SndMixerImpl::hardwareMasterSampleVolume( Snd::Volume newVolume )
{

}

//////////////////////////////////////////////////

// static
//Is the sound system initialised
bool SndMixer::isInitialised()
{
	return SndMixerImpl::isInitialised_();
}

//static
bool SndMixer::soundOn()
{
	static bool soundOn = (getenv("CB_SOUND_OFF") == NULL);
	return soundOn;
}

SndMixer::SndMixer()
{
	//Construct in the correct order
	SndWaveManager::instance();
	ALSound::instance();

	pImpl_ = _NEW(SndMixerImpl());
}


// STATIC
void SndMixer::initialise( const SndMixerParameters& params )
{
	//Check we are not already initialised
    PRE( not SndMixer::isInitialised() );

	SndMixerImpl::noOfChannels_ = params.noOfChannels();

    //If we are using DirectSound
    if
    (
    	params.soundSystem() == SndMixerParameters::DIRECTSOUND
    )
    {
		//Initialise DirectSound stuff only if sound is switched on
	    ALSound::initialise( params, ALSound::EXCLUSIVE );
    }
	else
	{
		ASSERT(NULL, "Trying to initialise undefined sound system");
	}



	//Set state so that we know we are now initialised
	SndMixerImpl::isInitialised_() = true;

	//Set the polyphony of the mixer to the desired parameter
	instance().pImpl_->polyphony_ = params.polyphony();
	instance().pImpl_->soundSystem_ = params.soundSystem();
	instance().pImpl_->pSoundIDGenerator_ = _NEW(UtlBoundedIdGenerator(params.noOfChannels()));

	instance().pImpl_->originalMasterSampleVolume_ = instance().pImpl_->hardwareMasterSampleVolume();

	RICHARD_STREAM("originalMasterSampleVolume_ == " << instance().pImpl_->originalMasterSampleVolume_ << std::endl);

	POST( isInitialised() );
}

// static
SndMixer& SndMixer::instance()
{
	//Check that the class is valid before we instantiate it
    PRE( SndMixer::isInitialised() );

    static SndMixer SndMixer_;
    return SndMixer_;
}

//static
Snd::RelativeTime SndMixer::SndSampleLength(const SndWaveformId& id)
{
	PRE(SndWaveform::isWaveFile(id.pathname().c_str()));
	SndWaveform	tempWave(id);

//	WaveFormat wavFormat = tempWave.format();
	uint dataSize = tempWave.dataSize();
	int bytesPerSec = tempWave.format()->nBytesPerSec();

	Snd::RelativeTime lengthInSecs = (float)dataSize / (float)bytesPerSec;

	return lengthInSecs;
}

//////////////////////////////////////////////////

SndMixer::~SndMixer()
{
	CB_DEPIMPL(UtlBoundedIdGenerator*, pSoundIDGenerator_);

	pImpl_->hardwareMasterSampleVolume(instance().pImpl_->originalMasterSampleVolume_);
	RICHARD_STREAM("pImpl_->hardwareMasterSampleVolume " << pImpl_->hardwareMasterSampleVolume() << std::endl);

	pImpl_->freeAllResources();

	ALSound::instance().shutdown();
	SndWaveManager::instance().shutdown();

    if(pSoundIDGenerator_)
	{
	    _DELETE(pSoundIDGenerator_);
	}

	_DELETE(pImpl_);
}

//////////////////////////////////////////////////

bool SndMixer::isActive(const SndSampleHandle& handle) const
{
	bool handleIsActive = false;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        handleIsActive = RecRecorderPrivate::instance().playbackSoundIsActive();
    else
    {
  		handleIsActive = isActiveNoRecord( handle );

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordSoundIsActive( handleIsActive );
	}

    return handleIsActive;
}

//////////////////////////////////////////////////

bool SndMixer::isActiveNoRecord(const SndSampleHandle& handle) const
{
	CB_DEPIMPL(SampleList, activeSamples_);

	bool handleIsActive = false;

	if(isAllocated(handle) && activeSamples_[handle]->isPlaying())
		handleIsActive = true;

    return handleIsActive;
}

//////////////////////////////////////////////////
//Has this sample stopped playing
bool SndMixer::isStopped( const SndSampleHandle& handle ) const
{
	PRE(isAllocated(handle));
	CB_DEPIMPL(SampleList, activeSamples_);

	bool returnVal = false;

   	if(activeSamples_[handle]->state() == Sample::STOPPED)
		returnVal = true;

	return returnVal;
}

//Is this sample handle a valid allocated handle
bool SndMixer::isAllocated( const SndSampleHandle& handle ) const
{
	return (pImpl_->isAllocated(handle));
}

bool SndMixer::isAudible(const SndSampleHandle& handle ) const
{
	CB_DEPIMPL(SampleList, activeSamples_);

	bool handleIsAudible = false;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        handleIsAudible = RecRecorderPrivate::instance().playbackSoundIsAudible();
    else
    {
    	if(isAllocated(handle) && activeSamples_[handle]->isAudible())
	    	handleIsAudible = true;

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordSoundIsAudible( handleIsAudible );
	}

    return handleIsAudible;
}

//////////////////////////////////////////////////

SndSampleHandle SndMixer::playSample( const SndSampleParameters& p )
{
	if(soundOn())
	{
		CB_DEPIMPL(UtlBoundedIdGenerator*, pSoundIDGenerator_);
		CB_DEPIMPL(SampleIdVector, idVector_);
		CB_DEPIMPL(SampleList, activeSamples_);
		CB_DEPIMPL(SndMixerParameters::SoundSystem, soundSystem_);
		CB_DEPIMPL(bool, sortRequired_);

		PRE(noOfFreeLogicalChannelsNoRecord() > 0);
		//Create a new sample
	    Sample* pSample = NULL;
	    //If we are using DirectSound then ...
	    if(soundSystem_ == SndMixerParameters::DIRECTSOUND)
	    {
	    	//create a new DXSample passing in the sample parameters
			pSample = _NEW(ALSample( p ));
	    }
	    else
	    {
	    	//no other sound system has been implemented
	    	ASSERT(false, "No sound system definition for this type");
	    }

		SndSampleHandle handle = pSoundIDGenerator_->nextId();
		ASSERT(pSoundIDGenerator_->isAllocated(handle), "Invalid SoundId");
		ASSERT(pSample, "Sample incorrectly NEWED");
		if(!pSample)
			SOUND_STREAM(pSample << "Invalid Sample" << std::endl);

		//Put the new sample into the list of current samples
		activeSamples_[handle] =  pSample;
		idVector_.push_back(handle);

		sampleVolume(0);

		POST(isAllocated(handle));

		return handle;
	}
	else
	{
		return 0;
	}
}

bool SndMixer::isClientResponsible( const SndSampleHandle& handle ) const
{
	CB_DEPIMPL(SampleList, activeSamples_);

	PRE(isAllocated(handle));

	return activeSamples_[handle]->getResourceResponsibility() == Sample::CLIENT;
}

bool SndMixer::isLibraryResponsible( const SndSampleHandle& handle ) const
{
	CB_DEPIMPL(SampleList, activeSamples_);

	PRE(isAllocated(handle));

	return activeSamples_[handle]->getResourceResponsibility() == Sample::LIBRARY;
}

void SndMixer::setResourcesResponsibilityLibrary( const SndSampleHandle& handle )
{
	CB_DEPIMPL(SampleList, activeSamples_);
	PRE(isAllocated(handle));
	PRE(activeSamples_[handle]);

	activeSamples_[handle]->setResourceResponsibility(Sample::LIBRARY);

	POST(isLibraryResponsible(handle));
}


void SndMixer::setResourcesResponsibilityClient( const SndSampleHandle& handle )
{
	CB_DEPIMPL(SampleList, activeSamples_);
	PRE(isAllocated(handle));
	PRE(activeSamples_[handle]);

	activeSamples_[handle]->setResourceResponsibility(Sample::CLIENT);

	POST(isClientResponsible(handle));
}

//////////////////////////////////////////////////
void SndMixer::stopSample(const SndSampleHandle& handle )
{
	if(soundOn())
	{
		CB_DEPIMPL(UtlBoundedIdGenerator*, pSoundIDGenerator_);
		CB_DEPIMPL(SampleIdVector, idVector_);
		CB_DEPIMPL(SampleList, activeSamples_);
		CB_DEPIMPL(bool, sortRequired_);

	//	PRE(isClientResponsible(handle));
	    PRE(isAllocated(handle));

	   	//Stop the sample
	    activeSamples_[handle]->stop();

		//Erase its id from the ID vector
	//	SampleIdVector::iterator i = idVector_.begin();
	//	SampleIdVector::iterator j = idVector_.end();
	//
	//	bool looping = true;
	//	while(looping && i!=j)
	//	{
	//		if((*i) == handle)
	//		{
	//			idVector_.erase(i);
	//			looping = false;
	//		}
	//		else
	//			++i;
	//	}
	    POST(!isActiveNoRecord(handle));
	}
}

//Frees sample memory and its handle
void SndMixer::freeSampleResources( const SndSampleHandle& handle )
{
	CB_DEPIMPL(UtlBoundedIdGenerator*, pSoundIDGenerator_);
	CB_DEPIMPL(SampleIdVector, idVector_);
	CB_DEPIMPL(SampleList, activeSamples_);
	CB_DEPIMPL(bool, sortRequired_);

    PRE( isAllocated(handle) );
	for(SampleIdVector::iterator i = idVector_.begin(); i!=idVector_.end(); ++i)
	{
		if(*i == handle)
		{
			idVector_.erase(i);
			break;
		}
	}

	//Delete the sample itself
	_DELETE(activeSamples_[handle]);
	activeSamples_[handle] = NULL;

	//Free its index
	pSoundIDGenerator_->freeId(handle);

    POST( !isAllocated(handle) );
}

void SndMixerImpl::freeAllResources()
{
	//erase all handles from the idVector
	for(SndMixer::SampleIdVector::iterator i = idVector_.begin(); i!=idVector_.end(); i = idVector_.begin())
		idVector_.erase(i);

	//Free up all handles and delete all samples active in the
	//system at present
	for(UtlId j=0; j<pSoundIDGenerator_->upperBound(); ++j)
	{
		if(isAllocated(j))
		{
			activeSamples_[j]->stop();
			_DELETE(activeSamples_[j]);
			pSoundIDGenerator_->freeId(j);
		}
		activeSamples_[j] = NULL;
	}

}
//////////////////////////////////////////////////

//Find out whether the sample is in memory
bool SndMixer::hasLoadedWaveform( const SndWaveformId& pathname ) const
{
    return SndWaveManager::instance().isLoaded(pathname);
}

//////////////////////////////////////////////////
//load the named sample
const SndWaveform& SndMixer::loadWaveform( const SndWaveformId& id, bool threeD )
{
	SndSampleParameters* pNewParams;
	if(threeD)
	{
		MexPoint3d pos;
		pNewParams = _NEW(SndSampleParameters(id, pos));
	}
	else
		pNewParams = _NEW(SndSampleParameters(id));

	pNewParams->setPreload(true);

	//If the wave is not previously loaded then
	//create it and insert it into the list of preloaded samples
	if(!hasLoadedWaveform( id ))
	{
	    ALSample* pNewSample = _NEW(ALSample(*pNewParams));
	    ALSound::instance().preLoadedSamples_.push_back(pNewSample);
        SndWaveManager::instance().loadedSoundBuffers_.insert(id, pNewSample->alBuffer_);
		_DELETE(pNewParams);
	}

    SndWaveform* pWave = SndWaveManager::instance().getWaveForm(id);
    POST( hasLoadedWaveform( id ) );


	return *pWave;
}
//////////////////////////////////////////////////

void SndMixer::unloadWaveform( const SndWaveformId& id )
{
	PRE( hasLoadedWaveform( id ) );

	SndWaveManager::instance().freeWaveForm(id);
}

//////////////////////////////////////////////////

//Update the state of each sample
void SndMixer::update()
{
	if(soundOn())
	{
		CB_DEPIMPL(Snd::Polyphony, polyphony_);
		CB_DEPIMPL(UtlBoundedIdGenerator*, pSoundIDGenerator_);
		CB_DEPIMPL(SampleIdVector, idVector_);
		CB_DEPIMPL(SampleList, activeSamples_);
		CB_DEPIMPL(bool, sortRequired_);

		bool keepLooping = true;
		if(ALSound::instance().is3DSystem_)
			ALSound::instance().commitDeferredSettings();


		for(SndSampleHandle i=0;i<idVector_.size() && keepLooping;++i)
		{
			SndSampleHandle index = idVector_[i];
			Sample* pThisSample = activeSamples_[index];
			Sample::SampleState sampleState = pThisSample->update();

			//ACtion according to the samples updated state
			switch(sampleState)
	        {
	        	//If the sample has been stopped
	        	case ALSample::STOPPED:
	            {
	            	//Stop the sample
					if(pThisSample->getResourceResponsibility()==Sample::LIBRARY)
					{
						freeSampleResources(index);
					}
	            }
	            break;

	        	//If the sample has been initialised, i.e. it is loaded
	            //but not playing
	        	case ALSample::INITIALISED:
	            {
	            	// play the sample
	//				keepLooping = false;
	            	pThisSample->play();
	            }
	            break;

	            default:
	            	//The sample is in some other state, probably playing
	            break;
	        }
		}
	}
}



//////////////////////////////////////////////
bool SndMixer::is3dSound( const SndSampleHandle& handle ) const
{
	CB_DEPIMPL(SampleList, activeSamples_);
	PRE(isAllocated(handle));

    bool    result;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        result = RecRecorderPrivate::instance().playbackSoundIs3dSound();
    else
    {
    	PRE( isActiveNoRecord( handle ) );

        result = activeSamples_[handle]->is3D();

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordSoundIs3dSound( result );
	}

    return result;
}

bool SndMixer::is3dSoundNoRecord( const SndSampleHandle& handle ) const
{
	CB_DEPIMPL(SampleList, activeSamples_);

	PRE( isActiveNoRecord( handle ) );

    return activeSamples_[handle]->is3D();
}

MexPoint3d SndMixer::samplePosition(const SndSampleHandle& handle)
{
    MexPoint3d result;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        result = RecRecorderPrivate::instance().playbackSoundSamplePosition();
    else
    {
        result = samplePositionNoRecord( handle );

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordSoundSamplePosition( result );
	}

    return result;
}

MexPoint3d SndMixer::samplePositionNoRecord(const SndSampleHandle& handle) const
{
	CB_DEPIMPL(SampleList, activeSamples_);

	PRE( isActiveNoRecord( handle ) );
	PRE( is3dSoundNoRecord( handle ) );

	MexVec3 returnVec;
	if(soundOn())
	{
		activeSamples_[handle]->position3D(&returnVec);
	}
	return returnVec;
}

void SndMixer::samplePosition(const SndSampleHandle& handle, const MexPoint3d& newPos)
{
	if(soundOn())
	{
		PRE(isAllocated(handle));

	    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
	    {
	    	CB_DEPIMPL(SampleList, activeSamples_);

	    	PRE( isActiveNoRecord( handle ) );

	    	activeSamples_[handle]->newPosition(newPos);
	    }
	}
}

MexPoint3d SndMixer::listenerPosition() const
{
	CB_DEPIMPL(SndMixerParameters::SoundSystem, soundSystem_);

	static MexPoint3d returnVal;
    if(soundSystem_ == SndMixerParameters::DIRECTSOUND)
    {
		returnVal = ALSound::instance().listenerPosition();
	}
	else
	{
		ASSERT(NULL, "Trying to use undefined sound system");
	}
	return returnVal;
}

bool SndMixer::is3dMixer() const
{
	return ALSound::instance().is3DSystem();
}

void SndMixer::listenerPosition(const MexPoint3d& newPos)
{
	CB_DEPIMPL(SndMixerParameters::SoundSystem, soundSystem_);

	PRE(is3dMixer())
    if(soundSystem_ == SndMixerParameters::DIRECTSOUND)
	{
		ALSound::instance().listenerPosition(newPos);
	}
	else
	{
		ASSERT(NULL, "Trying to use undefined sound system");
	}
	MexPoint3d temp = listenerPosition();
}


//Get the volume of the specified sample
Snd::Volume SndMixer::sampleVolume( const SndSampleHandle& handle ) const
{
    Snd::Volume vol;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        vol = RecRecorderPrivate::instance().playbackSoundSampleVolume();
    else
    {
    	CB_DEPIMPL(SampleList, activeSamples_);

        vol = sampleVolumeNoRecord( handle );

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordSoundSampleVolume( vol );
	}

	return vol;
}

//Get the volume of the specified sample
Snd::Volume SndMixer::sampleVolumeNoRecord( const SndSampleHandle& handle ) const
{
	CB_DEPIMPL(SampleList, activeSamples_);

    Snd::Volume vol = 0;
	if( isAllocated( handle ) )
    {
        vol = activeSamples_[handle]->volume();
    }

	return vol;
}

//Set the specified sample to the required volume
void SndMixer::sampleVolume( const SndSampleHandle& handle, Snd::Volume newVolume)
{
	CB_DEPIMPL(SampleList, activeSamples_);

    if( isAllocated( handle ) )
    {
    	activeSamples_[handle]->volume(newVolume);
	    ASSERT( sampleVolumeNoRecord( handle ) == newVolume, "" );
    }
}
//////////////////////////////////////////////

UtlId SndMixerImpl::noOfChannels_ = 0;

ulong SndMixer::noOfFreeLogicalChannels() const
{
    ulong result;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        result = RecRecorderPrivate::instance().playbackSoundNoOfFreeLogicalChannels();
    else
    {
	    result = noOfFreeLogicalChannelsNoRecord();

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordSoundNoOfFreeLogicalChannels( result );
	}

	return result;
}

ulong SndMixer::noOfFreeLogicalChannelsNoRecord() const
{
    ulong result;

  	CB_DEPIMPL(UtlBoundedIdGenerator*, pSoundIDGenerator_);

    result =  pSoundIDGenerator_->nUnusedIds();

	return result;
}

ulong SndMixer::noOfUsedLogicalChannels() const
{
    ulong occupiedChannels;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        occupiedChannels = RecRecorderPrivate::instance().playbackSoundNoOfUsedLogicalChannels();
    else
    {
    	CB_DEPIMPL(UtlId, noOfChannels_);
	    CB_DEPIMPL(UtlBoundedIdGenerator*, pSoundIDGenerator_);

    	occupiedChannels = noOfChannels_ - pSoundIDGenerator_->nUnusedIds();

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordSoundNoOfUsedLogicalChannels( occupiedChannels );
	}
	return occupiedChannels;
}

MexEulerAngles SndMixer::listenerOrientation() const
{
	MexEulerAngles orientation;
	if(soundOn())
	{
		orientation = ALSound::instance().listenerOrientation();
	}
	return orientation;
}

void SndMixer::listenerOrientation(const MexEulerAngles& newOrientation)
{
	if(soundOn())
	{
		PRE(is3dMixer());
		ALSound::instance().listenerOrientation(newOrientation);
	}
}

MexTransform3d SndMixer::listenerTransform() const
{
	return ALSound::instance().listenerTransform();
}

void SndMixer::listenerTransform(const MexTransform3d& newTransform)
{
	ALSound::instance().listenerTransform(newTransform);
}

//////////////////////////////////////////////////////////////////////


Snd::Volume SndMixer::masterSampleVolume() const
{
	CB_DEPIMPL(Snd::Volume, masterSampleVolume_);

	return masterSampleVolume_;
}

void SndMixer::masterSampleVolume( Snd::Volume newVolume )
{
	CB_DEPIMPL(Snd::Volume, masterSampleVolume_);

	masterSampleVolume_ = newVolume;
	pImpl_->hardwareMasterSampleVolume( newVolume );
	POST( masterSampleVolume() == newVolume );
}

void SndMixer::unloadAll()
{
	pImpl_->freeAllResources();
	SndWaveManager::instance().freeAll();
}

//////////////////////////////////////////////////////////////////////

/* end of SOUND.CPP *****************************/
