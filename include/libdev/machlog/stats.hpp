/*
 * S T A T S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogStats

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_STATS_HPP
#define _MACHLOG_STATS_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "machphys/machphys.hpp"

class MachLogStatsImpl;

class MachLogStats
// Canonical form revoked
{
public:

    MachLogStats( void );
    ~MachLogStats( void );

	int pcPriority() const;
	int aiPriority() const;
	PhysRelativeTime	maxMoveTime() const;
	double potentialKillRadiusMultiplier() const; 	// e.g. 30 means that radius weapons (vortexbomb) checks an extra 30% beyond its													
													//  blast radius for victims that might wander into blast range
	double scannerRangeToWeaponRangeRatio() const;	// e.g. 2.0 means that a turret with a weapon range of 50 metres can
													//  scan enemies up to 100 metres away, and begin responding accordingly

	PhysAbsoluteTime	connectionTime() const;

    //Set/get a time representing the difference between a sync time transmitted by a network host
    //and the current time on a local node. If the difference is less than this time, then resync messages
    //will be ignored.
    PhysRelativeTime resyncThreshold() const; 
    void resyncThreshold( PhysRelativeTime threshold ); 

	float cancelledRefundablePercentage() const;  // e.g. 0.75 means you only get 75% back from cancelled factory productions
	float secondhandRefundablePercentage() const; // e.g. 0.50 means you only get 50% back from deconstructed buildings and recycled machines
    double friendlyDeconstructMultiplier() const; // e.g. 3.0 means you can dismantle your own buildings at x3.0 the constructor's build rate
	double enemyDeconstructMultiplier() const; // e.g. 0.5 means you can dismantle enemy buildings at x0.5 the constructor's build rate
	double enemyCaptureMultiplier() const; // e.g. 2.0 means you can damage enemy buildings during a capture operation at x2.0 the constructor's build rate

	double nuclearMissileBlastRange() const;


	void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogStats& t );
private:
    MachLogStats( const MachLogStats& );
    MachLogStats& operator =( const MachLogStats& );
    bool operator ==( const MachLogStats& );


	MachLogStatsImpl*	pStatsImpl_;
};


#endif

/* End STATS.HPP ****************************************************/
