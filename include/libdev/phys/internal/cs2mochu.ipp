/*
 * C S 2 M O C H U . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
const PhysMotionChunk& PhysCS2dMotionChunk::motionChunk() const
{
    return motionChunk_;
}

_CODE_INLINE
const PhysConfigSpace2d::MotionChunkId& PhysCS2dMotionChunk::id() const
{
    return id_;
}

_CODE_INLINE
const PhysConfigSpace2d::ObjectId& PhysCS2dMotionChunk::objectId() const
{
    return objectId_;
}

_CODE_INLINE
void PhysCS2dMotionChunk::boundary( MexAlignedBox2d* pBox ) const
{
    motionChunk_.boundary( pBox );
}
/* End CS2MOCHU.IPP *************************************************/
