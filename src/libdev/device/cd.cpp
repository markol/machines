/*
 * C D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "afx/sdlapp.hpp"

#include "device/cd.hpp"
#include "device/cdlist.hpp"
#include "mathex/random.hpp"
#include "system/pathname.hpp"
#include <AL/alure.h>

#define STREAM_NUM_BUFFERS 3
#define STREAM_BUFFER_SIZE 250000
#define STREAM_UPDATE_INTERVAL 0.125f

class DevCDImpl
{
	enum CDVOLUME
	{
		MAX_CDVOLUME = 65535,
		MIN_CDVOLUME = 0
	};

	/*HMIXER hMixer_;
	MIXERLINE			mixerLine_;
    MIXERLINECONTROLS   mixerLineControl_;
    MIXERCONTROL        mixerControl_;
	MIXERCONTROLDETAILS mixerControlDetails_;
	MIXERCONTROLDETAILS_UNSIGNED* pMixerValues_;*/
	alureStream*        stream_;
    ALuint              source_;

	unsigned int savedVolume_;

	DevCDPlayList* pPlayList_;

	bool haveMixer_;

	DevCDTrackIndex randomStartTrack_;
	DevCDTrackIndex randomEndTrack_;
	MexBasicRandom	randomGenerator_;

	friend class DevCD;
};
/*
MCIERROR mciSendCommandWithDebugStreaming(MCIDEVICEID IDDevice, UINT uMsg, DWORD fdwCommand, DWORD dwParam, size_t lineNum )
{
	DWORD dwReturn = mciSendCommand( IDDevice, uMsg, fdwCommand, dwParam );

    if (dwReturn != 0)
    {
	 	char buffer[128];
		if ( mciGetErrorString( dwReturn, buffer, sizeof(buffer) ) )
		{
			DEBUG_STREAM( DIAG_NEIL, "CDError : " << lineNum << " " << buffer << std::endl );
		}
		else
		{
			DEBUG_STREAM( DIAG_NEIL, "CDError : " << lineNum << " Failed to get error message from mciGetErrorString" << std::endl );
		}
    }

	return dwReturn;
}*/

// static
DevCD& DevCD::instance()
{
    static DevCD instance;
    return instance;
}

