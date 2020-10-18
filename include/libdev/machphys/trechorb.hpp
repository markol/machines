/*
 * T R E C H O R B . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysTreacheryOrb

    A treachery orb projectile.
*/

#ifndef _MACHPHYS_TRECHORB_HPP
#define _MACHPHYS_TRECHORB_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "machphys/lineproj.hpp"

//forward refs
class MexTransform3d;
class MachPhysWeaponData;
class MachPhysRadialDisc;

class MachPhysTreacheryOrb : public MachPhysLinearProjectile
// Canonical form revoked
{
public:
    //ctor.
    MachPhysTreacheryOrb( W4dEntity* pParent, const MexTransform3d& localTransform );

    //dtor
    virtual ~MachPhysTreacheryOrb();

    //Kick off launching/flying animation at startTime.
    //Weapon has details defined in data.
    //The launching machine race is race.
    PhysRelativeTime beLaunched(  const PhysAbsoluteTime& startTime,
                                  const MachPhysWeaponData& data,
                                  MachPhys::Race race );

    //Do the animation of striking on object and sucking its race colour (oldRace) out at
    //destroyTime.
    //return duration of the animation.
    PhysRelativeTime suckRaceAt
    (
        const PhysAbsoluteTime& destroyTime, MachPhys::Race oldRace
    );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysTreacheryOrb& t );

    PER_MEMBER_PERSISTENT( MachPhysTreacheryOrb );
    PER_FRIEND_READ_WRITE( MachPhysTreacheryOrb );
    
private:
    MachPhysTreacheryOrb( const MachPhysTreacheryOrb& );
    MachPhysTreacheryOrb& operator =( const MachPhysTreacheryOrb& );
    bool operator ==( const MachPhysTreacheryOrb& );

    //set up the basic material per race
    static const RenMaterial& orbMaterial( MachPhys::Race race );
    static RenMaterial createOrbMaterial( MachPhys::Race race );

    //data members
    MachPhysRadialDisc* pDisc_; //the child radial disc
};

PER_DECLARE_PERSISTENT( MachPhysTreacheryOrb );

#endif

/* End TRECHORB.HPP *************************************************/
