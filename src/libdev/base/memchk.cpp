/*
 * M E M C H K . C P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <new>

#include "base/base.hpp"
#include "base/diag.hpp"
#include "base/cballoc.hpp"
#include "profiler/profiler.hpp"

#ifndef PRODUCTION
#include "base/memwatch.hpp"
void MemChkWatchNewBlock( void* address, size_t nRequestedBytes );
void MemChkWatchDeleteBlock( void* address );
#endif

// TODO: some depricated dos stuff?
#define MAX_PATH 0x00000104

static  void    writeMemoryAllocationData( ostream& ostr );

#ifdef NDEBUG

void    DbgMemChkRecordAllMemory( bool  )
{
}

void  DbgMemChkCheckAllMemory()
{
}

void DbgMemChkRegisterFileData( const char *, unsigned long )
{
}


#endif

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

static  ostream& logStream()
{
    return Diag::instance().memoryStream();
}

size_t  DbgMemChkFreeMemory()
{
    const   size_t  STORE_SIZE = 2048;
    void*   pStore[ STORE_SIZE ];
	size_t	blockSize = 0xffffffff;
	size_t	totalAllocated = 0;

	size_t	nAllocatedBlocks = 0;

	bool	finished = false;

    //  Halve the block size each time and allocate as much as we can

    while( ! finished )
    {
        if( nAllocatedBlocks >= STORE_SIZE )
        {
            finished = true;
            std::cerr << "DbgMemChkFreeMemory : Not enough internal array space" << std::endl;
        }
        else
        {
    		pStore[ nAllocatedBlocks ] = malloc( blockSize );

    		if( pStore[ nAllocatedBlocks ] == 0 )
    		{
                if( blockSize < 10 )
        			finished = true;
                else
                    blockSize /= 2;
    		}
    		else
    		{

    			totalAllocated += blockSize;
    			++nAllocatedBlocks;
    		}
        }
    }

    //  Free all of the memory we used in reverse order of
    //  allocation so we should be able to claim it all back

    for( size_t i = 0; i < nAllocatedBlocks; ++i )
        free( pStore[ nAllocatedBlocks - i - 1 ] );

	return totalAllocated;
}

static bool memoryWatchingOn = false;
void MemChkMemoryWatchingOn( bool doWatch )
{
    memoryWatchingOn = doWatch;
}

bool MemChkMemoryWatchingOn()
{
    return memoryWatchingOn;
}

//  Only include this code if this is the debug version of the library

#ifndef NDEBUG

enum allocationType { SINGLE_ALLOCATION, ARRAY_ALLOCATION };
enum checkType { BLOCK_ALLOCATED = 0xdeadbeef, BLOCK_DELETED = 0x00000000 };

#define MAX_FILE_CHARACTERS_TO_STORE    13

// the debugging info struct
struct DebugInfo
{
	char 			fname[ MAX_FILE_CHARACTERS_TO_STORE ];
	uint32			lnumber;
    uint32          mask;
    size_t          nBytes;
	checkType		CHECK_NUM;
	DebugInfo*		previous;
	DebugInfo*		next;
    allocationType  allocType;
    uint32          checkSum;
};

static	DebugInfo*	firstInList = NULL;

static  bool    DbgRecordAllMemory = false;
static  bool    memoryErrorsFound = false;
static  bool    saveDebugInfo = true;
static  bool    recordStackData = false;
static  bool    tracedStackAnchor = false;

static  unsigned    long    nNewCalls = 0;
static  unsigned    long    nDeleteCalls = 0;
static  unsigned    long    nNewArrayCalls = 0;
static  unsigned    long    nDeleteArrayCalls = 0;

static  unsigned    long    totalMemoryNewed = 0;
static  unsigned    long    totalMemoryDeleted = 0;
static  unsigned    long    maxMemoryNewed = 0;
//static  unsigned    long    maxActuallyAllocated = 0;
//static  unsigned    long    minFree = 0xffffffff;
//static  unsigned    long    initialFree = 0;

static  char                fileName[MAX_PATH];
static  unsigned    long    lineNumber;
static  const char*  ctorText = NULL;
static  bool                fileDataValid = false;

static  uint32              currentMask = 0;

// static  ofstream    logStream( "memchk.log" );

typedef void (*v_fn_v)();

static  void    *getMemory( size_t amt );
static  void    addBlockToList( DebugInfo* d );
static  void    removeBlockFromList( DebugInfo* d );
static  void    setupCommonData( DebugInfo *dPtr );
static  void    checkCommonData( DebugInfo *dPtr );
static  void    logNewBlock( const DebugInfo *dPtr );
static  void    logDeleteBlock( const DebugInfo *dPtr );
static  void    checkFirst();
static  uint32  calculateCheckSum( DebugInfo *dPtr );
static  void    checkMemoryBlock( DebugInfo* dPtr );

static  void    recordNew( size_t nBytes, void* ptr );
static  void    recordNewArray( size_t nBytes, void* ptr );
static  void    recordAllocation( size_t nBytes, void* ptr );
static  void    recordDelete( size_t nBytes );
static  void    recordDeleteArray( size_t nBytes );
static  void    recordDeallocation( size_t nBytes );

static  void    writeMemoryAllocationData( ostream& ostr );

#define BLOCK_WHERE( dPtr )     \
     (dPtr)->fname << "  " << (dPtr)->lnumber

#define CURRENT_WHERE                            \
    ( fileDataValid ? fileName : "Unknown file" ) << "  " << ( fileDataValid ? lineNumber : 0 )

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

//  Replacement for the global new operator
/*
void *operator new( size_t amt )
{
    checkFirst();

    if( amt == 0 )
        amt = 1;

    void *memory;

    if( saveDebugInfo )
    {
    	// allocate memory + room for debugging information
        //memory = getMemory( amt + sizeof( DebugInfo ) );
        memory = cbAlloc( amt + sizeof( DebugInfo ) );

        if( memory )
        {
        	// create a debugInfo pointer and point it to the new memory block
    	    DebugInfo* d = (DebugInfo*)memory;

        	// store the values in this memory block
            setupCommonData( d );

            d->nBytes = amt;
            d->allocType = SINGLE_ALLOCATION;

            addBlockToList( d );

            if( DbgRecordAllMemory )
                logNewBlock( d );

            memory = (uint8 *)( memory ) + sizeof( DebugInfo );
        }

        recordNew( amt, memory );
    }
    else
    {
        //memory = getMemory( amt );
        memory = cbAlloc( amt );
        saveDebugInfo = true;
    }

    if( MemChkMemoryWatchingOn() )
        MemChkWatchNewBlock( memory, amt );

    fileDataValid = false;
    ctorText = NULL;

    return memory;
}
*/
/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

