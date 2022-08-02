/*
 * I T R A C K . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _MACHPHYS_ITRACKS_HPP
#define _MACHPHYS_ITRACKS_HPP

#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "ctl/countptr.hpp"
#include "world4d/uvplan.hpp"

class W4dLink;
class W4dUVTranslation;

// Represents the animation of a single track for a Machine.  All our current
// Machines own two tracks.
class MachPhysITrack
{
public:
    MachPhysITrack(W4dLink* pLeg, MATHEX_SCALAR repeatsPerMeter);
	
	W4dLink* leg() const;
	MATHEX_SCALAR repeatsPerMeter() const;
	
	// Animate this track at a given constant speed.
	void runAtSpeed(MATHEX_SCALAR metersPerSecond, const PhysAbsoluteTime& start, 
					const PhysRelativeTime& duration);

	void stopDead();
		
    PER_MEMBER_PERSISTENT( MachPhysITrack );
    PER_FRIEND_READ_WRITE( MachPhysITrack );
    
private:
    //The cached world4d level plan.
    W4dUVTranslation& w4dUVTranslation();

	const MATHEX_SCALAR	repeatsPerMeter_;
	W4dLink*			leg_;
	W4dUVPlanPtr		uvPlan_;
};

PER_DECLARE_PERSISTENT( MachPhysITrack );

#endif

/* End ITRACK.HPP **************************************************/
