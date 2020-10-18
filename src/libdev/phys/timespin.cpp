/*
 * T I M E S P I N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "mathex/radians.hpp"
#include "phys/rampacce.hpp"
#include "mathex/epsilon.hpp"
#include "mathex/quatern.hpp"
#include "mathex/point3d.hpp"
#include "phys/timespin.hpp"
#include "phys/internal/timespni.hpp"

PER_DEFINE_PERSISTENT( PhysTimedSpinPlan );

//////////////////////////////////////////////////////////////////////////////////////////

PhysTimedSpinPlan::PhysTimedSpinPlan( const MexVec3& axis, const MexPoint3d& position,
                                      const MexRadians& startAngle, const MexRadians& startSpeed,
                                      size_t likelySegmentCount )
:   PhysMotionPlan( 0 ),
    pImpl_( _NEW( PhysTimedSpinPlanImpl( axis, position, startAngle, startSpeed) ) )
{
    MISC_STREAM( "Enter PhysTimedSpinPlan::PhysTimedSpinPlan " << (void*)this << std::endl );
    MISC_INDENT( 2 );

    MISC_INSPECT( axis );
    MISC_INSPECT( position );
    MISC_INSPECT( startAngle );
    MISC_INSPECT( startSpeed );
    MISC_INSPECT( likelySegmentCount );

    CB_DEPIMPL(Times, times_);
    CB_DEPIMPL(Angles, endAngles_);
    CB_DEPIMPL(Angles, endSpeeds_);
    CB_DEPIMPL(Angles, accelerations_);

    //Reserve the appropriate amount in the vectors
    times_.reserve( likelySegmentCount );
    endAngles_.reserve( likelySegmentCount );
    endSpeeds_.reserve( likelySegmentCount );
    accelerations_.reserve( likelySegmentCount );

    //Push the
    TEST_INVARIANT;

    MISC_INDENT( -2 );
    MISC_STREAM( "Exit  PhysTimedSpinPlan::PhysTimedSpinPlan " << (void*)this << std::endl );
}

PhysTimedSpinPlan::PhysTimedSpinPlan( const MexVec3& axis, const MexTransform3d& baseTransform,
                                      const MexRadians& startAngle, const MexRadians& startSpeed,
                                      size_t likelySegmentCount )
:   PhysMotionPlan( 0 ),
    pImpl_( _NEW( PhysTimedSpinPlanImpl( axis, baseTransform, startAngle, startSpeed) ) )
{
    MISC_STREAM( "Enter PhysTimedSpinPlan::PhysTimedSpinPlan " << (void*)this << std::endl );
    MISC_INDENT( 2 );

    MISC_INSPECT( axis );
    MISC_INSPECT( baseTransform );
    MISC_INSPECT( startAngle );
    MISC_INSPECT( startSpeed );
    MISC_INSPECT( likelySegmentCount );

    CB_DEPIMPL(Times, times_);
    CB_DEPIMPL(Angles, endAngles_);
    CB_DEPIMPL(Angles, endSpeeds_);
    CB_DEPIMPL(Angles, accelerations_);

    //Reserve the appropriate amount in the vectors
    times_.reserve( likelySegmentCount );
    endAngles_.reserve( likelySegmentCount );
    endSpeeds_.reserve( likelySegmentCount );
    accelerations_.reserve( likelySegmentCount );

    //Push the
    TEST_INVARIANT;

    MISC_INDENT( -2 );
    MISC_STREAM( "Exit  PhysTimedSpinPlan::PhysTimedSpinPlan " << (void*)this << std::endl );
}

PhysTimedSpinPlan::PhysTimedSpinPlan( PerConstructor con )
: PhysMotionPlan( con )
{
    MISC_STREAM( "Enter PhysTimedSpinPlan::PhysTimedSpinPlan " << (void*)this << std::endl );
    MISC_STREAM( "Exit  PhysTimedSpinPlan::PhysTimedSpinPlan " << (void*)this << std::endl );
}

//////////////////////////////////////////////////////////////////////////////////////////

PhysTimedSpinPlan::~PhysTimedSpinPlan()
{

    MISC_STREAM( "Enter PhysTimedSpinPlan::~PhysTimedSpinPlan " << (void*)this << std::endl );

    TEST_INVARIANT;

	_DELETE( pImpl_ );

    MISC_STREAM( "Exit  PhysTimedSpinPlan::~PhysTimedSpinPlan " << (void*)this << std::endl );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysTimedSpinPlan::addSegment( const PhysRelativeTime& segmentDuration, const MexRadians& acceleration )
{
    CB_DEPIMPL(Times, times_);
    CB_DEPIMPL(MexRadians, startAngle_);
    CB_DEPIMPL(MexRadians, startSpeed_);
    CB_DEPIMPL(Angles, endAngles_);
    CB_DEPIMPL(Angles, endSpeeds_);
    CB_DEPIMPL(Angles, accelerations_);

PRE( segmentDuration < 1000000.0 );
PRE( acceleration.asScalar() < 1000000.0 );

    MISC_STREAM( "Enter PhysTimedSpinPlan::addSegment " << (void*)this << std::endl );
    MISC_INDENT( 2 );

    MISC_INSPECT( segmentDuration );
    MISC_INSPECT( acceleration );

    //Get the start time, speed and angle for the new segment
    PhysRelativeTime startTime;
    MexRadians startAngle, startSpeed;

    if( times_.size() == 0 )
    {
        startTime = 0;
        startAngle = startAngle_;
        startSpeed = startSpeed_;
    }
    else
    {
        startTime = times_.back();
        startAngle = endAngles_.back();
        startSpeed = endSpeeds_.back();
    }

    //Compute the end angle and speed
    MexRadians endAngle =
        MexRadians( PhysRampAcceleration::distance( startAngle.asScalar(),
                                                    startSpeed.asScalar(),
                                                    acceleration.asScalar(),
                                                    segmentDuration ) );

    MexRadians endSpeed =
        MexRadians( PhysRampAcceleration::endSpeed( startSpeed.asScalar(),
                                                    acceleration.asScalar(),
                                                    segmentDuration ) );

    if( fabs( endSpeed.asScalar() ) < MexEpsilon::instance() )
        endSpeed = MexRadians( 0 );

    //Add the values to the collections
    PhysRelativeTime newDuration = startTime + segmentDuration;
    times_.push_back( newDuration );
    accelerations_.push_back( acceleration );
    endAngles_.push_back( endAngle );
    endSpeeds_.push_back( endSpeed );

    //Update the overall duration
    duration( newDuration );

    MISC_INDENT( -2 );
    MISC_STREAM( "Exit  PhysTimedSpinPlan::addSegment " << (void*)this << std::endl );
}
//////////////////////////////////////////////////////////////////////////////////////////

MexRadians PhysTimedSpinPlan::angle( const PhysRelativeTime& timeOffset ) const
{
    CB_DEPIMPL(Angles, accelerations_);
    CB_DEPIMPL(MexRadians, startAngle_);
    CB_DEPIMPL(MexRadians, startSpeed_);
    CB_DEPIMPL(Angles, endAngles_);
    CB_DEPIMPL(Angles, endSpeeds_);
    CB_DEPIMPL(size_t, cacheIndex_);
    CB_DEPIMPL(Times, times_);

    MISC_STREAM( "Enter PhysTimedSpinPlan::angle " << (void*)this << std::endl );
    MISC_INDENT( 2 );

    MISC_INSPECT( timeOffset );

    PRE( timeOffset >= 0 )
    MexRadians timeAngle;

    //Check for past duration
    if( timeOffset >= duration() )
    {
        //Just return the final angle
        timeAngle = (endAngles_.size() == 0 ? startAngle_ : endAngles_.back() );
    }
    else
    {
        //Update the cache segment
        PhysTimedSpinPlan* nonConstThis = (PhysTimedSpinPlan*)this;
        nonConstThis->setCacheSegment( timeOffset );

        //Now we have the right segment, compute the starting conditions for it
        MexRadians startSpeed, startAngle;
        PhysRelativeTime startTime;
        if( cacheIndex_ == 0 )
        {
            startTime = 0;
            startAngle = startAngle_;
            startSpeed = startSpeed_;
        }
        else
        {
            startTime = times_[cacheIndex_ - 1];
            startAngle = endAngles_[cacheIndex_ - 1];
            startSpeed = endSpeeds_[cacheIndex_ - 1];
        }


        //Hence compute the angle
        timeAngle =
            MexRadians( PhysRampAcceleration::distance( startAngle.asScalar(),
                                                        startSpeed.asScalar(),
                                                        accelerations_[cacheIndex_].asScalar(),
                                                        timeOffset - startTime ) );
    }

    MISC_INSPECT( timeAngle );
    MISC_INDENT( -2 );
    MISC_STREAM( "Exit  PhysTimedSpinPlan::angle " << (void*)this << std::endl );

    return timeAngle;
}
//////////////////////////////////////////////////////////////////////////////////////////

MexRadians PhysTimedSpinPlan::speed( const PhysRelativeTime& timeOffset ) const
{
    MISC_STREAM( "Enter PhysTimedSpinPlan::speed " << (void*)this << std::endl );
    MISC_INDENT( 2 );

    MISC_INSPECT( timeOffset );

    CB_DEPIMPL(MexRadians, startSpeed_);
    CB_DEPIMPL(Angles, accelerations_);
    CB_DEPIMPL(Angles, endSpeeds_);
    CB_DEPIMPL(size_t, cacheIndex_);
    CB_DEPIMPL(Times, times_);

    PRE( timeOffset >= 0 )
    MexRadians timeSpeed;

    //Check for past duration
    if( timeOffset >= duration() )
    {
        //Just return the final angle
        timeSpeed = (endSpeeds_.size() == 0 ? startSpeed_ : endSpeeds_.back() );
    }
    else
    {
        //Update the cache segment
        PhysTimedSpinPlan* nonConstThis = (PhysTimedSpinPlan*)this;
        nonConstThis->setCacheSegment( timeOffset );

        //Now we have the right segment, compute the starting conditions for it
        MexRadians startSpeed;
        PhysRelativeTime startTime;
        if( cacheIndex_ == 0 )
        {
            startTime = 0;
            startSpeed = startSpeed_;
        }
        else
        {
            startTime = times_[cacheIndex_ - 1];
            startSpeed = endSpeeds_[cacheIndex_ - 1];
        }

        //Hence compute the speed
        timeSpeed =
            MexRadians( PhysRampAcceleration::endSpeed( startSpeed.asScalar(),
                                                        accelerations_[cacheIndex_].asScalar(),
                                                        timeOffset - startTime ) );
    }

    MISC_INDENT( -2 );
    MISC_STREAM( "Exit  PhysTimedSpinPlan::speed " << (void*)this << std::endl );

    return timeSpeed;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysTimedSpinPlan::transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult) const
{
    MISC_STREAM( "Enter PhysTimedSpinPlan::transform " << (void*)this << std::endl );
    MISC_INDENT( 2 );

    MISC_INSPECT( timeOffset );

    CB_DEPIMPL(MexVec3, axis_);
    CB_DEPIMPL(MexTransform3d, baseTransform_);


    PRE( timeOffset >= 0 )

    //Get the angle at this time
    MexRadians timeAngle = angle( timeOffset );

    //Get the transform
    MexQuaternion params;
    params.set( axis_, timeAngle );

	MexTransform3d XformRotate(params);
	baseTransform_.transform(XformRotate, pResult);

    MISC_INSPECT( *pResult );
    MISC_INDENT( -2 );
    MISC_STREAM( "Enter PhysTimedSpinPlan::transform " << (void*)this << std::endl );
}

//////////////////////////////////////////////////////////////////////////////////////////

void PhysTimedSpinPlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const PhysTimedSpinPlan& t )
{

    o << "PhysTimedSpinPlan " << (void*)&t << " start" << std::endl;
    o << "PhysTimedSpinPlan " << (void*)&t << " end" << std::endl;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysTimedSpinPlan::setCacheSegment( const PhysRelativeTime& timeOffset )
{
    CB_DEPIMPL(Times, times_);
    CB_DEPIMPL(size_t, cacheIndex_);

    MISC_STREAM( "Enter PhysTimedSpinPlan::setCacheSegment " << (void*)this << std::endl );
    MISC_INDENT( 2 );

    MISC_INSPECT( timeOffset );
    MISC_INSPECT( cacheIndex_ );

     //Check for the desired time being in the most recent cache interval
    bool search = true;
    if( times_[cacheIndex_] < timeOffset )
        ++cacheIndex_;
    else if( cacheIndex_ != 0 and timeOffset < times_[cacheIndex_ - 1] )
        cacheIndex_ = 0;
    else search = false;

    MISC_INSPECT( search );

    //If no hit, search for the correct interval
    if( search )
    {
        size_t nSegments = times_.size();

        MISC_INSPECT( nSegments );
        MISC_INSPECT( times_[ cacheIndex_ ] );

        while( times_[cacheIndex_] < timeOffset )
        {
            MISC_INSPECT( cacheIndex_ );
            MISC_INSPECT( times_[ cacheIndex_ ] );

            ++cacheIndex_;
            ASSERT( cacheIndex_ < nSegments, "" );
        }
    }

    MISC_INDENT( -2 );
    MISC_STREAM( "Exit  PhysTimedSpinPlan::setCacheSegment " << (void*)this << std::endl );
}

void perWrite( PerOstream& ostr, const PhysTimedSpinPlan& plan )
{
    const PhysMotionPlan& base = plan;

    ostr << base;

    ostr << plan.pImpl_;
}

void perRead( PerIstream& istr, PhysTimedSpinPlan& plan )
{
    PhysMotionPlan& base = plan;

    istr >> base;

    istr >> plan.pImpl_;
}

//////////////////////////////////////////////////////////////////////////////////////////
/* End TIMESPIN.CPP *************************************************/
