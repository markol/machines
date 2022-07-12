/***********************************************************

 A L S A M P L E . C P P
 copyright (c) charybdis 1997 all rights reserved

***********************************************************/

/*
*/

#include "sound/internal/alsample.hpp"

#include "stdlib/string.hpp"
#include "sound/internal/dxs.hpp"
#include "sound/internal/waveform.hpp"
#include "sound/internal/alsound.hpp"
#include "sound/soundmix.hpp"
#include "sound/internal/waveman.hpp"

#include "device/timer.hpp"


#ifndef _INLINE
	#include "sound/internal/alsample.ipp"
#endif

int bobSoundError(
    char*  lpText,	// address of text in message box
    char*  lpCaption,	// address of title of message box
    uint  uType 	// style of message box
   );

//unsigned long getRefCount(IUnknown* pThisInterface);

//Maximum sound buffer size == 800K
const size_t MAX_BUFFER_SIZE = 800000;

extern void debugTiming( const char*, bool );

////////////////////////////////////////////////////////////

ALSample::ALSample( const SndSampleParameters& params )
	:Sample(params),
    alBuffer_(0),
    alSource_(0),
	currentWaveFilePosition_(0),
    sampleParameters_(params),
    loopCount_(params.loopCount()),
    state_(INITIALISED),
	bufferIncrement_(0),
    bufferSize_(MAX_BUFFER_SIZE),
	startOffset_(params.startOffset()),
	internalTimer_(NULL),
	stopAtEnd_(false),
	usingNotificationForStop_(false),
	sampleResidence_(UNKNOWN)
{

//	SOUND_STREAM("ALSample::ALSample params.id() " << params.id().pathname() << std::endl );
	static unsigned int sampleCount = 0;
	//SOUND_STREAM("Creating " << _STATIC_CAST(unsigned int, this) << " smp no " << ++sampleCount << std::endl);
	SOUND_STREAM("Creating " << _REINTERPRET_CAST(size_t, this) << " smp no " << ++sampleCount << std::endl);

	ASSERT(bufferSize_%2 == 0, "BufferSize must be even");

	//We are only going to use the timer if the sample is
	//to be played for a specified time
	if(params.duration() > 0.0)
	{
		internalTimer_ = _NEW(DevTimer());
		internalTimer_->time(0);
	}

	//Obtain a pointer to the waveform
    waveform_ = _CONST_CAST(SndWaveform*, SndWaveManager::instance().getWaveForm(params.id()));

  	//If our wave is smaller than the maximum buffer size
	if(waveform_->dataSize() < MAX_BUFFER_SIZE)
    {
    	//Set the buffer size to the size of the wave
    	bufferSize_ = waveform_->dataSize();
        //We will not be streaming data from disk
        storageType_ = WAVE_STATIC;
		//If the wave is to be played infintely or just once
		if(loopCount_ < 2)
		{
			//We don't need notification
			usingNotificationForStop_ = true;
		}
    }
    else
    {
        //We will be streaming data from disk
//        storageType_ = WAVE_STREAMING;
//      TODO: no streaming for now, loads big wavs into memory
        storageType_ = WAVE_STATIC; usingNotificationForStop_ = true;
    }


//	//Set the format as the format of the supplied waveform
//    WaveFormat* pTempWaveFormat = &(waveform_->format());
//	format_ = *pTempWaveFormat;
//    _DELETE( pTempWaveFormat );
	//Create the DirectSound buffer
    bool preloaded = true;

    alBuffer_ = SndWaveManager::instance().getSoundBuffer(params.id(), preloaded);

	if(!preloaded)
	{
		// Load into buffer
		char* pBufferData = _NEW_ARRAY(char, waveform_->dataSize());
		uint check = waveform_->read( pBufferData, 0, waveform_->dataSize());
		alBufferData(alBuffer_, waveform_->toALformat(), pBufferData,
            waveform_->dataSize(), waveform_->format()->sampleRateHz());
        alTestError("failed to load buffer data");
        _DELETE_ARRAY(pBufferData);
	}
	alGenSources((ALuint)1, &alSource_);
	alTestError("source generation");

	if(isValid())
    {
		//If we are dealing with a 3D sound buffer
		if(sampleParameters_.is3dSound())
		{
			if (alSource_)
			{
                // The distance that the source will be the loudest (if the listener is
                // closer, it won't be any louder than if they were at this distance)
                alSourcei(alSource_, AL_REFERENCE_DISTANCE, params.minAudibleDistance());
                // The distance that the source will be the quietest (if the listener is
                // farther, it won't be any quieter than if they were at this distance)
                alSourcei(alSource_, AL_MAX_DISTANCE, params.maxAudibleDistance());
			}
			else
				RICHARD_STREAM("Unable to initialise 3d Listener" << std::endl);

			//Set the samples 3D position to the requested MexPoint
			newPosition(params.initialPosition());
		}
		else
		{
            alSourcei(alSource_, AL_SOURCE_RELATIVE, AL_TRUE);
            alSource3f(alSource_, AL_POSITION, 0.0f, 0.0f, 0.0f);
		}
        alSourcei(alSource_, AL_LOOPING, AL_FALSE);
        alTestError("source looping");

		alSourcei(alSource_, AL_BUFFER, alBuffer_);
        alTestError("buffer binding");

		int offset = params.startOffset() * waveform_->format()->nBytesPerSec();
		if(offset % 2 == 1)
		{
			offset -=1;
		}
		ASSERT(offset < waveform_->dataSize(), "Offset point is too large");
        if(storageType_ == WAVE_STATIC)
		{
			currentWaveFilePosition_ = 0;

		}
		else
		{
			currentWaveFilePosition_ = offset;
		}

		volume(0);
    }
	else
	{
		SOUND_STREAM("Invalid sample construction" << std::endl );
	}
}

