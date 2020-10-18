/*
 * R A N D O M . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/random.hpp"

MachPhysRandom::MachPhysRandom()
{

    TEST_INVARIANT;
}

MachPhysRandom::~MachPhysRandom()
{
    TEST_INVARIANT;

}

//  static
double MachPhysRandom::randomDouble( MATHEX_SCALAR lowerLimit, MATHEX_SCALAR upperLimit )
{
    PRE( lowerLimit <= upperLimit );

    double result;

//    result = lowerLimit + ( (float)(rand()) / 32767.0 ) * ( upperLimit - lowerLimit );
    result = lowerLimit + ( (float)(rand()) / (float)RAND_MAX ) * ( upperLimit - lowerLimit );

    POST( lowerLimit <= result and result <= upperLimit );

    return result;
}

// static
int MachPhysRandom::randomInt( int lowerLimit, int upperLimit )
{
    PRE_INFO( lowerLimit );
    PRE_INFO( upperLimit );
    PRE( lowerLimit < upperLimit );

    int result = lowerLimit + rand() % ( upperLimit - lowerLimit );

    POST( lowerLimit <= result and result < upperLimit );

    return result;
}

// static
size_t MachPhysRandom::randomInt( size_t lowerLimit, size_t upperLimit )
{
    PRE( lowerLimit <= upperLimit );

    size_t result = lowerLimit + rand() % ( upperLimit - lowerLimit );

    POST( lowerLimit <= result and result < upperLimit );

    return result;
}

// static
size_t MachPhysRandom::randomInt( size_t upperLimit )
{
    size_t result = rand() % upperLimit;

    POST( result < upperLimit );

    return result;
}

void MachPhysRandom::CLASS_INVARIANT
{
}


/* End RANDOM.CPP ***************************************************/
