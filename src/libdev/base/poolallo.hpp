/*
 * P O O L A L L O . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    BasePoolAllocator

    Handles allocation and freeing of blocks of memory from a self-contained pool.
    Can be used for allocations which are extensive and temporary, so that the blocks
    allocated can be freed without fragmenting memory.
*/

#ifndef _BASE_POOLALLO_HPP
#define _BASE_POOLALLO_HPP

#include "base/base.hpp"

class BasePoolAllocator
// Canonical form revoked
{
public:
    //nInitialPoolBytes is the size of the block of memory to be allocated initially for the pool.
    //nExtensionBytes is the size of memory block to be allocated when additional memory is required
    //for the pool.
    BasePoolAllocator( ulong nInitialPoolBytes, ulong nExtensionBytes );

    ~BasePoolAllocator();
    //PRE( allBlocksFreed() );

    //Allocate a block of memory of at least nBytes in size, and return the ptr
    void* alloc( size_t nBytes );

    //Free a block of memory at pBlock, which must previously have been allocated from this pool
    //using the alloc function
    void free( void* pBlock );

    //True iff all blocks allocated using alloc() have been released using free()
    bool allBlocksFreed() const;

    void CLASS_INVARIANT;

private:
    //revoked
    BasePoolAllocator( const BasePoolAllocator& );
    BasePoolAllocator& operator =( const BasePoolAllocator& );

    enum { UNIT_SIZE_P2 = 2, UNIT_SIZE = 4, UNIT_THRESHOLD = 4097 };

    //Block that has been allocated. It always remains the same size.
    typedef ulong Unit;
    struct PoolBlock
    {
        size_t unitSize_; //Size of the block's usable space in units
        PoolBlock* pNext_; //Pointer to next block of same size on freelist
    };

    //Block from which pool blocks can be allocated
    struct UnusedBlock
    {
        UnusedBlock* pNext_;
        UnusedBlock* pLast_;
        size_t leftUnits_;
        Unit* firstFreeUnit_;
    };

    //Block allocated directly from the OS
    struct SystemBlock
    {
        SystemBlock* pNext_;
        UnusedBlock unused_;
    };

    //Data members
    UnusedBlock* pFirstUnusedBlock_;
    SystemBlock* pFirstSystemBlock_;
    ulong nInitialPoolBytes_;
    ulong nExtensionBytes_;
    ulong nUnitsAllocated_;
    ulong nBlocksAllocated_;
    PoolBlock* aPoolHeader_[ UNIT_THRESHOLD ];
};


#endif

/* End POOLALLO.HPP *************************************************/
