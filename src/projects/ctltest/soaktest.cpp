/*
 * S O A K T E S T . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdio.h>
#include <time.h>

#include "device/key.hpp"

#include "ctl/testh/soaktest.hpp"
#include "ctl/testh/random.hpp"
#include "ctl/testh/testvec.hpp"
#include "ctl/testh/clargs.hpp"

SoakTestAssert::SoakTestAssert()
: nTests_( 0 ),
  nFailures_( 0 ),
  expectedResult_( NOT_SET ),
  displayLogInfo_( false )
{
    readCount();
    updateFailureCount();
}

void SoakTestAssert::runSoakTestAssert()
{
    bool    finished = false;
    while( not finished )
    {
        ++nTests_;

        writeCount();

        std::cout << nTests_ << " tests, ";
        std::cout << nFailures_ << " failures" << "\n";

        DevKey  key;

        if( key.getKey() )
        {
            createFile( "abort.dat" );
            finished = true;
        }

        size_t  functionIndex = randomInt( testFunctions_.size() );

        expectedResult( NOT_SET );
        FILE*   fp = fopen( "testdata.dat", "w" );
        fprintf( fp, "%ld function %s ", ARGS().seed(), testFunctionName_[ functionIndex ].c_str() );
        fclose( fp );

        LOG_DATA( testFunctionName_[ functionIndex ] );

        testFunctions_[ functionIndex ]();

        if( expectedResult() == NOT_SET )
        {
            ++nFailures_;
            FILE* fp = fopen( "errors.log", "a" );
            fprintf( fp, "%ld function %s did not set the expected result flag\n", ARGS().seed(), testFunctionName_[ functionIndex ].c_str() );
            fclose( fp );
        }

        if( expectedResult() == FAIL )
        {
            ++nFailures_;
            FILE* fp = fopen( "errors.log", "a" );
            fprintf( fp, "%ld function %s succeeded when it should have failed\n", ARGS().seed(), testFunctionName_[ functionIndex ].c_str() );
            fclose( fp );
        }
    }
}

void SoakTestAssert::readCount()
{
    FILE* fp = fopen( "count.dat", "r" );
    if( fp )
    {
        fscanf( fp, "%ld %ld", &nTests_, &nFailures_ );
        fclose( fp );
    }
}

void SoakTestAssert::writeCount()
{
    FILE* fp = fopen( "count.dat", "w" );
    if( fp )
    {
        fprintf( fp, "%ld %ld", nTests_, nFailures_ );
        fclose( fp );
    }
}

void SoakTestAssert::updateFailureCount()
{
    FILE* fp = fopen( "failure.dat", "r" );
    if( fp )
    {
        ++nFailures_;
        fclose( fp );
        remove( "failure.dat" );
    }
}

void SoakTestAssert::createFile( const std::string& fileName )
{
    FILE* fp = fopen( fileName.c_str(), "w" );
    fprintf( fp, "a" );
    fclose( fp );
}

void SoakTestAssert::writeSucceedFailFiles( ExpectedResult expectedResult )
{
    if( expectedResult == SUCCEED )
    {
        remove( "fail.dat" );
        createFile( "succeed.dat" );
    }
    else if( expectedResult == FAIL )
    {
        remove( "succeed.dat" );
        createFile( "fail.dat" );
    }
    else
    {
        remove( "succeed.dat" );
        remove( "fail.dat" );
    }
}

void     SoakTestAssert::addTestFunction( const std::string& name, v_fn_v fn )
{
    testFunctions_.push_back( fn );
    testFunctionName_.push_back( name );
}

void    SoakTestAssert::expectedResult( ExpectedResult result )
{
    expectedResult_ = result;
    writeSucceedFailFiles( expectedResult_ );
}

SoakTestAssert::ExpectedResult    SoakTestAssert::expectedResult() const
{
    return expectedResult_;
}

SoakTestAssert::~SoakTestAssert()
{
    TEST_INVARIANT;

}

void SoakTestAssert::CLASS_INVARIANT
{
}

ostream& operator <<( ostream& o, const SoakTestAssert& t )
{

    o << "SoakTestAssert " << (void*)&t << " start" << std::endl;
    o << "SoakTestAssert " << (void*)&t << " end" << std::endl;

    return o;
}

/* End SOAKTEST.CPP *************************************************/
