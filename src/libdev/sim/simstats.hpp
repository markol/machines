/*
 * S I M S T A T S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    SimStats

    A brief description of the class should go in here
*/

#ifndef _SIM_SIMSTATS_HPP
#define _SIM_SIMSTATS_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"

class SimStats
// Canonical form revoked
{
public:
    //  Singleton class
    static SimStats& instance();
    ~SimStats();

	PhysRelativeTime		minComputationTime();
	PhysRelativeTime		targetRenderInterval();

    //  This is the minimum time that a process will get to update itself
	PhysRelativeTime		minProcessUpdateTime();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const SimStats& t );

private:
	void readInitialisationFile();

    SimStats( const SimStats& );
    SimStats& operator =( const SimStats& );
    bool operator ==( const SimStats& );

    SimStats();
	PhysRelativeTime		minComputationTime_;
	PhysRelativeTime		targetRenderInterval_;
	PhysRelativeTime		minProcessUpdateTime_;
};


#endif

/* End SIMSTATS.HPP *************************************************/
