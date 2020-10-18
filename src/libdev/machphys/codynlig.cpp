/*
 * C O D Y N L I G . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "machphys/codynlig.hpp"

MachPhysDynamicLightsComplexity::MachPhysDynamicLightsComplexity( const ItemId& id, bool enabled )
: MachPhysComplexityBooleanItem( id, enabled )
{
}

MachPhysDynamicLightsComplexity::~MachPhysDynamicLightsComplexity()
{
}

// virtual
void MachPhysDynamicLightsComplexity::update()
{
	if( W4dManager::instance().hasSceneManager() )
	{
		W4dSceneManager* manager = W4dManager::instance().sceneManager();
		manager->dynamicLightsEnabled( enabled_ );
		changed_=false;
	}
}

/* End CODYNLIG.CPP **************************************************/