DevCD::DevCD():
status_(NORMAL),
needsUpdate_(false),
pImpl_(_NEW(DevCDImpl()))
{
/*	CB_DEPIMPL(HMIXER, hMixer_);
	CB_DEPIMPL(MIXERLINE, mixerLine_);
    CB_DEPIMPL(MIXERLINECONTROLS, mixerLineControl_);
    CB_DEPIMPL(MIXERCONTROL, mixerControl_);
	CB_DEPIMPL(MIXERCONTROLDETAILS, mixerControlDetails_);
	CB_DEPIMPL(MIXERCONTROLDETAILS_UNSIGNED*, pMixerValues_);*/
	CB_DEPIMPL(alureStream*, stream_);
    CB_DEPIMPL(ALuint, source_);
	CB_DEPIMPL(unsigned int, savedVolume_);
	CB_DEPIMPL(DevCDPlayList*, pPlayList_);
	CB_DEPIMPL(bool, haveMixer_);
	CB_DEPIMPL(MexBasicRandom, randomGenerator_);

	haveMixer_ = false;
	savedVolume_ = 20;

//	int nDevices = mixerGetNumDevs();

	bool noErrors = true;

	/*if(!alureInitDevice(NULL, NULL)) //Device context is created by alsound on sndmixer init
    {
        std::cerr << "Failed to open OpenAL device: " << alureGetErrorString() << std::endl;
        noErrors = false;
    }*/

    alGenSources(1, &source_);
    if(alGetError() != AL_NO_ERROR)
    {
        std::cerr << "Failed to create OpenAL source for music mixer!" << std::endl;
        //alureShutdownDevice();
        noErrors = false;
    }
    else
    {
        alureStreamSizeIsMicroSec(AL_TRUE);
        alureUpdateInterval(STREAM_UPDATE_INTERVAL);
    }
    stream_ = NULL;
    haveMixer_ = noErrors;

	/*HRESULT hr;

	if(nDevices > 0)
	{
		//Open the mixer object
		//Attach the mixer to the desktop window otherwise we screw up AFX
		hr = mixerOpen( &hMixer_, 0, (DWORD)GetDesktopWindow(), 0, CALLBACK_WINDOW );
		if(hr != MMSYSERR_NOERROR)
		{
			RICHARD_STREAM("Couldn't open mixer" << std::endl);
		}
		else
		{
			RICHARD_STREAM("Opened mixer" << std::endl);
		}

		mixerLine_.cbStruct = sizeof( MIXERLINE );
		mixerLine_.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;

		if(hr == MMSYSERR_NOERROR)
		{
		    // TODO fix it
			//hr = mixerGetLineInfo(hMixer_, &mixerLine_, MIXER_GETLINEINFOF_COMPONENTTYPE);
		}
		if(hr == MMSYSERR_NOERROR)
		{
			RICHARD_STREAM("Got line info" << std::endl);
			RICHARD_STREAM("Number of chans " << mixerLine_.cChannels << std::endl);
		}

		mixerLineControl_.cbStruct       = sizeof( mixerLineControl_ );
		mixerLineControl_.dwLineID       = mixerLine_.dwLineID;
		mixerLineControl_.dwControlType  = MIXERCONTROL_CONTROLTYPE_VOLUME;
		mixerLineControl_.cControls      = 1;
		mixerLineControl_.cbmxctrl       = sizeof( mixerControl_ );
		mixerLineControl_.pamxctrl       = &mixerControl_;

		if(hr == MMSYSERR_NOERROR)
		{
			hr = mixerGetLineControls( ( HMIXEROBJ )hMixer_, &mixerLineControl_,
			MIXER_GETLINECONTROLSF_ONEBYTYPE );
		}
		if(hr == MMSYSERR_NOERROR)
		{
			RICHARD_STREAM("Got line control" << std::endl);
		}
		else
		{
			RICHARD_STREAM("Couldn't get line control 8" << std::endl);
		}

		pMixerValues_ = _NEW_ARRAY(MIXERCONTROLDETAILS_UNSIGNED, mixerLine_.cChannels);

		mixerControlDetails_.cbStruct       = sizeof( MIXERCONTROLDETAILS );
	    mixerControlDetails_.dwControlID    = mixerControl_.dwControlID;
	    mixerControlDetails_.cChannels      = mixerLine_.cChannels;
	    mixerControlDetails_.cMultipleItems = 0;
	    mixerControlDetails_.cbDetails      = sizeof( MIXERCONTROLDETAILS_UNSIGNED );
	    mixerControlDetails_.paDetails      = pMixerValues_;

		if(hr == MMSYSERR_NOERROR)
		{
		    hr = mixerGetControlDetails( ( HMIXEROBJ )hMixer_, &mixerControlDetails_,
				MIXER_GETCONTROLDETAILSF_VALUE );
		}
		if(hr == MMSYSERR_NOERROR)
		{
			RICHARD_STREAM("Got control details" << std::endl);
		}
		else
		{
			RICHARD_STREAM("Couldn't get control details" << std::endl);
		}
	}

	if(hr == MMSYSERR_NOERROR)
		haveMixer_ = true;

    DWORD dwReturn;
    MCI_SET_PARMS mciSetParms;
    MCI_OPEN_PARMS mciOpenParms;
    mciOpenParms.lpstrDeviceType = "cdaudio";
	dwReturn = mciSendCommandWithDebugStreaming(NULL, MCI_OPEN, MCI_OPEN_TYPE, (DWORD)(LPVOID) &mciOpenParms, __LINE__ );
/////DEBUG STUFF
//	int nDevices = auxGetNumDevs();
//	RICHARD_STREAM("nDevices " << nDevices << std::endl);
//	for(int i = 0; i<nDevices; ++i)
//	{
//		AUXCAPS theseCaps;
//		auxGetDevCaps(i, &theseCaps, sizeof(AUXCAPS));
//		RICHARD_STREAM("****************" << endl);
//		RICHARD_STREAM("Device " << i << endl);
//		RICHARD_STREAM("Product name " << theseCaps.szPname << endl);
//		if(theseCaps.wTechnology && AUXCAPS_AUXIN)
//			RICHARD_STREAM("Auxiliary in dev" << endl);
//		if(theseCaps.wTechnology && AUXCAPS_CDAUDIO)
//			RICHARD_STREAM("CDAudio in dev" << endl);
//		if(theseCaps.dwSupport && AUXCAPS_VOLUME)
//			RICHARD_STREAM("Vol support in dev" << endl);
//		if(theseCaps.dwSupport && AUXCAPS_LRVOLUME)
//			RICHARD_STREAM("LR vol support in dev" << endl);
//		RICHARD_STREAM("****************" << endl);
//	}
////////////////

    if (dwReturn == 0)
   	{
	    // Device opened successfully, get the device ID.
	    deviceID_ = mciOpenParms.wDeviceID;

	    mciSetParms.dwTimeFormat = MCI_FORMAT_TMSF;
		dwReturn = mciSendCommandWithDebugStreaming( deviceID_, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)(LPVOID) &mciSetParms, __LINE__);

	    if(dwReturn != 0)
	    {
	        mciSendCommandWithDebugStreaming(deviceID_, MCI_CLOSE, 0, NULL, __LINE__);
	    }
	}
		//Save the appVolume
	if(haveMixer_)
	{
		savedVolume_ = volume();
//		savedVolume_ = pMixerValues_[0].dwValue;
		RICHARD_STREAM("savedVolume_ " << savedVolume_ << std::endl);
	}
*/
	pPlayList_ = _NEW(DevCDPlayList(numberOfTracks()));

	randomGenerator_.seedFromTime();
}

