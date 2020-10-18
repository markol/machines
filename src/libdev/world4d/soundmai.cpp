/*
 * S O U N D M A I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "stdlib/string.hpp"
#include "world4d/internal/soundmai.hpp"
#include "world4d/internal/snddata.hpp"

#include "world4d/entity.hpp"
#include "world4d/manager.hpp"

#include "sound/smpparam.hpp"
#include "sound/sndwavid.hpp"
#include "sound/soundmix.hpp"

W4dSoundManagerImpl::W4dSoundManagerImpl()
:   hasPendingSound_( false ),
	definitionFileRead_(false),
	scalingOn_(false),
	scaleFactor_(1.0),
	maxScaleFactor_(1),
	minScaleFactor_(1),
	pAvailableSounds_(NULL),
	playSounds_(true),
	currentAudioType_(Snd::THREE_D)
{
}

W4dSoundManagerImpl::~W4dSoundManagerImpl()
{
	if( pAvailableSounds_ != NULL )
    {
    	for( SoundDataVector::iterator i = pAvailableSounds_->begin(); i != pAvailableSounds_->end(); ++i )
		{
    		if(*i)
				_DELETE( *i );
		}
		_DELETE( pAvailableSounds_ );
    }
}

W4dSoundManagerImpl::EntitySound::EntitySound
(
    const W4dEntity* pEntity, double maxPercentageVolume, MATHEX_SCALAR nearDistance,
    MATHEX_SCALAR farDistance, double currentPercentageVolume, const SysPathName& fileName,
    W4dSoundManager::Repetition repetition, const PhysAbsoluteTime& startTime,
    bool isPlaying, MATHEX_SCALAR duration, bool is3D, W4dSoundData* pData
)
:   pEntity_( pEntity ),
    maxPercentageVolume_( maxPercentageVolume ),
    nearDistance_( nearDistance ),
    farDistance_( farDistance ),
    sqrDistance_( 10000 ),
    currentPercentageVolume_( currentPercentageVolume ),
    pathname_( fileName ),
    repetitions_( repetition),
    startTime_( startTime ),
    isPlaying_( isPlaying ),
    isSelected_( true ),
	idSet_(false),
	duration_(duration),
	is3D_(is3D),
	pData_(pData),
	lastPosition_(0,0,0),
	selectionValue_(0)
{
	ASSERT(pData, "Invalid SoundData");
	priority_ = pData->priority();
}

W4dSoundManagerImpl::EntitySound::EntitySound()
{
}

const SndSampleHandle& W4dSoundManagerImpl::EntitySound::id() const
{
	PRE(idSet_);
	return id_;
}

void W4dSoundManagerImpl::EntitySound::setId(const SndSampleHandle& newId)
{
	id_ = newId;
	idSet_ = true;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool operator==( const W4dSoundManagerImpl::EntitySound& lhs, const W4dSoundManagerImpl::EntitySound& rhs )
{
    return lhs.pEntity_ == rhs.pEntity_;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool operator<( const W4dSoundManagerImpl::EntitySound& lhs, const W4dSoundManagerImpl::EntitySound& rhs )
{
    return lhs.currentPercentageVolume_ < rhs.currentPercentageVolume_;
}
//////////////////////////////////////////////////////////////////////////////////////////

double W4dSoundManagerImpl::computeVolume( const EntitySound& entitySound )
{
	PRE(!entitySound.is3D_);
    double currentPercentageVolume = entitySound.maxPercentageVolume_;

    //We have decided that the sound should drop off sooner
    double maxAudibleDistance = entitySound.farDistance_;
    double fallOffStartDistance = entitySound.nearDistance_;// * 0.25;
	if(W4dSoundManager::instance().scalingActive())
	{
		maxAudibleDistance *= scaleFactor_;
		fallOffStartDistance *= scaleFactor_;
	}

    double distance = sqrt( entitySound.sqrDistance_ );

    if( distance > fallOffStartDistance )
    {
        if( distance >= maxAudibleDistance )
            currentPercentageVolume = 0;
        else
        {
            double p = (maxAudibleDistance - distance) /
                       (maxAudibleDistance - fallOffStartDistance);
            currentPercentageVolume *= p;
        }
    }

    if( currentPercentageVolume > 100.0 )
        currentPercentageVolume = 100.0;
    else if( currentPercentageVolume < 0.0 )
        currentPercentageVolume = 0.0;

    return currentPercentageVolume;
}

void W4dSoundManagerImpl::playEntitySound(EntitySound* pThisSound)
{
	MexPoint3d pos;
	if(pThisSound->is3D_)
	{
	   	pThisSound->lastPosition_ = pThisSound->pEntity_->globalTransform().position();
		pThisSound->currentPercentageVolume_ = 100;
	}
	else
		pThisSound->currentPercentageVolume_ = computeVolume(*pThisSound);

	SndWaveformId newId(pThisSound->pathname_.c_str());
	SndSampleParameters* pTempParams;
	//If playing a durational sound
	if(pThisSound->duration_ > 0)
	{
		if(!pThisSound->is3D_)
		{
			pTempParams =
			_NEW(SndSampleParameters(newId, _STATIC_CAST(double, pThisSound->duration_), _STATIC_CAST(Snd::Volume, pThisSound->currentPercentageVolume_)));
			SOUND_STREAM("Playing 2DDurational sound " << std::endl);
		}
		else
		{
			pTempParams =
			_NEW(SndSampleParameters(newId, pThisSound->lastPosition_, pThisSound->duration_,
				pThisSound->farDistance_, pThisSound->nearDistance_, pThisSound->currentPercentageVolume_));
			SOUND_STREAM("Playing 3DDurational sound " << std::endl);
		}
	}
	//else playing a repetitional sound
	else
	{
		//if this sound is 2D
		if(!pThisSound->is3D_)
		{
			pTempParams =
			_NEW(SndSampleParameters(newId, _STATIC_CAST(Snd::LoopCount, pThisSound->repetitions_), _STATIC_CAST(Snd::Volume, pThisSound->currentPercentageVolume_)));
			SOUND_STREAM("Playing 2DRepetitional sound " << std::endl);
		}
		else
		{
			pTempParams =
			_NEW(SndSampleParameters(newId, pThisSound->lastPosition_, _STATIC_CAST(Snd::LoopCount, pThisSound->repetitions_),
		 	pThisSound->farDistance_, pThisSound->nearDistance_, pThisSound->currentPercentageVolume_));
			SOUND_STREAM("Playing 3DRepetitional sound " << std::endl);
		}
	}

	pTempParams->setHWMixing(pThisSound->pData_->isHWMixed());
	if(pThisSound->currentPercentageVolume_ > 0)
	{
		pThisSound->isPlaying_ = true;
		SOUND_STREAM("Playing Entity sound " << pThisSound->pathname_ << " at " << W4dManager::instance().time());
		SOUND_STREAM(" original start " << pThisSound->startTime_ << std::endl);
    	pThisSound->startTime_ = W4dManager::instance().time();
		pThisSound->setId(SndMixer::instance().playSample(*pTempParams));
		hasPendingSound_ = true;
	}
	else
	{
		SOUND_STREAM("Entity sound out of range" << std::endl);
	}
	_DELETE(pTempParams);
}
/* End SOUNDMAI.CPP *************************************************/
