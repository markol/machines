/*
 * S O U N D M A N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */


#include "base/diag.hpp"
#include "world4d/soundman.hpp"

#include "stdlib/string.hpp"
#include "world4d/entity.hpp"
#include "world4d/manager.hpp"
#include "world4d/internal/snddata.hpp"
#include "world4d/internal/soundmai.hpp"

#include "utility/linetok.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"

#include "sound/sndwavid.hpp"
//#include "sound/smpparams.hpp"
#include "sound/smpparam.hpp"
#include "sound/soundmix.hpp"
#include "sound/snd.hpp"

#include "ctl/vector.hpp"

#include "system/metafile.hpp"
#include "system/metaistr.hpp"
#include <memory>

//////////////////////////////////////////////////////////////////////////////////////////
// static
W4dSoundManager& W4dSoundManager::instance()
{
    static W4dSoundManager instance_;
    return instance_;
}
//////////////////////////////////////////////////////////////////////////////////////////

W4dSoundManager::W4dSoundManager()
{
    pImpl_ = _NEW(W4dSoundManagerImpl);
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

W4dSoundManager::~W4dSoundManager()
{
    CB_DEPIMPL(W4dSoundManagerImpl::EntitySounds, sounds_);
    CB_DEPIMPL(W4dSoundManagerImpl::SoundDataVector*, pAvailableSounds_);

    TEST_INVARIANT;

    if( sounds_.begin() != sounds_.end() )
        sounds_.erase( sounds_.begin(), sounds_.end() );


//	//If a sound definition file has been read
//	//Then clear the data structure.
//	if(pAvailableSounds_)
//	{
//		W4dSoundManagerImpl::SoundDataVector::iterator i = pAvailableSounds_->begin();
//		for( ; i!=pAvailableSounds_->end() ; ++i )
//		{
//			if(*i)
//				_DELETE(*i);
//			(*i) = NULL;
//		}
//		_DELETE(pAvailableSounds_);
//	}

	_DELETE(pImpl_);
}
//////////////////////////////////////////////////////////////////////////////////////////

void W4dSoundManager::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

void W4dSoundManager::load( const SysPathName& path )
{
    //Play the file and stop immediately
	SndWaveformId id(path);
	SndMixer::instance().loadWaveform(id);
}
//////////////////////////////////////////////////////////////////////////////////////////

void W4dSoundManager::unloadAll()
{
    CB_DEPIMPL(W4dSoundManagerImpl::EntitySounds, sounds_);

	W4dSoundManagerImpl::EntitySounds::iterator i = sounds_.begin();
	W4dSoundManagerImpl::EntitySounds::iterator j = sounds_.end();

	while(i!=j)
	{
        W4dSoundManagerImpl::EntitySound& entitySound = *i;
        if( entitySound.isPlaying_ )
		{
    		SndMixer::instance().stopSample(entitySound.id());
            SndMixer::instance().freeSampleResources( entitySound.id() );
		}

        //Decrement the count in the W4dEntity
        entitySound.pEntity_->decrementLinkedSoundCount();

		++i;
	}

    //Clear the collection
    if( sounds_.begin() != sounds_.end() )
        sounds_.erase( sounds_.begin(), sounds_.end() );
}

void W4dSoundManager::stopAll()
{
    CB_DEPIMPL(W4dSoundManagerImpl::EntitySounds, sounds_);

	SOUND_STREAM("Stopping the lot" << std::endl);

	W4dSoundManagerImpl::EntitySounds::iterator i = sounds_.begin();
	for(;i!=sounds_.end();++i)
	{
        W4dSoundManagerImpl::EntitySound& entitySound = *i;
        if( entitySound.isPlaying_ )
		{
    		SndMixer::instance().stopSample(entitySound.id());
            SndMixer::instance().freeSampleResources( entitySound.id() );
			entitySound.isPlaying_ = false;
		}


        //Decrement the count in the W4dEntity
//        entitySound.pEntity_->decrementLinkedSoundCount();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void W4dSoundManager::playForDuration
(
    const W4dEntity* pEntity,
    uint id,
    const PhysAbsoluteTime& startTime,
    MATHEX_SCALAR duration
)
{
    CB_DEPIMPL(MexPoint3d, lastEarLocation_);
    CB_DEPIMPL(MexTransform3d, lastEarTransform_);
    CB_DEPIMPL(bool, hasPendingSound_);
    CB_DEPIMPL(PhysAbsoluteTime, pendingStartTime_);
    CB_DEPIMPL(W4dSoundManagerImpl::EntitySounds, sounds_);
    CB_DEPIMPL(W4dSoundManagerImpl::SoundDataVector*, pAvailableSounds_);

    PRE( pEntity != NULL );
    //PRE( duration > 0 );

	W4dSoundData* pData = (*(pAvailableSounds_))[id];

	if(!pData)
    {
		SOUND_STREAM("Trying to access unregistered sound no(a) " << (unsigned int)id << std::endl);
    }
	else if(duration <= 0)
    {
		SOUND_STREAM("Invalid play duration " << duration << std::endl);
    }
	else
	{
		PhysAbsoluteTime realStartTime = startTime;
	    PhysAbsoluteTime timeNow = W4dManager::instance().time();
		if(realStartTime == 0)
			realStartTime = timeNow;

		W4dSoundManagerImpl::EntitySound newSound( pEntity, pData->maxPercentageVolume(), pData->nearAttenuationDistance(),
	    pData->farAttenuationDistance(), 0, pData->path(),
	    (Repetition)0, realStartTime, false, duration, pData->is3D(), pData );

	    //Add an entry to the list of sounds
	    sounds_.push_back( newSound );

        //Increment the count in the W4dEntity
        pEntity->incrementLinkedSoundCount();
	}
}

void W4dSoundManager::play
(
    const W4dEntity* pEntity, uint id, const PhysAbsoluteTime& startTime, size_t repetition
)
{
    CB_DEPIMPL(MexPoint3d, lastEarLocation_);
    CB_DEPIMPL(MexTransform3d, lastEarTransform_);
    CB_DEPIMPL(bool, hasPendingSound_);
    CB_DEPIMPL(PhysAbsoluteTime, pendingStartTime_);
    CB_DEPIMPL(W4dSoundManagerImpl::EntitySounds, sounds_);
    CB_DEPIMPL(W4dSoundManagerImpl::SoundDataVector*, pAvailableSounds_);

    PRE( pEntity != NULL );

	W4dSoundData* pData = (*(pAvailableSounds_))[id];

	if(!pData)
    {
		SOUND_STREAM("Trying to access unregistered sound no(a) " << (unsigned int)id << std::endl);
    }
	else
	{
		PhysAbsoluteTime realStartTime = startTime;
	    PhysAbsoluteTime timeNow = W4dManager::instance().time();
		if(realStartTime == 0)
			realStartTime = timeNow;

		W4dSoundManagerImpl::EntitySound newSound( pEntity, pData->maxPercentageVolume(), pData->nearAttenuationDistance(),
	    pData->farAttenuationDistance(), 0, pData->path(),
        (Repetition)repetition, startTime, false, 0.0, pData->is3D(), pData );

	    sounds_.push_back( newSound );

        //Increment the count in the W4dEntity
        pEntity->incrementLinkedSoundCount();
	}
}
//////////////////////////////////////////////////////////////////////////////////////////

void W4dSoundManager::stop( const W4dEntity* pEntity )
{
    CB_DEPIMPL(W4dSoundManagerImpl::EntitySounds, sounds_);
    PRE( pEntity != NULL );

    //Find the entry for this entity
	W4dSoundManagerImpl::EntitySounds::iterator it = sounds_.begin();
    W4dSoundManagerImpl::EntitySounds::iterator nextIt = it;
    for(; it != sounds_.end(); it = nextIt )
    {
//        nextIt = it;
        ++nextIt;

        if( (*it).pEntity_ == pEntity )
        {
            //Stop playing the sound if playing
            if( (*it).isPlaying_ )
            {
				(*it).isPlaying_ = false;
                SndMixer::instance().stopSample( (*it).id() );
                SndMixer::instance().freeSampleResources( (*it).id() );
            }
			else
			{
                //SOUND_STREAM("Stopping already freed id for entity " << _STATIC_CAST(void*, pEntity) << std::endl);
                SOUND_STREAM("Stopping already freed id for entity " << _CONST_CAST(void*, _REINTERPRET_CAST( const void*, pEntity) ) << std::endl);
			}
            sounds_.erase( it );

            //Decrement the count in the W4dEntity
            pEntity->decrementLinkedSoundCount();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
void W4dSoundManager::updateVolumes( const MexPoint3d& earLocation )
{
	ASSERT(false, "Should not be calling this");
	SOUND_STREAM("This function should not be being used" << std::endl);
}

void W4dSoundManager::updateVolumes( const MexTransform3d& earTransform )
{
	//if the sounds start time is in the future then move to
	//the next sound

	//if the sound is logically playing
		//if it is a repetitional sound
			//if it is not physically active
				//Free the resources
				//Remove the sound from the vector of entity sounds
				//Move to next sound
		//if it is durational and it is logically finished
			//if it is physically active
				//then stop it
			//Free the resources
			//Remove the sound from the vector of entity sounds
			//Move to next sound

	//Calculate its distance from the source
	//Calculate its selection factor
		//if the sound is closer than the minimum distance
		//rank it purely on priority
		//otherwise if the sound is outside the maximum distance
		//it should not be selected
		//else calculate the sounds selection value based on its
		//distance from the ear combined with its priority
	//If the selection factor is high enough then
	//put it on the list of playing sounds
	//If the selection factor is not high enough
		//If it is logically playing
			//then stop it, and free resources.

	//Run through the selection list
		//If the sound is not playing, play it
		//Otherwise do the volume/positin updates
			//If the sound is 3D update the position
			//Else if the sound is 2D compute the volume
    CB_DEPIMPL(MexPoint3d, lastEarLocation_);
    CB_DEPIMPL(MexTransform3d, lastEarTransform_);
    CB_DEPIMPL(bool, hasPendingSound_);
    CB_DEPIMPL(PhysAbsoluteTime, pendingStartTime_);
    CB_DEPIMPL(W4dSoundManagerImpl::EntitySounds, sounds_);
	CB_DEPIMPL(W4dSoundManagerImpl::SoundIDMap, registeredSounds_);
	CB_DEPIMPL(bool, definitionFileRead_);
	CB_DEPIMPL(MATHEX_SCALAR, scaleFactor_);
	CB_DEPIMPL(bool, scalingOn_);
    CB_DEPIMPL(W4dSoundManagerImpl::SoundDataVector*, pAvailableSounds_);
    CB_DEPIMPL(MATHEX_SCALAR, maxScaleFactor_);
    CB_DEPIMPL(MATHEX_SCALAR, minScaleFactor_);

    PhysAbsoluteTime timeNow = W4dManager::instance().time();
    static PhysAbsoluteTime lastUpdateTime = 0.0;
	const int N_SOUNDS = 16;
	static size_t updateCount = 0;
	//If there is a sound waiting to be played then we must update volumes
	//now instead of waiting
	if(updateCount < 2 && !hasPendingSound_)
	{
		++updateCount;
	}
	//if this_time is not the same as last_time
    else if( lastUpdateTime != timeNow )
    {
		SOUND_STREAM("########### Frame update" << std::endl);
		hasPendingSound_ = false;
		updateCount = 0;
		SndMixer::instance().update();

	//////////////////////////////////////////////////////////
	//Timer code for assesment purposes
	//		static UtlDebugTimer myTimer;
	//		static bool first = true;
	//		static float avgTime=0;
	//		static size_t numPasses=0;
	//		UtlDebugTimer::Time theTime;
	//		if(first)
	//		{
	//			 myTimer.calibrate(10);
	//			 first = false;
	//		}
	//		myTimer.time(0);
	//////////////////////////////////////////////////////////
        lastUpdateTime = timeNow;

        //Store the ear position and transform
        lastEarLocation_ = earTransform.position();
        lastEarTransform_ = earTransform;

		//Update the listener transform
		SndMixer::instance().listenerTransform(earTransform);

        typedef ctl_vector< W4dSoundManagerImpl::EntitySounds::iterator > EntitySoundsIts;
		EntitySoundsIts selectedIts;
        selectedIts.reserve( N_SOUNDS * 2 );

	    W4dSoundManagerImpl::EntitySounds::iterator nextIt;

		//Loop around each sound in the system
	    for( W4dSoundManagerImpl::EntitySounds::iterator it = sounds_.begin(); it != sounds_.end(); it = nextIt )
        {
            W4dSoundManagerImpl::EntitySound& entitySound = *it;

			nextIt = it;
			++nextIt;

			//If this sound has previously been marked as unselected
			//then erase it
			if(!entitySound.isSelected_)
			{
                entitySound.pEntity_->decrementLinkedSoundCount();
				sounds_.erase(it);
				continue;
			}

			//if the sounds start time is in the future then move to
			//the next sound
			if(entitySound.startTime_ > timeNow)
			{
				//It is possible that this situation could occur on
				//network games, causing all kinds of nonsense to
				//happen
				if(entitySound.isPlaying_)
				{
					SndMixer::instance().stopSample(entitySound.id());
					SndMixer::instance().freeSampleResources(entitySound.id());
					entitySound.isPlaying_ = false;
				}
				continue;
			}

			//if the sound is logically playing
			if(entitySound.isPlaying_)
			{
				//if it is a repetitional sound and isn't playing
				if(entitySound.repetitions_ > 0)
				{
					if(!SndMixer::instance().isActive(entitySound.id()))
	                {
						//Free the resources
						SndMixer::instance().freeSampleResources(entitySound.id());
						entitySound.isPlaying_ = false;

						//Remove the sound from the vector of entity sounds
		                entitySound.pEntity_->decrementLinkedSoundCount();
		                sounds_.erase( it );

						//Move to next sound
						continue;
					}
				}

				//if it is durational and it is physically finished
				if(	entitySound.duration_ > 0)
				{
					bool isPlaying = SndMixer::instance().isActive(entitySound.id());

					//if it is not physically active
					if(!isPlaying)
	                {
						//then stop it
						entitySound.isPlaying_ = false;
						SndMixer::instance().freeSampleResources(entitySound.id());

						//Remove the sound from the vector of entity sounds
		                entitySound.pEntity_->decrementLinkedSoundCount();
		                sounds_.erase( it );

						//Move to next sound
						continue;
	                }
				}
			}
			//If the sound is durational and has passed its
			//duration to play, but isn't playing, then
			//erase it
			else if(entitySound.duration_ > 0)
			{
				if(entitySound.duration_ + entitySound.startTime_ < timeNow)
				{
		            entitySound.pEntity_->decrementLinkedSoundCount();
	                sounds_.erase( it );
					continue;
				}
			}
			//If this sound isn't playing, and isn't an infinitely looped sound,
			//and has passed its best before date then remove it from the
			//sounds list
//			else if(entitySound.repetitions_ > 0 && entitySound.startTime_ < timeNow)
//			{
//	            entitySound.pEntity_->decrementLinkedSoundCount();
//                sounds_.erase( it );
//				continue;
//			}

			//Calculate its distance from the source
	        MATHEX_SCALAR sqrDistance = entitySound.sqrDistance_ =
    	        lastEarLocation_.sqrEuclidianDistance( entitySound.pEntity_->globalTransform().position() );

			//Calculate its selection factor
	        MATHEX_SCALAR squareMaxAudibleDistance = entitySound.farDistance_ * entitySound.farDistance_;
	        MATHEX_SCALAR squareMinAudibleDistance = entitySound.nearDistance_ * entitySound.nearDistance_;

			//if the sound is closer than the minimum distance
			//rank it purely on priority
			if(sqrDistance < squareMinAudibleDistance)
			{
				entitySound.selectionValue_ = entitySound.priority_;
			}
			//otherwise if the sound is outside the maximum distance
			//it should not be selected
			else if (sqrDistance > squareMaxAudibleDistance)
			{
				entitySound.selectionValue_ = 0;
			}
			//else calculate the sounds selection value based on its
			//distance from the ear combined with its priority
			else
			{
				//Calculate the selection value for this sound
				MATHEX_SCALAR s = 1 - ((sqrDistance - squareMinAudibleDistance)/
									 (squareMaxAudibleDistance - squareMinAudibleDistance));

				const int FUDGE = 100;
				entitySound.selectionValue_ =
				entitySound.priority_ * FUDGE * (s/sqrDistance);
			}

			//This sounds selection value means it should be stopped
			if(entitySound.selectionValue_ == 0)
			{
				//If the sound is playing then stop it
				if(entitySound.isPlaying_)
				{
					SndMixer::instance().stopSample(entitySound.id());
					SndMixer::instance().freeSampleResources(entitySound.id());
					entitySound.isPlaying_ = false;
				}
				//If it is a durational sound then reset start time
				//and duration to be played
				if(entitySound.duration_ > 0)
				{
					entitySound.duration_ -= (timeNow - entitySound.startTime_) ;
					entitySound.startTime_ = timeNow;
				}
				//If this sound is a non-continuous
				//sound that has not a sufficient selection value
				//then mark it to be erased from the array of available sounds
				//it has lost its chance.
				else if(entitySound.repetitions_ > 0)
				{
		            entitySound.pEntity_->decrementLinkedSoundCount();
					sounds_.erase(it);
				}
				//Process next sound
				continue;
			}

			//If the selection factor is high enough then
			//put it on the list of playing sounds
            EntitySoundsIts::iterator selectIt = selectedIts.begin();
            for( ; selectIt != selectedIts.end(); ++selectIt )
            {
                if( entitySound.selectionValue_ > (*(*selectIt)).selectionValue_ )
                    break;
            }

            selectedIts.insert( selectIt, it );

			//If the selection factor is not high enough
            if( selectedIts.size() > N_SOUNDS )
			{
				W4dSoundManagerImpl::EntitySound& temp = *(selectedIts.back());
				//If it is logically playing
				if(temp.isPlaying_)
				{
					//then stop it, and free resources.
					SndMixer::instance().stopSample(temp.id());
					SndMixer::instance().freeSampleResources(temp.id());
					temp.isPlaying_ = false;
				}
				//If the sound is durational then reset the startTime
				//and duration
				if(temp.duration_ > 0)
				{
					temp.duration_ -= (timeNow - temp.startTime_) ;
					temp.startTime_ = timeNow;
				}
				else if(temp.repetitions_ > 0)
				{
		            temp.pEntity_->decrementLinkedSoundCount();
					sounds_.erase(selectedIts.back());
	                selectedIts.pop_back();
					continue;
				}
                selectedIts.pop_back();
			}
		}
        EntitySoundsIts::iterator selectIt = selectedIts.begin();
		const int THRESHOLD_VALUE = 10;
		//Run through the selection list
        for( ; selectIt != selectedIts.end(); ++selectIt )
        {
			W4dSoundManagerImpl::EntitySound& thisSound = (*(*selectIt));
			//If the sound is not playing, play it only if suitable selection
			//value has been given

			if(!thisSound.isPlaying_ && thisSound.selectionValue_ > 0 && isPlayingSounds())
			{
				pImpl_->playEntitySound(&thisSound);
			}
			//otherwise do the volume/positional updates
            else if(thisSound.isPlaying_)
			{
	 			 //if the sound is 3D update the position
	            if( thisSound.is3D_ )
				{
	    			MexPoint3d pos = thisSound.pEntity_->globalTransform().position();
					//If required update this samples 3D pos
					if(pos != thisSound.lastPosition_)
					{
						SndMixer::instance().samplePosition(thisSound.id(), pos);
						thisSound.lastPosition_ = pos;
					}
				}
				//Else if the sound is 2D compute the volume
				else
				{
		            double thisVolume = pImpl_->computeVolume( thisSound );
			        if(thisSound.currentPercentageVolume_ != thisVolume)
					{
						thisSound.currentPercentageVolume_ = thisVolume;
						SndMixer::instance().sampleVolume(thisSound.id(), thisSound.currentPercentageVolume_);
					}
				}
			}
		}
//		SndMixer::instance().update();
		SOUND_STREAM("########### Frame update finished" << std::endl);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////

void W4dSoundManager::updateCycle()
{
    CB_DEPIMPL(MexPoint3d, lastEarLocation_);
    CB_DEPIMPL(MexTransform3d, lastEarTransform_);
    CB_DEPIMPL(bool, hasPendingSound_);

    //Check for need to initiate a pending sound
    if( hasPendingSound_ )
    {
        PhysAbsoluteTime timeNow = W4dManager::instance().time();
		if(SndMixer::instance().is3dMixer())
	        updateVolumes( lastEarTransform_ );
		else
	        updateVolumes( lastEarLocation_ );
    }
}

void W4dSoundManager::registerSoundID(const string& id, uint soundId)
{
	CB_DEPIMPL(W4dSoundManagerImpl::SoundIDMap, registeredSounds_);
	CB_DEPIMPL(bool, definitionFileRead_);

	PRE(!definitionFileRead_);
	SOUND_STREAM("Registering " << id << " at " << soundId << std::endl);
	registeredSounds_.insert(id, soundId);
}

void W4dSoundManager::readSoundDefinitionFile(const SysPathName& definitionFileName)
{
	CB_DEPIMPL(W4dSoundManagerImpl::SoundIDMap, registeredSounds_);
	CB_DEPIMPL(bool, definitionFileRead_);
    CB_DEPIMPL(W4dSoundManagerImpl::SoundDataVector*, pAvailableSounds_);

	if(!pAvailableSounds_)
		pAvailableSounds_ = _NEW(W4dSoundManagerImpl::SoundDataVector(registeredSounds_.size()));

	W4dSoundManagerImpl::SoundDataVector::iterator i = pAvailableSounds_->begin();

	for( ; i!=pAvailableSounds_->end() ; ++i )
	{
		if(*i)
			_DELETE(*i);
		(*i) = NULL;
	}

	SndMixer::instance().unloadAll();

    SysMetaFile metaFile( "mach1.met" );

    std::auto_ptr< std::istream > pIstream;

    if( SysMetaFile::useMetaFile() )
    {
        pIstream = std::auto_ptr< std::istream >(_NEW( SysMetaFileIstream( metaFile, definitionFileName, std::ios::in ) ));
    }
    else
    {
        ASSERT_FILE_EXISTS( definitionFileName.c_str() );
        //pIstream = _NEW( std::ifstream( definitionFileName.c_str(), std::ios::text | std::ios::in ) );
        pIstream = std::auto_ptr< std::istream > ( _NEW( std::ifstream( definitionFileName.c_str(), std::ios::in ) ));
    }

    UtlLineTokeniser    parser( *pIstream, definitionFileName );
	W4dSoundManagerImpl::SoundDataVector& availableSounds = *(pAvailableSounds_);
	while( not parser.finished() )
	{
        const UtlLineTokeniser::Tokens&    tokens = parser.tokens();

		ASSERT(tokens.size() >= 6, "Sound Definition file syntax error");

		if(availableSounds[registeredSounds_[tokens[0] ] ])
			SOUND_STREAM("Token " << tokens[0] << " already used" << std::endl);

		ASSERT(!availableSounds[registeredSounds_[tokens[0]]], "Duplicate sound id");

		bool is3D = false;
		bool isHWMixed = false;

		#ifndef PRODUCTION
		SysPathName wavPath(tokens[1]);
		if(!wavPath.existsAsFile())
		{
			ASSERT_INFO(wavPath)
			ASSERT_FAIL("Failed to find file");
			Diag::instance().forceCrash();
		}
		#endif

		switch (tokens.size())
		{
			case 9:
				if(tokens[8] == "HW_MIXED")
					isHWMixed = true;
			case 8:
				if(tokens[7] == "3D")
					is3D = true;
			case 7:
				if(tokens[6] == "PRELOAD")
				{
 					SOUND_STREAM("Preloading " << tokens[0] << std::endl);
					SndMixer::instance().loadWaveform( SndWaveformId( tokens[1] ),is3D );
				}
		}

		if(availableSounds[ registeredSounds_[tokens[0] ] ])
		{
			SOUND_STREAM("Attempting to overwrite existing data in " << registeredSounds_[tokens[0] ] << std::endl);
			SOUND_STREAM("This " << availableSounds[ registeredSounds_[tokens[0] ] ]->path() << " already in " << registeredSounds_[tokens[0] ] << std::endl);
		}
		else
		{
			availableSounds[ registeredSounds_[tokens[0] ] ] =
			_NEW(W4dSoundData
			(tokens[1], atoi(tokens[0].c_str()),
			 atoi(tokens[2].c_str()), atof(tokens[3].c_str()),
			 atof(tokens[4].c_str()), is3D, isHWMixed, atoi(tokens[5].c_str())));

		}
	    parser.parseNextLine();
	}
	definitionFileRead_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////


void W4dSoundManager::scalingActivate(bool scalingOn)
{
	CB_DEPIMPL(bool, scalingOn_);

	scalingOn_ = scalingOn;
}

bool W4dSoundManager::scalingActive() const
{
	CB_DEPIMPL(bool, scalingOn_);

	return scalingOn_;
}

void W4dSoundManager::setScaleFactor(MATHEX_SCALAR newScale)
{
	CB_DEPIMPL(MATHEX_SCALAR, scaleFactor_);

	scaleFactor_ = newScale;
}

MATHEX_SCALAR W4dSoundManager::scaleFactor() const
{
	CB_DEPIMPL(MATHEX_SCALAR, scaleFactor_);

	return scaleFactor_;
}

MATHEX_SCALAR W4dSoundManager::maxScaleFactor() const
{
    CB_DEPIMPL(MATHEX_SCALAR, maxScaleFactor_);

    return maxScaleFactor_;
}

MATHEX_SCALAR W4dSoundManager::minScaleFactor() const
{
    CB_DEPIMPL(MATHEX_SCALAR, minScaleFactor_);

    return minScaleFactor_;
}

void W4dSoundManager::setMinScaleFactor(MATHEX_SCALAR newScale)
{
    CB_DEPIMPL(MATHEX_SCALAR, minScaleFactor_);

    minScaleFactor_ = newScale;
}

void W4dSoundManager::setMaxScaleFactor(MATHEX_SCALAR newScale)
{
    CB_DEPIMPL(MATHEX_SCALAR, maxScaleFactor_);

    maxScaleFactor_ = newScale;
}

bool W4dSoundManager::isPlayingSounds() const
{
    CB_DEPIMPL(bool, playSounds_);
	return playSounds_;
}

void W4dSoundManager::setPlayingSoundsStatus(bool playStatus)
{
    CB_DEPIMPL(bool, playSounds_);
	playSounds_ = playStatus;
}

void W4dSoundManager::convertSoundDimensions(Snd::AudioType newAudioType)
{
    CB_DEPIMPL(W4dSoundManagerImpl::EntitySounds, sounds_);
	bool is3D;
	newAudioType == Snd::THREE_D ? is3D=true : is3D=false;
    for( W4dSoundManagerImpl::EntitySounds::iterator it = sounds_.begin(); it != sounds_.end(); ++it )
    {
        (*it).is3D_ = is3D;
	}
}

Snd::AudioType W4dSoundManager::currentSoundDimensions() const
{
    CB_DEPIMPL(Snd::AudioType, currentAudioType_);
	return currentAudioType_;
}

void W4dSoundManager::currentSoundDimensions(Snd::AudioType newAudioType)
{
    CB_DEPIMPL(Snd::AudioType, currentAudioType_);
	currentAudioType_ = newAudioType;
}

//////////////////////////////////////////////////////////////////////////////////////////
/* End SOUNDMAN.CPP *************************************************/

