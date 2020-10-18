/*
 * M E M W A T C H . C P P
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <string.h>
#include <stdio.h>
#include "base/memwatch.hpp"
#include "base/poolallo.hpp"
#include "profiler/profiler.hpp"

// static
BaseMemWatcher& BaseMemWatcher::instance()
{
    static BaseMemWatcher instance_;
    return instance_;
}

BaseMemWatcher::BaseMemWatcher()
{
    //Initialise the hash tables
    for( size_t i = 0; i != SITE_HASH_BOUND; ++i )
        aSiteHashTable_[i] = NULL;

    for( size_t i = 0; i != POINTER_HASH_BOUND; ++i )
        aBlockHashTable_[i] = NULL;

    TEST_INVARIANT;
}

BaseMemWatcher::~BaseMemWatcher()
{
    TEST_INVARIANT;
    clear();
}

void BaseMemWatcher::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const BaseMemWatcher& t )
{

    o << "BaseMemWatcher " << (void*)&t << " start" << std::endl;
    o << "BaseMemWatcher " << (void*)&t << " end" << std::endl;

    return o;
}

BasePoolAllocator& BaseMemWatcher::allocator()
{
    static BasePoolAllocator theAllocator( 500000, 500000 );
    return theAllocator;
}

void BaseMemWatcher::addBlock( void* pBlock, size_t nBytes, ulong lineNumber, const char* aNewText )
{
    //Get the call stack
    static const size_t* stackAddress = ProProfiler::instance().pCallStack();
    size_t nStackFrames = ProProfiler::instance().nCallStackEntries();

    //Get number of characters in new text
    size_t nNewTextChars = (aNewText ? 1 + strlen( aNewText ) : 0);

    //Compute the hash value, then zoom along the linked list looking for a match
    size_t siteHashValue = hashValue( nStackFrames, stackAddress, nBytes, lineNumber );

    AllocationSite* pSite = aSiteHashTable_[ siteHashValue ];
    for( ; pSite; pSite = pSite->pNext_ )
    {

        if( nBytes == pSite->nBytes_  and
            lineNumber == pSite->lineNumber_  and
            nStackFrames == pSite->nStackFrames_  and
            nNewTextChars == pSite->nNewTextChars_ )
        {
            //Compare the call stack
            bool match = true;
            size_t* siteCallStack = pSite->aCallStack_;
            for( size_t i = nStackFrames; match and i--; )
                match = siteCallStack[i] == stackAddress[i];

            //Compare the new text
            if( match  and  (nNewTextChars == 0  or  strcmp( aNewText, pSite->aNewText_ ) == 0) )
                break;
        }
    }

    //If we don't have an existing site, create a new one
    if( pSite == NULL )
    {
        //Initialise the data
        pSite = _STATIC_CAST( AllocationSite*, allocator().alloc( sizeof( AllocationSite ) ) );
        pSite->nStackFrames_ = nStackFrames;

        pSite->aCallStack_ = _STATIC_CAST( size_t*, allocator().alloc( sizeof( size_t ) * nStackFrames ) );
        for( size_t i = 0; i != nStackFrames; ++i )
            pSite->aCallStack_[i] = stackAddress[i];

        pSite->nBytes_ = nBytes;
        pSite->nNewTextChars_ = nNewTextChars;
        if( nNewTextChars == 0 )
            pSite->aNewText_ = NULL;
        else
        {
            pSite->aNewText_ = _STATIC_CAST( char*, allocator().alloc( sizeof( char ) * nNewTextChars ) );
            strcpy( pSite->aNewText_, aNewText );
        }

        pSite->lineNumber_ = lineNumber;

        //Insert at front of linked list for the hash value
        pSite->pNext_ = aSiteHashTable_[ siteHashValue ];
        aSiteHashTable_[ siteHashValue ] = pSite;
    }

    //Increment the usage count for the site
    pSite->refCount_++;

    //Get hash value for the pointer
    size_t pointerHashValue = hashValue( pBlock );

    //Create an object representing the allocation
    AllocatedBlock* pAllocatedBlock = _STATIC_CAST( AllocatedBlock*, allocator().alloc( sizeof( AllocatedBlock ) ) );
    pAllocatedBlock->address_ = pBlock;
    pAllocatedBlock->pSite_ = pSite;

    //Insert at front of linked list
    AllocatedBlock* pOldFirstBlock = aBlockHashTable_[ pointerHashValue ];
    pAllocatedBlock->pNext_ = pOldFirstBlock;
    pAllocatedBlock->pLast_ = NULL;
    aBlockHashTable_[ pointerHashValue ] = pAllocatedBlock;

    if( pOldFirstBlock )
        pOldFirstBlock->pLast_ = pAllocatedBlock;
}

void BaseMemWatcher::removeBlock( void* pBlock )
{
    PRE( pBlock );

    //Compute the hash value
    size_t pointerHashValue = hashValue( pBlock );

    //Search the link list for a match
    for( AllocatedBlock* pAllocatedBlock = aBlockHashTable_[ pointerHashValue ]; pAllocatedBlock;
         pAllocatedBlock = pAllocatedBlock->pNext_ )
    {
        if( pAllocatedBlock->address_ == pBlock )
        {
            //we have a match: remove the block from the link list
            AllocatedBlock* pLast = pAllocatedBlock->pLast_;
            AllocatedBlock* pNext = pAllocatedBlock->pNext_;
            if( pLast )
                pLast->pNext_ = pNext;
            else
                aBlockHashTable_[ pointerHashValue ] = pNext;

            if( pNext )
                pNext->pLast_ = pLast;

            //Decrement the reference count in the site
            pAllocatedBlock->pSite_->refCount_--;

            //Release the memory
            allocator().free( pAllocatedBlock );

            //Look no further
            break;
        }
    }
}

void BaseMemWatcher::clear()
{
    //Clear out all the site objects
    for( size_t i = 0; i != SITE_HASH_BOUND; ++i )
    {
        AllocationSite* pSite = aSiteHashTable_[i];
        aSiteHashTable_[i] = NULL;

        AllocationSite* pNextSite;
        for( ; pSite; pSite = pNextSite )
        {
            pNextSite = pSite->pNext_;

            allocator().free( pSite->aCallStack_ );

            if( pSite->aNewText_ )
                allocator().free( pSite->aNewText_ );

            allocator().free( pSite );
        }
    }

    //Clear out all the block objects
    for( size_t i = 0; i != POINTER_HASH_BOUND; ++i )
    {
        AllocatedBlock* pAllocatedBlock = aBlockHashTable_[i];
        aBlockHashTable_[i] = NULL;

        AllocatedBlock* pNextBlock;
        for( ; pAllocatedBlock; pAllocatedBlock = pNextBlock )
        {
            pNextBlock = pAllocatedBlock->pNext_;

            allocator().free( pAllocatedBlock );
        }
    }
}

//static
size_t BaseMemWatcher::hashValue( size_t nStackFrames, const size_t* aCallStack, size_t nBytes, ulong lineNumber )
{
    size_t result = 0;

    //xor some junior bits from each address on the call stack, but ignore the junior 2 bits, coz they'll
    //tend to not vary much
    for( size_t i = nStackFrames; i--; )
    {
        size_t j = aCallStack[i] >> 2;
        result ^= j;
    }

    //now do the same with the other 2 values
    result ^= nBytes ^ lineNumber;
    result &= (BaseMemWatcher::SITE_HASH_BOUND - 1);

    POST( result < BaseMemWatcher::SITE_HASH_BOUND );
    return result;
}

//static
size_t BaseMemWatcher::hashValue( void* address )
{
    return (_REINTERPRET_CAST( size_t, address) >> 2) & (BaseMemWatcher::POINTER_HASH_BOUND - 1);
}

void BaseMemWatcher::traceOustandingAllocations( ostream& outStream )
{
    bool traceAnchor = true;
    ProProfiler& profiler = ProProfiler::instance();

    //Iterate through all the allocated blocks
    for( size_t i = 0; i != POINTER_HASH_BOUND; ++i )
    {
        for( AllocatedBlock* pAllocatedBlock = aBlockHashTable_[i]; pAllocatedBlock;
             pAllocatedBlock = pAllocatedBlock->pNext_ )
        {
            //Construct a string of the form used by profanal to do memory analysis for an allocation
            AllocationSite* pSite = pAllocatedBlock->pSite_;
            char* pNewText = pSite->aNewText_;
            char buffer[64];
            if( pNewText != NULL )
                sprintf( buffer, "NEW %d %#08x @ %.32s @", pSite->nBytes_, pAllocatedBlock->address_, pNewText );
            else
                sprintf( buffer, "NEW %d %#08x", pSite->nBytes_, pAllocatedBlock->address_ );

            //get the profiler to write the call stack line in the correct format for profanal
            profiler.traceStack( outStream, traceAnchor, pSite->nStackFrames_, pSite->aCallStack_,
                                 pSite->lineNumber_, buffer );
            traceAnchor = false;
        }
    }
}
/* End MEMWATCH.CPP *************************************************/
