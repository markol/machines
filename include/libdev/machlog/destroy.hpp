/*
 * D E S T R O Y . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 * Mixin for logical objects that support 
 * an operation for their destruction.
 */

#ifndef _MACHLOG_DESTROY_HPP
#define _MACHLOG_DESTROY_HPP

#include "phys/phys.hpp"

/* //////////////////////////////////////////////////////////////// */

class MachLogCanBeDestroyed
{
public:

	virtual ~MachLogCanBeDestroyed() {};

    // Destroy oneself, returning 
    // the time any animation will take
    virtual PhysRelativeTime beDestroyed() = 0;
};

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHLOG_DESTROY_HPP	*/

/* End DESTROY.HPP **************************************************/
