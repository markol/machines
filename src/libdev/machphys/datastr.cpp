/*
 * D A T A S T R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/private/datastr.hpp"

#include "stdlib/string.hpp"
#include "utility/linetok.hpp"
#include "system/pathname.hpp"

#include "ctl/vector.hpp"

//static
ctl_vector< double > MachPhysDataStream::data( const SysPathName& fileName)
{

	ctl_vector< MATHEX_SCALAR > theData;
	theData.reserve( 450 );

    ASSERT_INFO( fileName.pathname() );
    ASSERT( fileName.existsAsFile(), " named file does not exist" );

    UtlLineTokeniser   parser( fileName );

    ASSERT( not parser.finished(), "named file empty" );


    while( not parser.finished() )
    {

	    theData.push_back( atof( parser.tokens()[ 0 ].c_str() ) );

	    parser.parseNextLine();
	}

	return theData;
}

MachPhysDataStream::~MachPhysDataStream()
{
    TEST_INVARIANT;

}

void MachPhysDataStream::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysDataStream& t )
{

    o << "MachPhysDataStream " << (void*)&t << " start" << std::endl;
    o << "MachPhysDataStream " << (void*)&t << " end" << std::endl;

    return o;
}

/* End DATASTR.CPP **************************************************/
