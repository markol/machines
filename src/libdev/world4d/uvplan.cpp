/*
 * U V P L A N . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "world4d/uvplan.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( W4dUVPlan );

W4dUVPlan::W4dUVPlan(const PhysRelativeTime& d, W4dLOD maxLOD):
	duration_(d),
    maxLOD_( maxLOD )
{
}

W4dUVPlan::W4dUVPlan( PerConstructor )
{
}

W4dUVPlan::~W4dUVPlan()
{
}

const PhysRelativeTime& W4dUVPlan::duration() const
{
	return duration_;
}

bool W4dUVPlan::isDone(const PhysRelativeTime& timeOffset) const
{
	return duration_ <= timeOffset;
}

ostream& operator <<( ostream& o, const W4dUVPlan& t )
{
	t.print(o);
    return o;
}

W4dLOD W4dUVPlan::maxLOD() const
{
    return maxLOD_;
}

void perWrite( PerOstream& ostr, const W4dUVPlan& plan )
{
    ostr << plan.duration_;
    ostr << plan.maxLOD_;
}

void perRead( PerIstream& istr, W4dUVPlan& plan )
{
    istr >> plan.duration_;
    istr >> plan.maxLOD_;
}

/* End UVPLAN.CPP ***************************************************/
