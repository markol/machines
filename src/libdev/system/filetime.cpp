/*
 * F I L E T I M E . C P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "system/filetime.hpp"

SysFileTime::SysFileTime()
: hoursSet_( false ),
  minutesSet_( false ),
  secondsSet_( false )
{
    POST( !hoursSet() );
    POST( !minutesSet() );
    POST( !secondsSet() );
}

SysFileTime::SysFileTime( size_t hours, size_t minutes, size_t seconds )
: hours_( hours ),
  minutes_( minutes ),
  seconds_( seconds ),
  hoursSet_( true ),
  minutesSet_( true ),
  secondsSet_( true )
{
    POST( hoursSet() );
    POST( minutesSet() );
    POST( secondsSet() );
}

SysFileTime::SysFileTime( size_t hours, size_t minutes )
: hours_( hours ),
  minutes_( minutes ),
  hoursSet_( true ),
  minutesSet_( true ),
  secondsSet_( false )
{
    POST( hoursSet() );
    POST( minutesSet() );
    POST( !secondsSet() );
}

SysFileTime::SysFileTime( const SysFileTime& copyMe )
: hours_( copyMe.hours_ ),
  minutes_( copyMe.minutes_ ),
  seconds_( copyMe.seconds_ ),
  hoursSet_( copyMe.hoursSet_ ),
  minutesSet_( copyMe.minutesSet_ ),
  secondsSet_( copyMe.secondsSet_ )
{
    // POST( *this == copyMe );
}

SysFileTime& SysFileTime::operator =( const SysFileTime& copyMe )
{
    hours_ = copyMe.hours_;
    minutes_ = copyMe.minutes_;
    seconds_ = copyMe.seconds_;
    hoursSet_ = copyMe.hoursSet_;
    minutesSet_ = copyMe.minutesSet_;
    secondsSet_ = copyMe.secondsSet_;
    
    return *this;
}

SysFileTime::~SysFileTime()
{
    TEST_INVARIANT;

}

bool    SysFileTime::timeSet() const
{
    return hoursSet() || minutesSet() || secondsSet();
}

bool    SysFileTime::hoursSet() const
{
    return hoursSet_;
}

bool    SysFileTime::minutesSet() const
{
    return minutesSet_;
}

bool    SysFileTime::secondsSet() const
{
    return secondsSet_;
}

size_t  SysFileTime::hours() const
{
    PRE( hoursSet() );
    
    return hours_;
}

size_t  SysFileTime::minutes() const
{
    PRE( minutesSet() );
    
    return minutes_;
}

size_t  SysFileTime::seconds() const
{
    PRE( secondsSet() );
    
    return seconds_;
}

ostream& operator <<( ostream& ostr, const SysFileTime& time )
{
    if( time.hoursSet() )
    {
        ostr.width( 2 );
        ostr.fill( '0' );
        ostr << time.hours();
    }
    else
        ostr << "??";

    ostr << ":";
            
    if( time.minutesSet() )
    {
        ostr.width( 2 );
        ostr.fill( '0' );
        ostr << time.minutes();
    }
    else
        ostr << "??";
        
    ostr << ":";

    if( time.secondsSet() )
    {
        ostr.width( 2 );
        ostr.fill( '0' );
        ostr << time.seconds();
    }
    else
        ostr << "??";
        
    return ostr;
}

/* End SysFileTime.CPP *************************************************/
