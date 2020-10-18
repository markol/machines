/*
 * F I L E P R O C . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    SysFileFinder

	Abstract class.
	Provides a way of finding all files matching a certain specification (e.g. "*.cpp")
	starting from a specified directory. As each file is "found" the function "processFile"
	is called in the derived class. This gives the ability to stop the process at any time
	by returning ABORT from the "processFile" function.
*/

#ifndef _SYS_FILEFIND_HPP
#define _SYS_FILEFIND_HPP

//#include <dos.h>
#include <ctime>
#include <string>

#include "system/pathname.hpp"
#include "system/filedata.hpp"

class SysFileFinder
{
public:
	SysFileFinder( const SysPathName& directory, const SysPathName& fileSpecification );
	// POST( includedInSearch() == FILES );

	virtual ~SysFileFinder();

	// Set a new fileSpecification
	void	fileSpecification( const SysPathName& fileSpec );
	//	PRE( isValid( fileSpec ));

	// Set a new starting directory
	void	directory( const SysPathName& dir );
	//	PRE( dir.existsAsDirectory() );

	void	examineSubdirectories( bool examine );

	enum SearchFor { FILES, DIRS, FILES_DIRS };
	// Specifies if Files and/or Directories should be included in the search
	void	includeInSearch( SearchFor );

	SearchFor includedInSearch( void ) const;

	// Start the finding of files matching file specification. This function calls
	// processFile every time a matching file is found.
    void    find( void );

	void CLASS_INVARIANT;

protected:
    enum ResultType { ABORT, CONTINUE };

	virtual ResultType  processFile( const SysFileData& fileData ) = 0;

private:

	//	operation intentionally revoked
	SysFileFinder( const SysFileFinder& );
    const SysFileFinder& operator =( const SysFileFinder& );
    bool operator ==( const SysFileFinder& ) const;

	int	processFiles( const SysPathName& directoryName );

	//Data...
	SysPathName	currentDirectory_;
	SysPathName	fileSpecification_;
	bool		examineSubdirectories_;
	SearchFor	includeInSearch_;
};

#endif