DevCD::~DevCD()
{
/*	CB_DEPIMPL(HMIXER, hMixer_);
	CB_DEPIMPL(MIXERLINE, mixerLine_);
    CB_DEPIMPL(MIXERLINECONTROLS, mixerLineControl_);
    CB_DEPIMPL(MIXERCONTROL, mixerControl_);
	CB_DEPIMPL(MIXERCONTROLDETAILS, mixerControlDetails_);
	CB_DEPIMPL(MIXERCONTROLDETAILS_UNSIGNED*, pMixerValues_);*/
	CB_DEPIMPL(alureStream*, stream_);
    CB_DEPIMPL(ALuint, source_);
	CB_DEPIMPL(unsigned int, savedVolume_);
	CB_DEPIMPL(DevCDPlayList*, pPlayList_);

	stopPlaying();

	RICHARD_STREAM("Setting vol to saved volume " << savedVolume_ << std::endl);
	volume(savedVolume_);

//	mixerClose(hMixer_);

    alDeleteSources(1, &source_);
    alureDestroyStream(stream_, 0, NULL);

    //alureShutdownDevice();

//	_DELETE_ARRAY(pMixerValues_);
	_DELETE( pPlayList_ );
	_DELETE( pImpl_ );

    TEST_INVARIANT;
}

void DevCD::update()
{
    if(needsUpdate_)
    {
        handleMessages(DevCD::SUCCESS, 0);
        needsUpdate_ = false;
        //std::cout << "music mixer update" << std::endl;
    }
}

bool DevCD::isPlayingAudioCd() const
{
	// This function is somewhat unreliable. If you query the CD for the existance
	// of a file, for example, whilst the CD is playing a music track then this
	// function returns false for a short while whilst the CD seeks back to its position in
	// the music track after looking up the existance of the file.
	/*DWORD dwReturn;

	MCI_STATUS_PARMS mciStatusParms;

    mciStatusParms.dwItem = MCI_STATUS_MEDIA_PRESENT;

    dwReturn = mciSendCommandWithDebugStreaming( deviceID_, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID) &mciStatusParms, __LINE__);
	bool isAudioCd = mciStatusParms.dwReturn;

	MCI_STATUS_PARMS mciStatusParms2;
    mciStatusParms2.dwItem = MCI_STATUS_MODE ;
    mciSendCommandWithDebugStreaming( deviceID_, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID) &mciStatusParms2, __LINE__);
	bool isPlaying = (mciStatusParms2.dwReturn == MCI_MODE_PLAY);
*/
//    return (isAudioCd and isPlaying);
    CB_DEPIMPL(ALuint, source_);

    ALint sourceState;
    alGetSourcei(source_, AL_SOURCE_STATE, &sourceState);
    return sourceState == AL_PLAYING;
}

