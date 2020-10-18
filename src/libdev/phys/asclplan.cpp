/*
 * A S C L P L A N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/asclplan.hpp"
#include "phys/internal/asclplai.hpp"

PhysAcceleratedScalarPlan::PhysAcceleratedScalarPlan(const RampAccelerations& rampAccelerations, MATHEX_SCALAR scale)
:	PhysScalarPlan(0),
	pImpl_( _NEW( PhysAcceleratedScalarPlanImpl( rampAccelerations,	scale) ) )
{
	CB_DEPIMPL(Distances, distances_);

	PhysRelativeTime timeDuration = 0;

	for(size_t i=0; i<rampAccelerations.size(); ++i)
	{
		timeDuration += rampAccelerations[i].totalTime();
		MATHEX_SCALAR dis = rampAccelerations[i].totalDistance();
		distances_.push_back(dis);
	}

	duration(timeDuration);

    TEST_INVARIANT;
}

PhysAcceleratedScalarPlan::PhysAcceleratedScalarPlan( const PhysAcceleratedScalarPlan& copyMe)
:PhysScalarPlan(copyMe.duration()),
 pImpl_( _NEW( PhysAcceleratedScalarPlanImpl(*copyMe.pImpl_) ) )
{
    TEST_INVARIANT;
}

PhysAcceleratedScalarPlan::~PhysAcceleratedScalarPlan()
{
    TEST_INVARIANT;
	_DELETE( pImpl_ );
}

const PhysAcceleratedScalarPlan::RampAccelerations& PhysAcceleratedScalarPlan::rampAccelerations() const
{
	CB_DEPIMPL(RampAccelerations, rampAccelerations_);
	return rampAccelerations_;
}

const PhysAcceleratedScalarPlan::Distances& PhysAcceleratedScalarPlan::distances() const
{
	CB_DEPIMPL(Distances, distances_);
	return distances_;
}

const MATHEX_SCALAR& PhysAcceleratedScalarPlan::initialScale() const
{
	CB_DEPIMPL(MATHEX_SCALAR, initialScale_);
	return 	initialScale_;
}

void  PhysAcceleratedScalarPlan::initialScale(const MATHEX_SCALAR& scale)
{
	CB_DEPIMPL(MATHEX_SCALAR, initialScale_);
	initialScale_ = scale;
}
//virtual
MATHEX_SCALAR PhysAcceleratedScalarPlan::scalar(const PhysRelativeTime& timeOffset) const
{
	CB_DEPIMPL(RampAccelerations, rampAccelerations_);
	CB_DEPIMPL(MATHEX_SCALAR, initialScale_);
	CB_DEPIMPL(Distances, distances_);

	PhysRelativeTime t0 = 0;
	MATHEX_SCALAR scale = initialScale_;
	size_t i = 0;

	while( ( i < rampAccelerations_.size()) && (t0 < timeOffset) )
	{
		PhysRelativeTime t1 = t0 + rampAccelerations_[i].totalTime();

		if (t1 > timeOffset )
		{
			PhysRelativeTime t = timeOffset - t0; // t1 + rampAccelerations_[i].totalTime();
			scale += rampAccelerations_[i].distance(t);
		}

		else
			scale += distances_[i];

		t0 = t1;
		++i;
	}

	return scale;
}

void PhysAcceleratedScalarPlan::CLASS_INVARIANT
{
	CB_DEPIMPL(RampAccelerations, rampAccelerations_);
	CB_DEPIMPL(Distances, distances_);

    INVARIANT( this != NULL );
	INVARIANT(distances_.size() == rampAccelerations_.size());
}

ostream& operator <<( ostream& o, const PhysAcceleratedScalarPlan& t )
{

    o << "PhysAcceleratedScalarPlan " << (void*)&t << " start" << std::endl;
    o << "PhysAcceleratedScalarPlan " << (void*)&t << " end" << std::endl;

    return o;
}

/* End ASCLPLAN.CPP *************************************************/
