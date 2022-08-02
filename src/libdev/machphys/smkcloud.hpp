/*
 * S M K C L O U D . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysSmokeCloud

    Models a cloud of smoke of various colours
*/

#ifndef _MACHPHYS_SMKCLOUD_HPP
#define _MACHPHYS_SMKCLOUD_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "mathex/mathex.hpp"
#include "phys/phys.hpp"
#include "world4d/entity.hpp"
#include "world4d/matplan.hpp"

//forward refs
class MexPoint3d;
class MexVec3;

class MachPhysSmokeCloud : public W4dEntity
// Canonical form revoked
{
public:
    //ctor. Attached to pParent at offset startPosition.
    MachPhysSmokeCloud( W4dEntity* pParent, const MexPoint3d& startPosition );

    //The cloud consists of nWisps wisps. Each wisp is a TTF, going through a sequence of
    //materials as defined by materialPlanPtr.
    //Each wisp starts at the origin and moves to endPosition relative this object.
    //These positions are varied randomly up to the amount specified by randomOffset
    //(in either +ve or -ve directions).
    //The first wisp starts at startTime, with successive ones at intervals of wispInterval.
    //The initial dimensions of each wisp are wispSize.
    //They scale up by a factor of 2 over the animation.
    //The z buffer depth offset is zDepthOffset. Note that -ve moves towards the eye (usual).
    //returns the time offset from startTime to the disappearance of the last wisp.
    PhysRelativeTime startSmoking( uint nWisps,
                        const MexPoint3d& endPosition,
                        const MexVec3& randomOffset,
                        const PhysAbsoluteTime& startTime, const PhysRelativeTime& wispInterval,
                        MATHEX_SCALAR wispSize, MATHEX_SCALAR zDepthOffset,
                        const W4dMaterialPlanPtr& materialPlanPtr, uint nRepetations, uint nBlankPeriods );

    PhysRelativeTime startSmoking( uint nWisps,
                        const MexPoint3d& endPosition,
                        const MexVec3& randomOffset,
                        const PhysAbsoluteTime& startTime, const PhysRelativeTime& wispInterval,
                        MATHEX_SCALAR wispSize, MATHEX_SCALAR zDepthOffset,
                        const W4dMaterialPlanPtr& materialPlanPtr );
    //PRE( nWisps != 0 );
    //PRE( wispSize > 0.0 );

    //dtor.
    virtual ~MachPhysSmokeCloud();

    //Initiate the smoking animation at startTime.
    //Returns the duration of the animation.
    PhysRelativeTime startSmoking();

    //Inherited from W4dEntity. returns false.
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                                 Accuracy accuracy ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysSmokeCloud& t );

    PER_MEMBER_PERSISTENT( MachPhysSmokeCloud );

private:
    MachPhysSmokeCloud( const MachPhysSmokeCloud& );
    MachPhysSmokeCloud& operator =( const MachPhysSmokeCloud& );
    bool operator ==( const MachPhysSmokeCloud& );

    //data members
};

PER_DECLARE_PERSISTENT( MachPhysSmokeCloud );
PER_READ_WRITE( MachPhysSmokeCloud );

#endif

/* End SMKCLOUD.HPP *************************************************/
