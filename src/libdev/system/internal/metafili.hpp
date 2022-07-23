/*
 * M E T A F I L I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    SysMetaFileImpl

    Implementation class for SysMetaFile
*/

#ifndef _SYSTEM_METAFILI_HPP
#define _SYSTEM_METAFILI_HPP

#include "base/base.hpp"

#include "system/pathname.hpp"

class SysMetaFileImpl
// Canonical form revoked
{
public:
    SysMetaFileImpl( const SysPathName& filename );
    // ASSERT_FILE_EXISTS( filename );
    ~SysMetaFileImpl();

    const SysPathName& filename() const;
    
    bool hasFile( const SysPathName& ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const SysMetaFileImpl& t );

private:
    SysMetaFileImpl( const SysMetaFileImpl& );
    SysMetaFileImpl& operator =( const SysMetaFileImpl& );
    bool operator ==( const SysMetaFileImpl& );

    const SysPathName filename_;
};


#endif

/* End METAFILI.HPP *************************************************/
