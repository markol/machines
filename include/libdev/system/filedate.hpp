/*
 * F I L E D A T E . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */


/*
    SysFileDate

    Stores file date information.
*/

#ifndef _SYS_FILEDATE_HPP
#define _SYS_FILEDATE_HPP

#include "base/base.hpp"

class SysFileDate
{
public:
    SysFileDate( void );
    //  POST( !dateSet() );

    SysFileDate( size_t year, size_t month, size_t dayOfMonth );
    //  POST( dateSet() );

    SysFileDate( const SysFileDate& copyMe );
    //  POST( dateSet() );

    SysFileDate& operator =( const SysFileDate& );

    ~SysFileDate( void );

    bool    dateSet( void ) const;

    bool    yearSet( void ) const;
    size_t  year( void ) const;
    //  PRE( yearSet() );

    bool    monthSet( void ) const;
    size_t  month( void ) const;
    //  PRE( monthSet() );
    //  POST( result >= 1 && result <= 12 )

    bool    dayOfMonthSet( void ) const;
    size_t  dayOfMonth( void ) const;
    //  PRE( dayOfMonthSet() );
    //  POST( result >= 1 && result <= 31 )

    void CLASS_INVARIANT
    {
		INVARIANT( this != NULL );
    }

private:
    // Operation deliberately revoked
    bool operator ==( const SysFileDate& );

    size_t  year_;
    size_t  month_;
    size_t  dayOfMonth_;

    bool    yearSet_;
    bool    monthSet_;
    bool    dayOfMonthSet_;
    bool    dateSet_;
};

ostream& operator <<( ostream&, const SysFileDate& );

#endif

/* End SysFileDate.HPP *************************************************/
