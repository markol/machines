/*
 * S A M P L E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
*/

#include "sound/internal/sample.hpp"
#ifndef _INLINE
	#include "sound/internal/sample.ipp"
#endif

Sample::Sample( const SndSampleParameters& params ):
sampleParameters_(params),
samplePriority_(params.priority()),
isPaused_(false),
isSilenced_(false),
id_(nextID()),
volume_(params.volume()),
resourceResponsibility_(Sample::CLIENT),
stopPending_(false)
{
	if(params.loopCount() == 0 && params.duration() == 0.0)
	{
		loopForever_ = true;
	}
	else
		loopForever_ = false;
}

Sample::~Sample()
{
}

const char* Sample::path() const
{
	return sampleParameters_.id().pathname().c_str();
}

int Sample::volume() const
{
	return volume_;
}

size_t Sample::id() const
{
	return id_;
}

size_t Sample::nextID()
{
	static size_t id = 0;
	return id++;
}

bool Sample::operator ==(const Sample& rhs)
{
	return (this->samplePriority_ == rhs.samplePriority_);
}

bool Sample::operator <(const Sample& rhs)
{
	return (this->samplePriority_ < rhs.samplePriority_);
}

bool Sample::operator >(const Sample& rhs)
{
	return (rhs.samplePriority_ < this->samplePriority_);
}

