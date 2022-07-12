/*
 * W E P U T I L . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysWeaponUtility

    A collection of facilities related to specific weapon types.
    Useful when an instance of a weapon type is not available.
*/

#ifndef _MACHPHYS_WEPUTIL_HPP
#define _MACHPHYS_WEPUTIL_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "machphys/machphys.hpp"

//forward refs
class MexLine3d;
class MachPhysMachine;
class MachPhysConstruction;
class MachPhysArtefact;

class MachPhysWeaponUtility
// Static methods only - no state
{
public:
    //Apply the victim animation for weapon type  at startTime to
    //machine pMachine or construction pConstruction or artefact pArtefact.
    //fromDirection indicates the flight path of the hitting projectile.
    //returns duration of the animation.
    static PhysRelativeTime victimAnimation( MachPhys::WeaponType type,
                                             const PhysAbsoluteTime& startTime,
                                             const MexLine3d& fromDirection,
                                             MachPhysMachine* pMachine );
                                            
    static PhysRelativeTime victimAnimation( MachPhys::WeaponType type,
                                             const PhysAbsoluteTime& startTime,
                                             const MexLine3d& fromDirection,
                                             MachPhysConstruction* pConstruction );
                                            
    static PhysRelativeTime victimAnimation( MachPhys::WeaponType type,
                                             const PhysAbsoluteTime& startTime,
                                             const MexLine3d& fromDirection,
                                             MachPhysArtefact* pArtefact );
                                            
private:
    MachPhysWeaponUtility();
    ~MachPhysWeaponUtility();
    MachPhysWeaponUtility( const MachPhysWeaponUtility& );
    MachPhysWeaponUtility& operator =( const MachPhysWeaponUtility& );
    bool operator ==( const MachPhysWeaponUtility& );
};


#endif

/* End WEPUTIL.HPP **************************************************/
