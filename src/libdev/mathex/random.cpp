/*
 * R A N D O M . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

//#include <stdlib.h>
#include <time.h>

#include "mathex/random.hpp"
#include "recorder/recorder.hpp"
#include "recorder/private/recpriv.hpp"

#define RAND_MAX 0x7fff

MexBasicRandom::MexBasicRandom()
: state_( 0 )
{
    seed_ = state_;

    TEST_INVARIANT;
}

MexBasicRandom::~MexBasicRandom()
{
    TEST_INVARIANT;

}

int MexBasicRandom::next()
{
    state_ = state_ * 1103515245 + 12345;
    int result = ((unsigned int)( state_ >> 16 ) & RAND_MAX );

    POST( 0 <= result and result < upperLimit() );

    return result;
}

// static
int MexBasicRandom::upperLimit()
{
    return (int)RAND_MAX + 1;
}

void    MexBasicRandom::seed( ulong newSeed )
{
    state_ = newSeed;
    seed_ = state_;
}

void    MexBasicRandom::seedFromTime()
{
    ulong result;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        result = RecRecorderPrivate::instance().playbackRandomSeed();
    else
    {
        result = time( NULL ) + clock() + seedIncrement();
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            RecRecorderPrivate::instance().recordRandomSeed( result );
    }

    state_ = result;
    seed_ = state_;
}

//  Construct a random number generator seeded by the
//  time and return it
// static
MexBasicRandom MexBasicRandom::constructSeededFromTime()
{
    MexBasicRandom  r;
    r.seedFromTime();

    return r;
}

//  static
size_t  MexBasicRandom::seedIncrement()
{
    static  size_t  increment = 0;

    return ++increment;
}

ulong MexBasicRandom::seed() const
{
    return seed_;
}

void MexBasicRandom::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MexBasicRandom& t )
{

    o << "MexBasicRandom " << (void*)&t << " start" << std::endl;
    o << "MexBasicRandom " << (void*)&t << " end" << std::endl;

    return o;
}

/* End RANDOM.CPP ***************************************************/
