/*
 * I T R A C K . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "machphys/internal/itrack.hpp"
#include "mathex/vec2.hpp"
#include "world4d/link.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/uvtrans.hpp"
#include "machphys/machphys.hpp"

PER_DEFINE_PERSISTENT( MachPhysITrack );

MachPhysITrack::MachPhysITrack(W4dLink* pLeg, MATHEX_SCALAR repeatsPerMeter):
	repeatsPerMeter_(repeatsPerMeter),
	leg_(pLeg)
{
}

MachPhysITrack::MachPhysITrack( PerConstructor )
: repeatsPerMeter_( 0 )
{
}

W4dLink* MachPhysITrack::leg() const
{
	return leg_;
}

MATHEX_SCALAR MachPhysITrack::repeatsPerMeter() const
{
	return repeatsPerMeter_;
}	

void MachPhysITrack::runAtSpeed
(
	MATHEX_SCALAR metersPerSecond, 
	const PhysAbsoluteTime& start, 
	const PhysRelativeTime& duration
)
{
	MexVec2 initialOffset(0,0);
	
	// If we previously had a plan, retrieve it's position and use that as a
	// starting offset for the new plan.
	if (uvPlan_.isDefined())
	{
		// Assume that the previous plan will run to completion and use it's end
		// position as the start position for the new plan.
        W4dUVTranslation& plan = w4dUVTranslation();
		plan.transform(plan.duration());
		initialOffset = plan.position();
	}
	
	const MexVec2 velocity(0, metersPerSecond * repeatsPerMeter_);
	uvPlan_ = _NEW(W4dUVTranslation(duration, W4dLOD( 1 ), velocity, initialOffset));
	W4dEntityPlan& ePlan = leg_->entityPlanForEdit();
	ePlan.uvPlan(uvPlan_, start, 0, MachPhys::MACHINE_LOCOMOTION);
}

W4dUVTranslation& MachPhysITrack::w4dUVTranslation()
{
    PRE( uvPlan_.isDefined() );

    //This cast is safe because this class only creates and stores
    //plans of this type. Note we need the counted pointer to be
    //at the base class level, hence the need for the cast.
    return _STATIC_CAST( W4dUVTranslation&, *uvPlan_ );
}

void perWrite( PerOstream& ostr, const MachPhysITrack& track )
{
    ostr << track.repeatsPerMeter_;
    ostr << track.leg_;
    ostr << track.uvPlan_;
}

void perRead( PerIstream& istr, MachPhysITrack& track )
{
    PER_READ_CONST_OBJECT( istr, MATHEX_SCALAR, track.repeatsPerMeter_ );
    istr >> track.leg_;
    istr >> track.uvPlan_;
}

void MachPhysITrack::stopDead()
{
	W4dEntityPlan& ePlan = leg_->entityPlanForEdit();
	ePlan.clearAnimation(MachPhys::MACHINE_LOCOMOTION);
}

/* End ITRACK.CPP **************************************************/
