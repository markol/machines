/*
 * TRACKS . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "machphys/tracks.hpp"
#include "machphys/internal/tracksi.hpp"
#include "world4d/link.hpp"

#include "phys/timeangl.hpp"
#include "phys/linetrav.hpp"

#include "machphys/internal/itrack.hpp"
#include "machphys/mcmovinf.hpp"
#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT( MachPhysTracks );

MachPhysTracks::MachPhysTracks
(
    MachPhysMachine* pMachine,
	W4dLink* pLLeg,
	W4dLink* pRLeg,
	MATHEX_SCALAR repeatsPerMeter
)
: MachPhysLocomotionMethod( pImpl_ = _NEW( MachPhysTracksImpl( pMachine, this ) ) ),
  lTrack_(_NEW(MachPhysITrack(pLLeg, repeatsPerMeter))),
  rTrack_(_NEW(MachPhysITrack(pRLeg, repeatsPerMeter)))
{
	PRE(pLLeg && pRLeg);
	ASSERT(lTrack_ && rTrack_, "Out of memory?");
}

MachPhysTracks::MachPhysTracks( PerConstructor con )
: MachPhysLocomotionMethod( con ),
  pImpl_( NULL )
{
}

MachPhysTracks::~MachPhysTracks()
{
    _DELETE( lTrack_ );
    _DELETE( rTrack_ );
    // MachPhysLocomotionMethod destructor will free it
    //_DELETE( pImpl_ );
}

// virtual
MachPhysLocomotionMethod* MachPhysTracks::clone( MachPhysMachine* pNewMachine, const W4dLinks& links )
{
    W4dLink*    pLLeg = links[lTrack_->leg()->id()];
    W4dLink*    pRLeg = links[rTrack_->leg()->id()];

    return _NEW(MachPhysTracks( pNewMachine, pLLeg, pRLeg, lTrack_->repeatsPerMeter()));
}

// virtual
void MachPhysTracks::moveAnimations
(
    const PhysLinearTravelPlan& linearTravelPlan, const TurnAngles& turnAngles,
    const MachPhysMachineMoveInfo& info
)
{
    PRE( linearTravelPlan.nSegments() == turnAngles.size() );

	PhysAbsoluteTime startTime = info.startTime();

    //Deal with each segment of the travel plan
    const PhysMotionPlan::Times& segmentTimes = *linearTravelPlan.segmentTimes();
    PhysMotionPlan::Times::const_iterator iTimes = segmentTimes.begin();
    TurnAngles::const_iterator iAngles = turnAngles.begin();

    PhysRelativeTime lastTime = 0;
    while( iAngles != turnAngles.end() )
    {
        PhysRelativeTime segmentTime = *iTimes;
        PhysRelativeTime segmentDuration = segmentTime - lastTime;

		// TBD: remove the hard-coded speed.
        MexRadians angle = *iAngles;
		MATHEX_SCALAR lSpeed = 1, rSpeed = 1;

        // If the Machine is turning, reverse the appropriate track.
		if (angle > MexRadians(0.0))
			rSpeed = -rSpeed;
        else if (angle < MexRadians(0.0))
			lSpeed = -lSpeed;

		const PhysAbsoluteTime segStart = startTime + lastTime;
		rTrack_->runAtSpeed(rSpeed, segStart, segmentDuration);
		lTrack_->runAtSpeed(lSpeed, segStart, segmentDuration);

        ++iTimes;
        ++iAngles;
        lastTime = segmentTime;
    }
}

// virtual
void MachPhysTracks::stopDead()
{
	lTrack_->stopDead();
	rTrack_->stopDead();
}

void perWrite( PerOstream& ostr, const MachPhysTracks& tracks )
{
    const MachPhysLocomotionMethod& base = tracks;

    ostr << base;

    ostr << tracks.lTrack_;
    ostr << tracks.rTrack_;
    ostr << tracks.pImpl_;
}

void perRead( PerIstream& istr, MachPhysTracks& tracks )
{
    MachPhysLocomotionMethod& base = tracks;

    istr >> base;

    istr >> tracks.lTrack_;
    istr >> tracks.rTrack_;
    istr >> tracks.pImpl_;
}

void MachPhysTracks::doFirstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state )
{
    //If we haven't changed state do nothing
    if( state != pImpl_->lastState() )
    {
		MATHEX_SCALAR lSpeed =1.0, rSpeed = 1.0;
		PhysAbsoluteTime now = SimManager::instance().currentTime();
        switch( state )
        {
            case MachPhysLocomotionMethod::STOPPED:
            {
                //Play the stop walking animation once
              	stopDead();
                break;
            }

            case MachPhysLocomotionMethod::MOVING_FORWARDS:
			{
				rSpeed = 1.0;
				lSpeed = 1.0;
				rTrack_->runAtSpeed(rSpeed, now, 1000000);
				lTrack_->runAtSpeed(lSpeed, now, 1000000);
				break;
			}
            case MachPhysLocomotionMethod::MOVING_BACKWARDS:
			{
				rSpeed = -1.0;
				lSpeed = -1.0;
				rTrack_->runAtSpeed(rSpeed, now, 1000000);
				lTrack_->runAtSpeed(lSpeed, now, 1000000);
				break;
			}
            case MachPhysLocomotionMethod::TURNING_LEFT:
			{
				rSpeed = 1.0;
				lSpeed = -1.0;
				rTrack_->runAtSpeed(rSpeed, now, 1000000);
				lTrack_->runAtSpeed(lSpeed, now, 1000000);
				break;
			}
            case MachPhysLocomotionMethod::TURNING_RIGHT:
			{
				rSpeed = -1.0;
				lSpeed = 1.0;
				rTrack_->runAtSpeed(rSpeed, now, 1000000);
				lTrack_->runAtSpeed(lSpeed, now, 1000000);
				break;
			}
            case MachPhysLocomotionMethod::ENTERING:
            case MachPhysLocomotionMethod::LEAVING:
                break;
        }
    }
}

/* End TRACKS.CPP ****************************************************/
