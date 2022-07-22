/*
 * M E M W A T C H . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    BaseMemWatcher

    Singleton object used for keeping track of outstanding allocations.
    Can be used for memory leak tracking, or getting handle on where memory
    allocation is happening.
*/

#ifndef _BASE_MEMWATCH_HPP
#define _BASE_MEMWATCH_HPP

#include "base/base.hpp"

class BasePoolAllocator;

class BaseMemWatcher
// Canonical form revoked
{
public:
    //  Singleton class
    static BaseMemWatcher& instance();
    ~BaseMemWatcher();

    //Register a block allocation at address pBlock, of requested size nBytes, at line lineNumber in
    //source file, with optional pointer to null-terminated string of new text.
    void addBlock( void* pBlock, size_t nBytes, ulong lineNumber, const char* aNewText );

    //Unregister the block allocated with address pBlock
    void removeBlock( void* pBlock );

    //Write profiler call stack lines to ostream for all the current outstanding blocks
    void traceOustandingAllocations( ostream& outStream );

    //Clear out all existing data
    void clear();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const BaseMemWatcher& t );

    //revoked
    BaseMemWatcher( const BaseMemWatcher& );
    BaseMemWatcher& operator =( const BaseMemWatcher& );

    BaseMemWatcher();

    enum { SITE_HASH_BOUND = 1024, POINTER_HASH_BOUND = 4096 };

    //Represents a site in the program at which memory allocation occurs
    struct AllocationSite
    {
        AllocationSite* pNext_; //Next on hash linked list
        size_t nStackFrames_; //Number of frames in aCallStack_
        size_t* aCallStack_; //Array of call stack addresses
        size_t nBytes_; //Size in bytes of object allocated at this site
        size_t nNewTextChars_; //Number of characters in aNewText_ (including null terminator)
        char* aNewText_; //Array of new allocation text
        ulong lineNumber_; //Source file line number for the site
        ulong refCount_; //Number of outstanding allocations at this site
    };

    //represents an allocated block
    struct AllocatedBlock
    {
        AllocatedBlock* pNext_; //next on hash linked list
        AllocatedBlock* pLast_; //previous on hash linked list
        void* address_; //The address of the allocated block
        AllocationSite* pSite_; //the place in the progam where this block was allocated
    };
    
    //Allocator to use for internal structures. Has to be created as a static
    //to prevent recursive calls to this class during construction, in case client
    //tries to log the memory allocation for the allocator object itself.
    BasePoolAllocator& allocator();

    //A hash value for indexing into the hash table aSiteHashTable_
    static size_t hashValue( size_t nStackFrames, const size_t* aCallStack, size_t nBytes, ulong lineNumber );
    //POST( result < SITE_HASH_BOUND );

    //A hash value for indexing into the hash table aBlockHashTable_
    static size_t hashValue( void* address );
    //POST( result < POINTER_HASH_BOUND );

    //data members
    AllocationSite* aSiteHashTable_[ SITE_HASH_BOUND ];
    AllocatedBlock* aBlockHashTable_[ POINTER_HASH_BOUND ];
};


#endif

/* End MEMWATCH.HPP *************************************************/
