/*
 * C O N S P L A N . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 * A MachLogConstructionPlan contains all the information
 * required to schedule the resources needed to create a
 * MachLogConstruction.
 */

#ifndef _MACHLOG_CONSPLAN_HPP
#define _MACHLOG_CONSPLAN_HPP

#include "phys/phys.hpp"

/* //////////////////////////////////////////////////////////////// */

class MachLogConstructionPlan
{
public:

	bool isFinished() const;

private:

	unsigned totalUnitsRequired_;
	unsigned unitsRequired_;
};

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHLOG_CONSPLAN_HPP	*/

/* End CONSPLAN.HPP *************************************************/
