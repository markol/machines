/*
 * S M O K P L U M . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysSmokePlume

    A plume of smoke consisting of a number of puffs with
    appropriate motion plans
*/

#ifndef _MACHPHYS_SMOKPLUM_HPP
#define _MACHPHYS_SMOKPLUM_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/vector.hpp"

#include "world4d/composit.hpp"

#include "machphys/machphys.hpp"

class PhysLinearMotionPlan;
class MachPhysSmokePuff;

class MachPhysSmokePlume : public W4dComposite
{
public:

    //  maxHeight is a relative value - it is the height from the baseTransform
    //  Start the smoke plume at the given startTime
    MachPhysSmokePlume(
        W4dEntity* pParent,
        const MexTransform3d& localBaseTransform,
        MATHEX_SCALAR maxHeight,
        MATHEX_SCALAR depthOffset,
        size_t nPuffs,
        MATHEX_SCALAR averagePuffSize,
        MachPhysPuffType    puffType,
        const PhysAbsoluteTime& startTime,
        const PhysRelativeTime& duration );

    //  Create the smoke plume but don't start it yet - wait for startSmokePlume to be called
    MachPhysSmokePlume(
        W4dEntity* pParent,
        const MexTransform3d& localBaseTransform,
        MATHEX_SCALAR maxHeight,
        MATHEX_SCALAR depthOffset,
        size_t nPuffs,
        MATHEX_SCALAR averagePuffSize,
        MachPhysPuffType    puffType,
        const PhysRelativeTime& duration );

    void startSmokePlume( const PhysAbsoluteTime& startTime );

    ~MachPhysSmokePlume( void );

    //Inherited from W4dEntity
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysSmokePlume& t );

    PER_MEMBER_PERSISTENT_VIRTUAL( MachPhysSmokePlume );
    PER_FRIEND_READ_WRITE( MachPhysSmokePlume );

private:
    // Operation deliberately revoked
    MachPhysSmokePlume( const MachPhysSmokePlume& );

    // Operation deliberately revoked
    MachPhysSmokePlume& operator =( const MachPhysSmokePlume& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysSmokePlume& );

    PhysLinearMotionPlan* generateMotionPlan(
        const PhysRelativeTime& riseTime,
        MATHEX_SCALAR maxHeight );

    void constructEntityPlan(
        W4dEntity* pParent,
        const MexTransform3d& localBaseTransform,
        MATHEX_SCALAR maxHeight,
        MATHEX_SCALAR depthOffset,
        size_t nPuffs,
        MATHEX_SCALAR averagePuffSize,
        MachPhysPuffType puffType,
        const PhysRelativeTime& duration );

    #include "machphys/puffdata.hpp"
    PER_FRIEND_READ_WRITE( PuffData );

    typedef ctl_vector< PuffData >  PuffDatas;

    PuffDatas   puffs_;
};

PER_DECLARE_PERSISTENT( MachPhysSmokePlume );

#endif

/* End SMOKPLUM.HPP *************************************************/
