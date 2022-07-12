/*
 * S H R M O U S E . C P P
 * (c) Charybdis Limited, 1995. All Rights Reserved.
 */

#include "device/mouse.hpp"

static  DevSharedMouse::Position  defaultMinRange = DevSharedMouse::Position( -320, -240 );
static  DevSharedMouse::Position  defaultMaxRange = DevSharedMouse::Position( 320, 240 );

//////////////////////////////////////////////////////////////////////

DevSharedMouse::DevSharedMouse():
        minRange_( defaultMinRange ),  
        maxRange_( defaultMaxRange ),  
        lastRightButtonState_( false ),
        lastLeftButtonState_( false )
{
}

void DevSharedMouse::range( const Position& min, const Position& max )
{
    PRE( min.first <= max.first );
    PRE( min.second <= max.second );

    minRange_ = min;
    maxRange_ = max;
}

const DevSharedMouse::Position&    DevSharedMouse::minRange() const
{
    return minRange_;
}

const DevSharedMouse::Position&    DevSharedMouse::maxRange() const
{
    return maxRange_;
}

void DevSharedMouse::clipToRange(DevSharedMouse::Position& pos) const
{
    if( pos.first < minRange_.first )
        pos.first = minRange_.first;
    if( pos.first > maxRange_.first )
        pos.first = maxRange_.first;

    if( pos.second < minRange_.second )
        pos.second = minRange_.second;
    if( pos.second > maxRange_.second )
        pos.second = maxRange_.second;
}

//////////////////////////////////////////////////////////////////////

/* End SHRMOUSE.CPP ****************************************************/
