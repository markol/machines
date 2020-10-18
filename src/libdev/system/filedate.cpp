/*
 * F I L E D A T E . C P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "system/filedate.hpp"

SysFileDate::SysFileDate()
: yearSet_( false ),
  monthSet_( false ),
  dayOfMonthSet_( false ),
  dateSet_( false )
{

    TEST_INVARIANT;
}

SysFileDate::SysFileDate( size_t year, size_t month, size_t dayOfMonth )
: year_( year ),
  month_( month ),
  dayOfMonth_( dayOfMonth ),
  yearSet_( true ),
  monthSet_( true ),
  dayOfMonthSet_( true ),
  dateSet_( true )
{
    POST( dateSet() );
}

SysFileDate::SysFileDate( const SysFileDate& copyMe )
: year_( copyMe.year_ ),
  month_( copyMe.month_ ),
  dayOfMonth_( copyMe.dayOfMonth_ ),
  yearSet_( copyMe.yearSet_ ),
  monthSet_( copyMe.monthSet_ ),
  dayOfMonthSet_( copyMe.dayOfMonthSet_ ),
  dateSet_( copyMe.dateSet_ )
{
}

SysFileDate& SysFileDate::operator =( const SysFileDate& copyMe )
{
    year_ = copyMe.year_;
    month_ = copyMe.month_;
    dayOfMonth_ = copyMe.dayOfMonth_;
    yearSet_ = copyMe.yearSet_;
    monthSet_ = copyMe.monthSet_;
    dayOfMonthSet_ = copyMe.dayOfMonthSet_;
    dateSet_ = copyMe.dateSet_;

    return *this;
}

SysFileDate::~SysFileDate()
{
    TEST_INVARIANT;

}

bool    SysFileDate::dateSet( void ) const
{
    return  dateSet_;
}
    
bool    SysFileDate::yearSet( void ) const
{
    return yearSet_;
}

size_t  SysFileDate::year( void ) const
{
    PRE( yearSet() );
    
    return year_;
}

bool    SysFileDate::monthSet( void ) const
{
    return monthSet_;
}

size_t  SysFileDate::month( void ) const
{
    PRE( monthSet() );

    size_t  result = month_;
        
    POST( result >= 1 && result <= 12 );
    
    return result;
}

bool    SysFileDate::dayOfMonthSet( void ) const
{
    return dayOfMonthSet_;
}

size_t  SysFileDate::dayOfMonth( void ) const
{
    PRE( dayOfMonthSet() );
    
    size_t  result = dayOfMonth_;
    
    POST( result >= 1 && result <= 31 );

    return result;
}

ostream& operator <<( ostream& ostr, const SysFileDate& date )
{
    if( date.dayOfMonthSet() )
    {
        ostr.width( 2 );
        ostr << date.dayOfMonth();
    }
    else
        ostr << "??";
        
    ostr << " ";

    if( date.monthSet() )
    {
        switch( date.month() )
        {
            case 1:   ostr << "Jan";   break;
            case 2:   ostr << "Feb";   break;
            case 3:   ostr << "Mar";   break;
            case 4:   ostr << "Apr";   break;
            case 5:   ostr << "May";   break;
            case 6:   ostr << "Jun";   break;
            case 7:   ostr << "Jul";   break;
            case 8:   ostr << "Aug";   break;
            case 9:   ostr << "Sep";   break;
            case 10:   ostr << "Oct";   break;
            case 11:   ostr << "Nov";   break;
            case 12:   ostr << "Dec";   break;
        }
    }
    else
        ostr << "??";
        
    ostr << " ";

    if( date.yearSet() )
    {
        ostr.width( 4 );
        ostr << date.year();
    }
    else
        ostr << "????";

    return ostr;        
}

/* End SysFileDate.CPP *************************************************/
