/*
 * V A P T R A I L . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysVapourTrail

    A vapur trail attached to the back of a missile
*/

#ifndef _MACHPHYS_VAPTRAIL_HPP
#define _MACHPHYS_VAPTRAIL_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point3d.hpp"
#include "ctl/pvector.hpp"
#include "phys/phys.hpp"

class MexTransform3d;
class W4dEntity;
class MachPhysVapourPuff;
class MachPhysWeaponData;

class MachPhysVapourTrail
{
public:
    MachPhysVapourTrail(
        W4dEntity* pParent,
        size_t nPuffs,
        MATHEX_SCALAR averagePuffSize, size_t missile_level );

    ~MachPhysVapourTrail();

//    void startTrail(
//        const PhysAbsoluteTime& startTime );
        
    //  Pass in the current position of the object the vapour trail is attached to
    //  and the offset used to get the position of the vapour puff
    void    update( const MexPoint3d& currentPosition, const MexVec3& offset );

    //Indicate that the vapour trail is not needed after endTime.
    //Must be called to ensure deletion of vapour puffs
    void finish( const PhysAbsoluteTime& endTime );
        
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysVapourTrail& t );

    PER_MEMBER_PERSISTENT( MachPhysVapourTrail );
    PER_FRIEND_READ_WRITE( MachPhysVapourTrail );

private:
    // Operation deliberately revoked
    MachPhysVapourTrail( const MachPhysVapourTrail& );

    // Operation deliberately revoked
    MachPhysVapourTrail& operator =( const MachPhysVapourTrail& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysVapourTrail& );

    ctl_pvector< MachPhysVapourPuff >    vapourPuffs_;

    size_t  updateRate_;    // in frames
    size_t  currentPuff_;
    size_t  updateAtFrame_;
    size_t  nPuffs_;
    
    MexPoint3d  lastPosition_;
};

PER_DECLARE_PERSISTENT( MachPhysVapourTrail );

#endif

/* End VAPTRAIL.HPP *************************************************/
