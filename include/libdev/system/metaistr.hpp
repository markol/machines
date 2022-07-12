/*
 * M E T A I S T R . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    SysMetaFileIstream

    This class allows you to read in a file contained
    in a meta file. It is derived from istream. Some
    istream functionality is not completely implemented:

      Seekg can only be called with an offest of 0, although
      the seek direction can be any of ios::beg, ios::cur or ios::end

      You should not attempt to read in a binary file as a text file
*/

#ifndef _SYSTEM_METAISTR_HPP
#define _SYSTEM_METAISTR_HPP

#include "base/base.hpp"

#include <iostream>

class SysMetaFile;
class SysPathName;
class SysMetaFileIstreamImpl;

class SysMetaFileIstream : public std::istream
// Canonical form revoked
{
public:
    SysMetaFileIstream(
        const SysMetaFile& metaFile,
        const SysPathName& fileName,
        std::ios::open_mode );
    // PRE( metaFile.hasFile( filename ) );

    ~SysMetaFileIstream();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const SysMetaFileIstream& t );

    SysMetaFileIstream( const SysMetaFileIstream& );
    SysMetaFileIstream& operator =( const SysMetaFileIstream& );

    SysMetaFileIstreamImpl* pImpl_;
};


#endif

/* End METAISTR.HPP *************************************************/
