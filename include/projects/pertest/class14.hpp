/*
 * C L A S S 1 4 . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    Class14

    A brief description of the class should go in here
*/

#ifndef _PERTEST_CLASS14_HPP
#define _PERTEST_CLASS14_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/mmvector.hpp"

class Class14 : public ctl_min_memory_vector< int >
// Canonical form revoked
{
public:
    Class14( void );
    virtual ~Class14( void );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_DEFAULT_VIRTUAL( Class14 );
    
    friend ostream& operator <<( ostream& o, const Class14& t );

    typedef ctl_min_memory_vector< int >    Base;
    
private:
    Class14( const Class14& );
    Class14& operator =( const Class14& );

};

PER_READ_WRITE( Class14 );
PER_DECLARE_PERSISTENT( Class14 );

bool operator ==( const Class14&, const Class14& );

#endif

/* End CLASS14.HPP **************************************************/
