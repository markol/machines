/*
 * M E M C A P S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "system/memcaps.hpp"


#ifndef _WIN95APP
    #ifndef _SDLAPP
        #error This is a Win95 only library; _WIN95APP or _SDLAPP must be defined
    #endif
#endif

// TODO from windows
// percent of memory in use
size_t SysMemoryCaps::percentageUsedMem()
{
	/*MEMORYSTATUS memoryStruc;
	memoryStruc.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memoryStruc);
	return memoryStruc.dwMemoryLoad;*/
	return 10;
}

// bytes of physical memory
unsigned int SysMemoryCaps::totalPhysicalMem()
{
	/*MEMORYSTATUS memoryStruc;
	memoryStruc.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memoryStruc);
	return memoryStruc.dwTotalPhys;*/return 640000000;
}

// available physical memory in bytes
unsigned int SysMemoryCaps::availPhysicalMem()
{
	/*MEMORYSTATUS memoryStruc;
	memoryStruc.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memoryStruc);
	return memoryStruc.dwAvailPhys;*/return 640000000;
}

// total bytes in page file
unsigned int SysMemoryCaps::totalPageSpace()
{
	/*MEMORYSTATUS memoryStruc;
	memoryStruc.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memoryStruc);
	return memoryStruc.dwTotalPageFile;*/return 640000000;
}

// total bytes available in page file
unsigned int SysMemoryCaps::availPageSpace()
{
	/*MEMORYSTATUS memoryStruc;
	memoryStruc.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memoryStruc);
	return memoryStruc.dwAvailPageFile;*/return 640000000;
}

// total number of user bytes of address space
unsigned int SysMemoryCaps::totalVirtual()
{
	/*MEMORYSTATUS memoryStruc;
	memoryStruc.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memoryStruc);
	return memoryStruc.dwTotalVirtual;*/return 640000000;
}

// available number of user bytes
unsigned int availVirtual()
{
	/*MEMORYSTATUS memoryStruc;
	memoryStruc.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memoryStruc);
	return memoryStruc.dwAvailVirtual;*/return 640000000;
}

SysMemoryCaps::SysMemoryCaps()
{
    TEST_INVARIANT;
}

SysMemoryCaps::~SysMemoryCaps()
{
    TEST_INVARIANT;

}

void SysMemoryCaps::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const SysMemoryCaps& t )
{

    o << "SysMemoryCaps " << (void*)&t << " start" << std::endl;
    o << "SysMemoryCaps " << (void*)&t << " end" << std::endl;

    return o;
}

//static
void SysMemoryCaps::outputInfo( ostream& o )
{
    /*MEMORYSTATUS status;
    status.dwLength = sizeof( MEMORYSTATUS );
    GlobalMemoryStatus( &status );
	o << "=====================================================" << std::endl;
    o << " System data:" << std::endl;
	o << "  memory load    " << status.dwMemoryLoad << std::endl;
    o << "  total physical " << status.dwTotalPhys << std::endl;
    o << "  avail physical " << status.dwAvailPhys << std::endl;
	DWORD dwUsedPhys = status.dwTotalPhys - status.dwAvailPhys;
    o << "  total virtual  " << status.dwTotalVirtual << std::endl;
    o << "  avail virtual  " << status.dwAvailVirtual << std::endl;
	DWORD dwUsedVirtual = status.dwTotalVirtual - status.dwAvailVirtual;
	o << "  total used     " << dwUsedPhys + dwUsedVirtual << std::endl;*/
}
/* End MEMCAPS.CPP **************************************************/
