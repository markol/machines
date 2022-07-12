/*******************************************************************
 *
 * filefind.cpp
 *
 * (c) Charybdis Limited, 1995. All Rights Reserved
 *
 *******************************************************************/

#include "system/filefind.hpp"
#include "system/filedata.hpp"
#include "system/filetime.hpp"
#include "system/filedate.hpp"

#include "ctl/vector.hpp"

#ifdef _WIN95APP
#include "windows.h"
#else
    #include <dirent.h>
#endif

SysFileFinder::SysFileFinder( const SysPathName& directory, const SysPathName& fileSpecification )
: examineSubdirectories_( false ),
  currentDirectory_( directory ),
  fileSpecification_( fileSpecification ),
  includeInSearch_(SysFileFinder::FILES)
{

	TEST_INVARIANT;

	POST( includedInSearch() == FILES );
}

SysFileFinder::~SysFileFinder()

{
	TEST_INVARIANT;

}

void SysFileFinder::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
	INVARIANT( ( includedInSearch() == FILES ) or ( includedInSearch() == DIRS ) or ( includedInSearch() == FILES_DIRS ) );
}

void	SysFileFinder::fileSpecification( const SysPathName& fileSpec )
{
	fileSpecification_ = fileSpec;
}

void	SysFileFinder::directory( const SysPathName& dir )
{
	currentDirectory_ = dir;
}

void	SysFileFinder::examineSubdirectories( bool examine )
{
	examineSubdirectories_ = examine;
}

void    SysFileFinder::find()
{
	processFiles( currentDirectory_ );
}

void	SysFileFinder::includeInSearch( SysFileFinder::SearchFor include )
{
	TEST_INVARIANT;

	includeInSearch_ = include;

	TEST_INVARIANT;
}

SysFileFinder::SearchFor SysFileFinder::includedInSearch() const
{
	TEST_INVARIANT;

	return includeInSearch_;
}

