/*
 * F R A M E R E G . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
const PhysAbsoluteTime& W4dFrameRegulator::startTime() const
{
    return startTime_;
}

_CODE_INLINE
const PhysRelativeTime& W4dFrameRegulator::idealDuration() const
{
    return idealDuration_;
}

_CODE_INLINE
const PhysRelativeTime& W4dFrameRegulator::maxDuration() const
{
    return maxDuration_;
}

_CODE_INLINE
size_t W4dFrameRegulator::startFrame() const
{
    return startFrame_;
}

_CODE_INLINE
size_t W4dFrameRegulator::nFrames() const
{
    return nFrames_;
}

_CODE_INLINE
void W4dFrameRegulator::startTime( const PhysAbsoluteTime& startTime )
{
    PRE( startFrame_ == 0 );

    startTime_ = startTime;
}
/* End FRAMEREG.IPP *************************************************/
