/*
 * D E S T R O Y . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 * Mixin for physical objects that support 
 * an animation for their destruction.
 */

#ifndef _MACHPHYS_DESTROY_HPP
#define _MACHPHYS_DESTROY_HPP

#include "phys/phys.hpp"

/* //////////////////////////////////////////////////////////////// */

class MachPhysCanBeDestroyed
{
public:

	virtual ~MachPhysCanBeDestroyed() {};

    //Destroy oneself, returning the time any animation will take    
    virtual PhysRelativeTime beDestroyed() = 0;
};

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef 	_MACHPHYS_DESTROY_HPP	*/

/* End DESTROY.HPP **************************************************/
