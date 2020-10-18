/*
 * F I L E T I M E . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */


/*
    SysFileTime

   	Stores information about a files last modification time.
*/

#ifndef _SYS_FILETIME_HPP
#define _SYS_FILETIME_HPP

#include "base/base.hpp"

class SysFileTime
{
public:
    SysFileTime( void );
    //  POST( !hoursSet() );
    //  POST( !minutesSet() );
    //  POST( !secondsSet() );

    SysFileTime( size_t hours, size_t minutes, size_t seconds );
    //  POST( hoursSet() );
    //  POST( minutesSet() );
    //  POST( secondsSet() );

    SysFileTime( size_t hours, size_t minutes );
    //  POST( hoursSet() );
    //  POST( minutesSet() );
    //  POST( !secondsSet() );

    SysFileTime( const SysFileTime& copyMe );

    SysFileTime& operator =( const SysFileTime& copyMe );

    ~SysFileTime( void );

    bool    timeSet( void ) const;
    bool    hoursSet( void ) const;
    bool    minutesSet( void ) const;
    bool    secondsSet( void ) const;

    size_t  hours( void ) const;
    //  PRE( hoursSet() );

    size_t  minutes( void ) const;
    //  PRE( minutesSet() );

    size_t  seconds( void ) const;
    //  PRE( secondsSet() );

    void CLASS_INVARIANT
    {
		INVARIANT( this != NULL );
    }

private:
    // Operation deliberately revoked
    bool operator ==( const SysFileTime& );

    size_t  hours_;
    size_t  minutes_;
    size_t  seconds_;

    bool    hoursSet_;
    bool    minutesSet_;
    bool    secondsSet_;
};

ostream& operator <<( ostream&, const SysFileTime& );

#endif

/* End SysFileTime.HPP *************************************************/