int	SysFileFinder::processFiles( const SysPathName& directoryName )
{
#if defined _DOSAPP

	find_t	data;
	int		err, abort;
	char	pathname[_MAX_PATH];

	abort = 0;

	#ifndef NDEBUG
	if ( directoryName.existsAsDirectory() )
	{
		DEBUG_STREAM( DIAG_NEIL, "SysFileFinder::processFiles " << directoryName << endl );
	}
	#endif

	//	Look for matching files/dirs in this directory

	_makepath( pathname, NULL, directoryName.pathname().c_str(), fileSpecification_.pathname().c_str(), NULL );

    // Look for files...
	if ( includedInSearch() == FILES or includedInSearch() == FILES_DIRS )
	{
		err = _dos_findfirst (pathname, _A_NORMAL | _A_HIDDEN, &data);

	   	while (!err && !abort)
		{
			SysFileTime    ftime(
	          ( data.wr_time & 0xf800 ) >> 11,                  //  hours
	          ( data.wr_time & 0x7e0 ) >> 5,                    //  minutes
	          ( data.wr_time & 0x1f ) << 1 );                   //  seconds

		    SysFileDate    fdate(
	          ( ( data.wr_date & 0xfe00 ) >> 9 ) + 1980,        //  year
	          ( ( data.wr_date & 0x1e0 ) >> 5 ),                //  month
	          ( data.wr_date & 0x1f ) );                        //  day of month

	        SysFileData::SysFileSize fsize = data.size;

			SysFileData	fd( directoryName, data.name, fdate, ftime, fsize );

	        processFile( fd );

			err = _dos_findnext (&data);
		}
	}

	// Look for directories...
	if ( includedInSearch() == DIRS or includedInSearch() == FILES_DIRS )
	{
		err = _dos_findfirst (pathname, _A_SUBDIR, &data);

	  	while (	not err and
	  			not abort and
	  			strcmp (data.name, ".") and strcmp (data.name, "..") ) // not interested in '.' and '..'
		{
	        SysFileTime    ftime(
	          ( data.wr_time & 0xf800 ) >> 11,                  //  hours
	          ( data.wr_time & 0x7e0 ) >> 5,                    //  minutes
	          ( data.wr_time & 0x1f ) << 1 );                   //  seconds

	        SysFileDate    fdate(
	          ( ( data.wr_date & 0xfe00 ) >> 9 ) + 1980,        //  year
	          ( ( data.wr_date & 0x1e0 ) >> 5 ),                //  month
	          ( data.wr_date & 0x1f ) );                        //  day of month

	        SysFileData::SysFileSize fsize = data.size;

			SysFileData	fd( directoryName, data.name, fdate, ftime, fsize );

	        processFile( fd );

			err = _dos_findnext (&data);
		}
	}

	if( examineSubdirectories_ )
	{
		//	Recursively look for files in subdirectories

		_makepath( pathname, NULL, directoryName.pathname().c_str(), "*.*", NULL );

		err = _dos_findfirst (pathname, _A_SUBDIR, &data);

		while (!err && !abort)
		{
			if (strcmp (data.name, ".") && strcmp (data.name, ".."))
			{
				_makepath( pathname, NULL, directoryName.pathname().c_str(), data.name, NULL );

				abort = processFiles( pathname );
			}

			err = _dos_findnext (&data);
		}
	}

	return(abort);

#elif defined _WIN95APP

	WIN32_FIND_DATA data;
	int		err, abort;
	char	pathname[_MAX_PATH];

	abort = 0;

	//	Look for matching files/dirs in this directory

	_makepath( pathname, NULL, directoryName.pathname().c_str(), fileSpecification_.pathname().c_str(), NULL );

	HANDLE hErr = FindFirstFile( pathname, &data );

	err = ( hErr == INVALID_HANDLE_VALUE );

	while (!err && !abort)
	{
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

		SysFileData	fd( directoryName, data.cFileName, fdate, ftime, fsize );

		DWORD fileAttributes = GetFileAttributes( fd.pathName().c_str() );

		bool dir = fileAttributes & FILE_ATTRIBUTE_DIRECTORY;
		bool incdirs = dir and ( ( includeInSearch_ == DIRS ) or ( includeInSearch_ == FILES_DIRS ) );
		bool incfile = not dir and ( ( includeInSearch_ == FILES ) or ( includeInSearch_ == FILES_DIRS ) );

		if ( ( incdirs or incfile ) and
			 strcmp (data.cFileName, ".") and // not interested in '.' or '..'
			 strcmp (data.cFileName, "..") )
		{
        	processFile( fd );
		}

		err = not FindNextFile( hErr, &data );
	}

	if( hErr != INVALID_HANDLE_VALUE )
        FindClose( hErr );

	if( examineSubdirectories_ )
	{
		//	Recursively look for files/dirs in subdirectories

		_makepath( pathname, NULL, directoryName.pathname().c_str(), "*.*", NULL );

		hErr = FindFirstFile( pathname, &data );
		err = ( hErr == INVALID_HANDLE_VALUE );

		while (!err && !abort)
		{
			SysPathName::Components comps;
            comps.reserve( 2 );

			comps.push_back( directoryName.pathname().c_str() );
			comps.push_back( data.cFileName );
			SysPathName directory;
			directory.createFromComponents( comps );

			DWORD fileAttributes = GetFileAttributes( directory.pathname().c_str() );

	  		if ( ( fileAttributes & FILE_ATTRIBUTE_DIRECTORY ) and
				 strcmp(data.cFileName, ".") and
				 strcmp(data.cFileName, "..") )
			{
				_makepath( pathname, NULL, directoryName.pathname().c_str(), data.cFileName, NULL );

				abort = processFiles( pathname );
			}

			err = not FindNextFile( hErr, &data );
		}
	}

	if( hErr != INVALID_HANDLE_VALUE )
        FindClose( hErr );

	return(abort);
#elif defined _SDLAPP
    std::string dirName(directoryName.pathname());
    if(dirName[dirName.length()-1] != '/')
        dirName += "/";

    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(dirName.c_str());

    // Unable to open directory stream
    if (!dir)
        return 0;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            // Construct new path from our base path
            std::string nextDirectoryName(dirName);
            nextDirectoryName += dp->d_name;

            if( (examineSubdirectories_ && !processFiles(nextDirectoryName)) || !examineSubdirectories_ )
            {
                std::string filename(dp->d_name);
                std::string last = filename.substr(filename.length() - 3);
                if(last == fileSpecification_.extension())
                {
                    SysFileData	fd( SysPathName(dirName), SysPathName(dp->d_name), SysFileDate(), SysFileTime(), 2 );
                    processFile(fd);
                }
            }
        }
    }

    closedir(dir);
    return 1;
#else
    #error Compiling system lib: one of _DOSAPP, _WIN95APP, etc. must be defined.
#endif

}

