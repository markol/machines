/*
 * F I L E E N U M . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    SysFileEnumerator

	Provides a way of finding all files matching a certain specification (e.g. "*.cpp")
	starting from a specified directory. The result of the call to "find" is accessible
	through the "files" member function. 

    Note : inherits "find" function from SysFileFinder. Calls to "find" will
		   NOT clear the previous list of files.
*/

#ifndef _SYS_FILEENUM_HPP
#define _SYS_FILEENUM_HPP

#include "base/base.hpp"
#include "system/filefind.hpp"

#include "ctl/vector.hpp"

class SysFileEnumerator	: public SysFileFinder
// canonical form revoked
{
public:
    SysFileEnumerator(  const SysPathName& directory, const SysPathName& fileSpecification );
	// POST( isEmpty() );

    ~SysFileEnumerator();

	typedef ctl_vector< SysFileData >   FileDatas;

	const FileDatas& files() const;

	void clearFiles();
	// POST( isEmpty() );

	bool isEmpty() const;

    void CLASS_INVARIANT;

protected:
	virtual ResultType  processFile( const SysFileData& fileData );

private:
    // Operation deliberately revoked
    SysFileEnumerator( const SysFileEnumerator& );
    SysFileEnumerator& operator =( const SysFileEnumerator& );
    bool operator ==( const SysFileEnumerator& );

    friend ostream& operator <<( ostream& o, const SysFileEnumerator& t );

	// Data...
	FileDatas   files_;
};


#endif

/* End FILEENUM.HPP *************************************************/
