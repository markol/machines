/*
 * S I M S T A T S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "sim/simstats.hpp"
#include "stdlib/string.hpp"
#include "utility/linetok.hpp"

#include "system/metafile.hpp"
#include "system/metaistr.hpp"
#include <memory>

// static
SimStats& SimStats::instance()
{
    static SimStats instance_;
    return instance_;
}

SimStats::SimStats()
:	minComputationTime_( 0.010 ),
	targetRenderInterval_( 0.030 ),
	minProcessUpdateTime_( 0.001 )
{
	readInitialisationFile();
    TEST_INVARIANT;
}

SimStats::~SimStats()
{
    TEST_INVARIANT;

}

void SimStats::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const SimStats& t )
{

    o << "SimStats " << (void*)&t << " start" << std::endl;
	o << " minComputationTime " << t.minComputationTime_ << std::endl;
	o << " TargetRenderInterval " << t.targetRenderInterval_ << std::endl;
    o << "SimStats " << (void*)&t << " end" << std::endl;

    return o;
}

PhysRelativeTime SimStats::minComputationTime()
{
	return minComputationTime_;
}

PhysRelativeTime SimStats::targetRenderInterval()
{
	return targetRenderInterval_;
}

PhysRelativeTime SimStats::minProcessUpdateTime()
{
	return minProcessUpdateTime_;
}

void SimStats::readInitialisationFile()
{
	const SysPathName pathName( "data/simstats.ini" );

    SysMetaFile metaFile( "mach1.met" );

    std::auto_ptr< istream > pIstream;

    if( SysMetaFile::useMetaFile() )
    {
        //pIstream = _NEW( SysMetaFileIstream( metaFile, pathName, ios::text ) );
        pIstream = std::auto_ptr< istream > (
            _NEW( SysMetaFileIstream( metaFile, pathName, std::ios::in ) ));
    }
    else
    {
        ASSERT_FILE_EXISTS( pathName.c_str() );
        //pIstream = _NEW( ifstream( pathName.c_str(), ios::text | ios::in ) );
        pIstream = std::auto_ptr< istream > (
             _NEW( std::ifstream( pathName.c_str(), std::ios::in ) ) );
    }

	UtlLineTokeniser parser( *pIstream, pathName );

	while( not parser.finished() )
	{
		if( parser.tokens()[0] == "minComputationTime" )
			minComputationTime_ = atof( parser.tokens()[1].c_str() );
		if( parser.tokens()[0] == "targetRenderInterval" )
			targetRenderInterval_ = atof( parser.tokens()[1].c_str() );
		if( parser.tokens()[0] == "minProcessUpdateTime" )
			minProcessUpdateTime_ = atof( parser.tokens()[1].c_str() );
		parser.parseNextLine();
	}
}
/* End SIMSTATS.CPP *************************************************/
