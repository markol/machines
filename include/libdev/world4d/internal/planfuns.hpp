/*
 * P L A N F U N S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    Global template functions used by W4dEntityPlan in conjunction with
    template class W4dPendingPlan.
*/

#ifndef _WORLD4D_PLANFUNS_HPP
#define _WORLD4D_PLANFUNS_HPP

//Find the actual plan to use from the list of pending plans in plans,
//at time. Returns the state of this list of plans at this time.
//If defined, return teh actual plan adn time offset to use from its start
//in pPlanPtr and pTimeOffset. The latter takes account of repetitions.
//pNObsoletePlans returns the count of any obsolete plans at time.
template< class PLAN_TYPE >
W4dEntityPlan::PlanState FindPlan
(
	const ctl_list< W4dPendingPlan< PLAN_TYPE > >& plans,
	const PhysAbsoluteTime& time,
	PLAN_TYPE* pPlanPtr, PhysRelativeTime* pTimeOffset,
	uint* pNObsoletePlans
);


//Delete any plans in pPlans that have expired at time.
template< class PLAN_TYPE >
void ClearObsoletePlansAtTime
(
	ctl_list< W4dPendingPlan< PLAN_TYPE > >* pPlans,
	const PhysAbsoluteTime& time
);

//Reset the repetition count of any plans so that the current repetition becomes the last
template< class PLAN_TYPE >
PhysAbsoluteTime FinishAnimation
(
	ctl_list< W4dPendingPlan< PLAN_TYPE > >* pPlans,
	uint animId
);

//Delete all plans with animation id animId
template< class PLAN_TYPE >
void ClearAnimation
(
	ctl_list< W4dPendingPlan< PLAN_TYPE > >* pPlans,
	uint animId
);

#endif

/* End PLANFUNS.HPP *************************************************/
