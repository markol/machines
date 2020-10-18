/*
 * M E M C A P S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    SysMemoryCaps

    A brief description of the class should go in here
*/

#ifndef _SYSTEM_MEMCAPS_HPP
#define _SYSTEM_MEMCAPS_HPP

#include "base/base.hpp"

class SysMemoryCaps
// Canonical form revoked
{
public:
    // percent of memory in use 
    static size_t percentageUsedMem();

	// bytes of physical memory
    static unsigned int totalPhysicalMem();

	// available physical memory in bytes 
    static unsigned int availPhysicalMem();

	// total bytes in page file
    static unsigned int totalPageSpace();

	// total bytes available in page file
    static unsigned int availPageSpace();

	// total number of user bytes of address space
    static unsigned int totalVirtual();

	// available number of user bytes
    static unsigned int availVirtual(); 

	//write the information to an ostream.
	static void outputInfo( ostream& );

    void CLASS_INVARIANT;

private:
    SysMemoryCaps( void );
    ~SysMemoryCaps( void );

    friend ostream& operator <<( ostream& o, const SysMemoryCaps& t );

    SysMemoryCaps( const SysMemoryCaps& );
    SysMemoryCaps& operator =( const SysMemoryCaps& );

};


#endif

/* End MEMCAPS.HPP **************************************************/
