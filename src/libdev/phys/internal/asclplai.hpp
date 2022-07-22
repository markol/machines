/*
 * A S C L P L A I . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysAcceleratedScalarPlanImpl

    A brief description of the class should go in here
*/

#ifndef _PHYS_ASCLPLAI_HPP
#define _PHYS_ASCLPLAI_HPP

#include "base/base.hpp"

#include "phys/rampacce.hpp"
#include "phys/asclplan.hpp"

#include "ctl/vector.hpp"

class PhysAcceleratedScalarPlanImpl
// Canonical form revoked
{
private:

	PhysAcceleratedScalarPlanImpl(const PhysAcceleratedScalarPlan::RampAccelerations& rampAccelerations, MATHEX_SCALAR scale);

    ~PhysAcceleratedScalarPlanImpl();

    PhysAcceleratedScalarPlanImpl( const PhysAcceleratedScalarPlanImpl& );
    PhysAcceleratedScalarPlanImpl& operator =( const PhysAcceleratedScalarPlanImpl& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysAcceleratedScalarPlanImpl& t );
	friend class PhysAcceleratedScalarPlan;

	PhysAcceleratedScalarPlan::RampAccelerations rampAccelerations_;
	PhysAcceleratedScalarPlan::Distances distances_;
	MATHEX_SCALAR initialScale_;
};


#endif

/* End ASCLPLAI.HPP *************************************************/
