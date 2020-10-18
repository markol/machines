/*
 * F I L E E N U M . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "system/fileenum.hpp"

SysFileEnumerator::SysFileEnumerator( const SysPathName& directory, const SysPathName& fileSpecification )
: SysFileFinder( directory, fileSpecification )
{
    files_.reserve( 512 );
    TEST_INVARIANT;
}

SysFileEnumerator::~SysFileEnumerator()
{
    TEST_INVARIANT;

}

void SysFileEnumerator::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

const SysFileEnumerator::FileDatas& SysFileEnumerator::files() const
{
	return files_;
}

void SysFileEnumerator::clearFiles()
{
	TEST_INVARIANT;

	files_.erase( files_.begin(), files_.end() );

	TEST_INVARIANT;

	POST( isEmpty() );
}

bool SysFileEnumerator::isEmpty() const
{
	TEST_INVARIANT;

	return (files_.empty());
}

//virtual
SysFileFinder::ResultType SysFileEnumerator::processFile( const SysFileData& fileData )
{
	TEST_INVARIANT;

	files_.push_back( fileData );

	TEST_INVARIANT;

	return SysFileFinder::CONTINUE;
}

ostream& operator <<( ostream& o, const SysFileEnumerator& t )
{

    o << "SysFileEnumerator " << (void*)&t << " start" << std::endl;
    o << "SysFileEnumerator " << (void*)&t << " end" << std::endl;

    return o;
}

/* End FILEENUM.CPP *************************************************/
