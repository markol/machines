/*
 * M E T A F I L E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    SysMetaFile

    A metafile is a single file that contains a number of
    other files. Individual files can be accessed using a
    SysMetaFileIstream.
*/

#ifndef _SYSTEM_METAFILE_HPP
#define _SYSTEM_METAFILE_HPP

#include "base/base.hpp"

class SysMetaFileImpl;
class SysPathName;

class SysMetaFile
// Canonical form revoked
{
public:
    SysMetaFile( const SysPathName& filename );
    ~SysMetaFile( void );

    const SysPathName& filename() const;
    
    bool hasFile( const SysPathName& ) const;

    enum EncryptionType
    {
        NO_ENCRYPTION, ENCRYPTION_1, ENCRYPTION_2
    };

    //  Sets the encryption to be used by all SysMetaIStreams
    static void encryptionType( EncryptionType );

    // By default, meta files should always be used. It is 
    // possible to set the environment variable CB_READ_FROM_METAFILE
    // to false which will set the output of this function to false.
    static bool useMetaFile();
 
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const SysMetaFile& t );

private:
    SysMetaFile( const SysMetaFile& );
    SysMetaFile& operator =( const SysMetaFile& );
    bool operator ==( const SysMetaFile& );

    SysMetaFileImpl* pImpl_;
};


#endif

/* End METAFILE.HPP *************************************************/

