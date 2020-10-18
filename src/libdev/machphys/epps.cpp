/*
 * E P P s. C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "machphys/epps.hpp"
#include "machphys/epp.hpp"

// #include<algorith.hpp>


/* //////////////////////////////////////////////////////////////// */

MachPhysEvasionPriorityPlans& MachPhysEvasionPriorityPlans::instance()
{
	static MachPhysEvasionPriorityPlans instance_;
	return instance_;
}

/* //////////////////////////////////////////////////////////////// */

MachPhysEvasionPriorityPlans::MachPhysEvasionPriorityPlans()
:	nEPPs_( 0 )
{
	EPPs_.reserve( 16 );
}

/* //////////////////////////////////////////////////////////////// */

MachPhysEvasionPriorityPlans::~MachPhysEvasionPriorityPlans()
{
	for (EPPs::iterator i = EPPs_.begin(); i != EPPs_.end(); ++i )
	{
		_DELETE (*i);
	}
}

//////////////////////////////////////////////////////////////////////////////////

void MachPhysEvasionPriorityPlans::addNewEPP( const string& EPPName )
{
	PRE( not EPPExists( EPPName ) );

	MachPhysEvasionPriorityPlan* pEPP = _NEW( MachPhysEvasionPriorityPlan( EPPName ) );
	EPPs_.push_back( pEPP );
	++nEPPs_;

	POST( EPPs_.size() == nEPPs_ );
}

//////////////////////////////////////////////////////////////////////////////////

void MachPhysEvasionPriorityPlans::garrisonPriority( const string& EPPName, int priority )
{
	PRE_INFO( priority );
	// Special value of -2 allowed for garrisons which will indicate that NO evasion is ever to be
	// undertaken by any machine with this priority plan.
	PRE( priority < N_PRIORITY_LEVELS and priority >= -2 );
	PRE( EPPExists ( EPPName ) );

	EPP( EPPName ).garrisonPriority( priority );
}

//////////////////////////////////////////////////////////////////////////////////

void MachPhysEvasionPriorityPlans::aggressivesPriority( const string& EPPName, int priority )
{
	PRE_INFO( priority );
	PRE( priority < N_PRIORITY_LEVELS and priority >= -1 );
	PRE( EPPExists ( EPPName ) );

	EPP( EPPName ).aggressivesPriority( priority );
}

//////////////////////////////////////////////////////////////////////////////////

void MachPhysEvasionPriorityPlans::podPriority( const string& EPPName, int priority )
{
	PRE_INFO( priority );
	PRE( priority < N_PRIORITY_LEVELS and priority >= -1 );
	PRE( EPPExists ( EPPName ) );

	EPP( EPPName ).podPriority( priority );
}

//////////////////////////////////////////////////////////////////////////////////

void MachPhysEvasionPriorityPlans::turretsPriority( const string& EPPName, int priority )
{
    PRE_INFO( priority );
	PRE( priority < N_PRIORITY_LEVELS and priority >= -1 );
	PRE( EPPExists ( EPPName ) );

	EPP( EPPName ).turretsPriority( priority );
}

//////////////////////////////////////////////////////////////////////////////////

int MachPhysEvasionPriorityPlans::garrisonPriority( const string& EPPName ) const
{
	PRE( EPPExists ( EPPName ) );

	return EPP( EPPName ).garrisonPriority();
}

//////////////////////////////////////////////////////////////////////////////////

int MachPhysEvasionPriorityPlans::aggressivesPriority( const string& EPPName ) const
{
	PRE( EPPExists ( EPPName ) );

	return EPP( EPPName ).aggressivesPriority();
}

//////////////////////////////////////////////////////////////////////////////////

int MachPhysEvasionPriorityPlans::podPriority( const string& EPPName ) const
{
	PRE( EPPExists ( EPPName ) );

	return EPP( EPPName ).podPriority();
}

//////////////////////////////////////////////////////////////////////////////////

int MachPhysEvasionPriorityPlans::turretsPriority( const string& EPPName ) const
{
	PRE( EPPExists ( EPPName ) );

	return EPP( EPPName ).turretsPriority();
}


//////////////////////////////////////////////////////////////////////////////////

bool MachPhysEvasionPriorityPlans::EPPExists( const string& EPPName ) const
{
	bool found = false;

	for (EPPs::const_iterator i = EPPs_.begin(); not found and i != EPPs_.end(); ++i )
	{
		if( (*i)->name() == EPPName )
			found = true;
	}

	return found;
}

//////////////////////////////////////////////////////////////////////////////////

const MachPhysEvasionPriorityPlan& MachPhysEvasionPriorityPlans::EPP( const string& EPPName ) const
{
	PRE( EPPExists ( EPPName ) );

	MachPhysEvasionPriorityPlan* pEPP = NULL;
	bool found = false;

	for (EPPs::const_iterator i = EPPs_.begin(); not found and i != EPPs_.end(); ++i )
	{
		if( (*i)->name() == EPPName )
		{
			found = true;
			pEPP = (*i);
		}
	}

	return *pEPP;
}

//////////////////////////////////////////////////////////////////////////////////

MachPhysEvasionPriorityPlan& MachPhysEvasionPriorityPlans::EPP( const string& EPPName )
{
	PRE( EPPExists ( EPPName ) );

	bool found = false;
	MachPhysEvasionPriorityPlan* pEPP = NULL;

	for (EPPs::iterator i = EPPs_.begin(); not found and i != EPPs_.end(); ++i )
	{
		if( (*i)->name() == EPPName )
		{
			found = true;
			pEPP = (*i);
		}

	}

	return *pEPP;
}

//////////////////////////////////////////////////////////////////////////////////


/* End EPPs.CPP *************************************************/