/** VIRTUAL **/
ALSample::~ALSample()
{
	static unsigned int sampleDCount = 0;
	stop();


//	format_.format( 0, 0.0f, 0 );
	if(internalTimer_)
	{
	    _DELETE(internalTimer_);
	}

	//Free or at least decrement the ref count of this waveform by one.
	SndWaveManager::instance().freeWaveForm(sampleParameters_.id());

	//If the sound buffer is alive (non NULL)
	if(alSource_)
	{
        alDeleteSources(1, &alSource_);
        alSource_ = 0;
	}
	if(alBuffer_)
	{
        SndWaveManager::instance().freeSoundBuffer( sampleParameters_.id() );
        alBuffer_ = 0;
	}
}

////////////////////////////////////////////////////////////
Sample::SampleState ALSample::update()
{
    Sample::SampleState returnState = Sample::SampleState::STOPPED;

	//If we have a sample ready to be initialised we don't want to bother
	//looking at any other querying, just pass its state atraight through to
	//the sound mixer that will then call play.
	//Likewise if a sound has stopped, it is waiting for something
	//external to be done to it.
   	if(state_ == INITIALISED || state_ == STOPPED)
	{
	    returnState = _STATIC_CAST( Sample::SampleState, state_ );
    	return returnState;
	}

	//If the wave is being streamed off disk and we have received a notification
	//that the first notification point has been reached, i.e. the sample is half way
	//through the buffer, then load the next section of the sample into the buffer

	ALint sourceState;
	alGetSourcei(alSource_, AL_SOURCE_STATE, &sourceState);
	alTestError("source state get");

	if(sourceState != AL_PLAYING)
	{
        //If the whole sample is in memory
        if(storageType_ == WAVE_STATIC)
        {
            if(usingNotificationForStop_ )
            {
                state_ = STOPPED;
            }
            else if(!loopForever())
            {
                //Decrement the loop count, and if we have played
                //the last cycle then stop the sample
                if(loopCount_ > 0)
                    --loopCount_;
                if(loopCount_ == 1)
                {
                    state_ = STOPPED;
                    loopForever_ = false;
                    alSourcePlay(alSource_);
                }
            }
        }
    }
    switch(state_)
    {
        case PLAYING:
		{
			if(internalTimer_)
			{
                //  We must use the "NoRecord" version of the time function to
                //  avoid the recording getting confused when playing sounds.
				//If this samples duration has been exceeded then stop it
				if(internalTimer_->timeNoRecord() > sampleParameters_.duration())
				{
					stop();
					_DELETE(internalTimer_);
					internalTimer_ = NULL;
				}
			}
		}
        case STOPPED:
		case PAUSED:
        	//returnState = state_;
            returnState = _STATIC_CAST( Sample::SampleState, state_ );
            break;

        case PLAYING_OUT:
        case STOPPING:
			//As far as the sound mixer calling this method is
			//concerned this object is still playing
        	returnState = Sample::PLAYING;
        	break;
        default:
        	ASSERT(false, "Bad case");
            break;
    }
    return returnState;
}