//  Replacement for the global delete operator
/*
void operator delete( void *p )
{
	//cout << "in function delete" << endl;
	//cout << "sizeof( debuginfo ) = " << sizeof( DebugInfo ) << endl;

    if( p )
    {
        if( MemChkMemoryWatchingOn() )
            MemChkWatchDeleteBlock( p );

        if( saveDebugInfo )
        {
    		// create a debugInfo pointer and point it to the relevant info block for object p
    		//cout << "p = " << p << endl;
    		DebugInfo* d = (DebugInfo*)( (uint8 *)( p ) - sizeof( DebugInfo ) );

            recordDelete( d->nBytes );

            if( d->allocType != SINGLE_ALLOCATION )
            {
                logStream() << "Memory allocated with array new at " << BLOCK_WHERE( d );
                logStream() << " but deleted with delete at " << CURRENT_WHERE << std::endl;
            }

            if( DbgRecordAllMemory )
                logDeleteBlock( d );

    		//cout << "d = " << d << endl;
    		//cout << "name: " << d->fname << "   line: " << d->lnumber << endl;

            removeBlockFromList( d );

            //  ORDER DEPENDENCY - this must follow removeBlockFromList
            //  so that the checksum can be correctly compared.

            checkCommonData( d );

            //free( (uint8 *)( p ) - sizeof( DebugInfo ) );
            cbFree( (uint8 *)( p ) - sizeof( DebugInfo ) );
        }
        else
        {
            //free( p );
            cbFree( p );
            saveDebugInfo = true;
        }
    }

    fileDataValid = false;
}
*/
/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

