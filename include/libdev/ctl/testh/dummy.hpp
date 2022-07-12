/*
 * D U M M Y . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    DummyClass

    A brief description of the class should go in here
*/

#ifndef _CTL_DUMMY_HPP
#define _CTL_DUMMY_HPP

#include "base/base.hpp"

class DummyClass
{
public:
    DummyClass();
    DummyClass( const DummyClass& );
    DummyClass& operator =( const DummyClass& );
    ~DummyClass();

    void CLASS_INVARIANT;

private:

};

bool operator ==( const DummyClass&, const DummyClass& );


#endif

/* End DUMMY.HPP ****************************************************/
