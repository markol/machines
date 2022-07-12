/*
 * C L A R G S . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <ctime>
#include <cstring>

#include "ctl/testh/clargs.hpp"

// static
CommandLineArguments& CommandLineArguments::instance()
{
    static CommandLineArguments instance_;
    return instance_;
}

CommandLineArguments::CommandLineArguments()
: seed_( time( NULL ) + clock() ),
  displayLogInfo_( false )
{

    TEST_INVARIANT;
}

CommandLineArguments::~CommandLineArguments()
{
    TEST_INVARIANT;

}

void CommandLineArguments::parseArguments( int argc, char** argv )
{
    for( size_t i = 1; i < argc; ++i )
    {
        if( not strncmp( argv[ i ], "-seed", 5 ) )
        {
            seed_ = atoi( &argv[ i ][ 5 ] );
        }

        if( not strncmp( argv[ i ], "-d", 2 ) )
            displayLogInfo_ = true;
    }

    srand( seed_ );
}

size_t  CommandLineArguments::seed() const
{
    return  seed_;
}

bool    CommandLineArguments::displayLogInfo() const
{
    return  displayLogInfo_;
}

void CommandLineArguments::CLASS_INVARIANT
{
}

ostream& operator <<( ostream& o, const CommandLineArguments& t )
{

    o << "CommandLineArguments " << (void*)&t << " start" << std::endl;
    o << "CommandLineArguments " << (void*)&t << " end" << std::endl;

    return o;
}

CommandLineArguments& ARGS()
{
    return CommandLineArguments::instance();
}

/* End CLARGS.CPP ***************************************************/