//  Replacement for the global new[] operator
/*
void *operator new []( size_t amt )
{
    checkFirst();

    if( amt == 0 )
        amt = 1;

	// allocate memory + room for debugging information
    void    *memory;

    if( saveDebugInfo )
    {
        //memory = getMemory( amt + sizeof( DebugInfo ) );
        memory = cbAlloc( amt + sizeof( DebugInfo ) );

        if( memory )
        {
        	// create a debugInfo pointer and point it to the new memory block
        	DebugInfo* d = (DebugInfo*)memory;

        	// store the values in this memory block
            setupCommonData( d );

            d->nBytes = amt;
            d->allocType = ARRAY_ALLOCATION;

            addBlockToList( d );

            if( DbgRecordAllMemory )
                logNewBlock( d );

            memory = (uint8 *)( memory ) + sizeof( DebugInfo );
        }
    }
    else
    {
        //memory = getMemory( amt );
        memory = cbAlloc( amt );
        saveDebugInfo = true;
    }

    recordNewArray( amt, memory );

    if( MemChkMemoryWatchingOn() )
        MemChkWatchNewBlock( memory, amt );

    fileDataValid = false;
    ctorText = NULL;

    return memory;
}
*/
/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

//  Replacement for the global delete[] operator
/*
void operator delete []( void *p )
{
    if( p )
    {
        if( MemChkMemoryWatchingOn() )
            MemChkWatchDeleteBlock( p );

        if( saveDebugInfo )
        {
    		// create a debugInfo pointer and point it to the relevant info block for object p
    		DebugInfo* d = (DebugInfo*)( (uint8 *)( p ) - sizeof( DebugInfo ) );

            recordDeleteArray( d->nBytes );

            if( d->allocType != ARRAY_ALLOCATION )
            {
                logStream() << "Memory allocated with new at " << BLOCK_WHERE( d );
                logStream() << " but deleted with array delete at " << CURRENT_WHERE << std::endl;
            }

            if( DbgRecordAllMemory )
                logDeleteBlock( d );

            removeBlockFromList( d );

            //  ORDER DEPENDENCY - this must follow removeBlockFromList
            //  so that the checksum can be correctly compared.

            checkCommonData( d );

            //free( (uint8 * )( p ) - sizeof( DebugInfo ) );
            cbFree( (uint8 * )( p ) - sizeof( DebugInfo ) );
        }
        else
        {
            //free( p );
            cbFree( p );
            saveDebugInfo = true;
        }
    }

    fileDataValid = false;
}
*/
/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

static  void    *getMemory( size_t amt )

