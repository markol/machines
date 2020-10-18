/*
 * E P P. C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "machphys/epp.hpp"

MachPhysEvasionPriorityPlan::MachPhysEvasionPriorityPlan( string EPPName )
:	EPPName_( EPPName ),
	garrisonPriority_( -1 ),
	aggressivesPriority_( -1 ),
	podPriority_( -1 ),
	turretsPriority_( -1 )
{
	// intentionally left blank
}

//////////////////////////////////////////////////////////////////////////////////

string MachPhysEvasionPriorityPlan::name()
{
	return EPPName_;
}

//////////////////////////////////////////////////////////////////////////////////

void MachPhysEvasionPriorityPlan::garrisonPriority( int priority )
{
	PRE( priority < 4 and priority >= -2 );
	garrisonPriority_ = priority;
}

//////////////////////////////////////////////////////////////////////////////////

void MachPhysEvasionPriorityPlan::aggressivesPriority( int priority )
{
	PRE( priority < 4 and priority > -2 );
	aggressivesPriority_ = priority;
}

//////////////////////////////////////////////////////////////////////////////////

void MachPhysEvasionPriorityPlan::podPriority( int priority )
{
	PRE( priority < 4 and priority > -2 );
	podPriority_ = priority;
}

//////////////////////////////////////////////////////////////////////////////////

void MachPhysEvasionPriorityPlan::turretsPriority( int priority )
{
	PRE( priority < 4 and priority > -2 );
	turretsPriority_ = priority;
}

//////////////////////////////////////////////////////////////////////////////////

int	MachPhysEvasionPriorityPlan::garrisonPriority() const
{
	return garrisonPriority_;
}

//////////////////////////////////////////////////////////////////////////////////

int MachPhysEvasionPriorityPlan::aggressivesPriority() const
	{
	return aggressivesPriority_;
}

//////////////////////////////////////////////////////////////////////////////////

int	MachPhysEvasionPriorityPlan::podPriority() const
	{
	return podPriority_;
}

//////////////////////////////////////////////////////////////////////////////////

int	MachPhysEvasionPriorityPlan::turretsPriority() const
	{
	return turretsPriority_;
}

//////////////////////////////////////////////////////////////////////////////////


/* End EPP.CPP *************************************************/
