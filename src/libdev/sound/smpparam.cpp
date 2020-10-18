/*
 * S M P P A R A M . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
*/

#include "sound/smpparam.hpp"
//#include "sound/internal/alsample.hpp"
//#include "sound/internal/dxs.hpp"
#include "sound/sndwavid.hpp"

#include "mathex/point3d.hpp"

#include "stdlib/string.hpp"

class SndSampleParametersImpl
{
public:
	SndSampleParametersImpl(const SndWaveformId& id);
	~SndSampleParametersImpl();

	SndWaveformId		id_;
	MexPoint3d			initialPosition_;
	Snd::Distance		maxAudibleDistance_;
	Snd::Distance		minAudibleDistance_;
	Snd::Volume 		sampleVolume_;
	Snd::SamplePriority samplePriority_;
	Snd::LoopCount 		loopCount_;
	Snd::RelativeTime	startOffset_;
	bool				is3DSound_;
	MATHEX_SCALAR		duration_;
	bool 				hwMixingOn_;
	bool 				preloaded_;
};

SndSampleParametersImpl::SndSampleParametersImpl(const SndWaveformId& id):
id_(id),
initialPosition_(0,0,0),
maxAudibleDistance_(0),
minAudibleDistance_(0),
sampleVolume_(Snd::minVolume()),
samplePriority_(Snd::LOW),
loopCount_(1),
startOffset_(0),
is3DSound_(false),
duration_(0),
hwMixingOn_(false),
preloaded_(false)
{
}

SndSampleParametersImpl::~SndSampleParametersImpl()
{
	//Empty
}

SndSampleParameters::SndSampleParameters
(
	const SndWaveformId& id,
	Snd::LoopCount loopcount,
	Snd::Volume volume,
	Snd::SamplePriority priority,
	Snd::RelativeTime startOffset
)
{
	pImpl_ = _NEW(SndSampleParametersImpl(id));
	CB_DEPIMPL(SndWaveformId, id_);
	CB_DEPIMPL(Snd::LoopCount, loopCount_);
	CB_DEPIMPL(Snd::Volume, sampleVolume_);
	CB_DEPIMPL(Snd::SamplePriority, samplePriority_);
	CB_DEPIMPL(Snd::RelativeTime, startOffset_);
	CB_DEPIMPL(bool, is3DSound_);
	CB_DEPIMPL(MATHEX_SCALAR, duration_);
	CB_DEPIMPL(bool, hwMixingOn_);

	id_ = id;
	sampleVolume_ = volume;
	samplePriority_ = priority;
	loopCount_ = loopcount;
	startOffset_ = startOffset;
	duration_ = 0;
	is3DSound_ = false;
	hwMixingOn_ = false;

	POST( not is3dSound() );
}

SndSampleParameters::SndSampleParameters
(
	const SndWaveformId& id,
	MATHEX_SCALAR time,
	Snd::Volume volume,
	Snd::SamplePriority priority,
	Snd::RelativeTime startOffset
)
{
	pImpl_ = _NEW(SndSampleParametersImpl(id));
	CB_DEPIMPL(SndWaveformId, id_);
	CB_DEPIMPL(Snd::LoopCount, loopCount_);
	CB_DEPIMPL(Snd::Volume, sampleVolume_);
	CB_DEPIMPL(Snd::SamplePriority, samplePriority_);
	CB_DEPIMPL(Snd::RelativeTime, startOffset_);
	CB_DEPIMPL(bool, is3DSound_);
	CB_DEPIMPL(MATHEX_SCALAR, duration_);
	CB_DEPIMPL(bool, hwMixingOn_);

	id_ = id;
	sampleVolume_ = volume;
	samplePriority_ = priority;
	loopCount_ = 0;
	startOffset_ = startOffset;
	duration_ = time;
	is3DSound_ = false;
	hwMixingOn_ = false;

	POST( not is3dSound() );
	POST( duration_ > 0.0 );
}


