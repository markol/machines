/*
 * P O O L A L L O . C P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdlib.h>
#include "base/poolallo.hpp"

BasePoolAllocator::BasePoolAllocator( ulong nInitialPoolBytes, ulong nExtensionBytes )
: pFirstUnusedBlock_( NULL ),
  pFirstSystemBlock_( NULL ),
  nInitialPoolBytes_( nInitialPoolBytes ),
  nExtensionBytes_( nExtensionBytes ),
  nUnitsAllocated_( 0 ),
  nBlocksAllocated_( 0 )

{
    //Initialise the pools to empty
    for( size_t i = 0; i != UNIT_THRESHOLD; ++i )
        aPoolHeader_[ i ] = NULL;

    TEST_INVARIANT;
}

BasePoolAllocator::~BasePoolAllocator()
{
    TEST_INVARIANT;
    ASSERT( allBlocksFreed(), "Deleting pool allocator with outstanding allocations" );
}

void BasePoolAllocator::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void* BasePoolAllocator::alloc( size_t nBytes )
{
    static size_t systemHeaderUnitSize = (sizeof( SystemBlock ) + (UNIT_SIZE - 1)) >> UNIT_SIZE_P2;

    //Compute the desired size in units
    size_t unitSize = (nBytes + (UNIT_SIZE - 1)) >> UNIT_SIZE_P2;

    PoolBlock* pReturnBlock;
    if( unitSize < UNIT_THRESHOLD )
    {
        //Get first pool block of this size on free list
        PoolBlock** pHeader = &aPoolHeader_[ unitSize ];
        pReturnBlock = *pHeader;

        if( pReturnBlock )
        {
            *pHeader = pReturnBlock->pNext_;
        }
        else
        {
            size_t poolBlockSize = unitSize + 1;
            if( not pFirstUnusedBlock_  ||  pFirstUnusedBlock_->leftUnits_ < poolBlockSize )
            {
                bool found = false;
                for( UnusedBlock* pTryUnusedBlock = pFirstUnusedBlock_;
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
                        pTryUnusedBlock->pNext_ = pFirstUnusedBlock_;
                        pFirstUnusedBlock_->pLast_ = pTryUnusedBlock;
                        pFirstUnusedBlock_ = pTryUnusedBlock;
                        found = true;
                        break;
                    }
                }

                if( not found )
                {
                    //Allocate a new system block
                    ulong nSystemBytes = (pFirstSystemBlock_ == NULL ? nInitialPoolBytes_ : nExtensionBytes_);
                    SystemBlock* pNewSystemBlock = (SystemBlock*)malloc( nSystemBytes );
                    size_t systemBlockUnitSize = nSystemBytes / sizeof( Unit );

                    if( pNewSystemBlock == NULL )
                    {
                        std::cout << "Out of memory" << std::endl;
                        MEMORY_STREAM( "Out of memory" << std::endl );
                    }

                    pNewSystemBlock->pNext_ = pFirstSystemBlock_;
                    pFirstSystemBlock_ = pNewSystemBlock;

                    UnusedBlock* pNewUnusedBlock = &pFirstSystemBlock_->unused_;
                    pNewUnusedBlock->pNext_ = pFirstUnusedBlock_;
                    pNewUnusedBlock->pLast_ = NULL;
                    pNewUnusedBlock->leftUnits_ = systemBlockUnitSize - systemHeaderUnitSize;
                    pNewUnusedBlock->firstFreeUnit_ = ((Unit*)pNewSystemBlock) + systemHeaderUnitSize;

                    if( pFirstUnusedBlock_ )
                        pFirstUnusedBlock_->pLast_ = pNewUnusedBlock;

                    pFirstUnusedBlock_ = pNewUnusedBlock;
                }
            }

            pReturnBlock = (PoolBlock*)pFirstUnusedBlock_->firstFreeUnit_;
            pFirstUnusedBlock_->firstFreeUnit_ += poolBlockSize;
            pFirstUnusedBlock_->leftUnits_ -= poolBlockSize;

            pReturnBlock->unitSize_ = unitSize;
        }
    }
    else
    {
        pReturnBlock = (PoolBlock*)malloc( (unitSize + 1) << UNIT_SIZE_P2 );

        if( pReturnBlock == NULL )
        {
            std::cout << "Out of memory" << std::endl;
            MEMORY_STREAM( "Out of memory" << std::endl );
        }

        pReturnBlock->unitSize_ = unitSize;
    }

    nUnitsAllocated_ += unitSize;
    ++nBlocksAllocated_;

    return &pReturnBlock->pNext_;
}

void BasePoolAllocator::free( void* pBlock )
{
    PoolBlock* pOldBlock = (PoolBlock*)(((Unit*)pBlock) - 1);
    size_t unitSize = pOldBlock->unitSize_;

    if( pOldBlock->unitSize_ < UNIT_THRESHOLD )
    {
        PoolBlock** pHeader = &aPoolHeader_[ unitSize ];
        pOldBlock->pNext_ = *pHeader;
        *pHeader = pOldBlock;
    }
    else
    {
        free( (((Unit*)pBlock) - 1) );
    }

    nUnitsAllocated_ -= unitSize;
    --nBlocksAllocated_;
}

bool BasePoolAllocator::allBlocksFreed() const
{
    return nUnitsAllocated_ == 0  and  nBlocksAllocated_ == 0;
}
/* End POOLALLO.CPP *************************************************/
