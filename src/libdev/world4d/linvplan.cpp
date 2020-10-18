/*
 * L I N V P L A N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/linvplan.hpp"

W4dLinearFloatValuePlan::W4dLinearFloatValuePlan(
    MATHEX_SCALAR initialValue,
    MATHEX_SCALAR finalValue,
    const PhysRelativeTime& time )
: W4dFloatValuePlan( time )
{
    PRE( time > 0 );

    entries_.reserve( 5 );
    entries_.push_back( Entry( initialValue, 0 ) );
    entries_.push_back( Entry( finalValue, time ) );

    TEST_INVARIANT;
}

W4dLinearFloatValuePlan::~W4dLinearFloatValuePlan()
{
    TEST_INVARIANT;

}

void W4dLinearFloatValuePlan::add( MATHEX_SCALAR value, const PhysRelativeTime& time )
{
    PRE( time > duration() );

    entries_.push_back( Entry( value, time ) );
    duration( time );
}

// virtual
MATHEX_SCALAR W4dLinearFloatValuePlan::value( const PhysRelativeTime& timeOffset ) const
{
    MATHEX_SCALAR result;

    if( timeOffset > entries_.back().time_ )
    {
        result = entries_.back().value_;
    }
    else
    {
        bool finished = false;
        Entries::const_iterator i = entries_.begin();

        while( timeOffset > (*(i + 1)).time_ )
        {
            ++i;
        }

        //  i now refers to the interval we are in

        ASSERT( i != entries_.end(), "" );
        ASSERT( i != entries_.end() - 1, "" );

        PhysRelativeTime        intervalSize = (*( i + 1 ) ).time_ - (*i).time_;
        MATHEX_SCALAR   valueRange = (*( i + 1 ) ).value_ - (*i).value_;

        PhysRelativeTime    timeIntoInterval = timeOffset - (*i).time_;

        result = ( timeIntoInterval / intervalSize ) * valueRange + (*i).value_;
    }

    return result;
}

void W4dLinearFloatValuePlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dLinearFloatValuePlan& t )
{

    o << "W4dLinearFloatValuePlan " << (void*)&t << " start" << std::endl;
    o << "W4dLinearFloatValuePlan " << (void*)&t << " end" << std::endl;

    return o;
}

/* End LINVPLAN.CPP *************************************************/
