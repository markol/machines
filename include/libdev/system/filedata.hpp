/*
 * F I L E D A T A . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */


/*
    SysFileData

    Store information about a file including pathname, date,
    time, size.
*/

#ifndef _SYS_FILEDATA_HPP
#define _SYS_FILEDATA_HPP

#include <string>

#include "base/base.hpp"

#include "system/pathname.hpp"
#include "system/filetime.hpp"
#include "system/filedate.hpp"

class SysFileData
{
public:
    typedef size_t  SysFileSize;

	SysFileData( void );

    SysFileData(
        const SysPathName& baseDirectory,
        const SysPathName& relativePathName,
        const SysFileDate& date,
        const SysFileTime& time );

    SysFileData(
        const SysPathName& baseDirectory,
        const SysPathName& relativePathName,
        const SysFileDate& date,
        const SysFileTime& time,
        SysFileSize size );

    //  This constructor will fill in the time, data and size automatically
    SysFileData( const SysPathName& pathName );
    // ASSERT_FILE_EXISTS( pathName );

    SysFileData( const SysFileData& copyMe );

    SysFileData& operator =( const SysFileData& );

    ~SysFileData( void );

    //  This is the absolute pathname (i.e. the base
    //  directory combined with the relative path name )
    const SysPathName&  pathName( void ) const;
    const SysPathName&  baseDirectory( void ) const;
    const SysPathName&  relativePathName( void ) const;

    enum MatchType { EQUAL, NAME_DIFFERENT, SIZE_DIFFERENT, DATE_DIFFERENT, TIME_DIFFERENT };

    MatchType    matches( const SysFileData& fdata ) const;
    //  Two files match if the data that they have both been
    //  provided with matches (i.e. if one of the files does
    //  not have a time set it will match if all of the other
    //  data matches ).

    bool            	sizeSet( void ) const;
    SysFileSize	    	size( void ) const;
    const SysFileTime& 	time( void ) const;
    const SysFileDate& 	date( void ) const;

    void	size( SysFileSize newSize );
    void    time( const SysFileTime& newTime );
    void    date( const SysFileDate& newDate );

    void CLASS_INVARIANT
    {
		INVARIANT( this != NULL );
    }

private:
    static  const   SysFileSize    UNKNOWN_SIZE;

    SysFileSize size_;
    SysFileDate date_;
    SysFileTime time_;
    SysPathName pathName_;
    SysPathName baseDirectory_;
    SysPathName relativePathName_;
};

bool operator ==( const SysFileData&, const SysFileData& );
bool operator <( const SysFileData&, const SysFileData& );

#endif

/* End SysFileData.HPP *************************************************/
