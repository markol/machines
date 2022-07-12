/*
 * C L A S S 8 . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    Class8

    This class is not persistent. It is here to test that objects stored
    in containers or counted pointers do not have to be persistent if the
    container / counted pointer is not written out.
*/

#ifndef _PERSIST_CLASS8_HPP
#define _PERSIST_CLASS8_HPP

#include "base/base.hpp"

class Class8
{
public:
    Class8();
    Class8( const Class8& );

    ~Class8();
    Class8& operator =( const Class8& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const Class8& t );

private:

    // Operation deliberately revoked
    bool operator ==( const Class8& );

};


#endif

/* End CLASS8.HPP ***************************************************/