bool DevCD::supportsVolumeControl() const
{
	CB_DEPIMPL(bool, haveMixer_);
	return haveMixer_;
}

Volume DevCD::volume() const
{
	Volume percentageVolume = 0;
	if(supportsVolumeControl())
	{
		/*CB_DEPIMPL(HMIXER, hMixer_);
		CB_DEPIMPL(MIXERLINE, mixerLine_);
	    CB_DEPIMPL(MIXERLINECONTROLS, mixerLineControl_);
	    CB_DEPIMPL(MIXERCONTROL, mixerControl_);
		CB_DEPIMPL(MIXERCONTROLDETAILS, mixerControlDetails_);
		CB_DEPIMPL(MIXERCONTROLDETAILS_UNSIGNED*, pMixerValues_);*/
		CB_DEPIMPL(unsigned int, savedVolume_);

	    /*HRESULT hr = mixerGetControlDetails( ( HMIXEROBJ )hMixer_, &mixerControlDetails_,
			MIXER_GETCONTROLDETAILSF_VALUE );
		if(hr == MMSYSERR_NOERROR)
		{
			RICHARD_STREAM("Got control details" << std::endl);
		}
		else
		{
			RICHARD_STREAM("Couldn't get control details" << std::endl);
		}

		float theVolume = pMixerValues_[0].dwValue;
		percentageVolume = (((float)theVolume / (float)DevCDImpl::MAX_CDVOLUME) * 100.0) + 0.5;
		RICHARD_STREAM("Current vol " << theVolume << std::endl);*/
		percentageVolume = savedVolume_;
		RICHARD_STREAM("Current percentage vol " << percentageVolume << std::endl);
	}
	return percentageVolume;
}

void DevCD::volume( Volume newLevel )
{
	if(supportsVolumeControl())
	{
		/*CB_DEPIMPL(HMIXER, hMixer_);
		CB_DEPIMPL(MIXERLINE, mixerLine_);
	    CB_DEPIMPL(MIXERLINECONTROLS, mixerLineControl_);
	    CB_DEPIMPL(MIXERCONTROL, mixerControl_);
		CB_DEPIMPL(MIXERCONTROLDETAILS, mixerControlDetails_);
		CB_DEPIMPL(MIXERCONTROLDETAILS_UNSIGNED*, pMixerValues_);*/
		CB_DEPIMPL(unsigned int, savedVolume_);
		CB_DEPIMPL(ALuint, source_);

		if(newLevel > 100)
		{
			newLevel = 100;
		}
        savedVolume_ = newLevel;
        ALfloat fVol = (float)(savedVolume_) / 100.0f; // Maybe use log model instead of linear?
        alSourcef(source_, AL_GAIN, fVol);
		/*unsigned int theVolume = ((float)newLevel / 100.0) * DevCDImpl::MAX_CDVOLUME;

		pMixerValues_[0].dwValue = theVolume;
		pMixerValues_[1].dwValue = theVolume;

		HRESULT hr;
		hr = mixerSetControlDetails( ( HMIXEROBJ )hMixer_, &mixerControlDetails_,
			MIXER_SETCONTROLDETAILSF_VALUE );
		if(hr == MMSYSERR_NOERROR)
		{
			RICHARD_STREAM("Percentage volume set to " << newLevel << std::endl);
			RICHARD_STREAM("Volume set to " << theVolume << std::endl);
		}
		else
		{
			RICHARD_STREAM("Couldn't set volume" << std::endl);
		}*/
		RICHARD_STREAM("NewVolume set to " << volume() << std::endl);
	}
}

DevCDTrackIndex DevCD::currentTrackIndex() const
{
	PRE( isPlayingAudioCd() );

	/*MCI_STATUS_PARMS mciStatusParms;

    mciStatusParms.dwItem = MCI_STATUS_CURRENT_TRACK;

    mciSendCommandWithDebugStreaming( deviceID_, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID) &mciStatusParms, __LINE__);

    return (mciStatusParms.dwReturn - 1);*/
    return trackPlaying_;
}

