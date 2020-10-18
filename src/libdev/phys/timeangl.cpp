/*
 * T I M E A N G L . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "mathex/transf3d.hpp"
#include "mathex/radians.hpp"
#include "phys/rampacce.hpp"

#include "phys/timeangl.hpp"

#include "phys/internal/timeangi.hpp"

#include "mathex/quatern.hpp"

PER_DEFINE_PERSISTENT( PhysTimedAnglePlan );

//////////////////////////////////////////////////////////////////////////////////////////

PhysTimedAnglePlan::PhysTimedAnglePlan( const PhysMotionPlan::AnglesPtr& anglesPtr,
                                        const PhysMotionPlan::TimesPtr& timesPtr,
                                        const MexVec3& axis, const MexVec3& position )
:   PhysMotionPlan( 0 ),
    pImpl_( _NEW( PhysTimedAnglePlanImpl( anglesPtr, timesPtr, axis, position ) ) )
{
    PRE( axis.modulus() > 0.0 )

    //Store the collections as local references
    PhysMotionPlan::Angles& angles = *anglesPtr;
    PhysMotionPlan::Times& times = *timesPtr;
    PRE( angles.size() > 1 )
    PRE( times.size() == angles.size() - 1 )

    //Check the times are in ascending order
    PRE( inAscendingOrder( times ) )

    //Set the actual duration
    duration( times.back() );

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysTimedAnglePlan::~PhysTimedAnglePlan()
{
    TEST_INVARIANT;
	_DELETE( pImpl_ );
}
//////////////////////////////////////////////////////////////////////////////////////////
MexRadians PhysTimedAnglePlan::angle(const PhysRelativeTime& timeOffset) const
{
    TEST_INVARIANT;

    CB_DEPIMPL(TimesPtr, timesPtr_);
    CB_DEPIMPL(AnglesPtr, anglesPtr_);
    CB_DEPIMPL(size_t, cacheIndex_);

    PRE_INFO( timeOffset );
    PRE( timeOffset >= 0.0 )

    //Check for past end of plan
    PhysMotionPlan::Times& times = *timesPtr_;
    PhysMotionPlan::Angles& angles = *anglesPtr_;
    MexRadians angle;
    if( timeOffset >= times.back() )
		angle = angles.back();
    else
    {
        //See if the cached segment is the correct one
        bool search = true;
        PhysTimedAnglePlan* nonConstThis = (PhysTimedAnglePlan*)this;

        if( timeOffset > times[cacheIndex_] )
            ++nonConstThis->pImpl_->cacheIndex_;
        else if( cacheIndex_ != 0  and  timeOffset < times[cacheIndex_ - 1] )
            nonConstThis->pImpl_->cacheIndex_ = 0;
        else search = false;

        //Find the segment in which the time offset applies if required
        if( search )
        {
            while( times[cacheIndex_] < timeOffset )
            {
                ++nonConstThis->pImpl_->cacheIndex_;
                POST( cacheIndex_ < times.size() )
            }
        }

        //Now compute the angle given proportion of distance through the segment
        PhysRelativeTime segmentStartTime =
            (cacheIndex_ == 0 ? 0.0 : times[cacheIndex_ - 1]);

        PhysRelativeTime segmentDuration = times[cacheIndex_] - segmentStartTime;
        MATHEX_SCALAR s = ( segmentDuration > 0                               ?
                            (timeOffset - segmentStartTime) / segmentDuration :
                            1.0 );
        angle = angles[cacheIndex_] * (1.0 - s) + angles[cacheIndex_ + 1] * s;
    }

	return angle;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysTimedAnglePlan::transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult ) const
{
    CB_DEPIMPL(MexVec3, axis_);
    CB_DEPIMPL(MexPoint3d, position_);

    MexQuaternion params;
    params.set( axis_, angle(timeOffset) );

    //Hence make the transform
    pResult->position( position_ );
    pResult->rotation( params );
}

//////////////////////////////////////////////////////////////////////////////////////////

void PhysTimedAnglePlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const PhysTimedAnglePlan& t )
{

    o << "PhysTimedAnglePlan " << (void*)&t << " start" << std::endl;
    o << "PhysTimedAnglePlan " << (void*)&t << " end" << std::endl;

    return o;
}

PhysTimedAnglePlan::PhysTimedAnglePlan( PerConstructor con )
: PhysMotionPlan( con )
{
}

void perWrite( PerOstream& ostr, const PhysTimedAnglePlan& plan )
{
    const PhysMotionPlan& base = plan;

    ostr << base;

    ostr << plan.pImpl_;
}

void perRead( PerIstream& istr, PhysTimedAnglePlan& plan )
{
    PhysMotionPlan& base = plan;

    istr >> base;

    istr >> plan.pImpl_;
}

//////////////////////////////////////////////////////////////////////////////////////////

/* End TIMEANGL.CPP *************************************************/
