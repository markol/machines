/*
 * V A L P L A N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/valplan.hpp"

W4dFloatValuePlan::W4dFloatValuePlan( const PhysRelativeTime& duration )
: duration_( duration )
{

    TEST_INVARIANT;
}

W4dFloatValuePlan::~W4dFloatValuePlan()
{
    TEST_INVARIANT;

}

const PhysRelativeTime& W4dFloatValuePlan::duration( void ) const
{
    return duration_;
}

bool W4dFloatValuePlan::isDone( const PhysRelativeTime& timeOffset ) const
{
    return timeOffset > duration_;
}

void W4dFloatValuePlan::duration( const PhysRelativeTime& newDuration)
{
    duration_ = newDuration;
}

void W4dFloatValuePlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dFloatValuePlan& t )
{

    o << "W4dFloatValuePlan " << (void*)&t << " start" << std::endl;
    o << "W4dFloatValuePlan " << (void*)&t << " end" << std::endl;

    return o;
}

/* End VALPLAN.CPP **************************************************/
