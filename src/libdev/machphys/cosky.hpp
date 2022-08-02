/*
 * S K Y . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysSkyComplexity

    A brief description of the class should go in here
*/

#ifndef _SKY_HPP
#define _SKY_HPP

#include "base/base.hpp"
#include "machphys/compitem.hpp"

class MachPhysSkyComplexity : public MachPhysComplexityChoiceItem
{
public:
	MachPhysSkyComplexity( const ItemId& id );
	~MachPhysSkyComplexity();

	virtual	void update();
};

#endif

/* End SKY.HPP **************************************************/