{
    bool    finished = false;
    void    *memory;

    while( !finished)
    {
        memory = malloc( amt );

        if (memory)
            finished = true;
        else
        {
            //  We didn't manage to malloc enough memory. If there is
            //  a new handler defined, call it and try again. Otherwise
            //  we just admit defeat and return 0

            logStream() << "Failed to allocate " << amt << " bytes ";

            logStream() << CURRENT_WHERE << std::endl;

            writeMemoryAllocationData( logStream() );

            memoryErrorsFound = true;

            v_fn_v  newHandler = std::set_new_handler( 0 );
            std::set_new_handler( newHandler );

            if( newHandler)
                (*newHandler)();
            else
                finished = true;
        }
    }

    return memory;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

static  void    addBlockToList( DebugInfo* d )
{
	d->previous = NULL;
	d->next = firstInList;

	// reset the static variable

	firstInList = d;

	if( d->next != NULL )
		d->next->previous = d;

    d->checkSum = calculateCheckSum( d );
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

static  void    removeBlockFromList( DebugInfo* d )

{
    checkMemoryBlock( d );

	if( d == firstInList )
		firstInList = d->next;

	if( d->next != NULL )
		d->next->previous = d->previous;

	if( d->previous != NULL )
		d->previous->next = d->next;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

static  void    setupCommonData( DebugInfo *dPtr )
{
    if( fileDataValid )
    {

        if( strlen( fileName ) < MAX_FILE_CHARACTERS_TO_STORE )
            strcpy( dPtr->fname, fileName );
        else
        	strncpy( dPtr->fname,
              &fileName[ strlen( fileName ) - ( MAX_FILE_CHARACTERS_TO_STORE - 1 ) ],
              MAX_FILE_CHARACTERS_TO_STORE - 1 );

        dPtr->fname[ MAX_FILE_CHARACTERS_TO_STORE - 1 ] = '\0';

	    dPtr->lnumber = lineNumber;

        //Removed 21/2/99 by JLG: set to false on exit from op new
        //fileDataValid = false;
    }
    else
    {
    	strncpy( dPtr->fname, "Unknown file", MAX_FILE_CHARACTERS_TO_STORE );
        dPtr->fname[ MAX_FILE_CHARACTERS_TO_STORE - 1 ] = '\0';

	    dPtr->lnumber = 0;
    }

    dPtr->mask = currentMask;
	dPtr->CHECK_NUM = BLOCK_ALLOCATED;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

static  void    checkCommonData( DebugInfo *dPtr )
{
    if( dPtr->CHECK_NUM == BLOCK_DELETED )
    {
        memoryErrorsFound = true;
        logStream() << "ERROR: Block deleted at " << CURRENT_WHERE << " already deleted" << std::endl;
    }
    else if( dPtr->CHECK_NUM != BLOCK_ALLOCATED )
    {
        memoryErrorsFound = true;
        logStream() << "ERROR: Block deleted at " << CURRENT_WHERE << " has been corrupted" << std::endl;
    }

    dPtr->CHECK_NUM = BLOCK_DELETED;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

static  void    logNewBlock( const DebugInfo *dPtr )
{
    void* address = (void *)( (uint8 *)dPtr + sizeof( DebugInfo ) );
    if( recordStackData )
    {
        if( not tracedStackAnchor )
        {
            tracedStackAnchor = true;
            ProProfiler::instance().traceStack( logStream(), true, 0, NULL );
        }

        //Set up string detailing the extra information we want
        char buffer[64];
        if( ctorText != NULL )
            sprintf( buffer, "NEW %d %#08x @ %.32s @", dPtr->nBytes, (ulong)address, ctorText );
        else
            sprintf( buffer, "NEW %d %#08x", dPtr->nBytes, (ulong)address );

        ProProfiler::instance().traceStack( logStream(), false, dPtr->lnumber, buffer );
    }

    logStream() << address;
    logStream() << " (" << dPtr->nBytes << " bytes)";
    logStream() << " allocated with ";

    if( dPtr->allocType == ARRAY_ALLOCATION )
        logStream() << "_NEW_ARRAY ";
    else
        logStream() << "_NEW       ";

    logStream() << "at " << BLOCK_WHERE( dPtr ) << std::endl;

//     SysMetrics  metrics;
//
//     logStream() << " Free memory " << metrics.freeMemory() << endl;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

static  void    logDeleteBlock( const DebugInfo *dPtr )
{
    void* address = (void *)( (uint8 *)dPtr + sizeof( DebugInfo ) );
    if( recordStackData )
    {
        //Set up string detailing the extra information we want
        char buffer[64];
        sprintf( buffer, "DELETE %#08x", (ulong)address );
        ProProfiler::instance().traceStack( logStream(), false, dPtr->lnumber, buffer );
    }

    logStream() << address;
    logStream() << " (" << dPtr->nBytes << " bytes)";
    logStream() << " deleted with ";

    if( dPtr->allocType == ARRAY_ALLOCATION )
        logStream() << "_DELETE_ARRAY ";
    else
        logStream() << "_DELETE       ";

    logStream() << "at " << CURRENT_WHERE;

    if( currentMask != 0 && !( dPtr->mask & currentMask ) )
    {
        logStream() << " ( deleting a block not allocated with the current mask )" << std::endl;
    }
    else
        logStream() << std::endl;

    //Removed 21/2/99 by JLG: set to false on exit from op delete
    //fileDataValid = false;

//     SysMetrics  metrics;
//
//     logStream() << " Free memory " << metrics.freeMemory() << endl;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

static  uint32    calculateCheckSum( DebugInfo *dPtr )
{
    //  Save the items in the structure that might legitimately change
    //  and set them to zero so that the checksum is repeatable.

    uint32      saveCheckSum = dPtr->checkSum;
    DebugInfo*  savePrevious = dPtr->previous;
    DebugInfo*  saveNext = dPtr->next;

    dPtr->checkSum = 0;
    dPtr->previous = NULL;
    dPtr->next = NULL;

    uint8*  pByte = (uint8*)dPtr;
    size_t  i = 0;
    uint32  checkSum = 0;

    for( i = 0; i < sizeof( DebugInfo ); ++i, ++pByte )
    {
        checkSum += (*pByte);
    }

    dPtr->checkSum = saveCheckSum;
    dPtr->previous = savePrevious;
    dPtr->next = saveNext;

    return checkSum;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

void    DbgMemChkRegisterFileData( const char *fName, unsigned long lineNum )

{
//    PRE( isValid( fName ) );

    strcpy( fileName, fName );
    lineNumber = lineNum;
    fileDataValid = true;
    ctorText = NULL;
/*
    outStream << lastMemoryPointer << "  " << lastCount;
    outStream << "    " << fName << "  " << lineNum << endl;
*/
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

void    DbgMemChkRegisterFileData2( const char *fName, unsigned long lineNum, const char* ctorString )

{
//    PRE( isValid( fName ) );

    strcpy( fileName, fName );
    lineNumber = lineNum;
    fileDataValid = true;
    ctorText = ctorString;
/*
    outStream << lastMemoryPointer << "  " << lastCount;
    outStream << "    " << fName << "  " << lineNum << endl;
*/
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

void    DbgMemChkRecordAllMemory( bool record )
{
    DbgRecordAllMemory = record;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

void    DbgMemChkRecordStackData( bool record )
{
    recordStackData = record;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

void    DbgMemChkWriteMemoryAllocationData()
{
    writeMemoryAllocationData( logStream() );
}

/*--------------------------------------------------------------------------
Check all memory blocks to see if any of them have been corrupted. If they
have, log this information.
--------------------------------------------------------------------------*/

void    DbgMemChkCheckAllMemory()
{
	DebugInfo* d = firstInList;

	while( d != NULL )
	{
        checkMemoryBlock( d );

		d = d->next;
	}
}

void    DbgMemChkMask( uint32 newMask )
{
    currentMask = newMask;
}

void    DbgMemChkLogAllAllocatedMemory( uint32 mask )
{
    logStream() << "~~~~~~ Start memory currently allocated with mask " << mask << " ~~~~~~~~~" << std::endl;

	DebugInfo* d = firstInList;

	while( d != NULL )
	{
        if( ( mask == 0 ) || ( d->mask & mask ) )
        {
		   	logStream() << "Memory ";
            logStream() << (void *)( (uint8 *)d + sizeof( DebugInfo ) );
            logStream() << " (" << d->nBytes << " bytes)";
            logStream() << " allocated ";
		   	logStream() << BLOCK_WHERE( d ) << std::endl;
		}

       	d = d->next;
    }

    logStream() << "~~~~~~ Finish memory currently allocated with mask " << mask << " ~~~~~~~~~" << std::endl;
}

const   DbgMemChkData& DbgMemChkAllocationData()
{
    static  DbgMemChkData   memoryData;

    memoryData.nNewCalls( nNewCalls );
    memoryData.nDeleteCalls( nDeleteCalls );
    memoryData.nNewArrayCalls( nNewArrayCalls );
    memoryData.nDeleteArrayCalls( nDeleteArrayCalls );
    memoryData.totalMemoryNewed( totalMemoryNewed );
    memoryData.totalMemoryDeleted( totalMemoryDeleted );
    memoryData.maxMemoryNewed( maxMemoryNewed );

    return memoryData;
}

static  bool    canAllocateNextBlock( void** ppBlockPtr, size_t* pBlockSize );

void    DbgMemChkWriteFragmentationData()
{
    void*   pBlock;
    size_t  blockSize;
    size_t  totalBlockSize = 0;
    void*   pLast = NULL;
//    void**  pFirst = NULL;

    logStream() << "^^^^^^^^^ Fragmentation Data start ^^^^^^^^^^^" << std::endl;

    while( canAllocateNextBlock( &pBlock, &blockSize ) )
    {
        totalBlockSize += blockSize;
        logStream() << "Block at " << pBlock << " size " << blockSize << std::endl;

        if( pBlock == pLast )
        {
            logStream() << "ERROR Block pointer was repeated" << std::endl;
            break;
        }

        *((void**)pBlock) = pLast;

//         if( pLast == NULL )
//         {
//             pFirst = (void**)pBlock;
//         }
//         else
//         {
//         logStream() << pLast << endl;
//             *pLast = pBlock;
//         }
//
         pLast = pBlock;
//        *pLast = NULL;
    }

    logStream() << "Total block size " << totalBlockSize << std::endl;

    logStream() << "^^^^^^^^^^ Fragmentation Data end ^^^^^^^^^^^^" << std::endl;

    while( pLast != NULL )
    {
        void* pNext;

        pNext = *((void**)pLast);

        free( pLast );

        pLast = pNext;
    }
}

static  bool    canAllocateNextBlock( void** ppBlockPtr, size_t* pBlockSize )
{
    bool    canAllocate = false;

    //  We must allocate a minimum of 4 bytes since we will use the
    //  block to store a pointer to the next block in.
    const   size_t  MINIMUM_TO_ALLOCATE = 4;

    void*   pBlock = malloc( MINIMUM_TO_ALLOCATE );

    size_t  blockSize = MINIMUM_TO_ALLOCATE;
    size_t  allocateSize = MINIMUM_TO_ALLOCATE;

    if( pBlock )
    {
        free( pBlock );
        bool    finished = false;

        enum { INCREASING, DECREASING } direction = INCREASING;

        while( !finished )
        {
            //  At this point in the loop, I know I can allocate a block
            //  of size blockSize at position startPtr. I do not have this
            //  block currently allocated.

            void* pNew = malloc( blockSize + allocateSize );

            if( pNew )
            {
                free( pNew );
            }

            if( pNew == pBlock )
            {
                blockSize += allocateSize;

                if( direction == INCREASING )
                    allocateSize *= 2;
            }
            else
            {
                if( direction == INCREASING )
                {
                    direction = DECREASING;
                }
                else
                {
                    if( allocateSize == 1 )
                        finished = true;
                }

                allocateSize /= 2;
            }
        }

        void* pTemp;

        while( ( pTemp = malloc( blockSize ) ) != NULL )
        {
            if( pTemp )
                free( pTemp );

            --blockSize;
        }

        canAllocate = true;
    }

    *ppBlockPtr = pBlock;
    *pBlockSize = blockSize;

    return  canAllocate;
}

/*--------------------------------------------------------------------------
Check a single memory block for corruption.
--------------------------------------------------------------------------*/

static  void    checkMemoryBlock( DebugInfo* dPtr )
{
    if( dPtr->checkSum != calculateCheckSum( dPtr ) )
    {
        memoryErrorsFound = true;

	   	logStream() << "ERROR: Memory ";
        logStream() << (void *)( (uint8 *)dPtr + sizeof( DebugInfo ) );
        logStream() << " (" << dPtr->nBytes << " bytes)";
        logStream() << " corrupted ";
	   	logStream() << BLOCK_WHERE( dPtr ) << std::endl;
    }
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

static  void    checkFirst()
{
    static bool first = true;

    if( first )
    {
        first = false;

        //  Commented out by Bob, 8 Feb 1999 as I felt this was serving
        //  no useful purpose.

//         cerr << endl;
//         cerr << "=======================================" << endl;
         std::cerr << "[Debug]" << std::endl;
//         cerr << "=======================================" << endl;

//         cerr << "logStream() bad " << logStream().bad() << endl;
//         cerr << "logStream() fail " << logStream().fail() << endl;
//         cerr << "logStream() eof " << logStream().eof() << endl;
//         cerr << "logStream() rdstate " << logStream().rdstate() << endl;

        logStream() << std::endl;
        logStream() << "--------------------------------------------------------------------" << std::endl;
        logStream() << std::endl;

        time_t  t = time( NULL );
        logStream() << ctime( &t );
        logStream() << std::endl;
        logStream() << "Use the cb_memory environment variable to set debugging options:" << std::endl;
        logStream() << "  set cb_memory=[logall];[stacktrace]" << std::endl;
        logStream() << "       logall       Log all memory allocations and deallocations" << std::endl;
        logStream() << "       stacktrace   Write out the stack with each allocation and deallocation" << std::endl;
        logStream() << std::endl;

        const char* envText = getenv( "cb_memory" );
        if( envText )
        {
            logStream() << "Current memory setting '" << envText << "'" << std::endl;

            char    text[100];
            strcpy( text, envText );

            char* pToken = strtok( text, ";" );

            while( pToken )
            {
                if ( !strncmp( pToken, "logall", 6 ) )
                {
                    DbgMemChkRecordAllMemory( true );
                }

                if ( !strncmp( pToken, "stacktrace", 10 ) )
                {
                    recordStackData = true;
                }

                pToken = strtok( NULL, ";" );
            }

            logStream() << std::endl;
        }
        else
        {
            logStream() << "cb_memory environment variable not set" << std::endl;
            logStream() << std::endl;
        }

        logStream() << "--------------------------------------------------------------------" << std::endl << std::endl;
    }
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

static  void    recordNew( size_t nBytes, void* ptr )
{
    ++nNewCalls;
    recordAllocation( nBytes, ptr );
}

static  void    recordNewArray( size_t nBytes, void* ptr )
{
    ++nNewArrayCalls;
    recordAllocation( nBytes, ptr );
}

static  void    recordAllocation( size_t nBytes, void* /* ptr */ )
{
    totalMemoryNewed += nBytes;

    if( totalMemoryNewed - totalMemoryDeleted > maxMemoryNewed )
    {
        maxMemoryNewed = totalMemoryNewed - totalMemoryDeleted;

        //  Commented out because the call to DbgMemChkFreeMemory
        //  is too slow to make every time we allocate some more memory.

//         size_t currentFree = DbgMemChkFreeMemory();
//
//         if( currentFree < minFree )
//             minFree = currentFree;
//
//         if ( initialFree - currentFree > maxActuallyAllocated )
//             maxActuallyAllocated = initialFree - currentFree;
    }

}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

//  Setting save debug info to false is a one shot operation - it will be
//  reset to true after the first time it is used.

void DbgMemChkSaveDebugInfo( bool saveDebug )
{
    saveDebugInfo = saveDebug;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

static  void    recordDelete( size_t nBytes )
{
    ++nDeleteCalls;
    recordDeallocation( nBytes );
}

static  void    recordDeleteArray( size_t nBytes )
{
    ++nDeleteArrayCalls;
    recordDeallocation( nBytes );
}

static  void    recordDeallocation( size_t nBytes )
{
    totalMemoryDeleted += nBytes;
}

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/

static  void    writeMemoryAllocationData( ostream& ostr )
{
    ostr << "...................................................................." << std::endl;

    ostr << nNewCalls << " NEWs, " << nDeleteCalls << " DELETEs ( " << (int32)nNewCalls - (int32)nDeleteCalls << " outstanding NEWs )" << std::endl;
    ostr << nNewArrayCalls << " NEW_ARRAYs, " << nDeleteArrayCalls << " DELETE_ARRAYs ( " << (int32)nNewArrayCalls - (int32)nDeleteArrayCalls << " outstanding NEW_ARRAYs )" << std::endl;
    ostr << totalMemoryNewed << " bytes newed in total, " << (int32)totalMemoryDeleted << " bytes deleted in total " << std::endl;
    ostr << totalMemoryNewed - totalMemoryDeleted << " bytes currently newed, " << maxMemoryNewed << " bytes maximum newed" << std::endl;
//    ostr << maxActuallyAllocated << " bytes max actually allocated  " << minFree << " bytes minimum free " << endl;
    ostr << "...................................................................." << std::endl;
}

struct MEMCHK_LEAKS
{
    MEMCHK_LEAKS()
    {
    }

    ~MEMCHK_LEAKS()
    {
        writeMemoryAllocationData( logStream() );


        if( nNewCalls != nDeleteCalls )
        {
            memoryErrorsFound = true;
            logStream() << "ERROR: ";
            logStream() << nNewCalls << " calls to new, ";
            logStream() << nDeleteCalls << " calls to delete" << std::endl;
        }

        if( nNewArrayCalls != nDeleteArrayCalls )
        {
            memoryErrorsFound = true;
            logStream() << "ERROR: ";
            logStream() << nNewArrayCalls << " calls to new[], ";
            logStream() << nDeleteArrayCalls << " calls to delete[]" << std::endl;
        }

		DebugInfo* d = firstInList;

		while( d != NULL )
		{
            memoryErrorsFound = true;

		   	logStream() << "ERROR: Memory ";
            logStream() << (void *)( (uint8 *)d + sizeof( DebugInfo ) );
            logStream() << " (" << d->nBytes << " bytes)";
            logStream() << " newed but never deleted ";

            if( d->allocType == ARRAY_ALLOCATION )
                logStream() << "( _NEW_ARRAY ) ";
            else
                logStream() << "( _NEW )       ";

		   	logStream() << BLOCK_WHERE( d ) << std::endl;

			d = d->next;
		}

        if( memoryErrorsFound )
            std::cerr << "Memory errors found" << std::endl;
    }
};

#else
//Disable those overloads for production version
/*void *operator new( size_t amt )
{
    void* p = cbAlloc( amt );

#ifndef PRODUCTION
    if( MemChkMemoryWatchingOn() )
        MemChkWatchNewBlock( p, amt );
#endif

    return p;
}

void operator delete( void *p )
{
    if( p )
    {
#ifndef PRODUCTION
        if( MemChkMemoryWatchingOn() )
            MemChkWatchDeleteBlock( p );
#endif
        cbFree( p );
    }
}

void *operator new []( size_t amt )
{
    void* p = cbAlloc( amt );

#ifndef PRODUCTION
    if( MemChkMemoryWatchingOn() )
        MemChkWatchNewBlock( p, amt );
#endif

    return p;
}

void operator delete []( void *p )
{
    if( p )
    {
#ifndef PRODUCTION
        if( MemChkMemoryWatchingOn() )
            MemChkWatchDeleteBlock( p );
#endif
        cbFree( p );
    }
}
*/
#endif

//==============================================
#ifndef PRODUCTION

void MemChkWatchNewBlock( void* address, size_t nRequestedBytes )
{
    ulong watchLineNumber = 0;
    const char* aNewText = NULL;

#ifndef NDEBUG
    if( fileDataValid )
    {
        watchLineNumber = lineNumber;
        aNewText = ctorText;
    }
#endif

    BaseMemWatcher::instance().addBlock( address, nRequestedBytes, watchLineNumber, aNewText );
}

void MemChkWatchDeleteBlock( void* address )
{
    BaseMemWatcher::instance().removeBlock( address );
}

#endif
//==============================================

//  This class exists purely to ensure that the streams are setup before
//  the leak checker is.
struct STREAM_INITIALISER
{
    STREAM_INITIALISER()
    {
        cbInitialise();
        Diag::instance();

#ifndef NDEBUG
        static MEMCHK_LEAKS leakDetector;
        DiagInternal::countedPointerTracker();
#endif
    }

    ~STREAM_INITIALISER()
    {
        cbClose();
    }
};

#ifdef __WATCOMC__

#pragma off(unreferenced);

//  We have to make sure that the streams and the leak detector are
//  initialised before any other static data including static data
//  in other libraries. This is something of a hack but I haven't
//  found any other way of making this work. I believe there is some
//  static data in the ObjectSpace STL which is allocating memory and
//  therefore messing up our initialisation ordering. BOB

#pragma initialize library

static STREAM_INITIALISER    initialiser;

#pragma on(unreferenced);


#endif
