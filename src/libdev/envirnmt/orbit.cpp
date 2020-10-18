/*
 * O R B I T . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "envirnmt/orbit.hpp"
#include "phys/rampacce.hpp"
#include "ctl/countptr.hpp"
#include "ctl/vector.hpp"
#include "mathex/vec3.hpp"
#include "mathex/radians.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"

#include "phys/linemoti.hpp"
#include "phys/timeangl.hpp"
#include "phys/linetrav.hpp"

#include "world4d/entity.hpp"
#include "world4d/generic.hpp"
#include "world4d/manager.hpp"
#include "world4d/entyplan.hpp"

#include <limits.h>
#include <math.h>

// With a period of 30 minutes, 10 million orbits take 500 years.
// TBD: should we worry about the year 3000 bug?
static const uint FOREVER = 10000000;

static void createOrbitSpin
(
	W4dEntityPlan& ePlan, 
	MexDegrees rotSpeed, 
	PhysAbsoluteTime startTime,
	MexRadians setHeading
)
{
	// Set up a motion plan which spins about the z-axis at a constant speed.
	PhysMotionPlan::AnglesPtr angles = _NEW(PhysMotionPlan::Angles);
    angles->reserve( 4 );
	angles->push_back(setHeading + MexDegrees(0));
	angles->push_back(setHeading + MexDegrees(120));
	angles->push_back(setHeading + MexDegrees(240));
	angles->push_back(setHeading + MexDegrees(360));

	PhysMotionPlan::TimesPtr times = _NEW(PhysMotionPlan::Times);
    times->reserve( 3 );
	times->push_back(120.0 / fabs(rotSpeed.asScalar()));
	times->push_back(240.0 / fabs(rotSpeed.asScalar()));
	times->push_back(360.0 / fabs(rotSpeed.asScalar()));

	// If the velocity is -ve, stick with +ve angles and reverse the axis.
	MexVec3 zAxis(0, 0, (rotSpeed < MexRadians(0))? -1: 1);
	PhysMotionPlanPtr plan = _NEW(PhysTimedAnglePlan(angles, times, zAxis, MexPoint3d()));
	
	ePlan.absoluteMotion(plan, startTime, FOREVER);
}

static PhysTimedAnglePlan* createRiseSetOrbit
(
	MexRadians minEl, 
	MexRadians maxEl, 
	PhysRelativeTime period
)
{
	// Set up a motion plan which repeatedly oscillates between two 
	// elevations (about the y-axis).
	PhysMotionPlan::AnglesPtr angles = _NEW(PhysMotionPlan::Angles);
    angles->reserve( 3 );
	angles->push_back(minEl);
	angles->push_back(maxEl);
	angles->push_back(minEl);

	// Set the times so that the angle is min at time=0 and at time=period.
	// The angle will be max at time=period/2.
	PhysMotionPlan::TimesPtr times = _NEW(PhysMotionPlan::Times);
    times->reserve( 2 );
	times->push_back(period / 2);
	times->push_back(period);

	MexVec3 yAxis(0, 1, 0);
	return _NEW(PhysTimedAnglePlan(angles, times, yAxis, MexPoint3d()));
}

static PhysTimedAnglePlan* createPartialOrbit
(
	MexRadians minEl, 
	MexRadians maxEl, 
	PhysRelativeTime startTime,
	PhysRelativeTime period
)
{
	PRE(period > startTime);
	const PhysRelativeTime halfPeriod = 0.5 * period;

	// Firstly, create a complete min-to-max cycle and determine the 
	// elevation at startTime.
	PhysTimedAnglePlan* complete = createRiseSetOrbit(minEl, maxEl, period);
	const MexRadians startEl = complete->angle(startTime);
	_DELETE(complete);

	PhysMotionPlan::AnglesPtr angles = _NEW(PhysMotionPlan::Angles);
    angles->reserve( 3 );
	PhysMotionPlan::TimesPtr  times  = _NEW(PhysMotionPlan::Times);
    times->reserve( 2 );
	PhysRelativeTime duration = period - startTime;

	// At time=0 (relative) be at startEl.
	angles->push_back(startEl);
	
	// Is this more than half a cycle?  If so, we need to attain the maximum
	// elevation as part of this plan before descending to the minimum.
	if (duration > halfPeriod)
	{
		times->push_back(duration - halfPeriod);
		angles->push_back(maxEl);
	}

	// Finish at minEl at time=duration.		
	angles->push_back(minEl);
	times->push_back(duration);

	MexVec3 yAxis(0, 1, 0);
	return _NEW(PhysTimedAnglePlan(angles, times, yAxis, MexPoint3d()));
}

static PhysTimedAnglePlan* createOrbitTransition
(
	MexRadians startEl, 
	MexRadians endEl,
	PhysRelativeTime within
)
{
	// Set up a motion plan which repeatedly oscillates between two 
	// elevations (about the y-axis).
	PhysMotionPlan::AnglesPtr angles = _NEW(PhysMotionPlan::Angles);
    angles->reserve( 2 );
	angles->push_back(startEl);
	angles->push_back(endEl);

	// Set the times so that the angle is max at time=0 and at time=period.
	// The angle will be min at time=period/2.
	PhysMotionPlan::TimesPtr times = _NEW(PhysMotionPlan::Times);
    times->reserve( 1 );
	times->push_back(within);

	MexVec3 yAxis(0, 1, 0);
	return _NEW(PhysTimedAnglePlan(angles, times, yAxis, MexPoint3d()));
}

EnvOrbit::EnvOrbit
(
	const string& n, 
	MexRadians minEl, 
	MexRadians maxEl, 
	MATHEX_SCALAR radius,
	MexRadians setHeading,
	PhysRelativeTime period, 
	PhysRelativeTime startTime, 
	W4dEntity* parent
):
	name_(n),
	velocity_(2*Mathex::PI / period),
	minElevation_(minEl),
	maxElevation_(maxEl),
	period_(fabs(period)),
	satellite1_(_NEW(W4dGeneric(parent,      MexTransform3d()))),
	satellite2_(_NEW(W4dGeneric(satellite1_, MexTransform3d()))),
	satellite3_(_NEW(W4dGeneric(satellite2_, MexTransform3d()))),
	radius_(radius),
	firstMinTime_((startTime == 0)? 0: period - startTime)
{
	PRE(parent); 
	PRE(period != 0);
	PRE(startTime < period_);
	PRE(startTime >= 0);

	size(0);
	PhysAbsoluteTime now = W4dManager::instance().time();
	
	// Apply two adjustments to the setHeading: 1) add the angle 
	// corresponding to startTime; and 2) the initial position of the
	// satellite is radius down the -ve x-axis, we define zero degrees
	// as being along the +ve x-axis, so add another 180.
	setHeading += (velocity_ * startTime);
	setHeading += MexDegrees(180);

	// For the first satellite entity create a plan which causes it to spin around 
	// the z-axis forever.  This is the orbit around the sky, it never changes.	
	W4dEntityPlan& ePlan1 = satellite1_->entityPlanForEdit();
	ePlan1.clearMotionPlans();
	createOrbitSpin(ePlan1, velocity_, now, setHeading);

	// The satellite mesh is initially assumed to be at the origin facing
	// along the positive x-axis.  Move it backwards along the -ve x-axis.
	const MexPoint3d newPos(-radius_, 0, 0);
	satellite3_->localTransform(newPos);

	changeOrbit(minEl, maxEl, 0);

	/*
	// Create an initial rise-set plan.	
	W4dEntityPlan& ePlan2 = satellite2_->entityPlanForEdit();
	ePlan2.clearMotionPlans();
	PhysTimedAnglePlan* p2 = createRiseSetOrbit(minElevation_, maxElevation_, period_);
	ePlan2.absoluteMotion(p2, creationTime_, FOREVER);
	*/

	POST(movingEntity());
    TEST_INVARIANT;
}

