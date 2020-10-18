/*
 * L I N E M O T I . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "mathex/radians.hpp"
#include "phys/rampacce.hpp"
#include "mathex/quatern.hpp"
#include "phys/linemoti.hpp"
#include "phys/internal/linemtii.hpp"

PER_DEFINE_PERSISTENT( PhysLinearMotionPlan );

//////////////////////////////////////////////////////////////////////////////////////////

PhysLinearMotionPlan::PhysLinearMotionPlan( const MexTransform3d& startPosition,
                          const MexTransform3d& endPosition,
                          const PhysRelativeTime& endTime)
:   PhysMotionPlan( endTime ),
    pImpl_( _NEW( PhysLinearMotionPlanImpl(startPosition,endPosition, endTime ) )	)
{
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysLinearMotionPlan::~PhysLinearMotionPlan()
{
    TEST_INVARIANT;
	_DELETE (pImpl_ );

}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysLinearMotionPlan::add( const MexTransform3d& endPosition, const PhysRelativeTime& endTime )
{
    //Add the entry
	pImpl_->add(endPosition, endTime);

    //Extend the duration
    duration( endTime );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysLinearMotionPlan::transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult) const
{
    CB_DEPIMPL(PhysLinearMotionPlanImpl::Entries, entries_);
    //CB_DEPIMPL(bool:1, interpolationCached_);
    CB_DEPIMPL(size_t, targetIndex_);

    PRE( pResult != NULL );
    PRE( timeOffset >= 0.0 );

    //Check for past duration
    if( timeOffset >= duration() )
    {
        //Just return the final transform
        *pResult = entries_.back()->transform;
    }
    else
    {
        //We'll need to modify some cached data
        PhysLinearMotionPlan* nonConstThis = (PhysLinearMotionPlan*)this;

        //Check for the desired time being in the most recent cache interval
        bool search;
        if( timeOffset > entries_[targetIndex_]->time ) search = true;
        else if( targetIndex_ != 0  and  timeOffset < entries_[targetIndex_ - 1]->time )
        {
            nonConstThis->pImpl_->targetIndex_ = 0;
            search = true;
        }
        else search = targetIndex_ == 0;

        //If required, find the new correct interval
        if( search )
        {
            nonConstThis->pImpl_->interpolationCached_ = false;
            size_t nEntries = entries_.size();
            while( entries_[++nonConstThis->pImpl_->targetIndex_]->time < timeOffset )
            {
                POST( targetIndex_ < nEntries );
            }
        }

        //If required, compute the cached data
        if( not pImpl_->interpolationCached_ ) nonConstThis->computeCacheData();

        //Compute the interpolated transform, or use target if at it
        if( timeOffset == entries_[targetIndex_]->time )
        {
            *pResult = entries_[targetIndex_]->transform;
        }
        else
        {
            computeInterpolatedTransform( timeOffset, pResult );
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////

void PhysLinearMotionPlan::computeCacheData()
{
    CB_DEPIMPL(size_t, targetIndex_);
    CB_DEPIMPL(MexPoint3d, translation_);
    CB_DEPIMPL(PhysLinearMotionPlanImpl::Entries, entries_);
    CB_DEPIMPL(MexVec3, rotationAxis_);
    CB_DEPIMPL(MATHEX_SCALAR, angle_);
    //CB_DEPIMPL(bool, interpolationCached_);

    PRE( targetIndex_ > 0  and  targetIndex_ < entries_.size() )

    //Get the difference transform
    MexTransform3d differenceTransform;
    entries_[targetIndex_ - 1]->transform.invert( &differenceTransform );
    differenceTransform.transform( entries_[targetIndex_]->transform );

    //Store the translation component of the difference transform
    differenceTransform.position( &translation_ );

    //Get the orientation component as an euler parameter set, and convert this
    //to rotation axis and angle.
    MexQuaternion params;
    differenceTransform.rotation( &params );
    angle_ = params.rotation( &rotationAxis_ );

    pImpl_->interpolationCached_ = true;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysLinearMotionPlan::computeInterpolatedTransform( const PhysRelativeTime& timeOffset,
                                                         MexTransform3d* pResult ) const
{
    CB_DEPIMPL(PhysLinearMotionPlanImpl::Entries, entries_);
    CB_DEPIMPL(MexPoint3d, translation_);
    CB_DEPIMPL(MexVec3, rotationAxis_);
    CB_DEPIMPL(MATHEX_SCALAR, angle_);
    //CB_DEPIMPL(bool, interpolationCached_);
    CB_DEPIMPL(size_t, targetIndex_);

    PRE( pImpl_->interpolationCached_ );

    //Compute the proportion of the distance between the current targt position
    //and the previous position.
    PhysRelativeTime beginTime = entries_[targetIndex_ - 1]->time;
    PhysRelativeTime targetTime = entries_[targetIndex_]->time;

    MATHEX_SCALAR s = (beginTime == targetTime ?
                       1.0 :
                       (timeOffset - beginTime) / (targetTime - beginTime) );

    POST( s >= 0.0  and  s <= 1.0 );

    //hence construct the difference transform a proportion s between the 2 positions
    MexQuaternion params;
    params.set( rotationAxis_, angle_ * s );

    MexPoint3d pos(
        translation_.x() * s,
        translation_.y() * s,
        translation_.z() * s );

    pResult->position( pos );
    pResult->rotation( params );

    //Combine this transform with the begin transform
    pResult->preTransform( entries_[targetIndex_ - 1]->transform );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysLinearMotionPlan::CLASS_INVARIANT
{
    CB_DEPIMPL(PhysLinearMotionPlanImpl::Entries, entries_);

    //Must have at least 2 entries
    INVARIANT( entries_.size() >= 2 );

    //End time for last transform is the duration
    INVARIANT( entries_.back()->time == duration() );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const PhysLinearMotionPlan& t )
{

    o << "PhysLinearMotionPlan " << (void*)&t << " start" << std::endl;
    o << "PhysLinearMotionPlan " << (void*)&t << " end" << std::endl;

    return o;
}

PhysLinearMotionPlan::PhysLinearMotionPlan( PerConstructor con )
: PhysMotionPlan( con )
{
}

void perWrite( PerOstream& ostr, const PhysLinearMotionPlan& plan )
{
    const PhysMotionPlan& base = plan;

    ostr << base;

    ostr << plan.pImpl_;
}

void perRead( PerIstream& istr, PhysLinearMotionPlan& plan )
{
    PhysMotionPlan& base = plan;

    istr >> base;

    istr >> plan.pImpl_;
}


//////////////////////////////////////////////////////////////////////////////////////////
/* End LINEMOTI.CPP *************************************************/
