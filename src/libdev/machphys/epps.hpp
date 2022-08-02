/*
 * E P P s . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 * MachPhysEvasionPriorityPlans is a collection of EPPs. An MachPhysEvasionPriorityPlan is an evasion priority
 * 	plan, which designates the preference of evasion strategy a machine with that plan
 *  will look for in order, e.g. attempts to find nearby garrisons to hide in first, if
 *	that fails, tries to find nearby turrets within whose weapon umbrella to hide etc.
 *	It is designed as a singleton class. 
 */
 
// canonical form revoked

#ifndef _MACHPHYS_EPPs_HPP
#define _MACHPHYS_EPPs_HPP

#include "stdlib/string.hpp"

#include "ctl/pvector.hpp"
#include "base/base.hpp"

/* //////////////////////////////////////////////////////////////// */

class MachPhysEvasionPriorityPlan;

class MachPhysEvasionPriorityPlans
{
public:

	enum NPriorityLevels { N_PRIORITY_LEVELS = 4 };

	typedef ctl_pvector< MachPhysEvasionPriorityPlan > EPPs;
		
	static MachPhysEvasionPriorityPlans& instance();

	~MachPhysEvasionPriorityPlans();
	
	///////////////////////////////
	
	
	void addNewEPP( const string& EPPName );
	
	void garrisonPriority( const string& EPPName, int priority );
	void aggressivesPriority( const string& EPPName, int priority );
	void podPriority( const string& EPPName, int priority );
	void turretsPriority( const string& EPPName, int priority );
	
	int garrisonPriority( const string& EPPName ) const;
	int aggressivesPriority( const string& EPPName ) const;
	int podPriority( const string& EPPName ) const;
	int turretsPriority( const string& EPPName ) const;

	const MachPhysEvasionPriorityPlan& EPP( const string& EPPName ) const;
	
	bool EPPExists( const string& EPPName ) const;
 
private:

	MachPhysEvasionPriorityPlans();
	
	// Operation deliberately revoked
    MachPhysEvasionPriorityPlans( const MachPhysEvasionPriorityPlans& );

    // Operation deliberately revoked
    MachPhysEvasionPriorityPlans& operator =( const MachPhysEvasionPriorityPlans& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysEvasionPriorityPlans& );
	
	MachPhysEvasionPriorityPlan& EPP( const string& EPPName );

	int							nEPPs_;
	EPPs						EPPs_;
};

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHPHYS_EPPs_HPP	*/

/* End EPPs.HPP *****************************************************/
