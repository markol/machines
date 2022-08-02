/***********************************************************

  A L S A M P L E . I P P

***********************************************************/

/*
*/

#ifdef _INLINE
	#define _CODE_INLINE inline
#else
	#define _CODE_INLINE
#endif

////////////////////////////////////////////////////////////
//VIRTUAL
_CODE_INLINE
bool ALSample::isPlaying( void )
{
	bool rval = true;
	if(state_ == STOPPED)
		rval = false;

	return rval;
}
////////////////////////////////////////////////////////////
_CODE_INLINE
bool ALSample::isValid() const
{
	//Check that our Buffer pointer is meaningful
	return alBuffer_ != 0 && alSource_ != 0;
}

////////////////////////////////////////////////////////////
_CODE_INLINE
Snd::RelativeTime ALSample::length( void ) const
{
	ASSERT(false, "Not implemented yet");
	return 0;
}
////////////////////////////////////////////////////////////
_CODE_INLINE
void ALSample::pause(void)
{
	state_ = _STATIC_CAST(ALSample::SampleState , Sample::PAUSED );
    alSourcePause(alSource_);
}
////////////////////////////////////////////////////////////
_CODE_INLINE
void ALSample::restart(void)
{
	state_ = PLAYING;
    alSourcei(alSource_, AL_LOOPING, AL_TRUE);
    alSourceRewind(alSource_);
    alSourcePlay(alSource_);
}
////////////////////////////////////////////////////////////
/* DXSAMPLE.IPP **********************************/
