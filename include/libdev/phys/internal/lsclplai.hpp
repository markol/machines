/*
 * L S C L P L A I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysLinearScalarPlanImpl

    A brief description of the class should go in here
*/

#ifndef _PHYS_LSCLPLAI_HPP
#define _PHYS_LSCLPLAI_HPP

#include "base/base.hpp"
#include "phys/sclplan.hpp"
#include "phys/lsclplan.hpp"

#include "ctl/vector.hpp"

class PhysLinearScalarPlanImpl
// Canonical form revoked
{
public:

private:
    PhysLinearScalarPlanImpl( const PhysLinearScalarPlan::ScalarVec& times, const PhysLinearScalarPlan::ScalarVec& scales );
    ~PhysLinearScalarPlanImpl();

    PhysLinearScalarPlanImpl( const PhysLinearScalarPlanImpl& );
    PhysLinearScalarPlanImpl& operator =( const PhysLinearScalarPlanImpl& );

     void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysLinearScalarPlanImpl& t );
	friend class PhysLinearScalarPlan;

	PhysLinearScalarPlan::ScalarVec times_;
	PhysLinearScalarPlan::ScalarVec scales_;

};


#endif

/* End LSCLPLAI.HPP *************************************************/
