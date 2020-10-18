/*
    Test out the base library
*/

#include <stdlib.h>
#include <iostream.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <math>

#include <string>

#include "base/base.hpp"
#include "base/hungaria.hpp"
#include "base/logbuff.hpp"
#include "base/diag.hpp"
#include "base/tracker.hpp"
#include "base/private/dualstr.hpp"

#include "profiler/profiler.hpp"

#include "testh/t1.hpp"

#include "base/private/indent.hpp"

void    testProfiler();
void    testStream();
void testAssert();

void t1()
{
    cout << "System crashing 1" << endl;
}

void t2()
{
    cout << "System crashing 2" << endl;
}

int main( int, char ** )

{
//     Diag::instance().addCrashFunction( t1 );
//     Diag::instance().addCrashFunction( t2 );
// 
//     testAssert();

    testStream();

//    testProfiler();

    return  0;
}


void    testProfiler()
{
    for( size_t i = 0; i < 100; ++i )
        ProProfiler::instance().outputStream() << i << " ";
        
    int* pI = (int*)0;
    *pI = 7;
}

void    testStream()
{
    BOB_STREAM( "Data sent to bob stream" << endl );
    HAL_STREAM( "Data sent to hal stream" << endl );

    ofstream str1( "test1" );
    ofstream str2( "test2" );
    
    BaseDualOstream  str( str1, str2 );
    
    str << "A test string" << endl;

//    BOB_DESTINATION( "bob2.log" );
    
    BOB_STREAM( "Some text sent to the new destination" << endl );
    
    BOB_STREAM( "Should go to Bob stream 1" << endl );

    cout << "Bob stream is " << ( BOB_STREAM_ENABLED ? "enabled" : "disabled" ) << endl;
        
    BOB_STREAM_DISABLE;

    cout << "Bob stream is " << ( BOB_STREAM_ENABLED ? "enabled" : "disabled" ) << endl;

    BOB_STREAM( "Should not go to Bob stream 1" << endl );

    cout << "Bob stream is " << ( BOB_STREAM_ENABLED ? "enabled" : "disabled" ) << endl;

    BOB_STREAM_ENABLE;

    cout << "Bob stream is " << ( BOB_STREAM_ENABLED ? "enabled" : "disabled" ) << endl;

    BOB_STREAM( "Should go to Bob stream 2" << endl );
}

void testAssert()
{
    const char* str= "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567abcdefghijklmnopqrst";
    const char* str1= "abcdefghij";

    double d = 4.0 * atan( 1.0 );

    cout << d << endl;
        
//    cout.setf( ios::fixed, ios::floatfield );
//    cout.width( 0 );
    cout.precision( 18 );

    cout << d << endl;

    double d1 = d;
    
    for( size_t i = 0; i < 35; ++i )
    {
        string  s;
        size_t n = rand() % 600;
        
        for( size_t i = 0; i < n; ++i )
            s += rand() % 26 + 'A';
                   
        ASSERT_INFO( s );
    }
    
    for( size_t i = 0; i < 10; ++i )
    {
//    cout.setf( ios::fi, ios::floatfield );
//    cout.width( 0 );
//    cout.precision( 20 );


        d /= 4.0;
        d1 *= 4.0;

        ASSERT_INFO( d );        
        ASSERT_INFO( d1 );

        cout << d << "  " << d1 << endl;
    }    
    
    ASSERT_INFO( str1 );
    ASSERT_FAIL( "abcd" );
}