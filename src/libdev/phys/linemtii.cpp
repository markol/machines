/*
 * L I N E M T I I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/radians.hpp"
#include "phys/rampacce.hpp"
#include "phys/internal/linemtii.hpp"

PER_DEFINE_PERSISTENT( PhysLinearMotionPlanImpl );

PhysLinearMotionPlanImpl::PhysLinearMotionPlanImpl( const MexTransform3d& startPosition,
                          const MexTransform3d& endPosition,
                          const PhysRelativeTime& endTime )
:   targetIndex_( 0 ),
    interpolationCached_( false )
{
    PRE( endTime >= 0 );

    //Construct Entry structures for the start and end positions
    entries_.reserve( 2 );
    entries_.push_back( _NEW( Entry( startPosition, (PhysRelativeTime)0 ) ) );
    entries_.push_back( _NEW( Entry( endPosition, endTime ) ) );

    TEST_INVARIANT;
}

PhysLinearMotionPlanImpl::~PhysLinearMotionPlanImpl()
{
    TEST_INVARIANT;

    //Delete every entry
    for( size_t i = entries_.size(); i--; )
    {
        _DELETE( entries_[i] );
    }
}

void PhysLinearMotionPlanImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void PhysLinearMotionPlanImpl::add( const MexTransform3d& endPosition, const PhysRelativeTime& endTime )
{
    //Check the new end time isn't earlier than the latest time we have
    PRE( entries_.back()->time <= endTime );

    //Add the entry
    entries_.push_back( _NEW( Entry( endPosition, endTime ) ) );

}

ostream& operator <<( ostream& o, const PhysLinearMotionPlanImpl& t )
{

    o << "PhysLinearMotionPlanImpl " << (void*)&t << " start" << std::endl;
    o << "PhysLinearMotionPlanImpl " << (void*)&t << " end" << std::endl;

    return o;
}

PhysLinearMotionPlanImpl::PhysLinearMotionPlanImpl( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const PhysLinearMotionPlanImpl& plan )
{
    ostr << plan.entries_;
    ostr << plan.targetIndex_;
    ostr << plan.translation_;
    ostr << plan.rotationAxis_;
    ostr << plan.angle_;

    bool temp = plan.interpolationCached_;
    PER_WRITE_RAW_OBJECT( ostr, temp );
}

void perRead( PerIstream& istr, PhysLinearMotionPlanImpl& plan )
{
    istr >> plan.entries_;
    istr >> plan.targetIndex_;
    istr >> plan.translation_;
    istr >> plan.rotationAxis_;
    istr >> plan.angle_;

    bool temp;
    PER_READ_RAW_OBJECT( istr, temp );
    plan.interpolationCached_ = temp;
}

PhysLinearMotionPlanImpl::Entry::Entry( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const PhysLinearMotionPlanImpl::Entry& entry )
{
    ostr << entry.time;
    ostr << entry.transform;
}

void perRead( PerIstream& istr, PhysLinearMotionPlanImpl::Entry& entry )
{
    istr >> entry.time;
    istr >> entry.transform;
}

/* End LINEMTII.CPP *************************************************/
