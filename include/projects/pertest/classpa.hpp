/*
 * C L A S S P A . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    ClassPair

    Test persistence of pairs
*/

#ifndef _PERTEST_CLASSPA_HPP
#define _PERTEST_CLASSPA_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "stdlib/utility.hpp"

class ClassPair
// Canonical form revoked
{
public:
    ClassPair( void );
    ~ClassPair( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const ClassPair& t );
    friend bool operator ==( const ClassPair& a, const ClassPair& b );
    friend bool operator !=( const ClassPair& a, const ClassPair& b );

    PER_MEMBER_PERSISTENT_DEFAULT( ClassPair );
    PER_FRIEND_READ_WRITE( ClassPair );

private:
    ClassPair( const ClassPair& );
    ClassPair& operator =( const ClassPair& );
    bool operator ==( const ClassPair& );
    bool operator !=( const ClassPair& );

    typedef std::pair< int, double > Pair;

    Pair    p1_;
    Pair*   pP1_;
};

PER_DECLARE_PERSISTENT( ClassPair );

#endif

/* End CLASSPA.HPP **************************************************/
