/*
 * C M O T C H U N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/cmotchun.hpp"
#include "phys/motchunk.hpp"

PhysCompressedMotionChunk::PhysCompressedMotionChunk()
:   createTime_( 0 ),
    motionTimeOffset_( 0 ),
    expiryTime_( 0 ),
    clearance_( 0 ),
    hasExpiryTime_( false ),
	minHeight_( 0 ),
    maxHeight_( 0 )

{

    TEST_INVARIANT;
}

PhysCompressedMotionChunk::PhysCompressedMotionChunk( const PhysMotionChunk& mot )
{
    startPoint_ = mot.startPoint_;
    endPoint_ = mot.endPoint_;
    motionProfile_ = mot.motionProfile_;
    createTime_ = mot.createTime_;
    motionTimeOffset_ = mot.motionTimeOffset_;
    expiryTime_ = mot.expiryTime_;
    clearance_ = mot.clearance_;
    hasExpiryTime_ = mot.hasExpiryTime_;
    minHeight_ = mot.minHeight_;
    maxHeight_ = mot.maxHeight_;
}


PhysCompressedMotionChunk::~PhysCompressedMotionChunk()
{
    TEST_INVARIANT;

}

void PhysCompressedMotionChunk::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PhysCompressedMotionChunk& t )
{

    o << "PhysCompressedMotionChunk " << (void*)&t << " start" << std::endl;
    o << "startPoint_ " << t.startPoint_ << std::endl;
    o << "endPoint_ " << t.endPoint_ << std::endl;
    o << "motionProfile_ " << t.motionProfile_ << std::endl;
    o << "createTime_ " << t.createTime_ << std::endl;
    o << "motionTimeOffset_ " << t.motionTimeOffset_ << std::endl;
    o << "expiryTime_ " << t.expiryTime_ << std::endl;
    o << "clearance_ " << t.clearance_ << std::endl;
    o << "hasExpiryTime_ " << t.hasExpiryTime_ << std::endl;
    o << "minHeight_ " << t.minHeight_ << std::endl;
    o << "maxHeight_ " << t.maxHeight_ << std::endl;
    o << "PhysCompressedMotionChunk " << (void*)&t << " end" << std::endl;

    return o;
}

/* End CMOTCHUN.CPP *************************************************/
