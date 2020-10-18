/*
 * M G S N D M A N . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/internal/mgsndman.hpp"
#include "sound/smphandl.hpp"
//Inclusion in here to stop compilation error
#include "stdlib/string.hpp"
#include "sound/soundmix.hpp"
#include "sound/sndwavid.hpp"
#include "sound/smpparam.hpp"
#include "system/pathname.hpp"
#include "ctl/list.hpp"

class MachGuiSoundManagerImpl
{
private:
	typedef	ctl_list<SndSampleHandle> SampleHandleList;

	SampleHandleList currentActiveHandles_;
	int delaySoundsFrameCount_;

	friend class MachGuiSoundManager;
};


// static
MachGuiSoundManager& MachGuiSoundManager::instance()
{
    static MachGuiSoundManager instance_;
    return instance_;
}

MachGuiSoundManager::MachGuiSoundManager()
:pImpl_(_NEW(MachGuiSoundManagerImpl()))
{
	CB_DEPIMPL( int, delaySoundsFrameCount_ );

	delaySoundsFrameCount_ = 2;

    TEST_INVARIANT;
}

MachGuiSoundManager::~MachGuiSoundManager()
{
	CB_DEPIMPL(MachGuiSoundManagerImpl::SampleHandleList, currentActiveHandles_);

	currentActiveHandles_.erase(currentActiveHandles_.begin(), currentActiveHandles_.end());

    TEST_INVARIANT;
}

bool MachGuiSoundManager::playSound(const SysPathName& wavFilePath)
{
	PRE(wavFilePath.existsAsFile());

	CB_DEPIMPL(MachGuiSoundManagerImpl::SampleHandleList, currentActiveHandles_);
	CB_DEPIMPL( int, delaySoundsFrameCount_ );

	bool playingSound = true;

	//If there is a 'channel' available
	if( delaySoundsFrameCount_ == 0 and currentActiveHandles_.size() < MAX_SOUNDS)
	{
		//Play the sound
		SndWaveformId id( wavFilePath );
		SndSampleParameters temp( id );
		SndSampleHandle tempHandle = SndMixer::instance().playSample(temp);
		currentActiveHandles_.push_back(tempHandle);
		SOUND_STREAM("Playing MGsound " << wavFilePath << " handle " << tempHandle << std::endl);
		SndMixer::instance().update();
	}
	else
	{
		SOUND_STREAM("Cannot play MGsound " << wavFilePath << std::endl);
		playingSound = false;
	}

	return playingSound;
}

void MachGuiSoundManager::update()
{
	CB_DEPIMPL(MachGuiSoundManagerImpl::SampleHandleList, currentActiveHandles_);
	CB_DEPIMPL( int, delaySoundsFrameCount_ );

	//If there is anything in the list to update
	if(currentActiveHandles_.size() > 0)
	{
		MachGuiSoundManagerImpl::SampleHandleList::iterator i = currentActiveHandles_.begin();
		MachGuiSoundManagerImpl::SampleHandleList::iterator nextIterator = i;
		for(;i!=currentActiveHandles_.end(); i=nextIterator)
		{
			++nextIterator;
			//If this sample is not playing
			if(!SndMixer::instance().isActive(*i))
			{
				SOUND_STREAM("Stopping MGsound " << (*i) << std::endl);
				//Free resources and remove it from the 
				//list of active samples
				SndMixer::instance().freeSampleResources(*i);
				currentActiveHandles_.erase(i);
			}
		}
	}

	if ( delaySoundsFrameCount_ > 0 )
	{
		--delaySoundsFrameCount_;
	}
}

void MachGuiSoundManager::clearAll()
{
	CB_DEPIMPL(MachGuiSoundManagerImpl::SampleHandleList, currentActiveHandles_);

	MachGuiSoundManagerImpl::SampleHandleList::iterator i = currentActiveHandles_.begin();
	//Run through the list, stop any samples that are playing, free the
	//resources of any sample not playing, and erase the contents
	//of the list
	for(;i!=currentActiveHandles_.end(); ++i)
	{
		if(SndMixer::instance().isActive(*i))
		{
			SndMixer::instance().stopSample(*i);
		}

		SndMixer::instance().freeSampleResources(*i);
	}
	currentActiveHandles_.erase(currentActiveHandles_.begin(), currentActiveHandles_.end());
}

void MachGuiSoundManager::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiSoundManager& t )
{

    o << "MachGuiSoundManager " << (void*)&t << " start" << std::endl;
    o << "MachGuiSoundManager " << (void*)&t << " end" << std::endl;

    return o;
}

void MachGuiSoundManager::delaySounds()
{
	CB_DEPIMPL( int, delaySoundsFrameCount_ );
    delaySoundsFrameCount_ = 2;
}

/* End MGSNDMAN.CPP *************************************************/
