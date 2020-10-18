/*
 * C O O R D S Y S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "mathex/coordsys.hpp"
#include <string.h>

#ifndef _INLINE
    #include "mathex/coordsys.ipp"
#endif

// static
MexCoordSystem& MexCoordSystem::instance()
{
    static MexCoordSystem instance_;
    return instance_;
}

void MexCoordSystem::setFromEnvironment()
{
    char* cs = getenv("CB_COORD_SYS");
    if (cs)
    {
		//if (stricmp(cs, "right") == 0)
		if (strcasecmp(cs, "right") == 0)
			set(MexCoordSystem::RIGHT_HANDED);
		else
		{
			//ASSERT(stricmp(cs, "left") == 0, "CB_COORD_SYS set to unrecognised value.");
			ASSERT(strcasecmp(cs, "left") == 0, "CB_COORD_SYS set to unrecognised value.");
			set(MexCoordSystem::LEFT_HANDED);
		}
    }
	else
	{
		set(MexCoordSystem::LEFT_HANDED);
	}
}

/* End COORDSYS.CPP *************************************************/
