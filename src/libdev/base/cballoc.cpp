/*
 * C B A L L O C . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include <cstdio>
#include <cstring>
#include <string>
#include "base/cballoc.hpp"
#include "base/poolallo.hpp"
#include "system/memcaps.hpp"
#include "profiler/profiler.hpp"

/////////////////////////////////////////////
//These constants define the parameters of the allocation system

typedef ulong Unit;

//The size of chunks allocated from the operating system in bytes
#define SYSTEM_BLOCK_SIZE 1048576

//Same value in Units
#define SYSTEM_BLOCK_UNIT_SIZE (SYSTEM_BLOCK_SIZE / sizeof( Unit ))

//The size in bytes of the units in which memory is allocated
#define UNIT_SIZE 4

//the unit size as a power of 2
#define UNIT_SIZE_P2 2

//One more than the number of units in the largest block we allocate from system blocks.
//Larger blocks are allocated directly from the system, and returned to the system when free'd.
#define UNIT_THRESHOLD 4097


/////////////////////////////////////////////

//Used for allocation tracing
#define CB_ALLOC_TRACING

#ifdef CB_ALLOC_TRACING
#define ALLOC_TRACE( x ) x
#else
#define ALLOC_TRACE( x )
#endif

/////////////////////////////////////////////

//Block that has been allocated. It always remains the same size.
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

/////////////////////////////////////////////

static bool cbInitialised = false;
static PoolBlock* aPoolHeader[ UNIT_THRESHOLD ];
static UnusedBlock* pFirstUnusedBlock = NULL;
static SystemBlock* pFirstSystemBlock = NULL;
static size_t systemHeaderUnitSize = (sizeof( SystemBlock ) + (UNIT_SIZE - 1)) >> UNIT_SIZE_P2;
static size_t unitSizeMap[ UNIT_THRESHOLD ];
static BasePoolAllocator* pCurrentPoolAllocator = NULL;

#ifdef CB_ALLOC_TRACING
//These are used to keep track of allocations
static uint systemMemoryAllocated = 0;
static uint systemMemoryBlocksAllocated = 0;
static uint anTotalBlocks[ UNIT_THRESHOLD ];
static uint anPoolBlocks[ UNIT_THRESHOLD ];
static uint nMallocAllocs = 0;
static uint nMallocFrees = 0;
static uint memoryMallocd = 0;
#endif

/////////////////////////////////////////////

void* cbAlloc( size_t nBytes )
{
    if( pCurrentPoolAllocator != NULL )
        return pCurrentPoolAllocator->alloc( nBytes );

    //Compute the desired size in units
    size_t unitSize = (nBytes + (UNIT_SIZE - 1)) >> UNIT_SIZE_P2;

    PoolBlock* pReturnBlock;
    if( unitSize < UNIT_THRESHOLD )
    {
        //Get the actual block size to allocate
        unitSize = unitSizeMap[ unitSize ];

        //Get first pool block of this size on free list
        PoolBlock** pHeader = &aPoolHeader[ unitSize ];
        pReturnBlock = *pHeader;

        if( pReturnBlock )
        {
            *pHeader = pReturnBlock->pNext_;

            //Maintain stats
            ALLOC_TRACE( --(anPoolBlocks[ unitSize ]);)
        }
        else
        {
            size_t poolBlockSize = unitSize + 1;
            if( not pFirstUnusedBlock  ||  pFirstUnusedBlock->leftUnits_ < poolBlockSize )
            {
                bool found = false;
                for( UnusedBlock* pTryUnusedBlock = pFirstUnusedBlock;
                     pTryUnusedBlock; pTryUnusedBlock = pTryUnusedBlock->pNext_ )
                {
                    if( pTryUnusedBlock->leftUnits_ > unitSize )
                    {
                        //Use this unused block. Swap to head of list
                        UnusedBlock* pNext = pTryUnusedBlock->pNext_;
                        UnusedBlock* pLast = pTryUnusedBlock->pLast_;

                        if( pNext )
                            pNext->pLast_ = pLast;

                        if( pLast )
                            pLast->pNext_ = pNext;

                        pTryUnusedBlock->pLast_ = NULL;
                        pTryUnusedBlock->pNext_ = pFirstUnusedBlock;
                        pFirstUnusedBlock->pLast_ = pTryUnusedBlock;
                        pFirstUnusedBlock = pTryUnusedBlock;
                        found = true;
                        break;
                    }
                }

                if( not found )
                {
                    //Allocate a new system block
                    SystemBlock* pNewSystemBlock = (SystemBlock*)malloc( SYSTEM_BLOCK_SIZE );

                    if( pNewSystemBlock == NULL )
                    {
                        std::cout << "Out of memory" << std::endl;
                        std::cerr << "Out of memory" << std::endl;
                        MEMORY_STREAM( "Out of memory" << std::endl );
                        ASSERT_INFO("Out of memory");
                    }

                    //Update stats
                    ALLOC_TRACE( systemMemoryAllocated += SYSTEM_BLOCK_SIZE; )
                    ALLOC_TRACE( ++systemMemoryBlocksAllocated; )

                    pNewSystemBlock->pNext_ = pFirstSystemBlock;
                    pFirstSystemBlock = pNewSystemBlock;

                    UnusedBlock* pNewUnusedBlock = &pFirstSystemBlock->unused_;
                    pNewUnusedBlock->pNext_ = pFirstUnusedBlock;
                    pNewUnusedBlock->pLast_ = NULL;
                    pNewUnusedBlock->leftUnits_ = SYSTEM_BLOCK_UNIT_SIZE - systemHeaderUnitSize;
                    pNewUnusedBlock->firstFreeUnit_ = ((Unit*)pNewSystemBlock) + systemHeaderUnitSize;

                    if( pFirstUnusedBlock )
                        pFirstUnusedBlock->pLast_ = pNewUnusedBlock;

                    pFirstUnusedBlock = pNewUnusedBlock;
                }
            }

            pReturnBlock = (PoolBlock*)pFirstUnusedBlock->firstFreeUnit_;
            pFirstUnusedBlock->firstFreeUnit_ += poolBlockSize;
            pFirstUnusedBlock->leftUnits_ -= poolBlockSize;

            pReturnBlock->unitSize_ = unitSize;

            //Update stats
            ALLOC_TRACE( ++(anTotalBlocks[ unitSize ]);)
        }
    }
    else
    {
        pReturnBlock = (PoolBlock*)malloc( (unitSize + 1) << UNIT_SIZE_P2 );

        if( pReturnBlock == NULL )
        {
            std::cout << "Out of memory" << std::endl;
            std::cerr << "Out of memory" << std::endl;
            MEMORY_STREAM( "Out of memory" << std::endl );
            ASSERT_INFO("Out of memory");
        }

        ALLOC_TRACE( ++nMallocAllocs; )
        ALLOC_TRACE( memoryMallocd += (unitSize + 1) << UNIT_SIZE_P2; )

        pReturnBlock->unitSize_ = unitSize;
    }

//Comment in to trap run time leaks	ProProfiler& profiler = ProProfiler::instance();
//Comment in to trap run time leaks	if( (
//Comment in to trap run time leaks		unitSize == 80 or
//Comment in to trap run time leaks		unitSize == 32 or
//Comment in to trap run time leaks		unitSize == 28 or
//Comment in to trap run time leaks		unitSize == 44 or
//Comment in to trap run time leaks		unitSize == 40 )
//Comment in to trap run time leaks		and profiler.isProfilingEnabled() )
//Comment in to trap run time leaks	{
//Comment in to trap run time leaks		char buffer[64];
//Comment in to trap run time leaks		sprintf( buffer, "NEW %d %#08x", nBytes, (void*)(&pReturnBlock->pNext_) );
//Comment in to trap run time leaks		profiler.traceStack( Diag::instance().danielStream(), true, 0, buffer );
//Comment in to trap run time leaks	}


    return &pReturnBlock->pNext_;
}

void cbFree( void* pBlock )
{
    if( pCurrentPoolAllocator != NULL )
    {
        pCurrentPoolAllocator->free( pBlock );
        return;
    }

    PoolBlock* pOldBlock = (PoolBlock*)(((Unit*)pBlock) - 1);
    size_t unitSize = pOldBlock->unitSize_;
//Comment in to trap run time leaks	ProProfiler& profiler = ProProfiler::instance();
//Comment in to trap run time leaks	if( (
//Comment in to trap run time leaks		unitSize == 80 or
//Comment in to trap run time leaks		unitSize == 32 or
//Comment in to trap run time leaks		unitSize == 28 or
//Comment in to trap run time leaks		unitSize == 44 or
//Comment in to trap run time leaks		unitSize == 40 )
//Comment in to trap run time leaks		and profiler.isProfilingEnabled() )
//Comment in to trap run time leaks	{
//Comment in to trap run time leaks	    //Set up string detailing the extra information we want
//Comment in to trap run time leaks	    char buffer[64];
//Comment in to trap run time leaks	    sprintf( buffer, "DELETE %#08x", pBlock );
//Comment in to trap run time leaks		profiler.traceStack( Diag::instance().danielStream(), true, 0, buffer );
//Comment in to trap run time leaks	}


    if( pOldBlock->unitSize_ < UNIT_THRESHOLD )
    {
        PoolBlock** pHeader = &aPoolHeader[ unitSize ];
        pOldBlock->pNext_ = *pHeader;
        *pHeader = pOldBlock;

        //Maintain stats
        ALLOC_TRACE( ++(anPoolBlocks[ unitSize ]);)
    }
    else
    {
        free( (((Unit*)pBlock) - 1) );

        //Maintain stats
        ALLOC_TRACE( ++nMallocFrees; )
        ALLOC_TRACE( memoryMallocd -= (unitSize + 1) << UNIT_SIZE_P2; )
    }
}

void cbInitialise()
{
    //Initialise all the pool headers
    for( size_t i = 0; i != UNIT_THRESHOLD; ++i )
    {
        aPoolHeader[ i ] = NULL;
#ifdef CB_ALLOC_TRACING
        anTotalBlocks[ i ] = 0;
        anPoolBlocks[ i ] = 0;
#endif
    }

    //Set up the map of actual sizes to allocate for given unit sizes
    unitSizeMap[ 0 ] = 1;
    unitSizeMap[ 1 ] = 1;
    unitSizeMap[ 2 ] = 2;
    unitSizeMap[ 3 ] = 3;
    unitSizeMap[ 4 ] = 4;

    size_t currentSize = 5;
    size_t limit = 64;
    size_t round = 4;

    while( currentSize < UNIT_THRESHOLD )
    {
        while( currentSize <= limit  and  currentSize < UNIT_THRESHOLD )
        {
            unitSizeMap[ currentSize ] = ((currentSize + round - 1) / round) * round;
            ++currentSize;
        }

        limit *= 2;
        round *= 2;
    }

    //And the others just to be safe
    pFirstUnusedBlock = NULL;
    pFirstSystemBlock = NULL;
    cbInitialised = true;
}

void cbClose()
{
    //Free all the system blocks
    SystemBlock* pSystemBlock = pFirstSystemBlock;
    while( pSystemBlock )
    {
        SystemBlock* pNext = pSystemBlock->pNext_;
        free( pSystemBlock );
        pSystemBlock = pNext;
    }

    cbInitialised = false;
}

void cbAllocTrace( ostream& o, const std::string& title,  AllocTraceOptions option )
{
    //Print the header including overall memory allocation info
    o << "=======================================================" << std::endl;
    o << title << std::endl;
    SysMemoryCaps::outputInfo( o );

    o << std::endl << "System blocks allocated " << systemMemoryBlocksAllocated
      << "  Total memory " << (systemMemoryBlocksAllocated * SYSTEM_BLOCK_SIZE) << std::endl;

    //Run through all the pool block sizes
    if( option == CB_ALLOC_DETAIL_BLOCKS )
    {
        o << "Size: nUsed    nFree    memUsed   memFree " << std::endl;
    }

    uint totalUsedMemory = 0;
    uint totalPoolMemory = 0;
    for( size_t i = 1; i < UNIT_THRESHOLD; ++i )
    {
        if( anTotalBlocks[i] != 0 )
        {
            uint usedMemory = ((anTotalBlocks[i] - anPoolBlocks[i])*(i+1)*UNIT_SIZE);
            uint poolMemory = (anPoolBlocks[i]*(i+1)*UNIT_SIZE);
            totalUsedMemory += usedMemory;
            totalPoolMemory += poolMemory;
            if( option == CB_ALLOC_DETAIL_BLOCKS )
            {
				o.width( 4 );
				o << i << ": ";
				o.width(8);
				o << (anTotalBlocks[i] - anPoolBlocks[i]) << " ";
				o.width(8);
				o << anPoolBlocks[i] << " ";
				o.width(9);
				o << usedMemory << " ";
				o.width(9);
				o << poolMemory << std::endl;
            }
        }
    }

    o << "Total used memory (normal size blocks) = " << totalUsedMemory << std::endl;
    o << "Total pool memory (normal size blocks) = " << totalPoolMemory << std::endl;
    o << "Total malloc allocations = " << nMallocAllocs << std::endl;
    o << "Outstanding malloc allocations = " << (nMallocAllocs - nMallocFrees) << " (" << memoryMallocd << ")" << std::endl;
}

void* cbRealloc( void* pBlock, size_t nBytes )
{
    void* pReturnBlock;

    //Check for simple allocation
    if( pBlock == NULL )
    {
        pReturnBlock = cbAlloc( nBytes );
    }
    else
    {
        //Get unit size required for new block
        size_t unitSize = (nBytes + (UNIT_SIZE - 1)) >> UNIT_SIZE_P2;
        if( unitSize == 0 )
            unitSize = 1;

        //Get the size of the existing block
        PoolBlock* pOldBlock = (PoolBlock*)(((Unit*)pBlock) - 1);
        size_t oldUnitSize = pOldBlock->unitSize_;

        //See if ok
        if( oldUnitSize >= unitSize )
        {
            //No need to change it
            pReturnBlock = pBlock;
        }
        else
        {
            //Allocate a new block
            pReturnBlock = cbAlloc( nBytes );

            //Copy the old contents into the new block
            std::memcpy( pReturnBlock, pBlock, oldUnitSize << UNIT_SIZE_P2 );

            //Release the old block
            cbFree( pBlock );
        }
    }

    return pReturnBlock;
}

void cbTouchAll()
{
    //Iterate thru the system blocks
    for( SystemBlock* pSystemBlock = pFirstSystemBlock; pSystemBlock != NULL; pSystemBlock = pSystemBlock->pNext_ )
    {
        ulong i = 537;

        //Visit words at 2k intervals of the system block
        ulong* pWord = (ulong*)pSystemBlock;
        for( ulong offset = 0; offset < SYSTEM_BLOCK_UNIT_SIZE; offset += 500 )
        {
            ulong j = *(pWord+offset);
            i = (i + j) % 533;
        }
    }
}

void pushCurrentPoolAllocator( BasePoolAllocator* pAllocator )
{
    PRE( pCurrentPoolAllocator == NULL );
    PRE( pAllocator != NULL );

    pCurrentPoolAllocator = pAllocator;
}

void popCurrentPoolAllocator()
{
    PRE( pCurrentPoolAllocator != NULL );

    pCurrentPoolAllocator = NULL;
}

/* End CBALLOC.CPP **************************************************/