SndSampleParameters::SndSampleParameters( const SndWaveformId& id,
				const MexPoint3d& initialPosition,
				Snd::LoopCount loopcount,
				Snd::Distance maxAudibleDistance,
				Snd::Distance minAudibleDistance,
				Snd::Volume volume,
				Snd::SamplePriority priority,
				Snd::RelativeTime startOffset)
{
	pImpl_ = _NEW(SndSampleParametersImpl(id));
	CB_DEPIMPL(MexPoint3d, initialPosition_);
	CB_DEPIMPL(SndWaveformId, id_);
	CB_DEPIMPL(Snd::LoopCount, loopCount_);
	CB_DEPIMPL(Snd::Volume, sampleVolume_);
	CB_DEPIMPL(Snd::SamplePriority, samplePriority_);
	CB_DEPIMPL(Snd::RelativeTime, startOffset_);
	CB_DEPIMPL(bool, is3DSound_);
	CB_DEPIMPL(MATHEX_SCALAR, duration_);
	CB_DEPIMPL(Snd::Distance, maxAudibleDistance_);
	CB_DEPIMPL(Snd::Distance, minAudibleDistance_);
	CB_DEPIMPL(bool, hwMixingOn_);

	id_ = id;
	initialPosition_ = initialPosition;
	maxAudibleDistance_ = maxAudibleDistance;
	minAudibleDistance_ = minAudibleDistance;
	sampleVolume_ = volume;
	samplePriority_ = priority;
	startOffset_ = startOffset;
	loopCount_ = loopcount;
	duration_ = 0;
	is3DSound_ = true;
	hwMixingOn_ = false;

	POST( is3dSound() );
}

// construct a 3d sound
SndSampleParameters::SndSampleParameters( const SndWaveformId& id,
				const MexPoint3d& initialPosition,
				MATHEX_SCALAR time,
				Snd::Distance maxAudibleDistance,
				Snd::Distance minAudibleDistance,
				Snd::Volume volume,
				Snd::SamplePriority priority,
				Snd::RelativeTime startOffset)
{
	pImpl_ = _NEW(SndSampleParametersImpl(id));
	CB_DEPIMPL(MexPoint3d, initialPosition_);
	CB_DEPIMPL(Snd::Volume, sampleVolume_);
	CB_DEPIMPL(Snd::SamplePriority, samplePriority_);
	CB_DEPIMPL(Snd::RelativeTime, startOffset_);
	CB_DEPIMPL(bool, is3DSound_);
	CB_DEPIMPL(Snd::LoopCount, loopCount_);
	CB_DEPIMPL(MATHEX_SCALAR, duration_);
	CB_DEPIMPL(Snd::Distance, maxAudibleDistance_);
	CB_DEPIMPL(Snd::Distance, minAudibleDistance_);
	CB_DEPIMPL(bool, hwMixingOn_);

	initialPosition_ = initialPosition;
	maxAudibleDistance_ = maxAudibleDistance;
	minAudibleDistance_ = minAudibleDistance;
	sampleVolume_ = volume;
	samplePriority_ = priority;
	startOffset_ = startOffset;
	loopCount_ = 0;
	duration_ = time;
	hwMixingOn_ = false;

	is3DSound_ = true;

	POST( is3dSound() );
	POST ( duration_ > 0.0 );
}

SndSampleParameters::SndSampleParameters( const SndSampleParameters& copyMe)
{
	pImpl_ = _NEW(SndSampleParametersImpl(copyMe.pImpl_->id_));

	CB_DEPIMPL(Snd::Distance, maxAudibleDistance_);
	CB_DEPIMPL(Snd::Distance, minAudibleDistance_);
	CB_DEPIMPL(MexPoint3d, initialPosition_);
	CB_DEPIMPL(Snd::Volume, sampleVolume_);
	CB_DEPIMPL(Snd::SamplePriority, samplePriority_);
	CB_DEPIMPL(Snd::RelativeTime, startOffset_);
	CB_DEPIMPL(bool, is3DSound_);
	CB_DEPIMPL(Snd::LoopCount, loopCount_);
	CB_DEPIMPL(MATHEX_SCALAR, duration_);
	CB_DEPIMPL(bool, hwMixingOn_);
	CB_DEPIMPL(bool, preloaded_);

	initialPosition_ = copyMe.pImpl_->initialPosition_;
	maxAudibleDistance_ = copyMe.pImpl_->maxAudibleDistance_;
	minAudibleDistance_ = copyMe.pImpl_->minAudibleDistance_;
	sampleVolume_ = copyMe.pImpl_->sampleVolume_;
	samplePriority_ = copyMe.pImpl_->samplePriority_;
	loopCount_ = copyMe.pImpl_->loopCount_;
	startOffset_ = copyMe.pImpl_->startOffset_;
	is3DSound_ = copyMe.pImpl_->is3DSound_;
	duration_ = copyMe.pImpl_->duration_;
	hwMixingOn_ = copyMe.pImpl_->hwMixingOn_;
	preloaded_ = copyMe.pImpl_->preloaded_;
}


