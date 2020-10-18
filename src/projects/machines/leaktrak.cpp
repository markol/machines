/*
 * L E A K T R A K . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machines/leaktrak.hpp"
#include "sim/manager.hpp"

void LeakTracker::update( Action action, SampleRate rate )
{
    static bool f12Down = false;
    static uint32 logValue = 700;
	static long sampleFrame = 0;
	static unsigned long sampleFrameNumber = 0;

    switch( action )
    {
        case INLOOP:
        {
            //Check for a new keypress on F12
            bool f12Hit = not f12Down and DevKeyboard::instance().keyCode( DevKey::F12 );
            if( f12Hit )
                f12Down = true;
			else
				f12Down = false;

            //Take action on key hit.
            if( f12Hit )
            {
				#ifndef NDEBUG
                //Write remaining blocks from previous marker
                if( logValue > 700 )
                {
                    MEMORY_STREAM(">>>>>> Writing remaining blocks for marker " << logValue << std::endl);
                    DbgMemChkLogAllAllocatedMemory( logValue );
                }

                //Increment the mask and set it for future NEWs
                DbgMemChkMask( ++logValue );

                //Write marker to stream
                MEMORY_STREAM("================================================================" << std::endl);
                MEMORY_STREAM("Marker set to " << logValue << std::endl);

                //Get the current memory allocation
                MEMORY_STREAM("Current newed value " << DbgMemChkAllocationData().currentMemoryNewed() << std::endl);
				#else
                //Write marker to stream
                MEMORY_STREAM("================================================================" << std::endl);
                MEMORY_STREAM("Marker set to " << (++logValue) << std::endl);
				#endif
                //get the system view of things
/*                MEMORYSTATUS status;
                status.dwLength = sizeof( MEMORYSTATUS );
                GlobalMemoryStatus( &status );
                MEMORY_STREAM("System data:" << std::endl);
                MEMORY_STREAM("  total phys " << status.dwTotalPhys << std::endl);
                MEMORY_STREAM("  avail phys " << status.dwAvailPhys << std::endl);
                MEMORY_STREAM("  total virtual " << status.dwTotalVirtual << std::endl);
                MEMORY_STREAM("  avail virtual " << status.dwAvailVirtual << std::endl);*/
            }
        }
		break;
        case SAMPLE:
        {
			if( ++sampleFrame > rate )
			{
				sampleFrame = 0;
				++sampleFrameNumber;
                //get the system view of things
                /*MEMORYSTATUS status;
                status.dwLength = sizeof( MEMORYSTATUS );
                GlobalMemoryStatus( &status );
				MEMORY_STREAM("=====================================================" << std::endl );
				MEMORY_STREAM("Sample Frame Number " << sampleFrameNumber << " at sample rate " << rate << " sim time " << SimManager::instance().currentTime() << std::endl );
                MEMORY_STREAM(" System data:" << std::endl);
				MEMORY_STREAM("  memory load    " << status.dwMemoryLoad << std::endl );
                MEMORY_STREAM("  total physical " << status.dwTotalPhys << std::endl);
                MEMORY_STREAM("  avail physical " << status.dwAvailPhys << std::endl);
				DWORD dwUsedPhys = status.dwTotalPhys - status.dwAvailPhys;
                MEMORY_STREAM("  total virtual  " << status.dwTotalVirtual << std::endl);
                MEMORY_STREAM("  avail virtual  " << status.dwAvailVirtual << std::endl);
				DWORD dwUsedVirtual = status.dwTotalVirtual - status.dwAvailVirtual;
				MEMORY_STREAM("  total used     " << dwUsedPhys + dwUsedVirtual << std::endl );*/
			}
		}
    }
}

LeakTracker::LeakTracker()
{
}

LeakTracker::~LeakTracker()
{
}
/* End LEAKTRAK.CPP *************************************************/
