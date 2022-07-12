/*
 * F I L E D A T A . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "system/filedata.hpp"

const   SysFileData::SysFileSize    SysFileData::UNKNOWN_SIZE = (size_t)-1;

SysFileData::SysFileData()
: size_ (0)
{
}

SysFileData::SysFileData(
    const SysPathName& baseDirectory,
    const SysPathName& relativePathName,
    const SysFileDate& date,
    const SysFileTime& time,
    SysFileSize size )
: baseDirectory_( baseDirectory ),
  relativePathName_( relativePathName ),
  date_( date ),
  time_( time ),
  size_( size )
{
    string pathName( baseDirectory.pathname() );
    if(pathName[pathName.length()-1] != '/')
        pathName += "/";
    pathName += relativePathName.pathname();

    pathName_ = pathName;
}

SysFileData::SysFileData(
    const SysPathName& baseDirectory,
    const SysPathName& relativePathName,
    const SysFileDate& date,
    const SysFileTime& time )
: baseDirectory_( baseDirectory ),
  relativePathName_( relativePathName ),
  date_( date ),
  time_( time ),
  size_( UNKNOWN_SIZE )
{

    string pathName( baseDirectory.pathname() );
    if(pathName[pathName.length()-1] != '/')
        pathName += "/";
    pathName += relativePathName.pathname();

    pathName_ = pathName;
}

SysFileData::SysFileData( const SysFileData& copyMe )
: pathName_( copyMe.pathName_ ),
  date_( copyMe.date_ ),
  time_( copyMe.time_ ),
  size_( copyMe.size_ )
{
}

SysFileData::SysFileData(
    const SysPathName& pathName )
: pathName_( pathName ),
  baseDirectory_( pathName.directory() ),
  relativePathName_( pathName.pathname() )
{
    ASSERT_FILE_EXISTS( pathName.c_str() );

#ifdef _WIN95APP
/* TODO windows
	WIN32_FIND_DATA data;

	HANDLE hErr = FindFirstFile( pathName.c_str(), &data );

	bool err = ( hErr == INVALID_HANDLE_VALUE );
    ASSERT( not err, "" );

	FILETIME convTime;
	SYSTEMTIME sysTime;
	FileTimeToLocalFileTime( &data.ftLastWriteTime, &convTime );
	FileTimeToSystemTime( &convTime, &sysTime );

    SysFileTime    ftime(
      	sysTime.wHour,            		//  hours
      	sysTime.wMinute,         		//  minutes
      	sysTime.wSecond );        		//  seconds

    SysFileDate    fdate(
      	sysTime.wYear,        			//  year
		sysTime.wMonth,                	//  month
      	sysTime.wDay );                	//  day of month

	// todo : fsize may overflow. This needs some investigation.
    SysFileData::SysFileSize fsize = (data.nFileSizeHigh * MAXDWORD) + data.nFileSizeLow;

    date_ = fdate;
    time_ = ftime;
    size_ = fsize;
    */
#elif defined _SDLAPP
        //TODO
#else
    #error Windows 95 only
#endif


}

SysFileData& SysFileData::operator =( const SysFileData& copyMe )
{
    pathName_ = copyMe.pathName_;
    date_ = copyMe.date_;
    time_ = copyMe.time_;
    size_ = copyMe.size_;

    return *this;
}

SysFileData::~SysFileData()
{
    TEST_INVARIANT;

}

SysFileData::MatchType    SysFileData::matches( const SysFileData& fdata ) const
{
    //  Two files match if the data that they have both been
    //  provided with matches (i.e. if one of the files does
    //  not have a time set it will match if all of the other
    //  data matches ).

    MatchType  result = EQUAL;

    if ( strcasecmp( pathName().pathname().c_str(), fdata.pathName().pathname().c_str() ) != 0 )
        result = NAME_DIFFERENT;

    if( result == EQUAL )
    {
        if( size() != UNKNOWN_SIZE && fdata.size() != UNKNOWN_SIZE && size() != fdata.size() )
        result = SIZE_DIFFERENT;
    }

    if( result == EQUAL )
    {
        if( date().dateSet() && fdata.date().dateSet() )
        {
            if( date().year() != fdata.date().year() ||
                date().month() != fdata.date().month() ||
                date().dayOfMonth() != fdata.date().dayOfMonth() )
            {
                result = DATE_DIFFERENT;
            }
        }
    }

    if( result == EQUAL )
    {
        if( time().hoursSet() && fdata.time().hoursSet() )
        {
            if( time().hours() != fdata.time().hours() )
                result = TIME_DIFFERENT;
        }
        if( time().minutesSet() && fdata.time().minutesSet() )
        {
            if( time().minutes() != fdata.time().minutes() )
                result = TIME_DIFFERENT;
        }
        if( time().secondsSet() && fdata.time().secondsSet() )
        {
#ifdef _DOS
            //  The last bit of the seconds information in DOS is not used
            if( ( time().seconds() >> 1 ) != ( fdata.time().seconds() >> 1 ) )
                result = TIME_DIFFERENT;
#else
            if( time().seconds() != fdata.time().seconds() )
                result = TIME_DIFFERENT;
#endif
        }
    }

    return result;
}

const SysPathName& 	SysFileData::pathName() const
{
	return pathName_;
}

const SysPathName& 	SysFileData::baseDirectory() const
{
	return baseDirectory_;
}

const SysPathName& 	SysFileData::relativePathName() const
{
	return relativePathName_;
}

bool SysFileData::sizeSet() const
{
    return size_ != UNKNOWN_SIZE;
}

SysFileData::SysFileSize	SysFileData::size() const
{
	return	size_;
}

void	SysFileData::size( SysFileSize newSize )
{
	size_ = newSize;
}

const SysFileTime& SysFileData::time() const
{
    return time_;
}

void SysFileData::time( const SysFileTime& newTime )
{
    time_ = newTime;
}

const SysFileDate& SysFileData::date() const
{
    return date_;
}

void SysFileData::date( const SysFileDate& newDate )
{
    date_ = newDate;
}

/* End SysFileData.CPP *************************************************/
