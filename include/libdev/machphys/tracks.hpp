/*
 * TRACKS . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _TRACKS_HPP
#define _TRACKS_HPP

#include "machphys/locomoti.hpp"

#include "base/persist.hpp"

class W4dLink;
class MachPhysITrack;
class MachPhysMachineMoveInfo;
class MachPhysTracksImpl;

// A locomotion sub-class which animates two moving caterpillar tracks.
class MachPhysTracks : public MachPhysLocomotionMethod
{
public:
	// Texture co-ordinate animations work in terms of UV units per second.
	// Travel plans work in meters per second.  In order to convert between
	// the two we need to know how many texture repeats there are per meter on
	// a given Machine's track.  This must be hardcoded by the client for each
	// track.  Hence the 5th paramter.  Both tracks are assumed to be the same.
	// PRE(pLLeg && pRLeg);
    MachPhysTracks(
        MachPhysMachine* pMachine,
        W4dLink* pLLeg, W4dLink* pRLeg, MATHEX_SCALAR repeatsPerMeter);

    virtual ~MachPhysTracks();

    virtual MachPhysLocomotionMethod* clone( MachPhysMachine* pMachine, const W4dLinks& links );

    virtual void moveAnimations( const PhysLinearTravelPlan& linearTravelPlan, const TurnAngles& turnAngles,
                                 const MachPhysMachineMoveInfo& info );

    //  Stop the move dead. Apply appropriate animations.
    virtual void stopDead();

    PER_MEMBER_PERSISTENT_VIRTUAL( MachPhysTracks );
    PER_FRIEND_READ_WRITE( MachPhysTracks );

private:
    friend class MachPhysTracksImpl;

    //Callback to do the work of displaying the locomotion animations
    void doFirstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state );

	MachPhysITrack* lTrack_;
	MachPhysITrack* rTrack_;
    MachPhysTracksImpl* pImpl_;

    // Operations deliberately revoked
    MachPhysTracks( const MachPhysTracks& );
    MachPhysTracks& operator =( const MachPhysTracks& );
    bool operator ==( const MachPhysTracks& );
};

PER_DECLARE_PERSISTENT( MachPhysTracks );

#endif

/* End TRACKS.HPP ****************************************************/