EnvOrbit::~EnvOrbit()
{
    TEST_INVARIANT;
	_DELETE( satellite1_ );
	_DELETE( satellite2_ );
	_DELETE( satellite3_ );
}

// Immediately move from the current orbit to an new orbit.  The new orbit
// will alternate between the min & max elevations.  The new orbit will be
// achieved at time=(now+within).
void EnvOrbit::changeOrbit(MexRadians min, MexRadians max, PhysRelativeTime within)
{
	PRE(min <= max);
	minElevation_ = min;
	maxElevation_ = max;
	
	W4dEntityPlan& ePlan = satellite2_->entityPlanForEdit();
	ePlan.clearMotionPlans();

	// Note: this is the elevation of the *whole* orbit.  We will use this as
	// the start elevation for the satellite2_ entity, making the assumption
	// that no other entities contribute to the elevation of movingEntity().
	const MexRadians currentEl = elevation();

	// Work out the times at which everything must happen: now, the 
	// intersection and the start of the next full cycle.
	const PhysAbsoluteTime now = W4dManager::instance().time();
	const PhysAbsoluteTime intersectionTimeAbs = now + within;
	
	// Find the next whole multiple of periods *after* the new orbit is attained.
	// Then find the time at which that number of whole periods will have occured.
	const double wholePeriods = ceil((intersectionTimeAbs-firstMinTime_) / period_);
	const PhysAbsoluteTime timeOfNextMin = firstMinTime_ + period_ * wholePeriods;
	const PhysAbsoluteTime timeOfLastMin = timeOfNextMin - period_;
	
	// If the transition time is non-zero, the next minimum after the intersection 
	// *must* occur at some time in the future, not now.
	ASSERT(implies(within > 0, timeOfNextMin > now), "Time calculation error.");

	// This is the intersection time relative to the start of the last cycle.
	const PhysRelativeTime intersectionTimeRel = intersectionTimeAbs - timeOfLastMin;

	// Create the new rise-set orbit first.  
	PhysTimedAnglePlan* newOrbit = createRiseSetOrbit(minElevation_, maxElevation_, period_);

	// The transition plan must intersect the new orbit at the correct 
	// elevation at time=(now+within).
	const MexRadians intersection = newOrbit->angle(intersectionTimeRel);
	
	// We only need a tranistion plan if the transition time is non-zero.
	// Otherwise, we can just snap to the new position.
	if (within > 0)
	{
		// Execute the transition plan once only.
		PhysTimedAnglePlan* transition = createOrbitTransition(currentEl, intersection, within);
		ePlan.absoluteMotion(transition, now);
	}
	
	// Me may need to execute a partial segment of the new orbit if the
	// intersection occurs at any point other than the minimum.
	if (intersectionTimeAbs != timeOfNextMin && intersectionTimeAbs != timeOfLastMin)
	{
		ASSERT(intersectionTimeRel > 0,       "Time calculation error.");
		ASSERT(intersectionTimeRel < period_, "Time calculation error.");

		// Execute this partial orbit once only.
		PhysTimedAnglePlan* p = createPartialOrbit(minElevation_, maxElevation_, intersectionTimeRel, period_);
		ePlan.absoluteMotion(p, intersectionTimeAbs);
	}
	
	// Repeat the new orbit forever.
	ePlan.absoluteMotion(newOrbit, timeOfNextMin, FOREVER);
}