DevCDTrackIndex DevCD::numberOfTracks() const
{
	/*MCI_STATUS_PARMS mciStatusParms;

   	mciStatusParms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
	mciSendCommandWithDebugStreaming( deviceID_, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID) &mciStatusParms, __LINE__);
	return mciStatusParms.dwReturn;*/
	return 10+1; // Hardcoded number
}

Seconds DevCD::currentTrackLengthInSeconds() const
{
	PRE( isPlayingAudioCd() );

	/*MCI_STATUS_PARMS mciStatusParms;

    mciStatusParms.dwItem = MCI_STATUS_LENGTH;
	mciStatusParms.dwTrack = currentTrackIndex()+1;

	mciSendCommandWithDebugStreaming( deviceID_, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD)(LPVOID) &mciStatusParms, __LINE__ );

    return (MCI_MSF_MINUTE(mciStatusParms.dwReturn)*60) +
    	    (MCI_MSF_SECOND(mciStatusParms.dwReturn));*/
}

//TBD - Unable to implement through MCI
Seconds DevCD::currentTrackRunningTime() const
{
	PRE( isPlayingAudioCd() );
//
//	MCI_STATUS_PARMS mciStatusParms;
//
//    mciStatusParms.dwItem = MCI_STATUS_POSITION;
//
//	mciSendCommand(deviceID_, MCI_STATUS,
//							 MCI_STATUS_ITEM,
//							 (DWORD)(LPVOID) &mciStatusParms);
//
//
//	return mciStatusParms.dwReturn;
	ASSERT(false, "Function not implemented");
	return 0;
}

Seconds DevCD::currentTrackTimeRemaining() const
{
	PRE( isPlayingAudioCd() );
	ASSERT(false, "Function not implemented");
	return 0;
}

void DevCD::play()
{
    /*DWORD dwReturn;
	DWORD bTrack = 1;
    MCI_PLAY_PARMS mciPlayParms;

    mciPlayParms.dwFrom = MCI_MAKE_TMSF(bTrack, 0, 0, 0);
    mciPlayParms.dwTo = MCI_MAKE_TMSF(numberOfTracks(), 0, 0, 0);
    mciPlayParms.dwCallback = (DWORD) AfxWin95App::win95Instance().window();

	dwReturn = mciSendCommandWithDebugStreaming( deviceID_, MCI_PLAY, MCI_FROM | MCI_TO | MCI_NOTIFY, (DWORD)(LPVOID) &mciPlayParms, __LINE__);
    if (dwReturn != 0)
    {
        mciSendCommandWithDebugStreaming(deviceID_, MCI_CLOSE, 0, NULL, __LINE__);
    }*/
    play(1);
}

void DevCD::playFrom( DevCDTrackIndex track )
{
	PRE( track >= 0 and track < numberOfTracks() );
/*
	track += 1;

    DWORD dwReturn;
    MCI_PLAY_PARMS mciPlayParms;

    mciPlayParms.dwFrom = MCI_MAKE_TMSF(track, 0, 0, 0);
    mciPlayParms.dwTo = MCI_MAKE_TMSF(numberOfTracks(), 0, 0, 0);

    if(track == numberOfTracks())
    {
    	MCI_STATUS_PARMS mciStatusParms;
    	mciStatusParms.dwItem = MCI_STATUS_LENGTH;
        mciStatusParms.dwTrack = track;
        mciSendCommandWithDebugStreaming( deviceID_, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD)(LPVOID)&mciStatusParms, __LINE__);

		int secs = MCI_MSF_SECOND(mciStatusParms.dwReturn);
		int mins = MCI_MSF_MINUTE(mciStatusParms.dwReturn);

	    mciPlayParms.dwTo = MCI_MAKE_TMSF(track, mins, secs, 0);
    }
    mciPlayParms.dwCallback = (DWORD) AfxWin95App::win95Instance().window();
	dwReturn = mciSendCommandWithDebugStreaming(deviceID_, MCI_PLAY, MCI_FROM | MCI_TO | MCI_NOTIFY, (DWORD)(LPVOID) &mciPlayParms, __LINE__);
    if (dwReturn != 0)
    {
        mciSendCommandWithDebugStreaming(deviceID_, MCI_CLOSE, 0, NULL, __LINE__);
    }*/
    play(track);
}

