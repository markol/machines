/*
 * D U M M Y . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/dummy.hpp"

DummyClass::DummyClass()
{

    TEST_INVARIANT;
}

DummyClass::~DummyClass()
{
    TEST_INVARIANT;

}

void DummyClass::CLASS_INVARIANT
{
}


/* End DUMMY.CPP ****************************************************/
