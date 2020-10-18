/*
 * C L A S S 1 3 . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    Class13

    A class designed to test that circular loops of pointers work properly
*/

#ifndef _PERTEST_CLASS13_HPP
#define _PERTEST_CLASS13_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

class Class13
// Canonical form revoked
{
public:
    Class13( void );
    ~Class13( void );

    Class13*    ptr_;
    int         i1_;
    
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const Class13& t );

    PER_MEMBER_PERSISTENT_DEFAULT( Class13 );
    
private:
    Class13( const Class13& );
    Class13& operator =( const Class13& );

};

PER_READ_WRITE( Class13 );
PER_DECLARE_PERSISTENT( Class13 );

bool operator ==( const Class13& a, const Class13& b );

#endif

/* End CLASS13.HPP **************************************************/
