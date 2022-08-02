/*
 * D Y N L I G H T S . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysDynamicLightsComplexity

    A brief description of the class should go in here
*/

#ifndef _DYNLIGHTS_HPP
#define _DYNLIGHTS_HPP

#include "base/base.hpp"
#include "machphys/compitem.hpp"

class MachPhysDynamicLightsComplexity : public MachPhysComplexityBooleanItem
{
public:
	MachPhysDynamicLightsComplexity( const ItemId& id, bool enabled );
	~MachPhysDynamicLightsComplexity();

	virtual	void update();
};

#endif

/* End DYNLIGHTS.HPP **************************************************/
