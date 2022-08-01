/*
 * B N D I D G E N . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    UtlBoundedIdGenerator

    Generates unique ids between zero and a fixed upper bound
*/

#ifndef _UTILITY_BNDIDGEN_HPP
#define _UTILITY_BNDIDGEN_HPP

#include "base/base.hpp"
#include "utility/id.hpp"

//orthodox canonical (revoked)
class UtlBoundedIdGenerator
{
public:
    //ctor defines upper bound for ids as upperBound. This also limits the
    //number of ids that can be concurrently allocated.
    UtlBoundedIdGenerator( UtlId upperBound );

    //dtor
    ~UtlBoundedIdGenerator();

    //The highest possible id + 1
    UtlId upperBound() const;

    //Number of ids that can still be allocated
    ulong nUnusedIds() const;

    //True if id has been allocated
    bool isAllocated( UtlId id ) const;
    //PRE( id < upperBound() )

    //Allocate and return a new unique id
    UtlId nextId();
    //PRE( nUnusedIds() > 0 )

    //Allocate and return a new unique id
    UtlId nextId( UtlId minId, UtlId maxId );
    //PRE( nUnusedIds() > 0 )

    //Allocates id (the id must have previously been determined to be free)
	//example: might occur via a network message
    void allocateId( UtlId id );
	//PRE( not isAllocated( id ) )

    //Frees id for re-use
    void freeId( UtlId id );
    //PRE( isAllocated( id ) )

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const UtlBoundedIdGenerator& t );

private:
    // Operations deliberately revoked
    UtlBoundedIdGenerator( const UtlBoundedIdGenerator& );
    UtlBoundedIdGenerator& operator =( const UtlBoundedIdGenerator& );
    bool operator ==( const UtlBoundedIdGenerator& );

    //Data members
    UtlId upperBound_; //Upper bound for ids
    UtlId lastAllocatedId_; //Caches last id allocated
    ulong nUnused_; //Number of ids not allocated
    uchar* aFlags_; //Pointer to array of flags: 1 => allocated, 0 =>unallocated
};

#ifdef _INLINE
    #include "utility/bndidgen.ipp"
#endif


#endif

/* End BNDIDGEN.HPP *************************************************/