void EnvOrbit::changeOrbit(MexRadians flatEl, PhysRelativeTime within)
{
	changeOrbit(flatEl, flatEl, within);
}

MexRadians EnvOrbit::elevation() const
{
	const MexTransform3d& xform = movingEntity()->globalTransform();
	const MexPoint3d pos = xform.position();
	const MATHEX_SCALAR adjacent = sqrt(pos.x() * pos.x() + pos.y() * pos.y());
	return atan2(pos.z(), adjacent);
}	

W4dGeneric* EnvOrbit::movingEntity() const
{
	return satellite3_;
}	

MexRadians EnvOrbit::minElevation() const
{
	return minElevation_;
}

MexRadians EnvOrbit::maxElevation() const
{
	return maxElevation_;
}

MexRadians EnvOrbit::velocity() const
{
	return velocity_;
}

PhysRelativeTime EnvOrbit::period() const
{
	return period_;
}

MATHEX_SCALAR EnvOrbit::radius() const
{
	return radius_;
}

const string& EnvOrbit::name() const
{
	return name_;
}

void EnvOrbit::CLASS_INVARIANT
{
    INVARIANT(this);
    INVARIANT(satellite1_);
    INVARIANT(satellite2_);
    INVARIANT(satellite3_);
}

ostream& operator <<( ostream& o, const EnvOrbit& t )
{
	MexDegrees min = t.minElevation_, max = t.maxElevation_;
    o << "Orbit " << t.name_ << ", min=" << min.asScalar() << ", max=" << max.asScalar();

    return o;
}

/* End ORBIT.CPP ****************************************************/
