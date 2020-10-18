/*
 * C L A S S 1 0 . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    Class10

    A brief description of the class should go in here
*/

#ifndef _PERSIST_CLASS10_HPP
#define _PERSIST_CLASS10_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "pertest/class9.hpp"

class Class10 : public Class9< int >
// Canonical form revoked
{
public:
    Class10( void );
    virtual ~Class10( void );

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( Class10 );
    friend ostream& operator <<( ostream& o, const Class10& t );

    virtual void overrideMe() const;

private:
    Class10( const Class10& );
    Class10& operator =( const Class10& );
    bool operator ==( const Class10& );

};

PER_READ_WRITE( Class10 );

bool operator ==( const Class10&, const Class10& );

PER_DECLARE_PERSISTENT( Class10 );

#endif

/* End CLASS10.HPP **************************************************/
