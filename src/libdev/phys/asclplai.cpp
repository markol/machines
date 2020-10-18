/*
 * A S C L P L A I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/internal/asclplai.hpp"

PhysAcceleratedScalarPlanImpl::PhysAcceleratedScalarPlanImpl(const PhysAcceleratedScalarPlan::RampAccelerations& rampAccelerations, MATHEX_SCALAR scale)
:	rampAccelerations_(rampAccelerations),
	initialScale_(scale)
{

    TEST_INVARIANT;
}

PhysAcceleratedScalarPlanImpl::PhysAcceleratedScalarPlanImpl(const PhysAcceleratedScalarPlanImpl& copyMe )
:	rampAccelerations_(copyMe.rampAccelerations_),
	initialScale_(copyMe.initialScale_)
{

    TEST_INVARIANT;
}

PhysAcceleratedScalarPlanImpl::~PhysAcceleratedScalarPlanImpl()
{
    TEST_INVARIANT;

}

void PhysAcceleratedScalarPlanImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PhysAcceleratedScalarPlanImpl& t )
{

    o << "PhysAcceleratedScalarPlanImpl " << (void*)&t << " start" << std::endl;
    o << "PhysAcceleratedScalarPlanImpl " << (void*)&t << " end" << std::endl;

    return o;
}

/* End ASCLPLAI.CPP *************************************************/
