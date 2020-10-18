/*
 * L S C L P L A I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/internal/lsclplai.hpp"

PhysLinearScalarPlanImpl::PhysLinearScalarPlanImpl(const PhysLinearScalarPlan::ScalarVec& times, const PhysLinearScalarPlan::ScalarVec& scales)
:times_(times),
scales_(scales)
{

    TEST_INVARIANT;
}

PhysLinearScalarPlanImpl::PhysLinearScalarPlanImpl(const PhysLinearScalarPlanImpl& copyMe )
:times_(copyMe.times_),
scales_(copyMe.scales_)
{

    TEST_INVARIANT;
}

PhysLinearScalarPlanImpl::~PhysLinearScalarPlanImpl()
{
    TEST_INVARIANT;

}

void PhysLinearScalarPlanImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PhysLinearScalarPlanImpl& t )
{

    o << "PhysLinearScalarPlanImpl " << (void*)&t << " start" << std::endl;
    o << "PhysLinearScalarPlanImpl " << (void*)&t << " end" << std::endl;

    return o;
}

/* End LSCLPLAI.CPP *************************************************/
