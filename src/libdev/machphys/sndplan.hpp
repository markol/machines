/*
 * S N D P L A N . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysSoundPlan

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_SNDPLAN_HPP
#define _MACHPHYS_SNDPLAN_HPP

#include "base/base.hpp"

class MachPhysSoundPlan
{
public:
    MachPhysSoundPlan();
    ~MachPhysSoundPlan();

    void CLASS_INVARIANT;

private:
    // Operation deliberately revoked
    MachPhysSoundPlan( const MachPhysSoundPlan& );

    // Operation deliberately revoked
    MachPhysSoundPlan& operator =( const MachPhysSoundPlan& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysSoundPlan& );

};


#endif

/* End SNDPLAN.HPP **************************************************/
