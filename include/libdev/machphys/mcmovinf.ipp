/*
 * M C M O V I N F . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

#include "mathex/epsilon.hpp"

_CODE_INLINE
const MexTransform3d& MachPhysMachineMoveInfo::startTransform() const
{
    return transforms()[ 0 ];
}

_CODE_INLINE
const MexTransform3d& MachPhysMachineMoveInfo::turnTransform() const
{
    PRE( needsToTurn() );
    
    return transforms()[ 1 ];
}

_CODE_INLINE
const MexTransform3d& MachPhysMachineMoveInfo::endTransform() const
{
    return transforms().back();
}

_CODE_INLINE
const MachPhysMachineMoveInfo::Transforms& MachPhysMachineMoveInfo::transforms() const
{
    return *transformsPtr_;
}

_CODE_INLINE
PhysRelativeTime MachPhysMachineMoveInfo::turnTime() const
{
    PhysRelativeTime result = 0.0;

    if( needsToTurn() )
        result = rampAccelerationsPtr_->front().totalTime();
    
    return result;
}

_CODE_INLINE
const MachPhysMachineMoveInfo::RampAccelerations& MachPhysMachineMoveInfo::moveProfiles() const
{
    return *rampAccelerationsPtr_;
}

_CODE_INLINE
const PhysAbsoluteTime& MachPhysMachineMoveInfo::startTime() const
{
    return startTime_;
}

_CODE_INLINE
const MexRadians& MachPhysMachineMoveInfo::turnAngle() const
{
    return turnAngle_;
}

_CODE_INLINE
bool MachPhysMachineMoveInfo::needsToTurn() const
{
    return needsToTurn_;
}

_CODE_INLINE
MachPhys::FinalState MachPhysMachineMoveInfo::finalState() const
{
    MachPhys::FinalState  result;

    if( MexEpsilon::isWithinEpsilonOf( rampAccelerationsPtr_->back().endSpeed(), 0.0 ) )
        result = MachPhys::AT_REST;
    else
        result = MachPhys::IN_MOTION;

    return result;
}

/* End MCMOVINF.IPP *************************************************/
