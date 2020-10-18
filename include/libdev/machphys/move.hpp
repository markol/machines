/*
 * M O V E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 * Mixin for physical objects that support
 * translational animation.
 */

#ifndef _MACHPHYS_MOVE_HPP
#define _MACHPHYS_MOVE_HPP

#include "stdlib/memory.hpp"
#include "phys/phys.hpp"
#include "mathex/mathex.hpp"
#include "machphys/mcmovinf.hpp"
#include "machphys/plansurf.hpp"
#include <memory>

//forward declarations
class MexTransform3d;
class MexPoint3d;

/* //////////////////////////////////////////////////////////////// */

class MachPhysCanMove
{
public:

	virtual ~MachPhysCanMove() {};


    //Compute move data including transforms, motion profile data etc for
    //a move from fromGlobalPosition to toPoint, starting at startTime.
    //Initial speed is startSpeed.
    //The orientatinInfo tells us whether we should use the orientation
    //given in fromGlobalPosition or whether we should calculate the starting
    //orientation ourselves
    //The top travel speed to use is limited by capSpeed.
    //If comeToRest is true, must come to rest at toPoint. Otherwise, should
    //be travelling with translation speed.

    enum OrientationInfo { USE_ORIENTATION, DONT_USE_ORIENTATION };

    typedef CtlCountedPtr< MachPhysPlanetSurface::Profile > ProfilePtr;

    // TBD: Add this virtual function - it isn't being done now
    // because it can't be cheated. Bob

//     virtual ProfilePtr profile(
//         const MexTransform3d& fromGlobalTransform,
//         OrientationInfo orientationInfo, const MexPoint3d& destination,
//         const MachPhysPlanetSurface& planetSurface ) const = 0;

    virtual std::auto_ptr< MachPhysMachineMoveInfo > moveInfo(
        ProfilePtr profilePtr,
        const PhysAbsoluteTime& startTime, MATHEX_SCALAR startSpeed,
        MATHEX_SCALAR capSpeed, MachPhys::FinalState finalState ) = 0;

    //Apply move animations to make the move as defined by info
    virtual void move( const MachPhysMachineMoveInfo& info ) = 0;

    //  Stop the move dead. Apply appropriate animations.
    virtual void stopDead() = 0;
};

ostream& operator <<( ostream&, MachPhysCanMove::OrientationInfo );

/* //////////////////////////////////////////////////////////////// */

#endif	//	#ifndef _MACHPHYS_MOVE_HPP

/* End MOVE.HPP *****************************************************/
