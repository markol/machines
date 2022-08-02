/*
 * M C M O V I N F . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysMachineMoveInfo

    Stores data representing a machine move. A machine move consists of turning
    at a start point, followed by a translation to a new destination point.
    Used to store the motion profile that would be taken by a machine for a
    proposed move.
*/

#ifndef _MACHPHYS_MCMOVINF_HPP
#define _MACHPHYS_MCMOVINF_HPP

#include "base/base.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/radians.hpp"
#include "phys/phys.hpp"
#include "phys/rampacce.hpp"

#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"

#include "machphys/machphys.hpp"

//memberwise canonical
class MachPhysMachineMoveInfo
{
public:
    typedef ctl_vector< MexTransform3d >        Transforms;
    typedef CtlCountedPtr< Transforms >         TransformsPtr;

    typedef ctl_vector< PhysRampAcceleration >  RampAccelerations;
    typedef CtlCountedPtr< RampAccelerations >  RampAccelerationsPtr;

    //  This exists only to keep vector happy. It should never be called.
    MachPhysMachineMoveInfo();

    // Construct move information. The first two transforms might be
    //  at the same position but have different orientations in which
    //  case there is a turn before the motion.    
    MachPhysMachineMoveInfo( 
         TransformsPtr transformsPtr,
         RampAccelerationsPtr rampAccelerationsPtr,
         const PhysAbsoluteTime& startTime );
    //  PRE( transformsPtr->size() > 1 );
    //  PRE( transformsPtr->size() == rampAccelerationsPtr->size() + 1 );
    //  PRE( MexUtility::inStraightLineXY( *transformsPtr ) );

    //dtor.
    ~MachPhysMachineMoveInfo();

    //Accessors
    const MexTransform3d& startTransform() const;
    const MexTransform3d& turnTransform() const;
    //  PRE( needsToTurn() );
    
    const MexTransform3d& endTransform() const;

    const Transforms& transforms() const;

    PhysRelativeTime turnTime() const;
    // POST( implies( not NeedsToTurn(), result == 0.0 ) );
    
    const RampAccelerations& moveProfiles() const;

    const PhysAbsoluteTime& startTime() const;

    //  Note that the time is an _absolute_ time, i.e. it is 
    // _not_ relative to the beginning of this motion.
    MexTransform3d transform( PhysAbsoluteTime time ) const;
    // PRE( time >= startTime() );
    // PRE( time <= startTime() + totalTime() );

    const MexRadians& turnAngle() const;

    bool needsToTurn() const;

    MachPhys::FinalState finalState() const;

    //  Returns the total time for this movement including any
    //  time spent turning at the beginning
    PhysRelativeTime    totalTime() const;

    //  Returns an acceleration profile which averages out correctly
    //  for the translation part of the motion.
    PhysRampAcceleration    averageProfile() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysMachineMoveInfo& t );

private:

    //Data members
    TransformsPtr           transformsPtr_;
    RampAccelerationsPtr    rampAccelerationsPtr_;
    PhysAbsoluteTime        startTime_; //Time the motion starts
    MexRadians              turnAngle_;//Angle machine rotates about z axis when turning if needsToTurn_ true
    bool                    needsToTurn_ :1; //True of a turn is needed.
    PhysRelativeTime        totalTime_;
};

bool operator ==( const MachPhysMachineMoveInfo&, const MachPhysMachineMoveInfo& );
bool operator <( const MachPhysMachineMoveInfo&, const MachPhysMachineMoveInfo& );

#ifdef _INLINE
    #include "machphys/mcmovinf.ipp"
#endif


#endif

/* End MCMOVINF.HPP *************************************************/
