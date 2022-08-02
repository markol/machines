/*
 * M A C H B U R N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysMachineBurning

    Controls the display of flames on a MachPhysMachine which is or may be on fire.
*/

#ifndef _MACHPHYS_MACHBURN_HPP
#define _MACHPHYS_MACHBURN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/pvector.hpp"
#include "mathex/mathex.hpp"
#include "phys/phys.hpp"

//forward refs
class MachPhysFlame;
class MachPhysMachine;
class W4dLink;

class MachPhysMachineBurning
// Canonical form revoked
{
public:
    //ctor. pMachine is the one which is to burn
    MachPhysMachineBurning( MachPhysMachine* pMachine );

    //dtor. Deletes the flame entities as well.
    ~MachPhysMachineBurning();

    //Initiate percentage level burning at startTime, to expire after duration.
    void burn( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration,    
               MATHEX_SCALAR percentage );
    //PRE( percentage > 0.0 );
    //PRE( percentage <= 100.0 );

    //true if all requested burning has now expired
    bool isBurningFinished() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysMachineBurning& t );

    //Persistence
    PER_MEMBER_PERSISTENT( MachPhysMachineBurning );
    PER_FRIEND_READ_WRITE( MachPhysMachineBurning );

private:
    //revoked
    MachPhysMachineBurning& operator =( const MachPhysMachineBurning& );
    MachPhysMachineBurning( const MachPhysMachineBurning& );
    bool operator ==( const MachPhysMachineBurning& );

    typedef ctl_pvector< MachPhysFlame > Flames;
    typedef ctl_pvector< W4dLink > Links;

    //returns a vector of up to nWanted link pointers for links from machine, sorted
    //into ascending order of size by bounding volume volume. Chooses the largest
    //links from the machine to include in the list.
    static Links burnLinks( const MachPhysMachine& machine, uint nWanted );

    //data members
    MachPhysMachine* pMachine_; //The poor wee machine that's getting singed
    PhysAbsoluteTime endBurningTime_; //Time at which final burning is finished
    Flames flames_; //Vector of pointers to flames attached to links
};

PER_DECLARE_PERSISTENT( MachPhysMachineBurning );

#endif

/* End MACHBURN.HPP *************************************************/
