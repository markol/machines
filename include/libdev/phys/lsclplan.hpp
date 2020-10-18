/*
 * A S C L P L A N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysLinearScalarPlan is
	scalar = Linear interpolation of discrete values
    
*/

#ifndef _PHYS_LSCLPLAN_HPP
#define _PHYS_LSCLPLAN_HPP

#include "base/base.hpp"
#include "phys/sclplan.hpp"

template <class T> class ctl_vector;
class PhysLinearScalarPlanImpl;

class PhysLinearScalarPlan : public PhysScalarPlan
// Canonical form revoked
{
public:
	typedef ctl_vector<MATHEX_SCALAR> ScalarVec;
	PhysLinearScalarPlan(const ScalarVec& times, const ScalarVec& scales);
    PhysLinearScalarPlan( const PhysLinearScalarPlan& );
	 
    virtual ~PhysLinearScalarPlan( void );

	const ScalarVec& times() const;
	const ScalarVec& scales() const;

	void  times(const ScalarVec&);
	void  scales(const ScalarVec&);

//	returns the scale value at timeOffset
	virtual MATHEX_SCALAR scalar(const PhysRelativeTime& timeOffset) const; 

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysLinearScalarPlan& t );

private:
    PhysLinearScalarPlan& operator =( const PhysLinearScalarPlan& );
    bool operator ==( const PhysLinearScalarPlan& );

	PhysLinearScalarPlanImpl* pImpl_;
};


#endif

/* End ASCLPLAN.HPP *************************************************/
