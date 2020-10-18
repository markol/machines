/*
 * S P Y L O C A I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysSpyLocatorImpl

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_SPYLOCAI_HPP
#define _MACHPHYS_SPYLOCAI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/countptr.hpp"
#include "phys/phys.hpp"
#include "world4d/compplan.hpp"

class MachPhysSpyLocatorImpl
// Canonical form revoked
{
public:
    MachPhysSpyLocatorImpl( void );
    ~MachPhysSpyLocatorImpl( void );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysSpyLocatorImpl );
    PER_FRIEND_READ_WRITE( MachPhysSpyLocatorImpl );

private:
    friend ostream& operator <<( ostream& o, const MachPhysSpyLocatorImpl& t );
    friend class MachPhysSpyLocator;

    MachPhysSpyLocatorImpl( const MachPhysSpyLocatorImpl& );
    MachPhysSpyLocatorImpl& operator =( const MachPhysSpyLocatorImpl& );

    //data members
    W4dCompositePlanPtr locatingPlanPtr_; //Animation for locating
    PhysAbsoluteTime locatingPlanEndTime_; //Time at which any queued locating animation ends
    bool isLocating_; //True if in locating mode

};

PER_DECLARE_PERSISTENT( MachPhysSpyLocatorImpl );

#endif

/* End SPYLOCAI.HPP *************************************************/
