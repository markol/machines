/*
 * S A M P L E . I P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
*/

#ifdef _INLINE
	#define _CODE_INLINE inline
#else
	#define _CODE_INLINE
#endif

/////////////////////////////////////////////////
_CODE_INLINE
bool Sample::isPaused() const
{
	return isPaused_;
}
/////////////////////////////////////////////////
_CODE_INLINE
void Sample::silence( void )
{
	isSilenced_ = true;
	unSilencedVolume_ = volume_;
	volume(0);
}
/////////////////////////////////////////////////
_CODE_INLINE
void Sample::unsilence( void )
{
	isSilenced_ = false;
	volume(unSilencedVolume_);
}
/////////////////////////////////////////////////
_CODE_INLINE
bool Sample::isSilenced() const
{
	return isSilenced_;
}
/////////////////////////////////////////////////
_CODE_INLINE
Snd::SamplePriority Sample::priority( void ) const
{
	return samplePriority_;
}
/////////////////////////////////////////////////
_CODE_INLINE
bool Sample::isAudible() const
{
	return !isSilenced_;
}

/////////////////////////////////////////////////
_CODE_INLINE
bool Sample::is3D() const
{
	return sampleParameters_.is3dSound();
}

/////////////////////////////////////////////////
_CODE_INLINE
bool Sample::loopForever() const
{
	return loopForever_;
}

/////////////////////////////////////////////////
_CODE_INLINE
Sample::SampleState Sample::state() const
{
	return state_;
}

/////////////////////////////////////////////////
_CODE_INLINE
Sample::Participant Sample::getResourceResponsibility( void )
{
	return resourceResponsibility_;
}

/////////////////////////////////////////////////
_CODE_INLINE
void Sample::setResourceResponsibility(Participant owner)
{
	resourceResponsibility_ = owner;
}

/////////////////////////////////////////////////
_CODE_INLINE
bool Sample::stopPending() const
{
	return stopPending_;
}

/////////////////////////////////////////////////
_CODE_INLINE
void Sample::stopPending(bool newPending)
{
	stopPending_ = newPending;
}
