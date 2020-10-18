/*
 * C L A S S 1 2 . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    Class12

    Test writing / reading of raw data
*/

#ifndef _PERTEST_CLASS12_HPP
#define _PERTEST_CLASS12_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

class Class12
// Canonical form revoked
{
public:
    Class12( void );
    ~Class12( void );

    PER_MEMBER_PERSISTENT_DEFAULT( Class12 );
    PER_FRIEND_READ_WRITE( Class12 );
    
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const Class12& t );

private:
    Class12( const Class12& );
    Class12& operator =( const Class12& );
    friend bool operator ==( const Class12&, const Class12& );

    size_t  nBytes_;
    char* ptr_;
};

PER_DECLARE_PERSISTENT( Class12 );

#endif

/* End CLASS12.HPP **************************************************/