/** VIRTUAL **/
void ALSample::play()
{
	PRE(isValid());

//   	currentWaveFilePosition_ = 0;

	//Play the sample
	if(storageType_ == WAVE_STATIC && (loopCount_ == 1))
	{
        alSourcePlay(alSource_);
	}
	else
	{
        alSourcei(alSource_, AL_LOOPING, AL_TRUE);
        alTestError("source looping");
        alSourcePlay(alSource_);
	}
    if(!alTestError("source play"))
    {
    	bobSoundError( "Can't play" , "Error", 0);
    }
    else
		state_ = PLAYING;

	//Set the volume to the requested level
	volume(sampleParameters_.volume());
	update();
}

////////////////////////////////////////////////////////////
void ALSample::stopAtEnd()
{
	if(loopCount_ != 1)
	{
//		stopAtEnd_ = true;
		loopCount_ = 1;
		loopForever_ = false;

        alSourcePlay(alSource_);
	}
}

void ALSample::stop()
{
	ASSERT(alSource_, "Bad Sample")
	//Immediatly treminate buffer playback
    alSourceStop(alSource_);

	state_ = STOPPED;
}

////////////////////////////////////////////////////////////

void ALSample::newPosition(const MexVec3& newPos)
{
	PRE(alSource_);
	//Set the 3D sample position
	alSourcef(alSource_, AL_PITCH, 1);
	alTestError("source pitch");
	alSource3f(alSource_, AL_POSITION, newPos.x(), newPos.y(), newPos.z());
	alTestError("source position");
	alSource3f(alSource_, AL_VELOCITY, 0, 0, 0);
	alTestError("source velocity");

}

////////////////////////////////////////////////////////////

void ALSample::position3D( MexVec3* position )
{
	//Obtain the current sample position
	ALfloat x, y, z;
	alGetSource3f(alSource_, AL_POSITION, &x, &y, &z );
	position->x(x);
	position->y(y);
	position->z(z);
}

////////////////////////////////////////////////////////////
void ALSample::volume( int v )
{
	//Convert the percentile value to an attenuation value
//	LONG newVolume = (v-100) * ((DSBVOLUME_MAX - (-5000))/100);
	//Set the new percentile volume
	volume_ = v;

	//If the samples volume is genuinely being changed i.e. it is not
	//being silenced (shifted outside the polyphony range),
	//then set the unSilenced volume to the current volume
	if(!isSilenced_)
		unSilencedVolume_ = volume_;

    ALfloat fVol = (float)(volume_) * SndMixer::instance().masterSampleVolume() / 10000.0f ;
    alSourcef(alSource_, AL_GAIN, fVol);
    alTestError("source gain");
}

void ALSample::silence()
{
//	ASSERT(false, "Unused at present");)
//	internalTimer_->time(0);
//	pause();
}

void ALSample::unsilence()
{
//	ASSERT(false, "Unused at present");)
//	double elapsedTimeSecs = internalTimer_->time();
//	int nReps = elapsedTimeSecs / (double)length();
//	double dRelativeTime = elapsedTimeSecs - ((double)nReps*(double)length());
//	int offsetPoint = format_.nBytesPerSec() * dRelativeTime;
//	restart(offsetPoint);
}

void ALSample::restart( int offset )
{
	PRE(offset <= bufferSize_);
	currentWaveFilePosition_ = offset;
    alSourcei(alSource_, AL_LOOPING, AL_TRUE);
    alTestError("source looping");
    alSourceRewind(alSource_);
    alSourcePlay(alSource_);
}
