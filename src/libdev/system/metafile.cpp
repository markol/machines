/*
 * M E T A F I L E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdlib.h>

#include "system/metafile.hpp"
#include "system/internal/metafili.hpp"

SysMetaFile::SysMetaFile( const SysPathName& filename )
{
}

SysMetaFile::~SysMetaFile()
{
}

const SysPathName& SysMetaFile::filename() const
{
    ASSERT_FAIL("Meta system not implemented.");
    return "";
}

bool SysMetaFile::hasFile( const SysPathName& filename ) const
{
    return false;
}

// static
void SysMetaFile::encryptionType( EncryptionType encryptionType )
{
}

// static
bool SysMetaFile::useMetaFile()
{
    //static bool useMetaFile_ = not ( getenv( "CB_READ_FROM_METAFILE" ) and strcmp( getenv( "CB_READ_FROM_METAFILE" ), "false" ) == 0 );

    //return useMetaFile_;
    return false;
}

void SysMetaFile::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const SysMetaFile& t )
{

    o << "SysMetaFile " << (void*)&t << " start" << std::endl;
    o << "SysMetaFile " << (void*)&t << " end" << std::endl;

    return o;
}

/* End METAFILE.CPP *************************************************/