static void eosCallback(void *unused, ALuint unused2)
{
    (void)unused;
    (void)unused2;
    //DevCD::instance().handleMessages(DevCD::SUCCESS, 0); // This called from alure thread creates little problems
    DevCD::instance().needsUpdate_ = true;
    //std::cout << "Done playing track" << std::endl;
}

void DevCD::play( DevCDTrackIndex track, bool repeat /* = false */ )
{
	CB_DEPIMPL(alureStream*, stream_);
    CB_DEPIMPL(ALuint, source_);
    CB_DEPIMPL(unsigned int, savedVolume_);

	PRE( track >= 0 and track < numberOfTracks() );

	trackPlaying_ = track;

	//track += 1; // MCI interface deals with tracks from 1 to X we deal with tracks from 0 to X-1
/*
    DWORD dwReturn;
    MCI_PLAY_PARMS mciPlayParms;

    mciPlayParms.dwFrom = MCI_MAKE_TMSF(track, 0, 0, 0);
   	mciPlayParms.dwTo = MCI_MAKE_TMSF(track + 1, 0, 0, 0);
	mciPlayParms.dwCallback = (DWORD) AfxWin95App::win95Instance().window();

    if(track == numberOfTracks())
    {
    	MCI_STATUS_PARMS mciStatusParms;
    	mciStatusParms.dwItem = MCI_STATUS_LENGTH;
        mciStatusParms.dwTrack = track;
        mciSendCommandWithDebugStreaming(deviceID_, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD)(LPVOID)&mciStatusParms, __LINE__);

		int secs = MCI_MSF_SECOND(mciStatusParms.dwReturn);
		int mins = MCI_MSF_MINUTE(mciStatusParms.dwReturn);

	    mciPlayParms.dwTo = MCI_MAKE_TMSF(track, mins, secs, 0);
    }
    mciPlayParms.dwCallback = (DWORD) AfxWin95App::win95Instance().window();
	dwReturn = mciSendCommandWithDebugStreaming(deviceID_, MCI_PLAY, MCI_FROM | MCI_TO | MCI_NOTIFY, (DWORD)(LPVOID) &mciPlayParms, __LINE__);
    if (dwReturn != 0)
    {
        mciSendCommandWithDebugStreaming(deviceID_, MCI_CLOSE, 0, NULL, __LINE__);
    }*/
    if(stream_)
        alureDestroyStream(stream_, 0, NULL);
    if(savedVolume_ <= 0) // Muted
        return;

    char fileName[40];
    sprintf(fileName, "sounds/music/track%d.ogg", trackPlaying_);
    SysPathName filePath(fileName);
    stream_ = alureCreateStreamFromFile(filePath.pathname().c_str(), STREAM_BUFFER_SIZE, 0, NULL);

    if(!stream_)
    {
        std::cerr << "Could not load " << filePath.pathname() <<" reason: " << alureGetErrorString() << std::endl;
        return;
    }

    ALfloat fVol = (float)(savedVolume_) / 100.0f;
    alSourcef(source_, AL_GAIN, fVol);
    if(!alurePlaySourceStream(source_, stream_, STREAM_NUM_BUFFERS, 0, eosCallback, NULL))
    {
        std::cerr << "Failed to play stream: " << alureGetErrorString() << std::endl;
        return;
    }
    //std::cout << "Playing track: " << trackPlaying_ << " volume: " << savedVolume_ << std::endl;

	if ( repeat )
		status_ = REPEAT;
	else
		status_ = SINGLE;
}

void DevCD::play( const DevCDPlayList& params )
{
	CB_DEPIMPL(DevCDPlayList*, pPlayList_);

	//Naughty and evil, replace with a copy construction
	*pPlayList_ = params;
	pPlayList_->reset();
	play(pPlayList_->firstTrack());

	status_ = PROGRAMMED;
}

void DevCD::stopPlaying()
{
//    mciSendCommandWithDebugStreaming(deviceID_, MCI_STOP, NULL, NULL, __LINE__);
    CB_DEPIMPL(ALuint, source_);
    alureStopSource(source_, AL_FALSE);
}

