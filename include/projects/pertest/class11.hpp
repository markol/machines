/*
 * C L A S S 1 1 . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    Class11

    A brief description of the class should go in here
*/

#ifndef _PERTEST_CLASS11_HPP
#define _PERTEST_CLASS11_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

class Class11;
PER_READ_WRITE( Class11 );

class Class11
// Canonical form revoked
{
public:
    Class11( void );
    ~Class11( void );

    int i_;
    
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const Class11& t );

    PER_MEMBER_PERSISTENT_DEFAULT_INLINE( Class11 );
    
private:
    Class11( const Class11& );
    Class11& operator =( const Class11& );
    bool operator ==( const Class11& );

};

#ifdef _INLINE
    #include "pertest/class11.ipp"
#endif


#endif

/* End CLASS11.HPP **************************************************/
