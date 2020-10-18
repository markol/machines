/*
 * A S C L P L A N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysAcceleratedScalarPlan is
	a derived class from PhysScalarPlan. the scalar -time  relation is defined by 
	RampAccelerations
    
*/

#ifndef _PHYS_ASCLPLAN_HPP
#define _PHYS_ASCLPLAN_HPP

#include "base/base.hpp"
#include "phys/sclplan.hpp"

class PhysRampAcceleration;
class PhysAcceleratedScalarPlanImpl;

template <class T> class ctl_vector;

class PhysAcceleratedScalarPlan : public PhysScalarPlan
// Canonical form revoked
{
public:

	typedef ctl_vector<PhysRampAcceleration> RampAccelerations;
	typedef ctl_vector<MATHEX_SCALAR> Distances;

	PhysAcceleratedScalarPlan(const RampAccelerations& rampAccelerations, MATHEX_SCALAR scale);
    PhysAcceleratedScalarPlan( const PhysAcceleratedScalarPlan& );
	 
    virtual ~PhysAcceleratedScalarPlan( void );

	const RampAccelerations& rampAccelerations() const;
	const Distances& distances() const;
	const MATHEX_SCALAR& initialScale() const;

	void  initialScale(const MATHEX_SCALAR&);

//	returns the scale value at timeOffset
	virtual MATHEX_SCALAR scalar(const PhysRelativeTime& timeOffset) const; 

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysAcceleratedScalarPlan& t );

private:
    PhysAcceleratedScalarPlan& operator =( const PhysAcceleratedScalarPlan& );
    bool operator ==( const PhysAcceleratedScalarPlan& );

	PhysAcceleratedScalarPlanImpl* pImpl_;
};


#endif

/* End ASCLPLAN.HPP *************************************************/