void DevCD::handleMessages( CDMessage message, unsigned int devID)
{
	CB_DEPIMPL(DevCDPlayList*, pPlayList_);
	CB_DEPIMPL(MexBasicRandom, randomGenerator_);
	CB_DEPIMPL(DevCDTrackIndex, randomStartTrack_);
	CB_DEPIMPL(DevCDTrackIndex, randomEndTrack_);

	switch(message)
	{
		case ABORT:
		break;

		case FAIL:
		break;

		case SUCCESS:
		{
			if(status_ == PROGRAMMED)
			{
				if(not pPlayList_->isFinished())
				{
					play(pPlayList_->nextTrack());
				}
			}
			else if ( status_ == REPEAT )
			{
				play( trackPlaying_, true );
			}
			else if ( status_ == RANDOM )
			{
				if ( randomStartTrack_ < numberOfTracks() )
				{
					// Make sure we're not asking it to randomise a number outside the range of tracks
					// on the CD.
					DevCDTrackIndex tmpEndTrack = std::min( numberOfTracks(), randomEndTrack_ );

					if ( randomStartTrack_ < tmpEndTrack )
					{
						// Make sure we don't play the same track twice (unless it is the only track)
						DevCDTrackIndex trackToPlay = trackPlaying_;
						while ( trackToPlay == trackPlaying_ and
								( trackToPlay != randomStartTrack_ or trackToPlay != tmpEndTrack ) )
						{
							trackToPlay = mexRandomInt( &randomGenerator_, randomStartTrack_, tmpEndTrack );
						}
						play( trackToPlay );
						status_ = RANDOM; // 'play' sets the status_ to SINGLE
					}
				}
			}
			break;
		}

		case SUPERSEDED:
		break;

		case UNKNOWN:
		break;

		default:
		break;
	}
}

bool DevCD::isAudioCDPresent()
{
	CB_DEPIMPL(unsigned int, savedVolume_);
	/*DWORD dwReturn;

	MCI_STATUS_PARMS mciStatusParms;

    mciStatusParms.dwItem = MCI_STATUS_MODE;

    dwReturn = mciSendCommandWithDebugStreaming( deviceID_, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID) &mciStatusParms, __LINE__);

	bool cdPresent = (mciStatusParms.dwReturn != MCI_MODE_NOT_READY) &&
					 (mciStatusParms.dwReturn != MCI_MODE_OPEN);

    return cdPresent;*/
    // If music is muted then just say no
    if(savedVolume_ <= 0)
        return false;
    return true;
}

ostream& operator <<( ostream& o, const DevCD& devCD)
{
	o << "Number of tracks " << devCD.numberOfTracks() << "\n"
	<< "Current Track " << devCD.currentTrackIndex() << "\n"
	<< "Track time " << devCD.currentTrackLengthInSeconds() << "\n"
	<< "Track running time " << devCD.currentTrackRunningTime() << "\n"
	<< "Track remaining time " << devCD.currentTrackTimeRemaining() << std::endl;

	return o;
}

void DevCD::randomPlay( DevCDTrackIndex startTrack, DevCDTrackIndex endTrack, DevCDTrackIndex firstTrack /*= -1*/ )
{
	CB_DEPIMPL(DevCDTrackIndex, randomStartTrack_);
	CB_DEPIMPL(DevCDTrackIndex, randomEndTrack_);
	CB_DEPIMPL(MexBasicRandom, randomGenerator_);

	PRE( startTrack >= 0 );
	PRE( startTrack <= endTrack );

    randomStartTrack_ = startTrack;
	randomEndTrack_ = endTrack + 1;

	if ( firstTrack != -1 )
	{
		play( firstTrack );
	}
	else
	{
		if ( randomStartTrack_ < numberOfTracks() )
		{
			// Make sure we're not asking it to randomise a number outside the range of tracks
			// on the CD.
			DevCDTrackIndex tmpEndTrack = std::min( numberOfTracks(), randomEndTrack_ );

			if ( randomStartTrack_ < tmpEndTrack )
			{
				DevCDTrackIndex trackToPlay = mexRandomInt( &randomGenerator_, randomStartTrack_, tmpEndTrack );
				play( trackToPlay );
			}
		}
	}

	status_ = RANDOM;
}

/* End CD.CPP *******************************************************/
