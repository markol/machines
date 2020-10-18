/*
 * C O V A P T R A . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/manager.hpp"
#include "machphys/compmgr.hpp"
#include "machphys/covaptra.hpp"

MachPhysVapourTrailsComplexity::MachPhysVapourTrailsComplexity( const ItemId& id, bool enabled )
: MachPhysComplexityBooleanItem( id, enabled )
{
}

MachPhysVapourTrailsComplexity::~MachPhysVapourTrailsComplexity()
{
}

// virtual
void MachPhysVapourTrailsComplexity::update()
{
		MachPhysComplexityManager::instance().vapourTrailsEnabled( enabled_ );
		changed_=false;
}

/* End COVAPTRA.CPP **************************************************/
