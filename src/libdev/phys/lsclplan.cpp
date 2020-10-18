/*
 * A S C L P L A N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/lsclplan.hpp"
#include "phys/internal/lsclplai.hpp"

PhysLinearScalarPlan::PhysLinearScalarPlan(const ScalarVec& times, const ScalarVec& scales)
:	PhysScalarPlan(times.back()),
	pImpl_( _NEW( PhysLinearScalarPlanImpl( times, scales) ))
{
	PRE(times.size()+1 == scales.size());
    TEST_INVARIANT;
}

PhysLinearScalarPlan::PhysLinearScalarPlan( const PhysLinearScalarPlan& copyMe)
:PhysScalarPlan(copyMe.duration()),
pImpl_( _NEW( PhysLinearScalarPlanImpl( *copyMe.pImpl_ ) ) )
{
    TEST_INVARIANT;
}

PhysLinearScalarPlan::~PhysLinearScalarPlan()
{
    TEST_INVARIANT;

    _DELETE( pImpl_ );
}

const PhysLinearScalarPlan::ScalarVec& PhysLinearScalarPlan::times() const
{
	CB_DEPIMPL(ScalarVec, times_);
	return times_;
}

const PhysLinearScalarPlan::ScalarVec& PhysLinearScalarPlan::scales() const
{
	CB_DEPIMPL(ScalarVec, scales_);
	return scales_;
}

void  PhysLinearScalarPlan::times(const ScalarVec& times)
{
	CB_DEPIMPL(ScalarVec, times_);
	times_ = times;
}

void  PhysLinearScalarPlan::scales(const ScalarVec& scales)
{
	CB_DEPIMPL(ScalarVec, scales_);
	scales_ = scales;
}

//virtual
MATHEX_SCALAR PhysLinearScalarPlan::scalar(const PhysRelativeTime& timeOffset) const
{
	CB_DEPIMPL(ScalarVec, times_);
	CB_DEPIMPL(ScalarVec, scales_);
	PRE(scales_.size() >= 1);			// implied by invariant

	// If time is past the end, return the last scale.
	if (timeOffset >= times_[times_.size()-1])
	{
		// IAIN_STREAM("scale(" << timeOffset << ")=" << scales_[scales_.size()-1] << " (past end)\n");
		return scales_[scales_.size()-1];
	}

	ASSERT(timeOffset < times_[times_.size()-1], "Time bounds error.");

	size_t i = 0;
	MATHEX_SCALAR scale = scales_[0];		// needs pre-condition
	PhysRelativeTime t1 = 0;
	bool done = false;

	// If this isn't true, return the first scale.
	if (timeOffset > 0)
	{
		while(  (i < times_.size()) and not done )
		{
			PhysRelativeTime t2 = times_[i];

			if( (t1 <= timeOffset ) && ( timeOffset <= t2) )
			{
				MATHEX_SCALAR alpha = (scales_[i+1] - scales_[i])/(t2 - t1);
				scale = scales_[i] + alpha * (timeOffset - t1);

				done = true;
			}

			t1 = t2;
			++i;
		}
	}

	// IAIN_STREAM("scale(" << timeOffset << ")=" << scale << "\n");
	return scale;
}

void PhysLinearScalarPlan::CLASS_INVARIANT
{
	CB_DEPIMPL(ScalarVec, times_);
	CB_DEPIMPL(ScalarVec, scales_);

    INVARIANT( this != NULL );
	INVARIANT(scales_.size() - times_.size() == 1);
}

ostream& operator <<( ostream& o, const PhysLinearScalarPlan& t )
{

    o << "PhysLinearScalarPlan " << (void*)&t << " start" << std::endl;
    o << "PhysLinearScalarPlan " << (void*)&t << " end" << std::endl;

    return o;
}

/* End ASCLPLAN.CPP *************************************************/
