/*
 * S T A T I M P L . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogStatsImpl

    This class holds lots of numbers which may be use in a variety of ways.
	Some of the numbers are used during initialisation whilst others are used
	for the heuristic scoring.
*/

#ifndef _MACHLOG_STATIMPL_HPP
#define _MACHLOG_STATIMPL_HPP

#include "base/base.hpp"
#include "machphys/machphys.hpp"

class MachLogStats;

class MachLogStatsImpl
// Canonical form revoked
{
public:
    MachLogStatsImpl();
    ~MachLogStatsImpl();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogStatsImpl& t );

private:
	void readStatsInitialisationFile();

	friend class MachLogStats;
	friend class MachLogRaces;

    MachLogStatsImpl( const MachLogStatsImpl& );
    MachLogStatsImpl& operator =( const MachLogStatsImpl& );
    bool operator ==( const MachLogStatsImpl& );

	int					pcPriority_;
	int					aiPriority_;
	PhysRelativeTime	maxMoveTime_;
	MATHEX_SCALAR		potentialKillRadiusMultiplier_;
	float 				cancelledRefundablePercentage_;
	float				secondhandRefundablePercentage_;
	double 				friendlyDeconstructMultiplier_; 
	double 				enemyDeconstructMultiplier_;
	double 				enemyCaptureMultiplier_;
	PhysAbsoluteTime	connectionTime_;
	double				scannerRangeToWeaponRangeRatio_;
	PhysRelativeTime	resyncThreshold_; //Time difference between host and local node
	                                      //below which resync messages are ignored
	double				nuclearMissileBlastRange_;
};


#endif

/* End STATIMPL.HPP *************************************************/
