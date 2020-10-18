/*
 * M O T C H U N K . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
const MexPoint2d& PhysMotionChunk::startPoint() const
{
    return startPoint_;
}

_CODE_INLINE
const MexPoint2d& PhysMotionChunk::endPoint() const
{
    return endPoint_;
}

_CODE_INLINE
MATHEX_SCALAR PhysMotionChunk::clearance() const
{
    return clearance_;
}

_CODE_INLINE
const PhysRampAcceleration& PhysMotionChunk::motionProfile() const
{
    return motionProfile_;
}

_CODE_INLINE
const PhysAbsoluteTime& PhysMotionChunk::createTime() const
{
    return createTime_;
}

_CODE_INLINE
const PhysRelativeTime& PhysMotionChunk::motionTimeOffset() const
{
    return motionTimeOffset_;
}

_CODE_INLINE
const PhysAbsoluteTime& PhysMotionChunk::expiryTime() const
{
	PRE( hasExpiryTime() );

    return expiryTime_;
}

_CODE_INLINE
const MexDouble& PhysMotionChunk::minHeight() const
{
    return minHeight_;
}

_CODE_INLINE
const MexDouble& PhysMotionChunk::maxHeight() const
{
    return maxHeight_;
}

_CODE_INLINE
bool PhysMotionChunk::hasExpiryTime() const
{
	return hasExpiryTime_;
}

/* End MOTCHUNK.IPP *************************************************/
