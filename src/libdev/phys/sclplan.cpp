/*
 * S C L P L A N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/sclplan.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( PhysScalarPlan );

PhysScalarPlan::PhysScalarPlan()
{
}

PhysScalarPlan::PhysScalarPlan(const PhysRelativeTime& duration)
: duration_(duration)
{

    TEST_INVARIANT;
}

PhysScalarPlan::~PhysScalarPlan()
{
    TEST_INVARIANT;

}

void PhysScalarPlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

const PhysRelativeTime& PhysScalarPlan::duration() const
{
	return duration_;
}

void PhysScalarPlan::duration(const PhysRelativeTime& time)

{
	duration_ = time;
}

ostream& operator <<( ostream& o, const PhysScalarPlan& t )
{

    o << "PhysScalarPlan " << (void*)&t << " start" << std::endl;
    o << "PhysScalarPlan " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const PhysScalarPlan& plan )
{
    ostr << plan.duration_;
}

void perRead( PerIstream& istr, PhysScalarPlan& plan )
{
    istr >> plan.duration_;
}

/* End SCLPLAN.CPP **************************************************/
