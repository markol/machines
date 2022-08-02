/*
 * E P P . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 * 	An EPP is an evasion priority
 * 	plan, which designates the preference of evasion strategy a machine with that plan
 *  will look for in order, e.g. attempts to find nearby garrisons to hide in first, if
 *	that fails, tries to find nearby turrets within whose weapon umbrella to hide etc.
 *	It is designed as a singleton class.
 */

#ifndef _MACHPHYS_EPP_HPP
#define _MACHPHYS_EPP_HPP

#include "stdlib/string.hpp"
#include "base/base.hpp"


/* //////////////////////////////////////////////////////////////// */

class MachPhysEvasionPriorityPlan
{
public:

	MachPhysEvasionPriorityPlan( string EPPName );

	~MachPhysEvasionPriorityPlan() {};

	string name();

	void garrisonPriority( int priority );
	void aggressivesPriority( int priority );
	void podPriority( int priority );
	void turretsPriority( int priority );

	int	garrisonPriority() const;
	int aggressivesPriority() const;
	int	podPriority() const;
	int	turretsPriority() const;

	////////////////////////////////////////////////////////////////

private:

	string 		EPPName_;
	int			garrisonPriority_;
	int 		aggressivesPriority_;
	int			podPriority_;
	int			turretsPriority_;
};

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHPHYS_EPPs_HPP	*/

/* End EPP.HPP *****************************************************/