SndSampleParameters::~SndSampleParameters()
{
	_DELETE(pImpl_);
}
///////////////////////////////

MATHEX_SCALAR SndSampleParameters::duration() const
{
	CB_DEPIMPL(MATHEX_SCALAR, duration_);

	return duration_;
}

Snd::Volume SndSampleParameters::volume() const
{
	CB_DEPIMPL(Snd::Volume, sampleVolume_);

	return sampleVolume_;
}

Snd::SamplePriority	SndSampleParameters::priority() const
{
	CB_DEPIMPL(Snd::SamplePriority, samplePriority_);

	return samplePriority_;
}

Snd::LoopCount SndSampleParameters::loopCount() const
{
	CB_DEPIMPL(Snd::LoopCount, loopCount_);

	return loopCount_;
}

const SndWaveformId& SndSampleParameters::id() const
{
	CB_DEPIMPL(SndWaveformId, id_);

	return id_;
}

Snd::RelativeTime SndSampleParameters::startOffset() const
{
	CB_DEPIMPL(Snd::RelativeTime, startOffset_);

	return startOffset_;
}

///////////////////////////////

bool SndSampleParameters::is3dSound() const
{
	CB_DEPIMPL(bool, is3DSound_);

	return is3DSound_;
}

const MexPoint3d& SndSampleParameters::initialPosition() const
{
	PRE( is3dSound() );
	CB_DEPIMPL(MexPoint3d, initialPosition_);

	return initialPosition_;
}

Snd::Distance SndSampleParameters::maxAudibleDistance() const
{
	PRE( is3dSound() );

	CB_DEPIMPL(Snd::Distance, maxAudibleDistance_);

	return maxAudibleDistance_;
}

Snd::Distance SndSampleParameters::minAudibleDistance() const
{
	PRE( is3dSound() );

	CB_DEPIMPL(Snd::Distance, minAudibleDistance_);

	return minAudibleDistance_;
}

void SndSampleParameters::setHWMixing(bool hwMixingOn)
{
	CB_DEPIMPL(bool, hwMixingOn_);
	hwMixingOn_ = hwMixingOn;
}

bool SndSampleParameters::isHWMixing(void) const
{
	CB_DEPIMPL(bool, hwMixingOn_);
	return hwMixingOn_;
}

void SndSampleParameters::setPreload(bool preloadingOn)
{
	CB_DEPIMPL(bool, preloaded_);
	preloaded_ = preloadingOn;
}

bool SndSampleParameters::isPreloading(void) const
{
	CB_DEPIMPL(bool, preloaded_);
	return preloaded_;
}

ostream& operator<<( ostream& o, const SndSampleParameters& t )
{
	o << "pImpl_->initialPosition_    " << t.pImpl_->initialPosition_ << std::endl;
	o << "pImpl_->maxAudibleDistance_ " << t.pImpl_->maxAudibleDistance_ << std::endl;
	o << "pImpl_->minAudibleDistance_ " << t.pImpl_->minAudibleDistance_ << std::endl;
	o << "pImpl_->sampleVolume_ " << t.pImpl_->sampleVolume_ << std::endl;
	o << "pImpl_->samplePriority_ " << t.pImpl_->samplePriority_ << std::endl;
	o << "pImpl_->loopCount_ " << t.pImpl_->loopCount_ << std::endl;
	o << "pImpl_->startOffset_ " << t.pImpl_->startOffset_ << std::endl;
	o << "pImpl_->is3DSound_ " << t.pImpl_->is3DSound_ << std::endl;
	o << "pImpl_->duration_ " << t.pImpl_->duration_ << std::endl;
	o << "pImpl_->hwMixingOn_ " << t.pImpl_->hwMixingOn_ << std::endl;
	return o;
}
///////////////////////////////

///* end of SMPPARAM.CPP ****************************/
