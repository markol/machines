/*
 * V I S P L A N . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/visplan.hpp"

PER_DEFINE_PERSISTENT( W4dVisibilityPlan );

W4dVisibilityPlan::W4dVisibilityPlan( bool visible )
{
    times_.reserve( 5 );
    visibleFlags_.reserve( 5 );
    visibleFlags_.push_back( visible );

    duration_ = 0.0;
        
    TEST_INVARIANT;
}

W4dVisibilityPlan::W4dVisibilityPlan( PerConstructor )
{
    TEST_INVARIANT;
}

W4dVisibilityPlan::~W4dVisibilityPlan()
{
    TEST_INVARIANT;

}

void W4dVisibilityPlan::add( bool visible, const PhysRelativeTime& time )
{
    visibleFlags_.push_back( visible );
    times_.push_back( time );
    
    duration_ = time;
}

bool W4dVisibilityPlan::visible( const PhysRelativeTime& time ) const
{
    bool    result = visibleFlags_.front();
    
    Times::const_iterator timeIterator = times_.begin();
    Visibles::const_iterator visibleIterator = visibleFlags_.begin();
    ++visibleIterator;

    bool    finished = false;
            
    while( ( timeIterator != times_.end() ) and ( not finished ) )
    {
        if( (*timeIterator) > time )
            finished = true;
        else
            result = *visibleIterator;
            
        if( ++timeIterator == times_.end() )
            finished = true;
            
        ++visibleIterator;
    }


    return result;
}

const PhysRelativeTime& W4dVisibilityPlan::duration( void ) const
{
    return duration_;
}

void W4dVisibilityPlan::CLASS_INVARIANT
{
    INVARIANT( times_.size() == visibleFlags_.size() - 1 );
}

void perWrite( PerOstream& ostr, const W4dVisibilityPlan& plan )
{
    ostr << plan.times_;
    ostr << plan.visibleFlags_;
    ostr << plan.duration_;
}

void perRead( PerIstream& istr, W4dVisibilityPlan& plan )
{
    istr >> plan.times_;
    istr >> plan.visibleFlags_;
    istr >> plan.duration_;
}

/* End VISPLAN.CPP **************************************************/
