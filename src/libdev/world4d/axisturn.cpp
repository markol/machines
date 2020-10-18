/*
 * A X I S T U R N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/axisturn.hpp"

#include "mathex/eulerang.hpp"
#include "world4d/manager.hpp"

W4dAxisTurnerPlan::W4dAxisTurnerPlan
(
    const MexTransform3d& baseTransform, W4d::Axis turningAxis, const PhysAbsoluteTime& startTime,
    MexRadians startAngle, MexRadians turnAngle, MexRadians turnRate,
    const PhysRelativeTime& duration
)
:   PhysMotionPlan( duration ),
    baseTransform_( baseTransform ),
    turningAxis_( turningAxis ),
    startTime_( startTime ),
    startAngle_( startAngle ),
    turnAngle_( turnAngle ),
    turnRate_( turnRate )
{

    TEST_INVARIANT;
}

W4dAxisTurnerPlan::~W4dAxisTurnerPlan()
{
    TEST_INVARIANT;

}

void W4dAxisTurnerPlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dAxisTurnerPlan& t )
{

    o << "PhysAxisTurner " << (void*)&t << " start" << std::endl;
    o << "PhysAxisTurner " << (void*)&t << " end" << std::endl;

    return o;
}

PhysRelativeTime W4dAxisTurnerPlan::turnBy( const PhysAbsoluteTime& startTime, MexRadians turnAngle )
{
    //Update current motion
    updateData( startTime );

    //reset the turn angle
    turnAngle_ = turnAngle;

    return turnAngle_.asScalar() / turnRate_.asScalar();
}

PhysRelativeTime W4dAxisTurnerPlan::turnTo( const PhysAbsoluteTime& startTime, MexRadians requiredAngle )
{
    //Update current motion
    updateData( startTime );

    //The new turn angle needs to be rounded to the shortest route
    turnAngle_ = requiredAngle - startAngle_;

    double twoPi = Mathex::PI * 2.0;
    double delta;

    if( turnAngle_.asScalar() > 0.0 )
        delta = -twoPi * double(long((turnAngle_.asScalar() + Mathex::PI) / twoPi));

    else
        delta = twoPi * double(long((turnAngle_.asScalar() - Mathex::PI) / (-twoPi)));

    turnAngle_ = turnAngle_ + MexRadians( delta );

	const PhysRelativeTime turnTime = fabs( turnAngle_.asScalar() / turnRate_.asScalar() );

    return turnTime;
}

void W4dAxisTurnerPlan::turnRate( const PhysAbsoluteTime& startTime, MexRadians rate )
{
    //Update current motion
    updateData( startTime );

    turnRate_ = rate;
}

MexRadians W4dAxisTurnerPlan::angle( const PhysAbsoluteTime& time ) const
{
    return startAngle_ + currentDeltaAngle( time );
}

MexRadians W4dAxisTurnerPlan::currentDeltaAngle( const PhysAbsoluteTime& time ) const
{
    MexRadians angle;

    PhysRelativeTime interval = time - startTime_;
    if( interval <= 0.0 )
        angle = 0.0;
    else if( turnAngle_.asScalar() > 0.0 )
    {
        angle = turnRate_ * interval;
        if( angle > turnAngle_ )
            angle = turnAngle_;
    }
    else
    {
        angle = -turnRate_ * interval;
        if( angle < turnAngle_ )
            angle = turnAngle_;
    }

    return angle;
}

const MexTransform3d& W4dAxisTurnerPlan::baseTransform() const
{
    return baseTransform_;
}

W4d::Axis W4dAxisTurnerPlan::turningAxis() const
{
    return turningAxis_;
}

const PhysAbsoluteTime& W4dAxisTurnerPlan::startTime() const
{
    return startTime_;
}

MexRadians W4dAxisTurnerPlan::startAngle() const
{
    return startAngle_;
}

MexRadians W4dAxisTurnerPlan::turnAngle() const
{
    return turnAngle_;
}

//virtual
void W4dAxisTurnerPlan::transform( const PhysRelativeTime&, MexTransform3d* pResult ) const
{
    //Use current time to compute angle
    MexRadians nowAngle = angle( W4dManager::instance().time() );

    //Compute an euler angle set based on the turning axis
    MexRadians angles[3];
    angles[0] = angles[1] = angles[2] = 0.0;
    angles[ turningAxis_ ] = nowAngle;

    //Hence evaluate the result
    baseTransform_.transform( MexTransform3d( MexEulerAngles( angles[2], angles[1], angles[0] ) ), pResult );
}

void W4dAxisTurnerPlan::updateData( const PhysAbsoluteTime& time )
{
    //Compute delta at this time
    MexRadians delta = currentDeltaAngle( time );
    startAngle_ += delta;
    turnAngle_ -= delta;
    startTime_ = time;
}

void W4dAxisTurnerPlan::baseTransform( const MexTransform3d& newTransform )
{
    baseTransform_ = newTransform;
}

void W4dAxisTurnerPlan::reset( const PhysAbsoluteTime& startTime, MexRadians startAngle )
{
    startTime_ = startTime;
    startAngle_ = startAngle;
    turnAngle_ = 0.0;
}
/* End AXISTURN.CPP *************************************************/
