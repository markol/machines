/*
 * C S 2 M O C H U . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysCS2dMotionChunk

    Represents a PhysMotionChunk registered in a PhysConfigSpace2d
    HAS-A PhysMotionChunk
*/

#ifndef _PHYS_CS2MOCHU_HPP
#define _PHYS_CS2MOCHU_HPP

#include "base/base.hpp"

#include "phys/cspace2.hpp"
#include "phys/motchunk.hpp"

//Orthodox canonical
class PhysCS2dMotionChunk
{
public:
    //Useful types
    typedef PhysConfigSpace2d::ObjectId ObjectId;
    typedef PhysConfigSpace2d::MotionChunkId MotionChunkId;

    //ctor. Specifies config space allocated id, client defined related objectId,
    //and the motion chunk data.
    PhysCS2dMotionChunk( const MotionChunkId& id, const ObjectId& objectId,
                         const PhysMotionChunk& motionChunk );

    //dtor.
    ~PhysCS2dMotionChunk();

    //Accessors
    const PhysMotionChunk& motionChunk() const;
    const MotionChunkId& id() const;
    const ObjectId& objectId() const;
    void boundary( MexAlignedBox2d* pBox ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysCS2dMotionChunk& t );

private:
    // Operation deliberately revoked
    PhysCS2dMotionChunk( const PhysCS2dMotionChunk& );
    PhysCS2dMotionChunk& operator =( const PhysCS2dMotionChunk& );
    bool operator ==( const PhysCS2dMotionChunk& );

    //Data members
    PhysMotionChunk motionChunk_; //Defines the motion in space and time
    ObjectId objectId_; //Id of the related object
    MotionChunkId id_; //Config space allocated id for this chunk
};

#ifdef _INLINE
    #include "internal/cs2mochu.ipp"
#endif


#endif

/* End CS2MOCHU.HPP *************************************************/
