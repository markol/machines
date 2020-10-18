/*
 * C B A L L O C . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    Memory allocator system
*/

#ifndef _BASE_CBALLOC_HPP
#define _BASE_CBALLOC_HPP

#include "base/base.hpp"
//#include <strfwd>

void* cbAlloc( size_t nBytes );
void cbFree( void* pBlock );
void* cbRealloc( void* pBlock, size_t nBytes );
void cbInitialise();
void cbClose();
void cbTouchAll();

//Temporray interface. Allows a pool allocator to be used instead of the standard.
//Although the interface is for a stack, only one allocator can currently be pushed
//at a time.
class BasePoolAllocator;
void pushCurrentPoolAllocator( BasePoolAllocator* pAllocator );
void popCurrentPoolAllocator();

enum AllocTraceOptions{ CB_ALLOC_DETAIL_BLOCKS, CB_ALLOC_DETAIL_NOBLOCKS };
void cbAllocTrace( ostream& o, const std::string& title,  AllocTraceOptions option );

#endif

/* End CBALLOC.HPP **************************************************/
