/*
 * C S 2 M O C H U . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/internal/cs2mochu.hpp"

#ifndef _INLINE
    #include "phys/internal/cs2mochu.ipp"
#endif

PhysCS2dMotionChunk::PhysCS2dMotionChunk
(
    const MotionChunkId& id, const ObjectId& objectId, const PhysMotionChunk& motionChunk
)
:   id_( id ),
    objectId_( objectId ),
    motionChunk_( motionChunk )
{

    TEST_INVARIANT;
}

PhysCS2dMotionChunk::~PhysCS2dMotionChunk()
{
    TEST_INVARIANT;

}

void PhysCS2dMotionChunk::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PhysCS2dMotionChunk& t )
{

    o << "PhysCS2dMotionChunk " << (void*)&t << " start" << std::endl;
    o << "PhysCS2dMotionChunk " << (void*)&t << " end" << std::endl;

    return o;
}

/* End CS2MOCHU.CPP *************************************************/
