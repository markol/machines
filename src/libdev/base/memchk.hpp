/*
 * M E M C H K . C P P
 * (c) Charybdis Limited, 1995,1996. All Rights Reserved
 */

#ifndef _MEMCHK_HPP
#define _MEMCHK_HPP

#include <stdlib.h>

#include "base/base.hpp"
#include "base/memcdata.hpp"

// #include <memory.hpp>

size_t  DbgMemChkFreeMemory();
void    DbgMemChkWriteFragmentationData();

//Set/get status of memory watching. When watching memory, all allocations and deletions are logged
//with the BaseMemWatcher singleton.
void MemChkMemoryWatchingOn( bool doWatch );
bool MemChkMemoryWatchingOn();

#ifndef NDEBUG

    void    DbgMemChkRegisterFileData( const char *fName, unsigned long lineNum );
    void    DbgMemChkRegisterFileData2( const char *fName, unsigned long lineNum, const char* ctor );
    void    DbgMemChkRecordAllMemory( bool );
    void    DbgMemChkCheckAllMemory();
    void    DbgMemChkWriteMemoryAllocationData();
    void    DbgMemChkMask( uint32 newMask );
    void    DbgMemChkLogAllAllocatedMemory( uint32 mask );
    void    DbgMemChkRecordStackData( bool );

    const   DbgMemChkData& DbgMemChkAllocationData();

// TODO: charybdis heap operators overload
	#define _NEW(t)   \
	    (DbgMemChkRegisterFileData2( __FILE__, __LINE__, #t ), \
		new t)

	#define _NEW_ALLOCATOR(t, ALLOCATOR)   \
	    (DbgMemChkRegisterFileData2( __FILE__, __LINE__, #t ), \
		(ALLOCATOR::pointer) new (ALLOCATOR().allocate( 1 ) ) t)

    #define _NEW_ARRAY_ALLOCATOR(t, n, ALLOCATOR) \
	    (DbgMemChkRegisterFileData2( __FILE__, __LINE__, #t ), \
		(t*)construct_array( (ALLOCATOR::pointer) ALLOCATOR().allocate( n ), n ) )

    #define _NEW_ARRAY(t, n) \
	    (DbgMemChkRegisterFileData2( __FILE__, __LINE__ , #t ), \
		new t[n])

	#define	_DELETE(t) \
		( t ? DbgMemChkRegisterFileData( __FILE__, __LINE__ ), \
		delete t : \
		delete t)

	#define	_DELETE_ALLOCATOR(t, ALLOCATOR) \
		( t ? DbgMemChkRegisterFileData( __FILE__, __LINE__ ), \
		ALLOCATOR().deallocate( t ) : \
		ALLOCATOR().deallocate( t ) )

    #define _DELETE_ARRAY(t) \
		( t ? DbgMemChkRegisterFileData( __FILE__, __LINE__ ), \
         delete [] t : \
		 delete [] t)

#else

	#define	_NEW(t)	            new t

    #define _NEW_ARRAY(t, n)    new t[n]

	#define	_DELETE(t)          delete t

    #define _DELETE_ARRAY(t)    delete [] t

#endif

#endif
